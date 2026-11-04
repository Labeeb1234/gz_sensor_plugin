#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include <chrono>
#include <thread>
#include <random>

// MAIN GZ SIM API HEADERS HERE
#include "gz/sim7/gz/sim/EntityComponentManager.hh"
#include "gz/sim7/gz/sim/System.hh"
#include "gz/sim7/gz/sim/Model.hh"
#include "gz/sim7/gz/sim/Entity.hh"
#include "gz/sim7/gz/sim/Link.hh"
#include "gz/sim7/gz/sim/components/Name.hh"
#include "gz/sim7/gz/sim/components/Sensor.hh"
#include "gz/sim7/gz/sim/components/Imu.hh"
#include "gz/sim7/gz/sim/components/LinearAcceleration.hh"
#include "gz/sim7/gz/sim/components/AngularVelocity.hh"


// GZ HEADERS FOR DATA PUBLISHING AND SUBSCRIBING TO GZ TOPICS
#include "gz/transport12/gz/transport/Node.hh"
#include "gz/msgs9/gz/msgs/vector3d.pb.h"
#include "gz/msgs9/gz/msgs/double_v.pb.h"

// UTILS HEADERS
#include "gz/sim7/gz/sim/config.hh"
#include "gz/common5/gz/common/Console.hh"
#include "gz/sim7/gz/sim/Util.hh"
#include "gz/sdformat13/sdf/Element.hh"
#include "gz/sdformat13/sdf/Sensor.hh"
#include "gz/sdformat13/sdf/Imu.hh"
#include "gz/sdformat13/sdf/Noise.hh"
#include "gz/plugin2/gz/plugin/Register.hh"
#include "gz/math7/gz/math/Vector3.hh"


// eventually convert this to the Pointer to implmentation format for complaice with ABI arch of GZ sim

class TestSensorDataPlugin: 
    public gz::sim::System, 
    public gz::sim::ISystemConfigure,
    public gz::sim::ISystemPreUpdate,
    public gz::sim::ISystemUpdate,
    public gz::sim::ISystemPostUpdate
{

private:
    // create the target entities
    gz::sim::Entity imu_entity_{gz::sim::kNullEntity};
    gz::sim::Entity base_entity_{gz::sim::kNullEntity};
    gz::sim::Entity link_entity_{gz::sim::kNullEntity};

    // create a publish to publish imu data with added noise and bias modelled using normal distribution
    gz::transport::Node node_;
    std::unique_ptr<gz::transport::Node::Publisher> accel_pub_;
    std::unique_ptr<gz::transport::Node::Publisher> gyro_pub_;


    // some important attributes
    bool publishers_initialized_ = false;
    std::string input_topic_name_;
    std::string sensor_params_topic_;
    float imu_update_rate_; // the og imu topic update rate used to update the publishing rate of this one too

    // custom construtor to store the Noise and bias params of the sensor model
    std::mt19937 gen_; // random num generator
    std::normal_distribution<double> accel_noise_{0.0, 0.0}, gyro_noise_{0.0, 0.0};
    std::vector<double> accel_noise_vec_{0.0, 0.0, 0.0}, gyro_noise_vec_{0.0, 0.0, 0.0};
    struct SensorParams{
        double mean;
        double std_dev;
        double bias_mean;
        double bias_std_dev;
        double dynamic_bias_std_dev;
        double dynamic_bias_correlation_time;
    }sensor_params_; // sensor_params_ attirb will act as a temp storage var to store the callback data
    std::vector<SensorParams> accel_params_, gyro_params_;


    bool sensor_params_updated_ = false;


    // mutex to protect imu params (when extracting from gui on changes)
    std::mutex imu_param_mutex_;

    void logCurrentSensorNoiseBiasParams(const sdf::Imu* imu_sdf){
        if (!imu_sdf){
            gzwarn << "Received null pointer for IMU SDF configuration!" << std::endl;
            return;
        }

        // Helper lambda to format and log individual noise parameters cleanly
        auto logAxisNoise = [](const std::string& _axisName, const sdf::Noise& _noise) {
            gzmsg << "    " << _axisName << " Axis -> "
                << "StdDev: " << _noise.StdDev() 
                << " | Mean: " << _noise.Mean() 
                << " | BiasMean: " << _noise.BiasMean() 
                << " | BiasStdDev: " << _noise.BiasStdDev() 
                << std::endl;
        };

        gzmsg << "==================================================\n";
        gzmsg << "          IMU NOISE & BIAS CONFIGURATION          \n";
        gzmsg << "==================================================\n";
        gzmsg << "[Linear Acceleration]\n";
        logAxisNoise("X", imu_sdf->LinearAccelerationXNoise());
        logAxisNoise("Y", imu_sdf->LinearAccelerationYNoise());
        logAxisNoise("Z", imu_sdf->LinearAccelerationZNoise());
        gzmsg << "\n[Angular Velocity]\n";
        logAxisNoise("X", imu_sdf->AngularVelocityXNoise());
        logAxisNoise("Y", imu_sdf->AngularVelocityYNoise());
        logAxisNoise("Z", imu_sdf->AngularVelocityZNoise());
        gzmsg << "==================================================" << std::endl;
            
    }


    // helper methods if required
    void publishSensorData(
        const gz::math::Vector3d& _new_accel, 
        const gz::math::Vector3d& _new_gyro, 
        const gz::sim::UpdateInfo& _info
    ){
        // std::lock_guard<std::mutex> lock(this->imu_param_mutex_);
        if(!this->accel_pub_ || !this->gyro_pub_){return;}

        // gz::msgs::IMU msg;
        gz::msgs::Vector3d accel_msg;
        gz::msgs::Vector3d gyro_msg;

        auto sim_time_sec = std::chrono::duration_cast<std::chrono::seconds>(_info.simTime).count();
        auto sim_time_nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(_info.simTime).count();
        accel_msg.mutable_header()->mutable_stamp()->set_sec(static_cast<uint32_t>(sim_time_sec));
        accel_msg.mutable_header()->mutable_stamp()->set_nsec(static_cast<uint64_t>(sim_time_nsec));
        gyro_msg.mutable_header()->mutable_stamp()->set_sec(static_cast<uint32_t>(sim_time_sec));
        gyro_msg.mutable_header()->mutable_stamp()->set_nsec(static_cast<uint64_t>(sim_time_nsec));

        // set the modded accel here
        accel_msg.set_x(_new_accel.X());
        accel_msg.set_y(_new_accel.Y());
        accel_msg.set_z(_new_accel.Z());
        // set the modded gyro data here
        gyro_msg.set_x(_new_gyro.X());
        gyro_msg.set_y(_new_gyro.Y());
        gyro_msg.set_z(_new_gyro.Z());

        this->accel_pub_->Publish(accel_msg);
        this->gyro_pub_->Publish(gyro_msg);

    }; // publish the new updates sensor readings to the respective gz topics also control the publish rate too based on sensor update rate
    
    
    // input args are the og accel and gyro data for adding to post update state need two args to store new accel and gyro readings
    void updateImuReadings(
        gz::math::Vector3d& _new_accel,
        gz::math::Vector3d& _new_gyro,
        const gz::math::Vector3d& _accel,
        const gz::math::Vector3d& _gyro
    ){
        {
            std::lock_guard<std::mutex> lock(this->imu_param_mutex_);
            _new_accel.X() = _accel.X() + this->accel_noise_vec_[0]; //  for now add only noise model for each axes
            _new_accel.Y() = _accel.Y() + this->accel_noise_vec_[1];
            _new_accel.Z() = _accel.Z() + this->accel_noise_vec_[2];
            _new_gyro.X() = _gyro.X() + this->gyro_noise_vec_[0];
            _new_gyro.Y() = _gyro.Y() + this->gyro_noise_vec_[1];
            _new_gyro.Z() = _gyro.Z() + this->gyro_noise_vec_[2];
        }
    } // update imu data with according to noise and bias param updates
    
    //  sensor noise and bias param updates taken from topic or should this be converted to service-client architecture
    void onParamUpdateCallback(const gz::msgs::Double_V& msg){
        std::lock_guard<std::mutex> lock(this->imu_param_mutex_);
        // size_t param_vec_size =  msg.data_size();
        // add error log for 0 size data vector
        this->sensor_params_.mean = msg.data(0);
        this->sensor_params_.std_dev = msg.data(1);
        this->sensor_params_.bias_mean = msg.data(2);
        this->sensor_params_.bias_std_dev = msg.data(3);
        this->sensor_params_.dynamic_bias_std_dev = msg.data(4);
        this->sensor_params_.dynamic_bias_correlation_time = msg.data(5);
        this->sensor_params_updated_= msg.data(6) == 1.0 ? true : false;
    };

public:

    TestSensorDataPlugin();
    ~TestSensorDataPlugin() override;

    void Configure(
        const gz::sim::Entity& _entity,
        const std::shared_ptr<const sdf::Element>& _sdf,
        gz::sim::EntityComponentManager& _ecm,
        gz::sim::EventManager &_eventMgr
    ) override;
    void PreUpdate(
        const gz::sim::UpdateInfo& _info,
        gz::sim::EntityComponentManager& _ecm
    ) override;
    void Update(
        const gz::sim::UpdateInfo& _info,
        gz::sim::EntityComponentManager& _ecm
    ) override;
    void PostUpdate(
        const gz::sim::UpdateInfo& _info,
        const gz::sim::EntityComponentManager& _ecm
    ) override;


};

TestSensorDataPlugin::TestSensorDataPlugin(): gen_(std::random_device{}()){
    gzmsg << "Loading Test Sensor Data Plugin for testing!" << std::endl;
}
TestSensorDataPlugin::~TestSensorDataPlugin() = default;

void TestSensorDataPlugin::Configure(
    const gz::sim::Entity& _entity,
    const std::shared_ptr<const sdf::Element>& _sdf,
    gz::sim::EntityComponentManager& _ecm,
    gz::sim::EventManager &_eventMgr
){
    // store base model
    this->base_entity_ = _entity;
    gz::sim::Model model(_entity);
    std::string model_name = model.Name(_ecm);

    gzmsg << "========================================" << std::endl;
    gzmsg << "TestSensorDataPlugin attached to Model Name: [" << model_name << "]" << std::endl;
    gzmsg << "========================================" << std::endl;

    // from model extract link as well as sensor info and data
    std::vector<gz::sim::Entity> link_entities = model.Links(_ecm);
    if(link_entities.size() == 1){
        // only one main link present
        const gz::sim::Entity link_entity = link_entities.at(0);
        // extract all sensor info and data structure attached to this link
        // from link info extract all available sensors ??
        std::vector<gz::sim::Entity> sensor_entities = _ecm.ChildrenByComponents(
            link_entity, 
            gz::sim::components::Sensor()
        );
        // int imu_sensor_count = 0;
        for(auto& sensor_entity: sensor_entities){
            auto sensor_name = _ecm.ComponentData<gz::sim::components::Name>(sensor_entity);
            if(sensor_name){
                gzmsg << "-> attached sensor: "<< *sensor_name << std::endl;
                // for now findig the first imu sensor in the model and breaking out not handling multiple imus
                if(_ecm.Component<gz::sim::components::Imu>(sensor_entity)){
                    this->imu_entity_ = sensor_entity;
                    gzmsg << "Found the First Imu Entity" << std::endl;
                    break;
                }
            }
        }
    }else{
        return;
    }

    // extract information subscription topic for the sensor noise and bias param updation
    if(_sdf->HasElement("sensor_params_topic")){
        // create the element
        this->sensor_params_topic_ = _sdf->Get<std::string>("sensor_params_topic");
    }else{
        this->sensor_params_topic_ = gz::transport::TopicUtils::AsValidTopic("/sensor_params");
    }
    gzmsg << "setting sensor parameter update topic as: " << this->sensor_params_topic_  << std::endl;

    // extract the imu noise and bias model the default initial values on loading of the plugin
    if(this->imu_entity_ != gz::sim::kNullEntity){
        if(!_ecm.EntityHasComponentType(this->imu_entity_, gz::sim::components::Imu::typeId)){
            // add error logs if not found imu component
            return;
        }
        // update initial values of the sensor noise and bias model here
        auto imu_comp = _ecm.Component<gz::sim::components::Imu>(this->imu_entity_);
        const sdf::Sensor& sensor_sdf = imu_comp->Data(); // get imu sdf element props from here 
        const sdf::Imu* imu_sdf = sensor_sdf.ImuSensor();

        // creating a lambda func for this task
        auto setSensorParams = [](const sdf::Noise& _noise, std::vector<SensorParams>& sens_params){
            SensorParams params;
            params.mean = _noise.Mean();
            params.std_dev = _noise.StdDev();
            params.bias_mean = _noise.BiasMean();
            params.bias_std_dev = _noise.BiasStdDev();
            params.dynamic_bias_std_dev = _noise.DynamicBiasStdDev();
            params.dynamic_bias_correlation_time = _noise.DynamicBiasCorrelationTime();
            sens_params.push_back(params);
        };

        this->accel_params_.reserve(3); // for 3 axis quantities
        this->gyro_params_.reserve(3);
        // set accelerometer params
        setSensorParams(imu_sdf->LinearAccelerationXNoise(), this->accel_params_);
        setSensorParams(imu_sdf->LinearAccelerationYNoise(), this->accel_params_);
        setSensorParams(imu_sdf->LinearAccelerationZNoise(), this->accel_params_);
        // set gyro params
        setSensorParams(imu_sdf->AngularVelocityXNoise(), this->gyro_params_);
        setSensorParams(imu_sdf->AngularVelocityYNoise(), this->gyro_params_);
        setSensorParams(imu_sdf->AngularVelocityZNoise(), this->gyro_params_);

        // check noise and bias model type (probability distribution function)
        // ADD LAST SON!!!!!!!!!!!!!!
        bool subscribed = this->node_.Subscribe(
            this->sensor_params_topic_,
            &TestSensorDataPlugin::onParamUpdateCallback,
            this
        );
        if(!subscribed){
                gzerr << "Failed to subscribe to sensor params topic: " << this->sensor_params_topic_ << std::endl;
        }
    }
}

void TestSensorDataPlugin::PreUpdate(
    const gz::sim::UpdateInfo& _info,
    gz::sim::EntityComponentManager& _ecm
){
    if(_info.paused){return;}
    float dt_sec = std::chrono::duration<float>(_info.dt).count();

    // Imu entity is detected here only after first forced step not in Configure unfortunately
    // check if imu entity was configured and detected properly from the Configure step
    if(this->imu_entity_ == gz::sim::kNullEntity){
        // TODO: add error log before crashout
        return;
    }
    if(!_ecm.EntityHasComponentType(this->imu_entity_, gz::sim::components::Imu::typeId)){
        // TODO: add error logs
        return;
    }

    // check if the required accel and gyro components are present in the imu entity
    if(
        _ecm.Component<gz::sim::components::LinearAcceleration>(this->imu_entity_) == nullptr ||
        _ecm.Component<gz::sim::components::AngularVelocity>(this->imu_entity_) == nullptr
    ){
        // TODO: add error logs
        return;
    }

    // if all ok then log the Noise and Bias model params of the sensor pre physics update
    auto imu_comp = _ecm.Component<gz::sim::components::Imu>(this->imu_entity_);
    const sdf::Sensor& sensor_sdf = imu_comp->Data();
    const sdf::Imu* imu_sdf = sensor_sdf.ImuSensor();
    this->input_topic_name_ = sensor_sdf.Topic();
    this->imu_update_rate_ = sensor_sdf.UpdateRate(); // Extract IMU update rate param too here    // a log before the physics update of one iteration

    // initialize imu data publisher
    if(!this->publishers_initialized_){
        // set the publisher topic settings like update rate
        gz::transport::AdvertiseMessageOptions opts;
        if (this->imu_update_rate_ > 0.0f){
            opts.SetMsgsPerSec(static_cast<unsigned int>(this->imu_update_rate_));
        }

        this->accel_pub_ = std::make_unique<gz::transport::Node::Publisher>(
            this->node_.Advertise<gz::msgs::Vector3d>("accel/real", opts)
        );
        this->gyro_pub_ = std::make_unique<gz::transport::Node::Publisher>(
            this->node_.Advertise<gz::msgs::Vector3d>("gyro/real", opts)
        );

        if (this->accel_pub_ && this->gyro_pub_){
            gzmsg << "Successfully advertising custom IMU topics on: [" << "accel/real & gyro/real" << "]" << std::endl;
            this->publishers_initialized_ = true;
        }
        else{
            gzerr << "Failed to advertise topics: " << "accel/real & gyro/real" << std::endl;
        }
    }

    // ---------------------- SENSOR PARAMS UPDATION LOGIC HERE ----------------------
    // more pre update logics
    // after subscription check for update flag if that is true then update the sensor noise and bias model params
    double accel_mean = 0.0, accel_sigma = 0.0;
    double gyro_mean = 0.0, gyro_sigma = 0.0; // later change to vector
    {
        std::lock_guard<std::mutex> lock(this->imu_param_mutex_);
        if(this->sensor_params_updated_){
            if(!this->accel_params_.empty()){
                for(int i=0; i<this->accel_params_.size(); ++i){
                    this->accel_params_[i].mean = this->sensor_params_.mean;
                    this->accel_params_[i].std_dev = this->sensor_params_.std_dev;
                    this->accel_params_[i].bias_mean = this->sensor_params_.bias_mean;
                    this->accel_params_[i].bias_std_dev = this->sensor_params_.bias_std_dev;
                    this->accel_params_[i].dynamic_bias_std_dev = this->sensor_params_.dynamic_bias_std_dev;
                    this->accel_params_[i].dynamic_bias_correlation_time = this->sensor_params_.dynamic_bias_correlation_time;
                }
            }
            if(!this->gyro_params_.empty()){
                for(int i=0; i<this->gyro_params_.size(); ++i){
                    this->gyro_params_[i].mean = this->sensor_params_.mean;
                    this->gyro_params_[i].std_dev = this->sensor_params_.std_dev;
                    this->gyro_params_[i].bias_mean = this->sensor_params_.bias_mean;
                    this->gyro_params_[i].bias_std_dev = this->sensor_params_.bias_std_dev;
                    this->gyro_params_[i].dynamic_bias_std_dev = this->sensor_params_.dynamic_bias_std_dev;
                    this->gyro_params_[i].dynamic_bias_correlation_time = this->sensor_params_.dynamic_bias_correlation_time;
                }
            }
            // since all axis params as same for now just safely store locally withing scope to prevent
            if(!this->accel_params_.empty()){
                accel_mean = this->accel_params_[0].mean;
                accel_sigma = this->accel_params_[0].std_dev;
            }
            if(!this->gyro_params_.empty()){
                gyro_mean = this->gyro_params_[0].mean;
                gyro_sigma = this->gyro_params_[0].std_dev;
            }
        }
    }
    // based on the existing noise and bias model params create a gaussian probability function to model the noise and bias of the sensor
    if (this->accel_noise_vec_.size() != 3) {this->accel_noise_vec_.resize(3);}
    if (this->gyro_noise_vec_.size() != 3) {this->gyro_noise_vec_.resize(3);}
    for(int i=0; i<3; ++i){
        this->accel_noise_.param(
        std::normal_distribution<double>::param_type(
            accel_mean, accel_sigma
        ));
        this->accel_noise_vec_[i] = this->accel_noise_(this->gen_);

        this->gyro_noise_.param(
        std::normal_distribution<double>::param_type(
            gyro_mean, gyro_sigma
        ));
        this->gyro_noise_vec_[i] = this->gyro_noise_(this->gen_);
    }





    // ---------------------- SENSOR PARAMS UPDATION LOGIC HERE ----------------------
    // gzmsg << "=== TestSensorDataPlugin: Accelerated Noise Parameters ===\n";
    // const std::string axes[] = {"X", "Y", "Z"};
    // for(size_t i = 0; i < this->accel_params_.size(); ++i) {
    //     gzmsg << "  Axis " << axes[i] << " -> "
    //             << "Mean: " << this->accel_params_[i].mean 
    //             << ", StdDev: " << this->accel_params_[i].std_dev 
    //             << ", BiasMean: " << this->accel_params_[i].bias_mean 
    //             << ", BiasStdDev: " << this->accel_params_[i].bias_std_dev << "\n";
    // }
}

void TestSensorDataPlugin::Update(
    const gz::sim::UpdateInfo& _info,
    gz::sim::EntityComponentManager& _ecm
){
    if(_info.paused){return;}

    // on physics step update mod the imu values ??
    // i think this should be in post update step for now
    // major physics logic changes should only be incorporated here


}

void TestSensorDataPlugin::PostUpdate(
    const gz::sim::UpdateInfo& _info,
    const gz::sim::EntityComponentManager& _ecm
){
    if(_info.paused){return;}

    // update is for physics sim update and pre-update to send control signal that is used to do the physics update step
    // log and publish data after physics update 
    auto accel_comp = _ecm.Component<gz::sim::components::LinearAcceleration>(this->imu_entity_);
    auto gyro_comp =  _ecm.Component<gz::sim::components::AngularVelocity>(this->imu_entity_);

   // Data altered inside the physics update step
    const gz::math::Vector3d& accel = accel_comp->Data();
    const gz::math::Vector3d& gyro = gyro_comp->Data();

    // for the first test logic for the imu data updation put logic here , lets not modify the in-physics sensor unless and untill we create our own sensor model
    gz::math::Vector3d new_accel;
    gz::math::Vector3d new_gyro;
    if(!this->accel_noise_vec_.empty()){
        this->updateImuReadings(
            new_accel,
            new_gyro,
            accel,
            gyro
        );
    }

    // publish imu data at the required rate to the final topic let name be "imu/real"
    if(this->publishers_initialized_){
        this->publishSensorData(new_accel, new_gyro, _info);
    }
            
}


GZ_ADD_PLUGIN(
    TestSensorDataPlugin,
    gz::sim::System,
    TestSensorDataPlugin::ISystemConfigure,
    TestSensorDataPlugin::ISystemPreUpdate,
    TestSensorDataPlugin::ISystemUpdate,
    TestSensorDataPlugin::ISystemPostUpdate
)
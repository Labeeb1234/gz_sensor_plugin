#include <iostream>
#include <memory>
#include <thread>

#include "gz/plugin2/gz/plugin/Plugin.hh"
#include "gz/plugin2/gz/plugin/Register.hh"
#include "gz/gui7/gz/gui/Application.hh"

#include "gz/msgs9/gz/msgs/double_v.pb.h"

#include "sensor_gui.hh"


namespace sensor_gui{

SensorParamPlugin::SensorParamPlugin(): gz::gui::Plugin(){
    // first construction of the required gz publisher nodes
    this->pub_floats_ = std::make_unique<gz::transport::Node::Publisher>( 
        this->node_.Advertise<gz::msgs::Double_V>(this->topic_name_.toStdString())
    );
}

SensorParamPlugin::~SensorParamPlugin() = default;

void SensorParamPlugin::LoadConfig(const tinyxml2::XMLElement *_pluginElem){
    if(this->title.empty()){
        this->title = "Sensor Parameter Control";
    }

    if(_pluginElem){
        auto elem = _pluginElem->FirstChildElement("topic_name");
        if(elem){
            this->SetTopicFloats(QString::fromStdString(elem->GetText()));
        }
    }
}

QString SensorParamPlugin::TopicFloats()const{return this->topic_name_;}
void SensorParamPlugin::SetTopicFloats(const QString& _topic){
    if(this->topic_name_ != _topic){
        this->topic_name_ = _topic;
        this->pub_floats_ = std::make_unique<gz::transport::Node::Publisher>( 
            this->node_.Advertise<gz::msgs::Double_V>(this->topic_name_.toStdString()) // optional can set custom data publishing rate if required using gz transport
        );
        emit TopicFloatsChanged();
    }
}

QVariantList SensorParamPlugin::FloatValues()const{return this->param_floats_;}
void SensorParamPlugin::SetFloatValues(const QVariantList &_values){
    {
        std::lock_guard<std::mutex> lock(this->param_mutex_);
        this->param_floats_ = _values;
        emit FloatValuesChanged();
    }
    publishFloats();
} 

void SensorParamPlugin::updateFloatValue(int index, double value){
    bool param_changed = false; 
    {
        std::lock_guard<std::mutex> lock(this->param_mutex_);
        if(index >= 0 && index < this->param_floats_.size()){
            if(this->param_floats_[index] != value){
                this->param_floats_[index] = value;
                emit FloatValuesChanged();
                param_changed = true;
            }
        }
    }
    if(param_changed){
        publishFloats();
    }
}

void SensorParamPlugin::publishFloats(){
    gz::msgs::Double_V msg;
    {
        std::lock_guard<std::mutex> lock(this->param_mutex_);
        for(auto& float_val: param_floats_){
            msg.add_data(float_val.toDouble());
        }
    }
    this->pub_floats_->Publish(msg);
}


} // namespace sensor_gui

GZ_ADD_PLUGIN(
  sensor_gui::SensorParamPlugin,
  gz::gui::Plugin
)
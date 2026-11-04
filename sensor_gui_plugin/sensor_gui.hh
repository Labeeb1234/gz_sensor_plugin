#ifndef SENSOR_PARAM_PLUGIN_HH_
#define SENSOR_PARAM_PLUGIN_HH_

#include "gz/gui7/gz/gui/Plugin.hh"

#include "gz/transport12/gz/transport/Node.hh"
#include "gz/msgs9/gz/msgs/float.pb.h"

#include <QObject>
#include <QString>
#include <QVariantList>
#include <thread>
#include <memory>

namespace sensor_gui {

class SensorParamPlugin : public gz::gui::Plugin {
    Q_OBJECT
    // Q_PROPERTY bindings expose CPP variables/functions directly to QML
    Q_PROPERTY(QString topicFloats READ TopicFloats WRITE SetTopicFloats NOTIFY TopicFloatsChanged)
    Q_PROPERTY(QVariantList floatValues READ FloatValues WRITE SetFloatValues NOTIFY FloatValuesChanged)

public:
    SensorParamPlugin();
    ~SensorParamPlugin() override;

    // Called when loaded from XML/SDF configuration
    void LoadConfig(const tinyxml2::XMLElement *_pluginElem) override;

    QString TopicFloats() const;
    void SetTopicFloats(const QString &_topic);

    QVariantList FloatValues() const;
    void SetFloatValues(const QVariantList &_values);

    // Invokable method from QML to update a single index value dynamically
    Q_INVOKABLE void updateFloatValue(int index, double val);
    Q_INVOKABLE void publishFloats();

signals:
  void TopicFloatsChanged();
  void FloatValuesChanged();

private:
    QString topic_name_{"/sensor_params"};
    QVariantList param_floats_{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; // the last index actually value represents a flag value to indicate data has been updated 
    gz::transport::Node node_;
    // for now
    std::unique_ptr<gz::transport::Node::Publisher> pub_floats_;
    float publish_rate_ = 10.0; // unused as of now
    std::mutex param_mutex_;
};

} // namespace sensor_gui

#endif
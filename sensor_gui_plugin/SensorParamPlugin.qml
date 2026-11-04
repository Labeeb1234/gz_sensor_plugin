import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
  color: "#2b2b2b"
  implicitWidth: 360
  implicitHeight: 540
  Layout.minimumWidth: 360
  Layout.minimumHeight: 540

  ColumnLayout {
    anchors.fill: parent
    anchors.margins: 12
    spacing: 10

    // Topic being published to
    Label {
      text: "Topic: " + SensorParamPlugin.topicFloats
      color: "#aaaaaa"
      Layout.fillWidth: true
      elide: Text.ElideRight
    }

    // Dynamic List View for N Float Parameters
    ListView {
      id: floatListView
      Layout.fillWidth: true
      Layout.fillHeight: true
      model: SensorParamPlugin.floatValues
      clip: true
      spacing: 8
      ScrollBar.vertical: ScrollBar { policy: ScrollBar.AsNeeded }

      // One entry per parameter, same order as the QVariantList
      property var paramConfig: [
        { name: "mean", min: -20.0, max: 20.0, step: 0.1 },
        { name: "std dev", min:   0.0, max:  5.0, step: 0.01 },
        { name: "bias mean", min:   -20.0, max: 20.0, step: 1.0 },
        { name: "bias std dev", min: 0.0, max: 5.0, step: 0.5 },
        { name: "dynamic bias std dev", min:   0.0, max:  5.0, step: 0.05 },
        { name: "dynamic correlation time", min: 0.0,  max:  500.0, step: 1.0 },
        { name: "parameter updated flag", min: 0.0, max: 1.0, step: 1.0}
      ]
      // Used for any index without an entry above
      property var defaultConfig: ({ name: "", min: -50.0, max: 50.0, step: 0.1 })

      delegate: ColumnLayout {
        width: floatListView.width
        spacing: 2

        property var cfg: floatListView.paramConfig[index] || floatListView.defaultConfig
        // Decimal places shown follow the step size (0.01 -> 2, 0.001 -> 3)
        property int decimals: Math.max(0, Math.ceil(-Math.log(cfg.step) / Math.LN10))

        Label {
          text: (cfg.name || "Parameter [" + index + "]") + ": "
                + Number(modelData).toFixed(decimals)
                + "   [" + cfg.min + " … " + cfg.max + "]"
          color: "white"
        }

        Slider {
          Layout.fillWidth: true
          from: cfg.min
          to: cfg.max
          stepSize: cfg.step
          snapMode: Slider.SnapAlways   // value always lands on a step
          value: modelData
          onMoved: SensorParamPlugin.updateFloatValue(index, value)
        }
      }
    }
  }
}
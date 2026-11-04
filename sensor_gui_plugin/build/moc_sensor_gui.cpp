/****************************************************************************
** Meta object code from reading C++ file 'sensor_gui.hh'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../sensor_gui.hh"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sensor_gui.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_sensor_gui__SensorParamPlugin_t {
    QByteArrayData data[10];
    char stringdata0[134];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_sensor_gui__SensorParamPlugin_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_sensor_gui__SensorParamPlugin_t qt_meta_stringdata_sensor_gui__SensorParamPlugin = {
    {
QT_MOC_LITERAL(0, 0, 29), // "sensor_gui::SensorParamPlugin"
QT_MOC_LITERAL(1, 30, 18), // "TopicFloatsChanged"
QT_MOC_LITERAL(2, 49, 0), // ""
QT_MOC_LITERAL(3, 50, 18), // "FloatValuesChanged"
QT_MOC_LITERAL(4, 69, 16), // "updateFloatValue"
QT_MOC_LITERAL(5, 86, 5), // "index"
QT_MOC_LITERAL(6, 92, 3), // "val"
QT_MOC_LITERAL(7, 96, 13), // "publishFloats"
QT_MOC_LITERAL(8, 110, 11), // "topicFloats"
QT_MOC_LITERAL(9, 122, 11) // "floatValues"

    },
    "sensor_gui::SensorParamPlugin\0"
    "TopicFloatsChanged\0\0FloatValuesChanged\0"
    "updateFloatValue\0index\0val\0publishFloats\0"
    "topicFloats\0floatValues"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_sensor_gui__SensorParamPlugin[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       2,   42, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x06 /* Public */,
       3,    0,   35,    2, 0x06 /* Public */,

 // methods: name, argc, parameters, tag, flags
       4,    2,   36,    2, 0x02 /* Public */,
       7,    0,   41,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Double,    5,    6,
    QMetaType::Void,

 // properties: name, type, flags
       8, QMetaType::QString, 0x00495003,
       9, QMetaType::QVariantList, 0x00495003,

 // properties: notify_signal_id
       0,
       1,

       0        // eod
};

void sensor_gui::SensorParamPlugin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SensorParamPlugin *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->TopicFloatsChanged(); break;
        case 1: _t->FloatValuesChanged(); break;
        case 2: _t->updateFloatValue((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 3: _t->publishFloats(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (SensorParamPlugin::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SensorParamPlugin::TopicFloatsChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (SensorParamPlugin::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&SensorParamPlugin::FloatValuesChanged)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<SensorParamPlugin *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->TopicFloats(); break;
        case 1: *reinterpret_cast< QVariantList*>(_v) = _t->FloatValues(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<SensorParamPlugin *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->SetTopicFloats(*reinterpret_cast< QString*>(_v)); break;
        case 1: _t->SetFloatValues(*reinterpret_cast< QVariantList*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject sensor_gui::SensorParamPlugin::staticMetaObject = { {
    QMetaObject::SuperData::link<gz::gui::Plugin::staticMetaObject>(),
    qt_meta_stringdata_sensor_gui__SensorParamPlugin.data,
    qt_meta_data_sensor_gui__SensorParamPlugin,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *sensor_gui::SensorParamPlugin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *sensor_gui::SensorParamPlugin::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_sensor_gui__SensorParamPlugin.stringdata0))
        return static_cast<void*>(this);
    return gz::gui::Plugin::qt_metacast(_clname);
}

int sensor_gui::SensorParamPlugin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = gz::gui::Plugin::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void sensor_gui::SensorParamPlugin::TopicFloatsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void sensor_gui::SensorParamPlugin::FloatValuesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

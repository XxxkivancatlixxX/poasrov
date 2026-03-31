/****************************************************************************
** Meta object code from reading C++ file 'Backend.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "Backend.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Backend.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH/****************************************************************************
** Meta object code from reading C++ file 'Backend.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Backend_t {
    QByteArrayData data[34];
    char stringdata0[392];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Backend_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Backend_t qt_meta_stringdata_Backend = {
    {
QT_MOC_LITERAL(0, 0, 7), // "Backend"
QT_MOC_LITERAL(1, 8, 23), // "connectionStatusChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 19), // "mavlinkReadyChanged"
QT_MOC_LITERAL(4, 53, 12), // "armedChanged"
QT_MOC_LITERAL(5, 66, 16), // "telemetryChanged"
QT_MOC_LITERAL(6, 83, 10), // "logMessage"
QT_MOC_LITERAL(7, 94, 3), // "msg"
QT_MOC_LITERAL(8, 98, 10), // "connectTcp"
QT_MOC_LITERAL(9, 109, 4), // "host"
QT_MOC_LITERAL(10, 114, 4), // "port"
QT_MOC_LITERAL(11, 119, 10), // "connectUdp"
QT_MOC_LITERAL(12, 130, 14), // "disconnectLink"
QT_MOC_LITERAL(13, 145, 8), // "setArmed"
QT_MOC_LITERAL(14, 154, 5), // "armed"
QT_MOC_LITERAL(15, 160, 12), // "setMotorTest"
QT_MOC_LITERAL(16, 173, 10), // "motorIndex"
QT_MOC_LITERAL(17, 184, 8), // "throttle"
QT_MOC_LITERAL(18, 193, 14), // "setAllThrottle"
QT_MOC_LITERAL(19, 208, 18), // "setJoystickEnabled"
QT_MOC_LITERAL(20, 227, 7), // "enabled"
QT_MOC_LITERAL(21, 235, 22), // "setJoystickMaxThrottle"
QT_MOC_LITERAL(22, 258, 3), // "max"
QT_MOC_LITERAL(23, 262, 19), // "isJoystickConnected"
QT_MOC_LITERAL(24, 282, 13), // "pollTelemetry"
QT_MOC_LITERAL(25, 296, 14), // "updateJoystick"
QT_MOC_LITERAL(26, 311, 16), // "connectionStatus"
QT_MOC_LITERAL(27, 328, 12), // "mavlinkReady"
QT_MOC_LITERAL(28, 341, 14), // "batteryVoltage"
QT_MOC_LITERAL(29, 356, 14), // "batteryPercent"
QT_MOC_LITERAL(30, 371, 5), // "depth"
QT_MOC_LITERAL(31, 377, 4), // "roll"
QT_MOC_LITERAL(32, 382, 5), // "pitch"
QT_MOC_LITERAL(33, 388, 3) // "yaw"

    },
    "Backend\0connectionStatusChanged\0\0"
    "mavlinkReadyChanged\0armedChanged\0"
    "telemetryChanged\0logMessage\0msg\0"
    "connectTcp\0host\0port\0connectUdp\0"
    "disconnectLink\0setArmed\0armed\0"
    "setMotorTest\0motorIndex\0throttle\0"
    "setAllThrottle\0setJoystickEnabled\0"
    "enabled\0setJoystickMaxThrottle\0max\0"
    "isJoystickConnected\0pollTelemetry\0"
    "updateJoystick\0connectionStatus\0"
    "mavlinkReady\0batteryVoltage\0batteryPercent\0"
    "depth\0roll\0pitch\0yaw"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Backend[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       9,  132, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x06 /* Public */,
       3,    0,   95,    2, 0x06 /* Public */,
       4,    0,   96,    2, 0x06 /* Public */,
       5,    0,   97,    2, 0x06 /* Public */,
       6,    1,   98,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       8,    2,  101,    2, 0x0a /* Public */,
      11,    2,  106,    2, 0x0a /* Public */,
      12,    0,  111,    2, 0x0a /* Public */,
      13,    1,  112,    2, 0x0a /* Public */,
      15,    2,  115,    2, 0x0a /* Public */,
      18,    1,  120,    2, 0x0a /* Public */,
      19,    1,  123,    2, 0x0a /* Public */,
      21,    1,  126,    2, 0x0a /* Public */,
      23,    0,  129,    2, 0x0a /* Public */,
      24,    0,  130,    2, 0x08 /* Private */,
      25,    0,  131,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    7,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    9,   10,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,    9,   10,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   14,
    QMetaType::Void, QMetaType::Int, QMetaType::QReal,   16,   17,
    QMetaType::Void, QMetaType::QReal,   17,
    QMetaType::Void, QMetaType::Bool,   20,
    QMetaType::Void, QMetaType::QReal,   22,
    QMetaType::Bool,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      26, QMetaType::QString, 0x00495001,
      27, QMetaType::Bool, 0x00495001,
      14, QMetaType::Bool, 0x00495001,
      28, QMetaType::Float, 0x00495001,
      29, QMetaType::Int, 0x00495001,
      30, QMetaType::Float, 0x00495001,
      31, QMetaType::Float, 0x00495001,
      32, QMetaType::Float, 0x00495001,
      33, QMetaType::Float, 0x00495001,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       3,
       3,
       3,
       3,
       3,

       0        // eod
};

void Backend::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Backend *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->connectionStatusChanged(); break;
        case 1: _t->mavlinkReadyChanged(); break;
        case 2: _t->armedChanged(); break;
        case 3: _t->telemetryChanged(); break;
        case 4: _t->logMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->connectTcp((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->connectUdp((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: _t->disconnectLink(); break;
        case 8: _t->setArmed((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->setMotorTest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 10: _t->setAllThrottle((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 11: _t->setJoystickEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->setJoystickMaxThrottle((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 13: { bool _r = _t->isJoystickConnected();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 14: _t->pollTelemetry(); break;
        case 15: _t->updateJoystick(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Backend::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Backend::connectionStatusChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Backend::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Backend::mavlinkReadyChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Backend::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Backend::armedChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Backend::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Backend::telemetryChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Backend::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Backend::logMessage)) {
                *result = 4;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<Backend *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->connectionStatus(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->mavlinkReady(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->armed(); break;
        case 3: *reinterpret_cast< float*>(_v) = _t->batteryVoltage(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->batteryPercent(); break;
        case 5: *reinterpret_cast< float*>(_v) = _t->depth(); break;
        case 6: *reinterpret_cast< float*>(_v) = _t->roll(); break;
        case 7: *reinterpret_cast< float*>(_v) = _t->pitch(); break;
        case 8: *reinterpret_cast< float*>(_v) = _t->yaw(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject Backend::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Backend.data,
    qt_meta_data_Backend,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Backend::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Backend::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Backend.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Backend::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 9;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 9;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void Backend::connectionStatusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Backend::mavlinkReadyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Backend::armedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Backend::telemetryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Backend::logMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

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
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Backend_t {
    QByteArrayData data[53];
    char stringdata0[714];
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
QT_MOC_LITERAL(5, 66, 23), // "armingInProgressChanged"
QT_MOC_LITERAL(6, 90, 24), // "motorTestCooldownChanged"
QT_MOC_LITERAL(7, 115, 16), // "telemetryChanged"
QT_MOC_LITERAL(8, 132, 10), // "logMessage"
QT_MOC_LITERAL(9, 143, 3), // "msg"
QT_MOC_LITERAL(10, 147, 16), // "cameraUrlChanged"
QT_MOC_LITERAL(11, 164, 22), // "cameraConnectedChanged"
QT_MOC_LITERAL(12, 187, 18), // "cameraFrameChanged"
QT_MOC_LITERAL(13, 206, 10), // "connectTcp"
QT_MOC_LITERAL(14, 217, 4), // "host"
QT_MOC_LITERAL(15, 222, 4), // "port"
QT_MOC_LITERAL(16, 227, 10), // "connectUdp"
QT_MOC_LITERAL(17, 238, 14), // "disconnectLink"
QT_MOC_LITERAL(18, 253, 10), // "armVehicle"
QT_MOC_LITERAL(19, 264, 13), // "disarmVehicle"
QT_MOC_LITERAL(20, 278, 15), // "forceArmVehicle"
QT_MOC_LITERAL(21, 294, 12), // "setMotorTest"
QT_MOC_LITERAL(22, 307, 10), // "motorIndex"
QT_MOC_LITERAL(23, 318, 8), // "throttle"
QT_MOC_LITERAL(24, 327, 14), // "setAllThrottle"
QT_MOC_LITERAL(25, 342, 18), // "setJoystickEnabled"
QT_MOC_LITERAL(26, 361, 7), // "enabled"
QT_MOC_LITERAL(27, 369, 22), // "setJoystickMaxThrottle"
QT_MOC_LITERAL(28, 392, 3), // "max"
QT_MOC_LITERAL(29, 396, 19), // "isJoystickConnected"
QT_MOC_LITERAL(30, 416, 12), // "setCameraUrl"
QT_MOC_LITERAL(31, 429, 3), // "url"
QT_MOC_LITERAL(32, 433, 13), // "connectCamera"
QT_MOC_LITERAL(33, 447, 16), // "disconnectCamera"
QT_MOC_LITERAL(34, 464, 13), // "pollTelemetry"
QT_MOC_LITERAL(35, 478, 14), // "updateJoystick"
QT_MOC_LITERAL(36, 493, 15), // "sendRCHeartbeat"
QT_MOC_LITERAL(37, 509, 21), // "updateCooldownDisplay"
QT_MOC_LITERAL(38, 531, 16), // "connectionStatus"
QT_MOC_LITERAL(39, 548, 12), // "mavlinkReady"
QT_MOC_LITERAL(40, 561, 5), // "armed"
QT_MOC_LITERAL(41, 567, 16), // "armingInProgress"
QT_MOC_LITERAL(42, 584, 19), // "motorTestCooldownMs"
QT_MOC_LITERAL(43, 604, 20), // "motorTestCoolingDown"
QT_MOC_LITERAL(44, 625, 14), // "batteryVoltage"
QT_MOC_LITERAL(45, 640, 14), // "batteryPercent"
QT_MOC_LITERAL(46, 655, 5), // "depth"
QT_MOC_LITERAL(47, 661, 4), // "roll"
QT_MOC_LITERAL(48, 666, 5), // "pitch"
QT_MOC_LITERAL(49, 672, 3), // "yaw"
QT_MOC_LITERAL(50, 676, 9), // "cameraUrl"
QT_MOC_LITERAL(51, 686, 15), // "cameraConnected"
QT_MOC_LITERAL(52, 702, 11) // "cameraFrame"

    },
    "Backend\0connectionStatusChanged\0\0"
    "mavlinkReadyChanged\0armedChanged\0"
    "armingInProgressChanged\0"
    "motorTestCooldownChanged\0telemetryChanged\0"
    "logMessage\0msg\0cameraUrlChanged\0"
    "cameraConnectedChanged\0cameraFrameChanged\0"
    "connectTcp\0host\0port\0connectUdp\0"
    "disconnectLink\0armVehicle\0disarmVehicle\0"
    "forceArmVehicle\0setMotorTest\0motorIndex\0"
    "throttle\0setAllThrottle\0setJoystickEnabled\0"
    "enabled\0setJoystickMaxThrottle\0max\0"
    "isJoystickConnected\0setCameraUrl\0url\0"
    "connectCamera\0disconnectCamera\0"
    "pollTelemetry\0updateJoystick\0"
    "sendRCHeartbeat\0updateCooldownDisplay\0"
    "connectionStatus\0mavlinkReady\0armed\0"
    "armingInProgress\0motorTestCooldownMs\0"
    "motorTestCoolingDown\0batteryVoltage\0"
    "batteryPercent\0depth\0roll\0pitch\0yaw\0"
    "cameraUrl\0cameraConnected\0cameraFrame"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Backend[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   14, // methods
      15,  204, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  154,    2, 0x06 /* Public */,
       3,    0,  155,    2, 0x06 /* Public */,
       4,    0,  156,    2, 0x06 /* Public */,
       5,    0,  157,    2, 0x06 /* Public */,
       6,    0,  158,    2, 0x06 /* Public */,
       7,    0,  159,    2, 0x06 /* Public */,
       8,    1,  160,    2, 0x06 /* Public */,
      10,    0,  163,    2, 0x06 /* Public */,
      11,    0,  164,    2, 0x06 /* Public */,
      12,    0,  165,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    2,  166,    2, 0x0a /* Public */,
      16,    2,  171,    2, 0x0a /* Public */,
      17,    0,  176,    2, 0x0a /* Public */,
      18,    0,  177,    2, 0x0a /* Public */,
      19,    0,  178,    2, 0x0a /* Public */,
      20,    0,  179,    2, 0x0a /* Public */,
      21,    2,  180,    2, 0x0a /* Public */,
      24,    1,  185,    2, 0x0a /* Public */,
      25,    1,  188,    2, 0x0a /* Public */,
      27,    1,  191,    2, 0x0a /* Public */,
      29,    0,  194,    2, 0x0a /* Public */,
      30,    1,  195,    2, 0x0a /* Public */,
      32,    0,  198,    2, 0x0a /* Public */,
      33,    0,  199,    2, 0x0a /* Public */,
      34,    0,  200,    2, 0x08 /* Private */,
      35,    0,  201,    2, 0x08 /* Private */,
      36,    0,  202,    2, 0x08 /* Private */,
      37,    0,  203,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   14,   15,
    QMetaType::Void, QMetaType::QString, QMetaType::Int,   14,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::QReal,   22,   23,
    QMetaType::Void, QMetaType::QReal,   23,
    QMetaType::Void, QMetaType::Bool,   26,
    QMetaType::Void, QMetaType::QReal,   28,
    QMetaType::Bool,
    QMetaType::Void, QMetaType::QString,   31,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
      38, QMetaType::QString, 0x00495001,
      39, QMetaType::Bool, 0x00495001,
      40, QMetaType::Bool, 0x00495001,
      41, QMetaType::Bool, 0x00495001,
      42, QMetaType::Int, 0x00495001,
      43, QMetaType::Bool, 0x00495001,
      44, QMetaType::Float, 0x00495001,
      45, QMetaType::Int, 0x00495001,
      46, QMetaType::Float, 0x00495001,
      47, QMetaType::Float, 0x00495001,
      48, QMetaType::Float, 0x00495001,
      49, QMetaType::Float, 0x00495001,
      50, QMetaType::QString, 0x00495001,
      51, QMetaType::Bool, 0x00495001,
      52, QMetaType::QImage, 0x00495001,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       4,
       4,
       5,
       5,
       5,
       5,
       5,
       5,
       7,
       8,
       9,

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
        case 3: _t->armingInProgressChanged(); break;
        case 4: _t->motorTestCooldownChanged(); break;
        case 5: _t->telemetryChanged(); break;
        case 6: _t->logMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->cameraUrlChanged(); break;
        case 8: _t->cameraConnectedChanged(); break;
        case 9: _t->cameraFrameChanged(); break;
        case 10: _t->connectTcp((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: _t->connectUdp((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 12: _t->disconnectLink(); break;
        case 13: _t->armVehicle(); break;
        case 14: _t->disarmVehicle(); break;
        case 15: _t->forceArmVehicle(); break;
        case 16: _t->setMotorTest((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< qreal(*)>(_a[2]))); break;
        case 17: _t->setAllThrottle((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 18: _t->setJoystickEnabled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 19: _t->setJoystickMaxThrottle((*reinterpret_cast< qreal(*)>(_a[1]))); break;
        case 20: { bool _r = _t->isJoystickConnected();
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        case 21: _t->setCameraUrl((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 22: _t->connectCamera(); break;
        case 23: _t->disconnectCamera(); break;
        case 24: _t->pollTelemetry(); break;
        case 25: _t->updateJoystick(); break;
        case 26: _t->sendRCHeartbeat(); break;
        case 27: _t->updateCooldownDisplay(); break;
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
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Backend::armingInProgressChanged)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Backend::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Backend::motorTestCooldownChanged)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Backend::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Backend::telemetryChanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Backend::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Backend::logMessage)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (Backend::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Backend::cameraUrlChanged)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (Backend::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Backend::cameraConnectedChanged)) {
                *result = 8;
                return;
            }
        }
        {
            using _t = void (Backend::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Backend::cameraFrameChanged)) {
                *result = 9;
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
        case 3: *reinterpret_cast< bool*>(_v) = _t->armingInProgress(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->motorTestCooldownMs(); break;
        case 5: *reinterpret_cast< bool*>(_v) = _t->motorTestCoolingDown(); break;
        case 6: *reinterpret_cast< float*>(_v) = _t->batteryVoltage(); break;
        case 7: *reinterpret_cast< int*>(_v) = _t->batteryPercent(); break;
        case 8: *reinterpret_cast< float*>(_v) = _t->depth(); break;
        case 9: *reinterpret_cast< float*>(_v) = _t->roll(); break;
        case 10: *reinterpret_cast< float*>(_v) = _t->pitch(); break;
        case 11: *reinterpret_cast< float*>(_v) = _t->yaw(); break;
        case 12: *reinterpret_cast< QString*>(_v) = _t->cameraUrl(); break;
        case 13: *reinterpret_cast< bool*>(_v) = _t->cameraConnected(); break;
        case 14: *reinterpret_cast< QImage*>(_v) = _t->cameraFrame(); break;
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
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 28)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 28;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 15;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 15;
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
void Backend::armingInProgressChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Backend::motorTestCooldownChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void Backend::telemetryChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void Backend::logMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void Backend::cameraUrlChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void Backend::cameraConnectedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void Backend::cameraFrameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

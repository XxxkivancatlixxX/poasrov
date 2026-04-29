/****************************************************************************
** Meta object code from reading C++ file 'video_provider.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.18)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "video_provider.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'video_provider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.18. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_VideoProvider_t {
    QByteArrayData data[19];
    char stringdata0[226];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_VideoProvider_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_VideoProvider_t qt_meta_stringdata_VideoProvider = {
    {
QT_MOC_LITERAL(0, 0, 13), // "VideoProvider"
QT_MOC_LITERAL(1, 14, 12), // "frameChanged"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 17), // "connectionChanged"
QT_MOC_LITERAL(4, 46, 16), // "recordingChanged"
QT_MOC_LITERAL(5, 63, 13), // "errorOccurred"
QT_MOC_LITERAL(6, 77, 5), // "error"
QT_MOC_LITERAL(7, 83, 12), // "pictureSaved"
QT_MOC_LITERAL(8, 96, 4), // "path"
QT_MOC_LITERAL(9, 101, 14), // "recordingSaved"
QT_MOC_LITERAL(10, 116, 15), // "connectToCamera"
QT_MOC_LITERAL(11, 132, 7), // "rtspUrl"
QT_MOC_LITERAL(12, 140, 10), // "disconnect"
QT_MOC_LITERAL(13, 151, 14), // "startRecording"
QT_MOC_LITERAL(14, 166, 8), // "savePath"
QT_MOC_LITERAL(15, 175, 13), // "stopRecording"
QT_MOC_LITERAL(16, 189, 11), // "takePicture"
QT_MOC_LITERAL(17, 201, 12), // "currentFrame"
QT_MOC_LITERAL(18, 214, 11) // "isConnected"

    },
    "VideoProvider\0frameChanged\0\0"
    "connectionChanged\0recordingChanged\0"
    "errorOccurred\0error\0pictureSaved\0path\0"
    "recordingSaved\0connectToCamera\0rtspUrl\0"
    "disconnect\0startRecording\0savePath\0"
    "stopRecording\0takePicture\0currentFrame\0"
    "isConnected"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_VideoProvider[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       2,   92, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       6,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x06 /* Public */,
       3,    0,   70,    2, 0x06 /* Public */,
       4,    0,   71,    2, 0x06 /* Public */,
       5,    1,   72,    2, 0x06 /* Public */,
       7,    1,   75,    2, 0x06 /* Public */,
       9,    1,   78,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,   81,    2, 0x0a /* Public */,
      12,    0,   84,    2, 0x0a /* Public */,
      13,    1,   85,    2, 0x0a /* Public */,
      15,    0,   88,    2, 0x0a /* Public */,
      16,    1,   89,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void, QMetaType::QString,    8,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   14,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   14,

 // properties: name, type, flags
      17, QMetaType::QImage, 0x00495001,
      18, QMetaType::Bool, 0x00495001,

 // properties: notify_signal_id
       0,
       1,

       0        // eod
};

void VideoProvider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<VideoProvider *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->frameChanged(); break;
        case 1: _t->connectionChanged(); break;
        case 2: _t->recordingChanged(); break;
        case 3: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->pictureSaved((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: _t->recordingSaved((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->connectToCamera((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->disconnect(); break;
        case 8: _t->startRecording((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: _t->stopRecording(); break;
        case 10: _t->takePicture((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (VideoProvider::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VideoProvider::frameChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (VideoProvider::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VideoProvider::connectionChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (VideoProvider::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VideoProvider::recordingChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (VideoProvider::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VideoProvider::errorOccurred)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (VideoProvider::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VideoProvider::pictureSaved)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (VideoProvider::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&VideoProvider::recordingSaved)) {
                *result = 5;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<VideoProvider *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QImage*>(_v) = _t->currentFrame(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->isConnected(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject VideoProvider::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_VideoProvider.data,
    qt_meta_data_VideoProvider,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *VideoProvider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *VideoProvider::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_VideoProvider.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int VideoProvider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
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
void VideoProvider::frameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void VideoProvider::connectionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void VideoProvider::recordingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void VideoProvider::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void VideoProvider::pictureSaved(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void VideoProvider::recordingSaved(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

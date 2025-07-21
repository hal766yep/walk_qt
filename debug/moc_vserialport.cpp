/****************************************************************************
** Meta object code from reading C++ file 'vserialport.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../vSerialPortCfg/vserialport.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vserialport.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_vSerialPort_t {
    QByteArrayData data[14];
    char stringdata0[162];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_vSerialPort_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_vSerialPort_t qt_meta_stringdata_vSerialPort = {
    {
QT_MOC_LITERAL(0, 0, 11), // "vSerialPort"
QT_MOC_LITERAL(1, 12, 8), // "vTxError"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 11), // "vSerialOpen"
QT_MOC_LITERAL(4, 34, 5), // "bool&"
QT_MOC_LITERAL(5, 40, 6), // "isOpen"
QT_MOC_LITERAL(6, 47, 12), // "vSerialClose"
QT_MOC_LITERAL(7, 60, 10), // "vWriteData"
QT_MOC_LITERAL(8, 71, 3), // "str"
QT_MOC_LITERAL(9, 75, 15), // "vSerialComScanf"
QT_MOC_LITERAL(10, 91, 12), // "vSerialError"
QT_MOC_LITERAL(11, 104, 28), // "QSerialPort::SerialPortError"
QT_MOC_LITERAL(12, 133, 15), // "serialPortError"
QT_MOC_LITERAL(13, 149, 12) // "vClearCntNum"

    },
    "vSerialPort\0vTxError\0\0vSerialOpen\0"
    "bool&\0isOpen\0vSerialClose\0vWriteData\0"
    "str\0vSerialComScanf\0vSerialError\0"
    "QSerialPort::SerialPortError\0"
    "serialPortError\0vClearCntNum"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_vSerialPort[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   49,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       3,    1,   50,    2, 0x0a /* Public */,
       6,    0,   53,    2, 0x0a /* Public */,
       7,    1,   54,    2, 0x0a /* Public */,
       9,    0,   57,    2, 0x0a /* Public */,
      10,    1,   58,    2, 0x0a /* Public */,
      13,    0,   61,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 4,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QByteArray,    8,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void,

       0        // eod
};

void vSerialPort::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<vSerialPort *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->vTxError(); break;
        case 1: _t->vSerialOpen((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->vSerialClose(); break;
        case 3: _t->vWriteData((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 4: _t->vSerialComScanf(); break;
        case 5: _t->vSerialError((*reinterpret_cast< QSerialPort::SerialPortError(*)>(_a[1]))); break;
        case 6: _t->vClearCntNum(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (vSerialPort::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&vSerialPort::vTxError)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject vSerialPort::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_vSerialPort.data,
    qt_meta_data_vSerialPort,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *vSerialPort::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *vSerialPort::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_vSerialPort.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int vSerialPort::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void vSerialPort::vTxError()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}
struct qt_meta_stringdata_vSerialThread_t {
    QByteArrayData data[1];
    char stringdata0[14];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_vSerialThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_vSerialThread_t qt_meta_stringdata_vSerialThread = {
    {
QT_MOC_LITERAL(0, 0, 13) // "vSerialThread"

    },
    "vSerialThread"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_vSerialThread[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void vSerialThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject vSerialThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_vSerialThread.data,
    qt_meta_data_vSerialThread,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *vSerialThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *vSerialThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_vSerialThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int vSerialThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

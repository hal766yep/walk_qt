/****************************************************************************
** Meta object code from reading C++ file 'vheatmap.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.14.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../vHeatMap/vheatmap.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'vheatmap.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.14.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_vHeatMap_t {
    QByteArrayData data[15];
    char stringdata0[147];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_vHeatMap_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_vHeatMap_t qt_meta_stringdata_vHeatMap = {
    {
QT_MOC_LITERAL(0, 0, 8), // "vHeatMap"
QT_MOC_LITERAL(1, 9, 8), // "sendData"
QT_MOC_LITERAL(2, 18, 0), // ""
QT_MOC_LITERAL(3, 19, 20), // "std::vector<double>&"
QT_MOC_LITERAL(4, 40, 8), // "channels"
QT_MOC_LITERAL(5, 49, 12), // "vSerialRxIRQ"
QT_MOC_LITERAL(6, 62, 14), // "recvColorScale"
QT_MOC_LITERAL(7, 77, 5), // "value"
QT_MOC_LITERAL(8, 83, 7), // "savePng"
QT_MOC_LITERAL(9, 91, 19), // "InterpolationMethod"
QT_MOC_LITERAL(10, 111, 3), // "IDW"
QT_MOC_LITERAL(11, 115, 15), // "ModifiedShepard"
QT_MOC_LITERAL(12, 131, 3), // "RBF"
QT_MOC_LITERAL(13, 135, 7), // "Kriging"
QT_MOC_LITERAL(14, 143, 3) // "NNI"

    },
    "vHeatMap\0sendData\0\0std::vector<double>&\0"
    "channels\0vSerialRxIRQ\0recvColorScale\0"
    "value\0savePng\0InterpolationMethod\0IDW\0"
    "ModifiedShepard\0RBF\0Kriging\0NNI"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_vHeatMap[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       1,   42, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   34,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   37,    2, 0x0a /* Public */,
       6,    1,   38,    2, 0x0a /* Public */,
       8,    0,   41,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    7,
    QMetaType::Void,

 // enums: name, alias, flags, count, data
       9,    9, 0x0,    5,   47,

 // enum data: key, value
      10, uint(vHeatMap::IDW),
      11, uint(vHeatMap::ModifiedShepard),
      12, uint(vHeatMap::RBF),
      13, uint(vHeatMap::Kriging),
      14, uint(vHeatMap::NNI),

       0        // eod
};

void vHeatMap::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<vHeatMap *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendData((*reinterpret_cast< std::vector<double>(*)>(_a[1]))); break;
        case 1: _t->vSerialRxIRQ(); break;
        case 2: _t->recvColorScale((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->savePng(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (vHeatMap::*)(std::vector<double> & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&vHeatMap::sendData)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject vHeatMap::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_vHeatMap.data,
    qt_meta_data_vHeatMap,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *vHeatMap::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *vHeatMap::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_vHeatMap.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int vHeatMap::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
    return _id;
}

// SIGNAL 0
void vHeatMap::sendData(std::vector<double> & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
struct qt_meta_stringdata_vHeatMapThread_t {
    QByteArrayData data[1];
    char stringdata0[15];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_vHeatMapThread_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_vHeatMapThread_t qt_meta_stringdata_vHeatMapThread = {
    {
QT_MOC_LITERAL(0, 0, 14) // "vHeatMapThread"

    },
    "vHeatMapThread"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_vHeatMapThread[] = {

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

void vHeatMapThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

QT_INIT_METAOBJECT const QMetaObject vHeatMapThread::staticMetaObject = { {
    QMetaObject::SuperData::link<QThread::staticMetaObject>(),
    qt_meta_stringdata_vHeatMapThread.data,
    qt_meta_data_vHeatMapThread,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *vHeatMapThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *vHeatMapThread::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_vHeatMapThread.stringdata0))
        return static_cast<void*>(this);
    return QThread::qt_metacast(_clname);
}

int vHeatMapThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

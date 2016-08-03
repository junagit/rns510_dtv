/****************************************************************************
** Meta object code from reading C++ file 'QMostTvList.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QMostTvList.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMostTvList.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QCANrcTV[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      10,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   10,    9,    9, 0x05,
      40,   33,    9,    9, 0x05,
      61,   58,    9,    9, 0x05,
      82,   33,    9,    9, 0x05,
      99,   33,    9,    9, 0x05,
     125,  117,    9,    9, 0x05,
     157,    9,    9,    9, 0x05,
     168,    9,    9,    9, 0x05,
     193,  188,    9,    9, 0x05,
     215,   58,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     242,  237,    9,    9, 0x08,
     268,    9,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QCANrcTV[] = {
    "QCANrcTV\0\0state\0sigTvOnOff(bool)\0"
    "on_off\0epgFromMOST(bool)\0ch\0"
    "ChSwitchFromCAN(int)\0epgFromCAN(bool)\0"
    "scanFromCAN(bool)\0up_down\0"
    "ChSwitchFromCAN_next_prev(bool)\0"
    "sendList()\0startMediaMplayer()\0code\0"
    "sig_can_rc_code(char)\0ChSwitchFromMOST(int)\0"
    "data\0onCanRecieve(QByteArray*)\0"
    "DoChannelSwitch()\0"
};

void QCANrcTV::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QCANrcTV *_t = static_cast<QCANrcTV *>(_o);
        switch (_id) {
        case 0: _t->sigTvOnOff((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->epgFromMOST((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->ChSwitchFromCAN((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->epgFromCAN((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->scanFromCAN((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->ChSwitchFromCAN_next_prev((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->sendList(); break;
        case 7: _t->startMediaMplayer(); break;
        case 8: _t->sig_can_rc_code((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 9: _t->ChSwitchFromMOST((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->onCanRecieve((*reinterpret_cast< QByteArray*(*)>(_a[1]))); break;
        case 11: _t->DoChannelSwitch(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QCANrcTV::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QCANrcTV::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QCANrcTV,
      qt_meta_data_QCANrcTV, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QCANrcTV::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QCANrcTV::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QCANrcTV::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QCANrcTV))
        return static_cast<void*>(const_cast< QCANrcTV*>(this));
    return QObject::qt_metacast(_clname);
}

int QCANrcTV::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void QCANrcTV::sigTvOnOff(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QCANrcTV::epgFromMOST(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QCANrcTV::ChSwitchFromCAN(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QCANrcTV::epgFromCAN(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QCANrcTV::scanFromCAN(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QCANrcTV::ChSwitchFromCAN_next_prev(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void QCANrcTV::sendList()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void QCANrcTV::startMediaMplayer()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void QCANrcTV::sig_can_rc_code(char _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void QCANrcTV::ChSwitchFromMOST(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 9, _a);
}
QT_END_MOC_NAMESPACE

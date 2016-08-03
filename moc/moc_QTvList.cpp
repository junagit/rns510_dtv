/****************************************************************************
** Meta object code from reading C++ file 'QTvList.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QTvList.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QTvList.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QTvList[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,    9,    8,    8, 0x05,
      35,   32,    8,    8, 0x05,
      64,   57,    8,    8, 0x05,
      82,   57,    8,    8, 0x05,
     109,  101,    8,    8, 0x05,
     142,    8,    8,    8, 0x05,
     164,  162,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
     192,  185,    8,    8, 0x0a,
     213,    8,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QTvList[] = {
    "QTvList\0\0state\0sigTvOnOff(bool)\0ch\0"
    "ChSwitchFromMOST(int)\0on_off\0"
    "epgFromMOST(bool)\0scanFromMOST(bool)\0"
    "up_down\0ChSwitchFromMOST_next_prev(bool)\0"
    "startMediaMplayer()\0p\0winDestroy(QObject*)\0"
    "action\0doSigMenuAction(int)\0doTimer()\0"
};

void QTvList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QTvList *_t = static_cast<QTvList *>(_o);
        switch (_id) {
        case 0: _t->sigTvOnOff((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->ChSwitchFromMOST((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->epgFromMOST((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->scanFromMOST((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->ChSwitchFromMOST_next_prev((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->startMediaMplayer(); break;
        case 6: _t->winDestroy((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 7: _t->doSigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->doTimer(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QTvList::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QTvList::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QTvList,
      qt_meta_data_QTvList, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QTvList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QTvList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QTvList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QTvList))
        return static_cast<void*>(const_cast< QTvList*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QTvList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void QTvList::sigTvOnOff(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QTvList::ChSwitchFromMOST(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QTvList::epgFromMOST(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QTvList::scanFromMOST(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QTvList::ChSwitchFromMOST_next_prev(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QTvList::startMediaMplayer()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void QTvList::winDestroy(QObject * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'QMostMediaCtrlList.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QMostMediaCtrlList.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMostMediaCtrlList.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMostMediaCtrlList[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x05,
      28,   19,   19,   19, 0x05,
      38,   19,   19,   19, 0x05,
      49,   19,   19,   19, 0x05,
      59,   19,   19,   19, 0x05,
      69,   19,   19,   19, 0x05,
      79,   19,   19,   19, 0x05,
      94,   90,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
     121,  116,   19,   19, 0x08,
     150,  148,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QMostMediaCtrlList[] = {
    "QMostMediaCtrlList\0\0sigTV()\0sigPlay()\0"
    "sigPause()\0sigNext()\0sigPrev()\0sigView()\0"
    "sendList()\0evt\0sigMediaCtrlList(int)\0"
    "data\0onMOSTrecieve(QByteArray*)\0p\0"
    "onMediaCtrlItemCB(QMediaItem*)\0"
};

void QMostMediaCtrlList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QMostMediaCtrlList *_t = static_cast<QMostMediaCtrlList *>(_o);
        switch (_id) {
        case 0: _t->sigTV(); break;
        case 1: _t->sigPlay(); break;
        case 2: _t->sigPause(); break;
        case 3: _t->sigNext(); break;
        case 4: _t->sigPrev(); break;
        case 5: _t->sigView(); break;
        case 6: _t->sendList(); break;
        case 7: _t->sigMediaCtrlList((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->onMOSTrecieve((*reinterpret_cast< QByteArray*(*)>(_a[1]))); break;
        case 9: _t->onMediaCtrlItemCB((*reinterpret_cast< QMediaItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QMostMediaCtrlList::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QMostMediaCtrlList::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QMostMediaCtrlList,
      qt_meta_data_QMostMediaCtrlList, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QMostMediaCtrlList::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QMostMediaCtrlList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QMostMediaCtrlList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMostMediaCtrlList))
        return static_cast<void*>(const_cast< QMostMediaCtrlList*>(this));
    return QObject::qt_metacast(_clname);
}

int QMostMediaCtrlList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void QMostMediaCtrlList::sigTV()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QMostMediaCtrlList::sigPlay()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void QMostMediaCtrlList::sigPause()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QMostMediaCtrlList::sigNext()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void QMostMediaCtrlList::sigPrev()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void QMostMediaCtrlList::sigView()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void QMostMediaCtrlList::sendList()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void QMostMediaCtrlList::sigMediaCtrlList(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'QUpdateWin.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QUpdateWin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QUpdateWin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QUpdateWin[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   12,   11,   11, 0x05,
      44,   38,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      64,   12,   11,   11, 0x08,
      85,   38,   11,   11, 0x08,
     102,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QUpdateWin[] = {
    "QUpdateWin\0\0action\0sigMenuAction(int)\0"
    "value\0sigSetValue(qint64)\0"
    "doSigMenuAction(int)\0setValue(qint64)\0"
    "media_timerEvent()\0"
};

void QUpdateWin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QUpdateWin *_t = static_cast<QUpdateWin *>(_o);
        switch (_id) {
        case 0: _t->sigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->sigSetValue((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 2: _t->doSigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->setValue((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        case 4: _t->media_timerEvent(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QUpdateWin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QUpdateWin::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QUpdateWin,
      qt_meta_data_QUpdateWin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QUpdateWin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QUpdateWin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QUpdateWin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QUpdateWin))
        return static_cast<void*>(const_cast< QUpdateWin*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QUpdateWin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void QUpdateWin::sigMenuAction(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QUpdateWin::sigSetValue(qint64 _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE

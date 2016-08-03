/****************************************************************************
** Meta object code from reading C++ file 'QScreenScallingWin.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QScreenScallingWin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QScreenScallingWin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QScreenScallingWin[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   20,   19,   19, 0x05,

 // slots: signature, parameters, type, tag, flags
      46,   20,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QScreenScallingWin[] = {
    "QScreenScallingWin\0\0action\0"
    "sigMenuAction(int)\0doSigMenuAction(int)\0"
};

void QScreenScallingWin::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QScreenScallingWin *_t = static_cast<QScreenScallingWin *>(_o);
        switch (_id) {
        case 0: _t->sigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->doSigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QScreenScallingWin::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QScreenScallingWin::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QScreenScallingWin,
      qt_meta_data_QScreenScallingWin, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QScreenScallingWin::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QScreenScallingWin::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QScreenScallingWin::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QScreenScallingWin))
        return static_cast<void*>(const_cast< QScreenScallingWin*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QScreenScallingWin::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void QScreenScallingWin::sigMenuAction(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

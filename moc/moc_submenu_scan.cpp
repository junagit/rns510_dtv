/****************************************************************************
** Meta object code from reading C++ file 'submenu_scan.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/submenu_scan.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'submenu_scan.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_submenu_scan[] = {

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
      21,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      40,   14,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_submenu_scan[] = {
    "submenu_scan\0\0action\0sigMenuAction(int)\0"
    "doSigMenuAction(int)\0"
};

void submenu_scan::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        submenu_scan *_t = static_cast<submenu_scan *>(_o);
        switch (_id) {
        case 0: _t->sigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->doSigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData submenu_scan::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject submenu_scan::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_submenu_scan,
      qt_meta_data_submenu_scan, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &submenu_scan::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *submenu_scan::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *submenu_scan::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_submenu_scan))
        return static_cast<void*>(const_cast< submenu_scan*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int submenu_scan::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void submenu_scan::sigMenuAction(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

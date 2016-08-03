/****************************************************************************
** Meta object code from reading C++ file 'most_interface.h'
**
** Created: Wed Mar 20 09:50:03 2013
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/most_interface.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'most_interface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_most_interface[] = {

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
      19,   16,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      41,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_most_interface[] = {
    "most_interface\0\0ch\0ChSwitchFromMOST(int)\0"
    "most_uart_read_ready()\0"
};

void most_interface::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        most_interface *_t = static_cast<most_interface *>(_o);
        switch (_id) {
        case 0: _t->ChSwitchFromMOST((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->most_uart_read_ready(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData most_interface::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject most_interface::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_most_interface,
      qt_meta_data_most_interface, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &most_interface::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *most_interface::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *most_interface::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_most_interface))
        return static_cast<void*>(const_cast< most_interface*>(this));
    return QObject::qt_metacast(_clname);
}

int most_interface::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void most_interface::ChSwitchFromMOST(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

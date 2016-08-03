/****************************************************************************
** Meta object code from reading C++ file 'QMoreInfoWindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QMoreInfoWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMoreInfoWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMoreInfoWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,
      38,   35,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      67,   60,   16,   16, 0x08,
      93,   88,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QMoreInfoWindow[] = {
    "QMoreInfoWindow\0\0sigQuitFromMore()\0"
    "ch\0ChSwitchFromMOST(int)\0action\0"
    "doSigMenuAction(int)\0data\0"
    "onMOSTrecieve(QByteArray*)\0"
};

void QMoreInfoWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QMoreInfoWindow *_t = static_cast<QMoreInfoWindow *>(_o);
        switch (_id) {
        case 0: _t->sigQuitFromMore(); break;
        case 1: _t->ChSwitchFromMOST((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->doSigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->onMOSTrecieve((*reinterpret_cast< QByteArray*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QMoreInfoWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QMoreInfoWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QMoreInfoWindow,
      qt_meta_data_QMoreInfoWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QMoreInfoWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QMoreInfoWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QMoreInfoWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMoreInfoWindow))
        return static_cast<void*>(const_cast< QMoreInfoWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QMoreInfoWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void QMoreInfoWindow::sigQuitFromMore()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void QMoreInfoWindow::ChSwitchFromMOST(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE

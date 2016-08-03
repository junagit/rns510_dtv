/****************************************************************************
** Meta object code from reading C++ file 'main_menu.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/main_menu.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main_menu.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_main_menu[] = {

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
      18,   11,   10,   10, 0x05,
      37,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      57,   11,   10,   10, 0x0a,
      78,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_main_menu[] = {
    "main_menu\0\0action\0sigMenuAction(int)\0"
    "startMediaMplayer()\0doSigMenuAction(int)\0"
    "doConnect()\0"
};

void main_menu::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        main_menu *_t = static_cast<main_menu *>(_o);
        switch (_id) {
        case 0: _t->sigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->startMediaMplayer(); break;
        case 2: _t->doSigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->doConnect(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData main_menu::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject main_menu::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_main_menu,
      qt_meta_data_main_menu, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &main_menu::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *main_menu::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *main_menu::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_main_menu))
        return static_cast<void*>(const_cast< main_menu*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int main_menu::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void main_menu::sigMenuAction(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void main_menu::startMediaMplayer()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE

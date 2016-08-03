/****************************************************************************
** Meta object code from reading C++ file 'QMostFileBrowser.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QMostFileBrowser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMostFileBrowser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QFileBrowser[] = {

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
      23,   14,   13,   13, 0x05,
      58,   54,   13,   13, 0x05,
      87,   80,   13,   13, 0x05,
     111,   80,   13,   13, 0x05,
     139,  135,   13,   13, 0x05,
     161,   13,   13,   13, 0x05,
     180,  175,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     202,   80,   13,   13, 0x08,
     225,   80,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QFileBrowser[] = {
    "QFileBrowser\0\0file,dir\0"
    "file_selected(QString,QString)\0dir\0"
    "dir_selected(QString)\0src_id\0"
    "sigMediaRemove(uint8_t)\0sigMediaInsert(uint8_t)\0"
    "evt\0sigMediaCtrlList(int)\0return2ctrl()\0"
    "code\0sig_can_rc_code(char)\0"
    "onMediaRemove(uint8_t)\0onMediaInsert(uint8_t)\0"
};

void QFileBrowser::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QFileBrowser *_t = static_cast<QFileBrowser *>(_o);
        switch (_id) {
        case 0: _t->file_selected((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 1: _t->dir_selected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->sigMediaRemove((*reinterpret_cast< uint8_t(*)>(_a[1]))); break;
        case 3: _t->sigMediaInsert((*reinterpret_cast< uint8_t(*)>(_a[1]))); break;
        case 4: _t->sigMediaCtrlList((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->return2ctrl(); break;
        case 6: _t->sig_can_rc_code((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 7: _t->onMediaRemove((*reinterpret_cast< uint8_t(*)>(_a[1]))); break;
        case 8: _t->onMediaInsert((*reinterpret_cast< uint8_t(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QFileBrowser::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QFileBrowser::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QFileBrowser,
      qt_meta_data_QFileBrowser, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QFileBrowser::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QFileBrowser::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QFileBrowser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QFileBrowser))
        return static_cast<void*>(const_cast< QFileBrowser*>(this));
    return QObject::qt_metacast(_clname);
}

int QFileBrowser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
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
void QFileBrowser::file_selected(const QString & _t1, const QString & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QFileBrowser::dir_selected(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QFileBrowser::sigMediaRemove(uint8_t _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void QFileBrowser::sigMediaInsert(uint8_t _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void QFileBrowser::sigMediaCtrlList(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void QFileBrowser::return2ctrl()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void QFileBrowser::sig_can_rc_code(char _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}
QT_END_MOC_NAMESPACE

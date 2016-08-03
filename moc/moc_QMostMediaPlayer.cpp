/****************************************************************************
** Meta object code from reading C++ file 'QMostMediaPlayer.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QMostMediaPlayer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QMostMediaPlayer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMediaPlayerMain[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      25,   18,   17,   17, 0x05,
      50,   44,   17,   17, 0x05,
      67,   17,   17,   17, 0x05,
      84,   81,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     101,   18,   17,   17, 0x08,
     131,  122,   17,   17, 0x08,
     169,  165,   17,   17, 0x08,
     194,   17,   17,   17, 0x08,
     212,   17,   17,   17, 0x08,
     227,  220,   17,   17, 0x08,
     250,  220,   17,   17, 0x08,
     273,   17,   17,   17, 0x08,
     283,   17,   17,   17, 0x08,
     293,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QMediaPlayerMain[] = {
    "QMediaPlayerMain\0\0action\0sigMenuAction(int)\0"
    "state\0sigTvOnOff(bool)\0return2ctrl()\0"
    "en\0menuEnable(bool)\0doSigMenuAction(int)\0"
    "file,dir\0on_file_selected(QString,QString)\0"
    "dir\0on_dir_selected(QString)\0"
    "onMediaPrepared()\0onEOS()\0src_id\0"
    "onMediaRemove(uint8_t)\0onMediaInsert(uint8_t)\0"
    "doPause()\0doDelay()\0doBackToCurrentTrack()\0"
};

void QMediaPlayerMain::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QMediaPlayerMain *_t = static_cast<QMediaPlayerMain *>(_o);
        switch (_id) {
        case 0: _t->sigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->sigTvOnOff((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->return2ctrl(); break;
        case 3: _t->menuEnable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->doSigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_file_selected((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 6: _t->on_dir_selected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: _t->onMediaPrepared(); break;
        case 8: _t->onEOS(); break;
        case 9: _t->onMediaRemove((*reinterpret_cast< uint8_t(*)>(_a[1]))); break;
        case 10: _t->onMediaInsert((*reinterpret_cast< uint8_t(*)>(_a[1]))); break;
        case 11: _t->doPause(); break;
        case 12: _t->doDelay(); break;
        case 13: _t->doBackToCurrentTrack(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QMediaPlayerMain::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QMediaPlayerMain::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_QMediaPlayerMain,
      qt_meta_data_QMediaPlayerMain, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QMediaPlayerMain::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QMediaPlayerMain::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QMediaPlayerMain::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMediaPlayerMain))
        return static_cast<void*>(const_cast< QMediaPlayerMain*>(this));
    return QObject::qt_metacast(_clname);
}

int QMediaPlayerMain::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void QMediaPlayerMain::sigMenuAction(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QMediaPlayerMain::sigTvOnOff(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QMediaPlayerMain::return2ctrl()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QMediaPlayerMain::menuEnable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE

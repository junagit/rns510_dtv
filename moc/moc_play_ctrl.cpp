/****************************************************************************
** Meta object code from reading C++ file 'play_ctrl.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/play_ctrl.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'play_ctrl.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_play_ctrl[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      29,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
      11,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   11,   10,   10, 0x05,
      48,   37,   10,   10, 0x05,
      83,   80,   10,   10, 0x05,
     113,  105,   10,   10, 0x05,
     153,  146,   10,   10, 0x05,
     171,  146,   10,   10, 0x05,
     196,  190,   10,   10, 0x05,
     213,   10,   10,   10, 0x05,
     226,  224,   10,   10, 0x05,
     247,   10,   10,   10, 0x05,
     270,  267,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
     295,  287,   10,   10, 0x0a,
     320,  267,   10,   10, 0x08,
     339,  224,   10,   10, 0x08,
     362,   10,   10,   10, 0x08,
     382,   10,   10,   10, 0x08,
     398,   10,   10,   10, 0x08,
     416,  190,   10,   10, 0x08,
     435,   10,   10,   10, 0x08,
     457,   37,   10,   10, 0x08,
     491,   80,   10,   10, 0x08,
     520,   10,   10,   10, 0x08,
     530,   10,   10,   10, 0x08,
     553,   10,   10,   10, 0x08,
     572,   10,   10,   10, 0x08,
     596,   10,   10,   10, 0x08,
     618,  146,   10,   10, 0x08,
     638,  146,   10,   10, 0x08,
     659,  105,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_play_ctrl[] = {
    "play_ctrl\0\0action\0sigMenuAction(int)\0"
    "f,type,msg\0SigSystemInfo(bool,int,QString)\0"
    "ch\0ChSwitchFromMOST(int)\0up_down\0"
    "ChSwitchFromMOST_next_prev(bool)\0"
    "on_off\0epgFromMOST(bool)\0scanFromMOST(bool)\0"
    "state\0sigTvOnOff(bool)\0sendList()\0p\0"
    "winDestroy(QObject*)\0startMediaMplayer()\0"
    "en\0menuEnable(bool)\0rc_code\0"
    "do_sig_can_rc_code(char)\0domenuEnable(bool)\0"
    "doWinDestroy(QObject*)\0doSigQuitFromMore()\0"
    "procReadyRead()\0DoChannelSwitch()\0"
    "doSigTvOnOff(bool)\0doStartMediaMplayer()\0"
    "doSigSystemInfo(bool,int,QString)\0"
    "DoChannelSwitchFromMOST(int)\0DoDEBUG()\0"
    "doMainBarShowTimeOut()\0doSigHideMainBar()\0"
    "doSigHideVolumeWindow()\0doVolumeShowTimeOut()\0"
    "doEpgFromMOST(bool)\0doScanFromMOST(bool)\0"
    "doChSwitchFromMOST_next_prev(bool)\0"
};

void play_ctrl::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        play_ctrl *_t = static_cast<play_ctrl *>(_o);
        switch (_id) {
        case 0: _t->sigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->SigSystemInfo((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 2: _t->ChSwitchFromMOST((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->ChSwitchFromMOST_next_prev((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->epgFromMOST((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->scanFromMOST((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->sigTvOnOff((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->sendList(); break;
        case 8: _t->winDestroy((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 9: _t->startMediaMplayer(); break;
        case 10: _t->menuEnable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->do_sig_can_rc_code((*reinterpret_cast< char(*)>(_a[1]))); break;
        case 12: _t->domenuEnable((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->doWinDestroy((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 14: _t->doSigQuitFromMore(); break;
        case 15: _t->procReadyRead(); break;
        case 16: _t->DoChannelSwitch(); break;
        case 17: _t->doSigTvOnOff((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->doStartMediaMplayer(); break;
        case 19: _t->doSigSystemInfo((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 20: _t->DoChannelSwitchFromMOST((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->DoDEBUG(); break;
        case 22: _t->doMainBarShowTimeOut(); break;
        case 23: _t->doSigHideMainBar(); break;
        case 24: _t->doSigHideVolumeWindow(); break;
        case 25: _t->doVolumeShowTimeOut(); break;
        case 26: _t->doEpgFromMOST((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 27: _t->doScanFromMOST((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 28: _t->doChSwitchFromMOST_next_prev((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData play_ctrl::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject play_ctrl::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_play_ctrl,
      qt_meta_data_play_ctrl, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &play_ctrl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *play_ctrl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *play_ctrl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_play_ctrl))
        return static_cast<void*>(const_cast< play_ctrl*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int play_ctrl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    return _id;
}

// SIGNAL 0
void play_ctrl::sigMenuAction(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void play_ctrl::SigSystemInfo(bool _t1, int _t2, QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void play_ctrl::ChSwitchFromMOST(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void play_ctrl::ChSwitchFromMOST_next_prev(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void play_ctrl::epgFromMOST(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void play_ctrl::scanFromMOST(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void play_ctrl::sigTvOnOff(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void play_ctrl::sendList()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void play_ctrl::winDestroy(QObject * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}

// SIGNAL 9
void play_ctrl::startMediaMplayer()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void play_ctrl::menuEnable(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 10, _a);
}
QT_END_MOC_NAMESPACE

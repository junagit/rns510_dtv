/****************************************************************************
** Meta object code from reading C++ file 'QAutoScanWindow.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/QAutoScanWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'QAutoScanWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QAutoScanWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      24,   17,   16,   16, 0x05,
      47,   43,   16,   16, 0x05,
      67,   16,   16,   16, 0x05,
      95,   93,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
     132,   17,   16,   16, 0x08,
     153,   43,   16,   16, 0x08,
     175,   16,   16,   16, 0x08,
     200,   93,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QAutoScanWindow[] = {
    "QAutoScanWindow\0\0action\0sigMenuAction(int)\0"
    "val\0updateProgress(int)\0"
    "sigShowMessageAfterScan()\0l\0"
    "AddChannelsToList(dvb_service_list*)\0"
    "doSigMenuAction(int)\0doUpdateProgress(int)\0"
    "doShowMessageAfterScan()\0"
    "doAddChannelsToList(dvb_service_list*)\0"
};

void QAutoScanWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QAutoScanWindow *_t = static_cast<QAutoScanWindow *>(_o);
        switch (_id) {
        case 0: _t->sigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->updateProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->sigShowMessageAfterScan(); break;
        case 3: _t->AddChannelsToList((*reinterpret_cast< dvb_service_list*(*)>(_a[1]))); break;
        case 4: _t->doSigMenuAction((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->doUpdateProgress((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->doShowMessageAfterScan(); break;
        case 7: _t->doAddChannelsToList((*reinterpret_cast< dvb_service_list*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QAutoScanWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QAutoScanWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_QAutoScanWindow,
      qt_meta_data_QAutoScanWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QAutoScanWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QAutoScanWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QAutoScanWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QAutoScanWindow))
        return static_cast<void*>(const_cast< QAutoScanWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int QAutoScanWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void QAutoScanWindow::sigMenuAction(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void QAutoScanWindow::updateProgress(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void QAutoScanWindow::sigShowMessageAfterScan()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void QAutoScanWindow::AddChannelsToList(dvb_service_list * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE

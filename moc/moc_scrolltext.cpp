/****************************************************************************
** Meta object code from reading C++ file 'scrolltext.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/scrolltext.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'scrolltext.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ScrollText[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       2,   39, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   11,   12,   11, 0x0a,
      32,   27,   11,   11, 0x0a,
      49,   11,   12,   11, 0x0a,
      71,   61,   11,   11, 0x0a,
      93,   11,   11,   11, 0x08,

 // properties: name, type, flags
      27,   12, 0x0a095103,
      61,   12, 0x0a095103,

       0        // eod
};

static const char qt_meta_stringdata_ScrollText[] = {
    "ScrollText\0\0QString\0text()\0text\0"
    "setText(QString)\0separator()\0separator\0"
    "setSeparator(QString)\0timer_timeout()\0"
};

void ScrollText::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ScrollText *_t = static_cast<ScrollText *>(_o);
        switch (_id) {
        case 0: { QString _r = _t->text();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 1: _t->setText((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: { QString _r = _t->separator();
            if (_a[0]) *reinterpret_cast< QString*>(_a[0]) = _r; }  break;
        case 3: _t->setSeparator((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 4: _t->timer_timeout(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ScrollText::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ScrollText::staticMetaObject = {
    { &QLabel::staticMetaObject, qt_meta_stringdata_ScrollText,
      qt_meta_data_ScrollText, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ScrollText::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ScrollText::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ScrollText::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ScrollText))
        return static_cast<void*>(const_cast< ScrollText*>(this));
    return QLabel::qt_metacast(_clname);
}

int ScrollText::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QLabel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = text(); break;
        case 1: *reinterpret_cast< QString*>(_v) = separator(); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setText(*reinterpret_cast< QString*>(_v)); break;
        case 1: setSeparator(*reinterpret_cast< QString*>(_v)); break;
        }
        _id -= 2;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 2;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 2;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_END_MOC_NAMESPACE

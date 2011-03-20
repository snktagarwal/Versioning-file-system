/****************************************************************************
** Meta object code from reading C++ file 'pieview.h'
**
** Created: Sun Mar 20 21:44:16 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "pieview.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'pieview.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PieView[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      29,    9,    8,    8, 0x09,
      83,   66,    8,    8, 0x09,
     117,   66,    8,    8, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_PieView[] = {
    "PieView\0\0topLeft,bottomRight\0"
    "dataChanged(QModelIndex,QModelIndex)\0"
    "parent,start,end\0rowsInserted(QModelIndex,int,int)\0"
    "rowsAboutToBeRemoved(QModelIndex,int,int)\0"
};

const QMetaObject PieView::staticMetaObject = {
    { &QAbstractItemView::staticMetaObject, qt_meta_stringdata_PieView,
      qt_meta_data_PieView, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PieView::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PieView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PieView::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PieView))
        return static_cast<void*>(const_cast< PieView*>(this));
    return QAbstractItemView::qt_metacast(_clname);
}

int PieView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractItemView::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: dataChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 1: rowsInserted((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 2: rowsAboutToBeRemoved((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

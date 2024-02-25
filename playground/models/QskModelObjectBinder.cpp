//
// Created by aldo on 27/01/21.
//

#include "QskModelObjectBinder.h"
#include <QMetaProperty>

class QskModelObjectBinder::PrivateData
{
public:
    PrivateData( )
    {
    }

    struct Item {
        Item() :valid(false), column(-1) {};
        Item(QObject *obj, int column, const QByteArray& property)
         : valid (true), object( obj ), column( column ), property( property )
        {
        };
        bool valid;
        QObject* object;
        int column;
        QByteArray property;
    };

    void updateObjectProperty(Item & x)
    {
        Q_ASSERT( x.object != nullptr );
        Q_ASSERT( ! x.property.isEmpty() );
        x.object->setProperty(x.property,
              m->index(currentRowIndex.row(),x.column).data(Qt::EditRole));
    }

    void updateAllObjects()
    {
        for (auto it = items.begin(); it != items.end(); ++it) {
            updateObjectProperty(it.value());
        }
    }

    void updateModelField(Item& x)
    {
        Q_ASSERT( x.valid );
        Q_ASSERT( x.object != nullptr );
        auto prop =x.object->property( x.property );
        Q_ASSERT( prop.isValid() );

        m->setData(m->index(currentRowIndex.row(),x.column),prop);
    }

    void updateModelFields()
    {
        for (auto it = items.begin(); it != items.end(); ++it)
        {
            updateModelField( it.value() );
        }
    }

public:
    QMap<QObject*,Item> items;
    QAbstractItemModel* m;
    QPersistentModelIndex currentRowIndex;
};

QskModelObjectBinder::QskModelObjectBinder(QObject* parent)
: QObject( parent )
{
    m_data=std::make_unique<PrivateData>(  );
}

QskModelObjectBinder::QskModelObjectBinder(QAbstractItemModel *model, QObject* parent)
: QObject( parent )
{
    m_data=std::make_unique<PrivateData>();
    bindModel(model);
}


void QskModelObjectBinder::bindObject(QObject *obj, int column, const QByteArray &propertyName)
{
    Q_ASSERT( obj != nullptr );

    if(obj == nullptr)
        return;

    QMetaProperty prop;
    auto meta = obj->metaObject();
    if (propertyName.isEmpty())
    {
        prop = meta->userProperty();
    }else
    {
        auto idx = meta->indexOfProperty( propertyName );
        if(idx>=0)
        {
            prop=meta->property(idx);
        }
    }
    Q_ASSERT(! prop.isValid());

    PrivateData::Item item(obj,column,prop.name());
    m_data->items[obj]=item;
}

void QskModelObjectBinder::unbindObject(QObject *obj)
{
    m_data->items.remove(obj);
}

void QskModelObjectBinder::bindModel(QAbstractItemModel *model)
{
	m_data->m=model;
    m_data->currentRowIndex=QModelIndex();
    if(model) {
    	connect(m_data->m,&QAbstractItemModel::dataChanged,this,
			&QskModelObjectBinder::modelDataChanged);
    }
}

void QskModelObjectBinder::modelDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &)
{
    const int row = m_data->currentRowIndex.row();
    for (auto it = m_data->items.begin(); it != m_data->items.end(); ++it) {
        auto & x = it.value();
        const int col = x.column;
        if( qBound( topLeft.row(),row,bottomRight.row()) == row
            && qBound( topLeft.column(), col, bottomRight.column()) == col) {
			m_data->updateObjectProperty(x);
		}
	}
}

void QskModelObjectBinder::selectRow( int row )
{
    Q_ASSERT( m_data->m != nullptr );

    if ( !m_data->m )
        return;

    m_data->currentRowIndex = m_data->m->index( row, 0 );
    m_data->updateAllObjects();
}

int QskModelObjectBinder::currentRow() const
{
    return m_data->currentRowIndex.row();
}

void QskModelObjectBinder::updateModel()
{
    m_data->updateModelFields();
}

#include "moc_QskModelObjectBinder.cpp"

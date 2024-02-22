//
// Created by aldo on 27/01/21.
//

#include "QskDataControlMapper.h"

QskDataControlMapper::QskDataControlMapper() : m(nullptr){

}

void QskDataControlMapper::addMapping(QskControl *widget, int section) {
    if(widget)
    	addMapping(widget,section,widget->metaObject()->userProperty().name());
}

void QskDataControlMapper::addMapping(QskControl *widget, int section, const QByteArray &propertyName) {
    Item item;
    item.control=widget;
    item.section=section;
    item.property=propertyName;
    item.index=m->index(currentIndex.row(),section);
    items.append(item);
}

void QskDataControlMapper::setModel(QAbstractItemModel *model) {
	m=model;
	currentIndex=QModelIndex();
    if(m) {
    	connect(m,&QAbstractItemModel::dataChanged,this,
			&QskDataControlMapper::_dataChanged);
    }
}

bool QskDataControlMapper::qContainsIndex(const QModelIndex & idx, const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
	return idx.row() >= topLeft.row() && idx.row() <= bottomRight.row()
		&& idx.column() >= topLeft.column() && idx.column() <= bottomRight.column();
}

void QskDataControlMapper::_dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &)
{
	for (auto& x : items) {
		if (qContainsIndex(x.index, topLeft, bottomRight)) {
			populate(x);
		}
	}
}

void QskDataControlMapper::populate(QskDataControlMapper::Item & x)
{
	x.control->setProperty(x.property,
						   m->index(currentIndex.row(),x.section).data(Qt::EditRole));
}

void QskDataControlMapper::populate()
{
	for (auto& x : items) {
		populate(x);
	}
}

void QskDataControlMapper::submit() {
    if(!m)
        return;
	for(auto & x  : items) {
		m->setData(m->index(currentIndex.row(), x.section),
				   x.control->property(x.property));
	}
}

void QskDataControlMapper::setCurrentIndex(int index) {
    if(!m)
        return;
	currentIndex=m->index(index,0);
	populate();
}

#include "moc_QskDataControlMapper.cpp"

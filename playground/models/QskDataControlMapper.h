#ifndef QSKDATACONTROLMAPPER_H
#define QSKDATACONTROLMAPPER_H

#include <QskControl.h>
#include <QAbstractItemModel>

class QskDataControlMapper : public QObject {
	Q_OBJECT

private:

	struct Item;

public:

	QskDataControlMapper();

	void addMapping(QskControl* widget, int section);
	void addMapping(QskControl* widget, int section, const QByteArray &propertyName);

	void setModel(QAbstractItemModel* model);

	static bool qContainsIndex(const QModelIndex &idx, const QModelIndex &topLeft,
							   const QModelIndex &bottomRight);

	void _dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &);

	void populate(QskDataControlMapper::Item & x);

	void populate ();

	void submit();

	void setCurrentIndex(int index);

private:

	QList<Item> items;

	QAbstractItemModel* m;
	QPersistentModelIndex currentIndex;

	struct Item {
		QPointer<QskControl> control;
		int section;
		QByteArray property;
		QPersistentModelIndex index;
	};

};



#endif //QSKDATACONTROLMAPPER_H
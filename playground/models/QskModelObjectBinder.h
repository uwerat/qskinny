#ifndef QSKDATACONTROLMAPPER_H
#define QSKDATACONTROLMAPPER_H

#include <QskControl.h>
#include <QAbstractItemModel>

class QskModelObjectBinder : public QObject {
	Q_OBJECT

public:
	QskModelObjectBinder(QObject *parent=nullptr);
	QskModelObjectBinder(QAbstractItemModel* model, QObject *parent=nullptr);

	void bindObject(QObject*, int column, const QByteArray &propertyName="");
    void unbindObject(QObject*o);

	void bindModel(QAbstractItemModel* model);

    void selectRow(int row);
    [[nodiscard]] int currentRow() const;

    void updateModel();

private Q_SLOTS:
    void modelDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> &);

private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};



#endif //QSKDATACONTROLMAPPER_H
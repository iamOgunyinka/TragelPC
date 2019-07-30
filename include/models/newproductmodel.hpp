#ifndef NEWPRODUCTMODEL_H
#define NEWPRODUCTMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include "utils/resources.hpp"

class NewProductModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit NewProductModel(QObject *parent = nullptr);

    void AddItem( utilities::Product const & item );
    void RemoveItem( int const row );
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const
        override;
    bool insertRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count,
                    const QModelIndex &parent = QModelIndex()) override;
    QVector<utilities::Product>& GetProducts();
private:
    QVector<utilities::Product> products;
};

#endif // NEWPRODUCTMODEL_H

#ifndef ORDERINGITEMMODEL_H
#define ORDERINGITEMMODEL_H

#include <QAbstractTableModel>
#include <QMimeData>
#include <QDataStream>
#include <QVector>
#include <QInputDialog>
#include "utils/resources.hpp"

class OrderingItemModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit OrderingItemModel( QObject *parent = nullptr );
    QVector<utilities::MakeOrderItem> const & Items() const;
    Qt::ItemFlags flags( QModelIndex const & index ) const override;
    QVariant data( QModelIndex const &, int role = Qt::EditRole )const override;
    QVariant headerData( int, Qt::Orientation, int )const override;
    int rowCount( QModelIndex const & )const override;
    int columnCount( QModelIndex const & )const override;
    Qt::DropActions supportedDropActions() const override {
        return Qt::CopyAction;
    }
    bool canDropMimeData( QMimeData const *data, Qt::DropAction action, int row,
                         int column, QModelIndex const &parent ) const override;
    bool dropMimeData( QMimeData const *data, Qt::DropAction action, int row,
                       int column, QModelIndex const &parent ) override;
    bool removeRows( int row, int count,
                     QModelIndex const &parent = QModelIndex() ) override;
private:
    QVector<utilities::MakeOrderItem> items_;
};

#endif // ORDERINGITEMMODEL_H

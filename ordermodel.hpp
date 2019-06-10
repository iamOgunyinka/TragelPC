#ifndef ORDERMODEL_HPP
#define ORDERMODEL_HPP

#include <QAbstractItemModel>
#include <QVector>
#include "resources.hpp"

class OrderModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit OrderModel( QVector<utilities::OrderData> &&orders,
                         QObject *parent = nullptr );
    Qt::ItemFlags flags( QModelIndex const &index ) const override;
    QVariant data( QModelIndex const& index, int role) const override;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;
    int rowCount( QModelIndex const& parent) const override;
    int columnCount( QModelIndex const& parent) const override;
    QVector<utilities::OrderData::Item> const & ItemDataAt( int const row ) const;
    QString ReferenceIdAtIndex( int const row ) const;
signals:

public slots:
private:
    QVector<utilities::OrderData> orders_;
};

class OrderDetailModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit OrderDetailModel( QVector<utilities::OrderData::Item> const &order_items,
                         QObject *parent = nullptr );
    Qt::ItemFlags flags( QModelIndex const &index ) const override;
    QVariant data( QModelIndex const& index, int role) const override;
    QVariant headerData( int section, Qt::Orientation orientation, int role ) const override;
    int rowCount( QModelIndex const& parent) const override;
    int columnCount( QModelIndex const& parent) const override;
signals:

public slots:
private:
    QVector<utilities::OrderData::Item> const & order_items_;
};

#endif // ORDERMODEL_HPP

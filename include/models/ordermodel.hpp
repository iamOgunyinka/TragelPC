#ifndef ORDERMODEL_HPP
#define ORDERMODEL_HPP

#include <QAbstractItemModel>
#include <QVector>

namespace utilities
{
struct OrderResultData;
struct OrderingResultItem;
}

class OrderModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit OrderModel( QVector<utilities::OrderResultData> &orders,
                         QObject *parent = nullptr );
    Qt::ItemFlags flags( QModelIndex const &index ) const override;
    QVariant data( QModelIndex const& index, int role) const override;
    QVariant headerData( int section, Qt::Orientation orientation, int role )
            const override;
    int rowCount( QModelIndex const& parent) const override;
    int columnCount( QModelIndex const& parent) const override;
    bool setData( QModelIndex const &index, QVariant const &value,
                  int role = Qt::EditRole ) override;
    bool removeRows( int row, int count, QModelIndex const &parent ) override;
    QVector<utilities::OrderingResultItem> const & ItemDataAt( int const row )const;
signals:

public slots:
private:
    QVector<utilities::OrderResultData>& orders_;
};

class OrderDetailModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit OrderDetailModel( QVector<utilities::OrderingResultItem> const &order_items,
                         QObject *parent = nullptr );
    Qt::ItemFlags flags( QModelIndex const &index ) const override;
    QVariant data( QModelIndex const& index, int role) const override;
    QVariant headerData( int section, Qt::Orientation orientation, int role )
                        const override;
    int rowCount( QModelIndex const& parent) const override;
    int columnCount( QModelIndex const& parent) const override;
signals:

public slots:
private:
    QVector<utilities::OrderingResultItem> const & order_items_;
};

#endif // ORDERMODEL_HPP

#include <QJsonValue>
#include "models/orderingitemmodel.hpp"

OrderingItemModel::OrderingItemModel( QObject *parent )
    : QAbstractTableModel( parent )
{
}

QVector<utilities::MakeOrderItem> const& OrderingItemModel::Items() const
{
    return items_;
}

Qt::ItemFlags OrderingItemModel::flags( QModelIndex const & ) const
{
    return Qt::ItemIsDropEnabled | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant OrderingItemModel::headerData( int section,Qt::Orientation orientation,
                                        int role ) const
{
    if( role != Qt::DisplayRole ) return QVariant{};
    if( orientation == Qt::Horizontal ){
        switch( section ){
        case 0:
            return QString( "Product" );
        case 1:
            return QString( "Quantity" );
        default:
            return QVariant{};
        }
    }
    return section + 1;
}

int OrderingItemModel::rowCount( QModelIndex const & ) const
{
    return items_.size();
}

int OrderingItemModel::columnCount( QModelIndex const & ) const
{
    return 2;
}

QVariant OrderingItemModel::data( QModelIndex const &index, int role ) const
{
    utilities::MakeOrderItem const & row_data{ items_[index.row()] };
    if( role == Qt::DisplayRole ){
        switch( index.column() ){
        case 0:
            return row_data.product.name;
        case 1:
            return row_data.quantity;
        default:
            return QVariant{};
        }
    }
    return QVariant{};
}

bool OrderingItemModel::removeRows( int row, int count,
                                    QModelIndex const &index )
{
    beginRemoveRows( index, row, row );
    items_.remove( row, count );
    endRemoveRows();
    return true;
}

bool OrderingItemModel::canDropMimeData( QMimeData const *data,
                                         Qt::DropAction, int, int column,
                                         QModelIndex const & ) const
{
    if( !data->hasFormat( "application/tgl.text.list" ) ) return false;
    if( column > 0 ) return false;
    return true;
}

bool OrderingItemModel::dropMimeData( QMimeData const *data,
                                      Qt::DropAction action,
                                      int row, int column,
                                      QModelIndex const &parent )
{
    if( !canDropMimeData( data, action, row, column, parent ) ) return false;
    if( action == Qt::IgnoreAction ) return true;
    QByteArray encoded_data{ data->data( "application/tgl.text.list" ) };
    QDataStream stream{ &encoded_data, QIODevice::ReadOnly };
    QVector<utilities::MakeOrderItem> new_items{};
    while( !stream.atEnd() ){
        utilities::MakeOrderItem data_{};
        stream >> data_.product.name
                >> static_cast<qint64&>( data_.product.product_id )
                >> data_.product.price;
        data_.quantity = 1;
        new_items.append( data_ );
    }
    if( new_items.empty() ) return false;
    auto found = std::find_if( items_.cbegin(), items_.cend(),
                               [&]( utilities::MakeOrderItem const & item )
    {
            return item.product.product_id == new_items[0].product.product_id;
    });
    if( found != items_.cend() ) return false;
    bool ok = true;
    int const quantity {
        QInputDialog::getInt( nullptr, "Quantity", "What's the quantity?",
                          1, 1, 2147483647, 1, &ok )
    };
    if( !ok ) return false;
    new_items[0].quantity = quantity;

    int const begin_row { items_.size() };
    beginInsertRows( QModelIndex{}, begin_row, begin_row );
    items_.append( new_items[0] );
    endInsertRows();
    return true;
}

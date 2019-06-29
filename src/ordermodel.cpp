#include "ordermodel.hpp"

OrderModel::OrderModel( QVector<utilities::OrderData> &orders,
                        QObject *parent )
    : QAbstractTableModel( parent ), orders_{ orders }
{
}

Qt::ItemFlags OrderModel::flags( QModelIndex const & ) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool OrderModel::setData( QModelIndex const &index, QVariant const &value,
                         int role )
{
    if( role != Qt::EditRole ) return false;
    utilities::OrderData& order{ orders_[index.row()] };
    switch( index.column() )
    {
    case 0:
        order.order_date = value.toDateTime();
        emit dataChanged( QModelIndex(), QModelIndex() );
        return true;
    case 1:
        order.reference_id = value.toString();
        emit dataChanged( QModelIndex(), QModelIndex() );
        return true;
    case 2:
        order.staff_username = value.toString();
        emit dataChanged( QModelIndex(), QModelIndex() );
        return true;
    case 3:{
        int payment_type = value.toInt();
        order.payment_type = payment_type == utilities::Cash ?
                    utilities::Cash: utilities::E_Banking;
        emit dataChanged( QModelIndex(), QModelIndex() );
        return true;
    }
    case 4:
        order.confirmation_data.confirmed = value.toBool();
        emit dataChanged( QModelIndex(), QModelIndex() );
        return true;
    default:
        return false;
    }
}

QVariant OrderModel::data( QModelIndex const &index, int role ) const
{
    utilities::OrderData const & row_data{ orders_[index.row()] };
    if( role == Qt::DisplayRole ){
        switch( index.column() ){
        case 0:
            return row_data.order_date;
        case 1:
            return row_data.reference_id;
        case 2:
            return row_data.staff_username;
        case 3:
            return row_data.payment_type == utilities::Cash ?
                        QString( "Cash" ): QString( "E-Banking" );
        case 4:
            return row_data.confirmation_data.confirmed ? QString( "Confirmed" )
                                                        : tr( "Unconfirmed" );
        case 5:
            return QString( "%1 items" ).arg( row_data.items.length() );
        default:
            return QVariant{};
        }
    } else if( role == Qt::ToolTipRole ){
        return QString( "Right click to show details" );
    }
    return QVariant{};
}

int OrderModel::rowCount( QModelIndex const & ) const
{
    return orders_.size();
}

int OrderModel::columnCount( QModelIndex const &) const
{
    return 6;
}

bool OrderModel::removeRows( int row, int count, QModelIndex const & parent )
{
    beginRemoveRows( parent, row, row );
    orders_.remove( row, count );
    endRemoveRows();
    return true;
}

QVariant OrderModel::headerData( int section, Qt::Orientation orientation,
                                 int role) const
{
    if( role != Qt::DisplayRole ) return QVariant{};
    if( orientation == Qt::Horizontal ){
        switch( section ){
        case 0:
            return QString( "Date of purchase" );
        case 1:
            return QString( "Reference ID" );
        case 2:
            return QString( "Staff" );
        case 3:
            return QString( "Payment type" );
        case 4:
            return QString( "Confirmation" );
        case 5:
            return QString( "Items purchased" );
        default:
            return QVariant{};
        }
    }
    return section + 1;
}


OrderDetailModel::OrderDetailModel(
        QVector<utilities::OrderData::Item> const &order_items,
        QObject *parent ) : QAbstractTableModel( parent ),
    order_items_{ order_items }
{
}

Qt::ItemFlags OrderDetailModel::flags( QModelIndex const & ) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant OrderDetailModel::data( QModelIndex const &index, int role ) const
{
    if( role == Qt::DisplayRole ){
        int const row_number{ index.row() };
        utilities::OrderData::Item const & row_data{ order_items_[row_number] };
        switch( index.column() ){
        case 0:
            return row_data.product_name;
        case 1:
            return row_data.quantity;
        case 2:
            return row_data.price;
        case 3:
            return row_data.quantity * row_data.price;
        default:
            return QVariant{};
        }
    }
    return QVariant{};
}

int OrderDetailModel::rowCount( QModelIndex const & ) const
{
    return order_items_.size();
}

int OrderDetailModel::columnCount( QModelIndex const &) const
{
    return 4;
}

QVariant OrderDetailModel::headerData( int section, Qt::Orientation orientation,
                                       int role )const
{
    if( role != Qt::DisplayRole ) return QVariant{};
    if( orientation == Qt::Horizontal ){
        switch( section ){
        case 0:
            return QString( "Product" );
        case 1:
            return QString( "Quantity" );
        case 2:
            return QString( "Price" );
        case 3:
            return QString( "Total" );
        default:
            return QVariant{};
        }
    }
    return section + 1;
}

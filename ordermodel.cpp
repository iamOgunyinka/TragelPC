#include "ordermodel.hpp"

OrderModel::OrderModel( QVector<utilities::OrderData> &&orders, QObject *parent )
    : QAbstractTableModel( parent ), orders_{ std::move( orders ) }
{
}

Qt::ItemFlags OrderModel::flags( QModelIndex const & ) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
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
    return 4;
}

QVariant OrderModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            return QString( "Number of items purchased" );
        default:
            return QVariant{};
        }
    }
    return section + 1;
}

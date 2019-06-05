#include "ordermodel.hpp"

OrderModel::OrderModel( QVector<utilities::OrderData> &&orders, QObject *parent )
    : QAbstractTableModel( parent ), products_{ std::move( products ) }
{
}

Qt::ItemFlags OrderModel::flags( QModelIndex const & ) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant OrderModel::data( QModelIndex const &index, int role ) const
{
    utilities::OrderData const & row_data{ products_[index.row()] };
    if( role == Qt::DisplayRole ){
        switch( index.column() ){
        case 0:
            return row_data.name;
        case 1:
            return row_data.price;
        case 2:
            return row_data.thumbnail_location;
        default:
            return QVariant{};
        }
    } else if( role == Qt::ToolTipRole ){
        if( index.column() == 2 ){
            return QString( "Click to show full thumbnail(if available)" );
        }
    }
    return QVariant{};
}


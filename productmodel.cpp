#include "productmodel.hpp"
#include <QJsonObject>


ProductModel::ProductModel( QVector<utilities::ProductData> &&products, QObject *parent )
    : QAbstractTableModel( parent ), products_{ std::move( products ) }
{
}

Qt::ItemFlags ProductModel::flags( QModelIndex const & ) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ProductModel::data( QModelIndex const &index, int role ) const
{
    utilities::ProductData const & row_data{ products_[index.row()] };
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

QVariant ProductModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
    if( role != Qt::DisplayRole ) return QVariant{};
    if( orientation == Qt::Horizontal ){
        if( section == 0 ) return QString( "Product name" );
        else if( section == 1) return QString( "Product price" );
        else return QString( "Thumbnail" );
    }
    return section + 1;
}

int ProductModel::rowCount( QModelIndex const & ) const
{
    return products_.size();
}

int ProductModel::columnCount( QModelIndex const & ) const
{
    return 3;
}

utilities::ProductData& ProductModel::DataAtIndex( int const row )
{
    return products_[row];
}

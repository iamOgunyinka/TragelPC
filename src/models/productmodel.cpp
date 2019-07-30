#include <QJsonObject>
#include <QMimeData>
#include <QDataStream>
#include "models/productmodel.hpp"

ProductModel::ProductModel( QVector<utilities::Product>& products,
                            QObject *parent )
    : QAbstractTableModel( parent ), products_{ products }
{
}

Qt::ItemFlags ProductModel::flags( QModelIndex const & ) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ProductModel::data( QModelIndex const &index, int role ) const
{
    utilities::Product const & row_data{ products_[index.row()] };
    if( role == Qt::DisplayRole ){
        switch( index.column() ){
        case 0:
            return row_data.name;
        case 1:
            return row_data.price;
        case 2:
            return row_data.categories;
        case 3:
            if ( row_data.description.length() > 10 )
                return row_data.description.left( 10 );
            return row_data.description;
        case 4:
            return row_data.thumbnail_location;
        default:
            return QVariant{};
        }
    } else if( role == Qt::ToolTipRole ){
        if( index.column() == 4 ){
            return QString( "Click to show full thumbnail(if available)" );
        } else if( index.column() == 3 ){
            return row_data.description;
        }
    }
    return QVariant{};
}

QVariant ProductModel::headerData( int section, Qt::Orientation orientation,
                                   int role ) const
{
    if( role != Qt::DisplayRole ) return QVariant{};
    if( orientation == Qt::Horizontal ){
        switch( section ) {
        case 0: return QString( "Name" );
        case 1: return QString( "Price" );
        case 2: return QString( "Category" );
        case 3: return QString( "Description" );
        case 4: return QString( "Thumbnail" );
        }
    }
    return section + 1;
}

int ProductModel::rowCount( QModelIndex const & ) const
{
    return products_.size();
}

int ProductModel::columnCount( QModelIndex const & ) const
{
    return 5;
}

bool ProductModel::removeRows( int row, int count, QModelIndex const & parent )
{
    beginRemoveRows( parent, row, row );
    products_.remove( row, count );
    endRemoveRows();
    return true;
}


OrderingProductModel::OrderingProductModel(
        QVector<utilities::Product> &products, QObject *parent ):
    QAbstractTableModel{ parent }, products_{ products }
{
}

QStringList OrderingProductModel::mimeTypes() const
{
    return QStringList() << "application/tgl.text.list";
}

QMimeData* OrderingProductModel::mimeData( QModelIndexList const&indexes ) const
{
    QMimeData* mime_data{ new QMimeData };
    QByteArray encoded_data;
    QDataStream stream{ &encoded_data, QIODevice::WriteOnly };
    for( QModelIndex const & index: indexes ){
        if( index.isValid() ){
            utilities::Product const product = products_[index.row() ];
            stream << product.name << product.product_id << product.price;
        }
    }
    mime_data->setData( "application/tgl.text.list", encoded_data );
    return mime_data;
}

int OrderingProductModel::rowCount( QModelIndex const & ) const
{
    return products_.size();
}

int OrderingProductModel::columnCount( QModelIndex const & ) const
{
    return 2;
}

Qt::ItemFlags OrderingProductModel::flags( QModelIndex const &index ) const
{
    if( index.isValid() ){
        return Qt::ItemIsDragEnabled | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant OrderingProductModel::headerData( int section,
                                           Qt::Orientation orientation,
                                           int role ) const
{
    if( role != Qt::DisplayRole ) return QVariant{};
    if( orientation == Qt::Horizontal ){
        if( section == 0 ) return QString( "Product" );
        return QString( "Price" );
    }
    return section + 1;
}

QVariant OrderingProductModel::data( QModelIndex const &index, int role ) const
{
    utilities::Product const & row_data{ products_[index.row()] };
    if( role == Qt::DisplayRole ){
        switch( index.column() ){
        case 0:
            return row_data.name;
        case 1:
            return row_data.price;
        default:
            return QVariant{};
        }
    }
    return QVariant{};
}

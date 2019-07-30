#include "models/newproductmodel.hpp"
#include <QPixmapCache>

NewProductModel::NewProductModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

QVariant NewProductModel::headerData(int section, Qt::Orientation orientation,
                                     int role ) const
{
    if( role != Qt::DisplayRole ) return QVariant{};
    if( orientation == Qt::Horizontal ){
        switch (section){
        case 0: return "Name";
        case 1: return "Price";
        case 2: return "Category";
        case 3: return "Description";
        case 4: return "Preview";
        }
    }
    return section + 1;
}

int NewProductModel::rowCount( QModelIndex const & ) const
{
    return products.size();
}

int NewProductModel::columnCount( QModelIndex const &parent ) const
{
    if (parent.isValid())
        return 0;

    return 5;
}

QVariant NewProductModel::data( QModelIndex const &index, int role ) const
{
    if (!index.isValid())
        return QVariant();
    int const column = index.column();
    utilities::Product const & row_data{ products[index.row()] };
    if( role == Qt::DisplayRole ){
        switch( column ){
        case 0:
            return row_data.name;
        case 1:
            return row_data.price;
        case 2:
            return row_data.categories;
        case 3:
            if( row_data.description.length() > 10 )
                return row_data.description.left( 10 );
            return row_data.description;
        case 4:
            if( row_data.thumbnail_location.isEmpty() )
                return "No image used";
            return "Hover to show preview";
        default:
            return QVariant{};
        }
    } else if( role == Qt::ToolTipRole ){
        if( column == 3 ){
            return row_data.description;
        }
    } else if( role == Qt::SizeHintRole ){
        if( !row_data.thumbnail_location.isEmpty() )
            return QSize( 120, 180 );
    } else if( role == Qt::DecorationRole ){
        if( column == 4 && !row_data.thumbnail_location.isEmpty() ){
            QPixmap picture_thumbnail;
            // have we cached this image recently? If yes, use it.
            if( QPixmapCache::find( row_data.thumbnail_location,
                                    picture_thumbnail ) )
            {
                return picture_thumbnail.scaled( 120, 180 );
            } else {
                return QPixmap( row_data.thumbnail_location ).scaled( 120, 180 );
            }
        }
    }
    return QVariant{};
}

void NewProductModel::AddItem( utilities::Product const & product )
{
    products.append( product );
    insertRows( products.size() - 1, 1 );
}

void NewProductModel::RemoveItem( int const row )
{
    removeRows( row, 1 );
}

bool NewProductModel::insertRows( int row, int count, QModelIndex const &parent)
{
    beginInsertRows( parent, row, row + count - 1 );
    endInsertRows();
    return true;
}

bool NewProductModel::removeRows( int row, int count, QModelIndex const& parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    products.remove( row, count );
    endRemoveRows();
    return true;
}

QVector<utilities::Product>& NewProductModel::GetProducts()
{
    return products;
}

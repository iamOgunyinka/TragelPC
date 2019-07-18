#include "models/usermodel.hpp"

UserModel::UserModel( QVector<utilities::UserData> &users, QObject *parent )
    : QAbstractTableModel( parent ), users_{ users }
{
}

Qt::ItemFlags UserModel::flags( QModelIndex const & ) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant UserModel::data( QModelIndex const &index, int role ) const
{
    utilities::UserData const & row_data{ users_[index.row()] };
    if( role == Qt::DisplayRole ){
        switch( index.column() ){
        case 0:
        {
            // usernames usually come like so: username@company_id,
            // and sometimes `username` could be josh20@yahoo.com, so the
            // real username is everything but the last part separated by @
            int const last_index{ row_data.username.lastIndexOf( '@') };
            return row_data.username.left( last_index );
        }
        case 1:
            return row_data.fullname;
        case 2:
            return RoleToString( row_data.user_role );
        default:
            return QVariant{};
        }
    } else if( role == Qt::ToolTipRole ){
        return QString( "Right click to edit user details" );
    }
    return QVariant{};
}

int UserModel::rowCount( QModelIndex const & ) const
{
    return users_.size();
}

int UserModel::columnCount( QModelIndex const &) const
{
    return 3;
}

bool UserModel::removeRows( int row, int count, QModelIndex const &parent )
{
    beginRemoveRows( parent, row, row );
    users_.remove( row, count );
    endRemoveRows();
    return true;
}

bool UserModel::setData( QModelIndex const &index, QVariant const &value,
                         int role )
{
    if( role != Qt::EditRole ) return false;
    utilities::UserData& user_data{ users_[index.row()] };
    int const column{ index.column() };
    if( column == 0 ){
        user_data.username = value.toString();
    } else if( column == 1 ){
        user_data.fullname = value.toString();
    } else if( column == 2 ){
        user_data.user_role = value.toInt();
    } else {
        return false;
    }
    emit dataChanged( QModelIndex(), QModelIndex() );
    return true;
}

QVariant UserModel::headerData( int section, Qt::Orientation orientation,
                                int role ) const
{
    if( role != Qt::DisplayRole ) return QVariant{};
    if( orientation == Qt::Horizontal ){
        switch( section ){
        case 0:
            return QString( "Username" );
        case 1:
            return QString( "Fullname" );
        case 2:
            return QString( "Role" );
        default:
            return QVariant{};
        }
    }
    return section + 1;
}

QString UserModel::RoleToString( int const role ) const
{
    switch( role ){
    case UserRole::BasicUser:
        return "Basic User";
    case UserRole::Administrator:
        return "Administrator";
    default:
        return "Role unknown";
    }
}

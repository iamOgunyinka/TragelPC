#include "allsubscriptionmodel.hpp"

AllSubscriptionModel::AllSubscriptionModel( SubscriptionList const &list_,
                                            QObject *parent )
    : QAbstractTableModel(parent), subscriptions{ list_ }
{
}

QVariant AllSubscriptionModel::headerData( int section,
                                           Qt::Orientation orientation,
                                           int role ) const
{
    if( role != Qt::DisplayRole ) return QVariant{};
    if( orientation == Qt::Horizontal ){
        return section == 0 ? "Valid from": "To";
    }
    return section + 1;
}

int AllSubscriptionModel::rowCount( QModelIndex const & ) const
{
    return subscriptions.size();
}

int AllSubscriptionModel::columnCount( QModelIndex const & ) const
{
    return 2;
}

QVariant AllSubscriptionModel::data( QModelIndex const &index, int role ) const
{
    if( role == Qt::DisplayRole ){
        utilities::Subscription const& subscription{subscriptions[index.row()]};
        return index.column() == 0 ? subscription.from: subscription.to;
    }
    return QVariant{};
}

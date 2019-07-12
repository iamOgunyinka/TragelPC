#ifndef ALLSUBSCRIPTIONMODEL_H
#define ALLSUBSCRIPTIONMODEL_H

#include <QAbstractTableModel>
#include <QDate>

namespace utilities
{
    struct Subscription
    {
        QDate from;
        QDate to;
    };
    using SubscriptionList = QVector<Subscription>;
}

class AllSubscriptionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    using SubscriptionList = utilities::SubscriptionList;

    explicit AllSubscriptionModel( SubscriptionList const &,
                                   QObject *parent = nullptr );

    QVariant headerData( int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole ) const override;
    int rowCount( QModelIndex const & = QModelIndex() ) const override;
    int columnCount( QModelIndex const & = QModelIndex()) const override;
    QVariant data( QModelIndex const &index,
                   int role = Qt::DisplayRole ) const override;
private:
    SubscriptionList const &subscriptions;
};

#endif // ALLSUBSCRIPTIONMODEL_H

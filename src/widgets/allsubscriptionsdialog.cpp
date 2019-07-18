#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>

#include "widgets/allsubscriptionsdialog.hpp"
#include "models/allsubscriptionmodel.hpp"
#include "utils/resources.hpp"

#include "ui_allsubscriptionsdialog.h"

AllSubscriptionsDialog::AllSubscriptionsDialog(QWidget *parent) :
    QDialog(parent), ui( new Ui::AllSubscriptionsDialog )
{
    ui->setupUi(this);
}

AllSubscriptionsDialog::~AllSubscriptionsDialog()
{
    delete ui;
}

void AllSubscriptionsDialog::GetSubscriptions()
{
    QUrl const url{ utilities::Endpoint::GetEndpoints().GetSubscriptions() };
    auto on_success = [=]( QJsonObject const & result ){
        OnResultObtained( result );
    };
    utilities::SendNetworkRequest( url, on_success, []{}, this );
}

void AllSubscriptionsDialog::OnResultObtained( QJsonObject const & result )
{
    utilities::PageResultQuery page_query{};
    bool result_found{ utilities::ParsePageUrls( result, page_query )};
    if( !result_found ){
        QString title = result.value( "error" ).toString();
        QString message = result.value( "message" ).toString();
        if( title.isEmpty() ) title = "Subscription";
        if( message.isEmpty() ) message = "No result matching that criteria";
        QMessageBox::information( this, title, message );
        return;
    }
    QJsonArray subscription_list{ result.value( "subscriptions" ).toArray() };
    while( !subscription_list.isEmpty() ){
        QJsonValue const value{ subscription_list[0] };
        if( value.isArray() ) subscription_list = value.toArray();
        else break;
    }
    ParseSubscriptionResult( subscription_list );
    AllSubscriptionModel* sub_model{
        new AllSubscriptionModel( subscriptions_, ui->tableView )
    };
    ui->tableView->setVisible( false );
    QObject::connect( sub_model,
                      SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                      ui->tableView,
                      SLOT(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    ui->tableView->setModel( sub_model );
    ui->tableView->horizontalHeader()
            ->setSectionResizeMode( QHeaderView::Stretch );
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setVisible( true );
}

void AllSubscriptionsDialog::ParseSubscriptionResult( QJsonArray const &list )
{
    for( auto const value: list )
    {
        QJsonObject const sub_object{ value.toObject() };
        QString const date_from{ sub_object.value( "from" ).toString() };
        QString const date_to{ sub_object.value( "to" ).toString() };
        utilities::Subscription sub{
            QDateTime::fromString( date_from, Qt::ISODate ).date(),
            QDateTime::fromString( date_to, Qt::ISODate ).date()
        };
        subscriptions_.append( sub );
    }
}

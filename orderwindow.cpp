#include <QUrl>
#include <QUrlQuery>
#include <QDate>
#include <QDebug>
#include <QProgressDialog>
#include <QMessageBox>

#include "orderwindow.hpp"
#include "ui_orderwindow.h"
#include "resources.hpp"


OrderWindow::OrderWindow( QWidget *parent ): QMainWindow( parent ), ui( new Ui::OrderWindow )
{
    ui->setupUi( this );
    ui->search_limit_checkbox->setChecked( false );
    ui->search_date_from->setEnabled( false );
    ui->search_date_to->setEnabled( false );

    QObject::connect( ui->search_limit_checkbox, &QCheckBox::toggled, [=]( bool is_checked ){
        ui->search_date_from->setEnabled( is_checked );
        ui->search_date_to->setEnabled( is_checked );
        ui->order_by_combo->setDisabled( is_checked );
    });

    QObject::connect( ui->find_guest_button, &QPushButton::clicked, this,
                      &OrderWindow::OnFindButtonClicked );
    QStringList const search_criteria{ "Show today's order", "Show last 7 days",
                                       "Show this month's" };
    ui->order_by_combo->addItems( search_criteria );
    ui->search_date_from->setDate( QDate::currentDate() );
    ui->search_date_to->setDate( QDate::currentDate() );
}

OrderWindow::~OrderWindow()
{
    delete ui;
}

QString OrderWindow::DateToString( QDate const & date )
{
    return date.toString( "yyyy-MM-dd" );
}

void OrderWindow::OnFindButtonClicked()
{
    ui->find_guest_button->setDisabled( true );
    QUrlQuery query {};

    if( ui->search_limit_checkbox->isChecked() ){
        query.addQueryItem( "from", DateToString( ui->search_date_from->date() ) );
        query.addQueryItem( "to", DateToString( ui->search_date_to->date() ) );
    } else {
        int const search_term_index = ui->order_by_combo->currentIndex();
        QDate const today{ QDate::currentDate() };
        if( search_term_index == 0 ){
            query.addQueryItem( "date", DateToString( today ) );
        } else if( search_term_index == 1 ){
            QDate const last_seven_days{ today.addDays( -7 ) };
            query.addQueryItem( "from", DateToString( last_seven_days ) );
            query.addQueryItem( "to", DateToString( today ) );
        } else {
            QDate const first_of_this_month{ today.addDays( today.day() - 1 ) };
            query.addQueryItem( "from", DateToString( first_of_this_month ) );
            query.addQueryItem( "to", DateToString( today ) );
        }
    }
    QUrl address{ utilities::Endpoint::GetEndpoints().GetOrders() };
    address.setQuery( query );
    SendNetworkRequest( address );
}

void OrderWindow::SendNetworkRequest( QUrl const &address )
{
    QNetworkAccessManager& network_manager{ utilities::NetworkManager::GetNetwork() };
    auto& session_cookie = utilities::NetworkManager::GetSessionCookie();
    network_manager.setCookieJar( &session_cookie );
    session_cookie.setParent( nullptr );
    QNetworkRequest const request{ utilities::GetRequestInterface( address ) };
    QNetworkReply* const reply{ network_manager.get( request ) };

    QProgressDialog *progress_dialog{ new QProgressDialog( "Sending request to server",
                                                           "Cancel", 1, 100 ) };
    progress_dialog->setAttribute( Qt::WA_DeleteOnClose );
    progress_dialog->show();
    QObject::connect( progress_dialog, &QProgressDialog::canceled, reply, &QNetworkReply::abort );
    QObject::connect( reply, &QNetworkReply::downloadProgress,
                      [=]( qint64 const received, qint64 const total)
    {
        progress_dialog->setMaximum( total + ( total * 0.25 ) );
        progress_dialog->setValue( received );
    });
    QObject::connect( reply, &QNetworkReply::finished, [=]{
        progress_dialog->close();
        ui->find_guest_button->setEnabled( true );
        QJsonObject const response{ utilities::GetJsonNetworkData( reply ) };
        if( response.isEmpty() ){
            QMessageBox::information( this, "Orders", "No result obtained" );
            return;
        }
        DisplayOrderData( response );
    });
}

void OrderWindow::DisplayOrderData( QJsonObject const & data )
{
    QJsonObject const orders{ data.value( "orders" ).toObject() };
    if( orders.isEmpty() ){
        QMessageBox::information( this, "Orders", "No result matching that criteria" );
        return;
    }
}

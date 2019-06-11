#include <QUrl>
#include <QUrlQuery>
#include <QDate>
#include <QDebug>
#include <QJsonArray>
#include <QProgressDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include <QAction>

#include "orderwindow.hpp"
#include "ordermodel.hpp"
#include "ui_orderwindow.h"
#include "resources.hpp"
#include "orderitemdetaildialog.hpp"

OrderWindow::OrderWindow( QWidget *parent ): QMainWindow( parent ), ui( new Ui::OrderWindow )
{
    ui->setupUi( this );
    ui->search_limit_checkbox->setChecked( false );
    ui->search_date_from->setEnabled( false );
    ui->search_date_to->setEnabled( false );
    ui->tableView->setContextMenuPolicy( Qt::CustomContextMenu );
    ui->first_page_button->setDisabled( true );
    ui->last_page_button->setDisabled( true );
    ui->prev_button->setDisabled( true );
    ui->next_button->setDisabled( true );

    QObject::connect( ui->search_limit_checkbox, &QCheckBox::toggled, [=]( bool is_checked ){
        ui->search_date_from->setEnabled( is_checked );
        ui->search_date_to->setEnabled( is_checked );
        ui->order_by_combo->setDisabled( is_checked );
    });
    QObject::connect( ui->tableView, &QTableView::customContextMenuRequested, this,
                      &OrderWindow::OnCustomContextMenuRequested );
    QObject::connect( ui->find_guest_button, &QPushButton::clicked, this,
                      &OrderWindow::OnFindButtonClicked );
    QObject::connect( ui->next_button, &QToolButton::clicked, this,
                      &OrderWindow::OnNextPageButtonClicked );
    QObject::connect( ui->prev_button, &QToolButton::clicked, this,
                      &OrderWindow::OnPreviousPageButtonClicked );
    QObject::connect( ui->last_page_button, &QToolButton::clicked, this,
                      &OrderWindow::OnLastPageButtonClicked );
    QObject::connect( ui->first_page_button, &QToolButton::clicked, this,
                      &OrderWindow::OnFirstPageButtonClicked );
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

void OrderWindow::OnCustomContextMenuRequested( QPoint const & point )
{
    QModelIndex const index{ ui->tableView->indexAt( point )};
    if( !index.isValid() ) return;

    QMenu custom_menu( this );
    custom_menu.setTitle( "Orders" );
    if( !index.parent().isValid() ){
        QAction* action_details{ new QAction( "Details" ) };
        QAction* action_remove{ new QAction( "Remove" ) };
        QObject::connect( action_details, &QAction::triggered, this,
                          &OrderWindow::OnOrderDetailsRequested );
        QObject::connect( action_remove, &QAction::triggered, this,
                          &OrderWindow::OnRemoveItemActionClicked );
        custom_menu.addAction( action_details );
        custom_menu.addAction( action_remove );
    }
    custom_menu.exec( ui->tableView->mapToGlobal( point ) );
}

void OrderWindow::OnOrderDetailsRequested()
{
    QModelIndex const index{ ui->tableView->currentIndex() };
    if( !index.isValid() ) return;
    OrderModel* model{ qobject_cast<OrderModel*>( ui->tableView->model() ) };
    auto const & order_items{ model->ItemDataAt( index.row() )};
    OrderItemDetailDialog *order_detail_dialog{ new OrderItemDetailDialog( order_items ) };

    auto window_title{ QString( "Order: %1" ).arg( model->ReferenceIdAtIndex( index.row() ) )};
    order_detail_dialog->setWindowTitle( window_title );

    order_detail_dialog->setWindowModality( Qt::WindowModal );
    order_detail_dialog->setAttribute( Qt::WA_DeleteOnClose );
    order_detail_dialog->show();
}

void OrderWindow::OnRemoveItemActionClicked()
{
    QModelIndex const index{ ui->tableView->currentIndex() };
    if( !index.isValid() ) return;

    ui->tableView->setEnabled( false );
    OrderModel* model = qobject_cast<OrderModel*>( ui->tableView->model() );
    QString const order_id{ QString::number( model->OrderIdAtIndex( index.row() ) ) };

    auto const reason = QInputDialog::getText( this, "Delete order",
                                               "Why are you deleting this order?" ).trimmed();
    if( reason.isEmpty() ){
        ui->tableView->setEnabled( true );
        return;
    }
    QNetworkAccessManager& network_manager{ utilities::NetworkManager::GetNetwork() };
    auto& session_cookie = utilities::NetworkManager::GetSessionCookie();
    network_manager.setCookieJar( &session_cookie );
    session_cookie.setParent( nullptr );
    QUrl address{ utilities::Endpoint::GetEndpoints().RemoveOrder() };
    QUrlQuery query{};
    query.addQueryItem( "order_id", order_id );
    query.addQueryItem( "reason", reason );
    address.setQuery( query );
    QNetworkRequest const request{ utilities::GetRequestInterface( address ) };
    QNetworkReply* reply{ network_manager.deleteResource( request ) };
    QProgressDialog *progress_dialog{ new QProgressDialog( "Sending request to server",
                                                           "Cancel", 1, 100 ) };
    progress_dialog->setAttribute( Qt::WA_DeleteOnClose );
    progress_dialog->show();
    connect( progress_dialog, &QProgressDialog::canceled, reply, &QNetworkReply::abort );
    connect( reply, &QNetworkReply::downloadProgress,
                      [=]( qint64 const received, qint64 const total )
    {
        progress_dialog->setMaximum( total + ( total * 0.25 ) );
        progress_dialog->setValue( received );
    } );
    QObject::connect( reply, &QNetworkReply::finished, [=]{
        ui->tableView->setEnabled( true );
        progress_dialog->close();
        QJsonObject const response{ utilities::GetJsonNetworkData( reply, true ) };
        if( response.isEmpty() ) return;
        model->removeRows( index.row(), 1, index.parent() );
    });
}

void OrderWindow::OnFirstPageButtonClicked()
{
    QUrl const first_page_address{ orders_page_query.first_url };
    SendNetworkRequest( first_page_address );
}

void OrderWindow::OnLastPageButtonClicked()
{
    QUrl const last_page_address{ orders_page_query.last_url };
    SendNetworkRequest( last_page_address );
}

void OrderWindow::OnNextPageButtonClicked()
{
    QUrl const& next_page_address{ orders_page_query.other_url.next_url };
    SendNetworkRequest( next_page_address );
}

void OrderWindow::OnPreviousPageButtonClicked()
{
    QUrl const& next_page_address{ orders_page_query.other_url.previous_url };
    SendNetworkRequest( next_page_address );
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
    ui->first_page_button->setDisabled( true );
    ui->last_page_button->setDisabled( true );
    ui->prev_button->setDisabled( true );
    ui->next_button->setDisabled( true );

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

void OrderWindow::UpdatePageData()
{
    utilities::UrlData const& url_data{ orders_page_query.other_url };
    ui->page_label->setText( QString( "Page %1 of %2" ).arg( url_data.page_number )
                             .arg( orders_page_query.number_of_pages ));
    auto& page_number = url_data.page_number;
    auto& number_of_pages = orders_page_query.number_of_pages;
    if( page_number < number_of_pages ){
        ui->next_button->setEnabled( true );
        ui->last_page_button->setEnabled( true );
    } else if( page_number > 1 ){
        if( page_number == number_of_pages ){
            ui->prev_button->setEnabled( true );
            ui->first_page_button->setEnabled( true );
        } else {
            ui->first_page_button->setEnabled( true );
            ui->last_page_button->setEnabled( true );
            ui->prev_button->setEnabled( true );
            ui->next_button->setEnabled( true );
        }
    }
}

void OrderWindow::DisplayOrderData( QJsonObject const & data )
{
    bool const result_found{ utilities::ParsePageUrls( data, orders_page_query )};
    if( !result_found ){
        QMessageBox::information( this, "Orders", "No result matching that criteria" );
        return;
    }
    QJsonArray order_list{ data.value( "orders" ).toArray() };
    QVector<utilities::OrderData> orders {};
    while( !order_list.isEmpty() ){
        QJsonValue const value{ order_list[0] };
        if( value.isArray() ){
            order_list = value.toArray();
        } else {
            break;
        }
    }
    ParseOrderResult( order_list, orders );
    if( orders.isEmpty() ){
        QMessageBox::information( this, "Orders", "No result found" );
        return;
    }
    UpdatePageData();
    OrderModel *model{ new OrderModel( std::move( orders ), ui->tableView ) };
    ui->tableView->setVisible( false );
    QObject::connect( model, SIGNAL( dataChanged( QModelIndex,QModelIndex,QVector<int> ) ),
                      ui->tableView, SLOT(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    ui->tableView->setModel( model );
    ui->tableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->tableView->resizeColumnsToContents();
    ui->tableView->setVisible( true );
}

void OrderWindow::ParseOrderResult( QJsonArray const & order_object_list,
                                    QVector<utilities::OrderData> &orders )
{
    for( auto const &value: order_object_list ){
        QJsonObject const & object = value.toObject();
        utilities::OrderData order {};
        QString const date_str{ object.value( "date" ).toString() };
        auto const order_date{ QDateTime::fromString( date_str, Qt::ISODateWithMs )};
        qint64 const order_id{ object.value( "id" ).toInt() };

        for( auto const & item: object.value( "items" ).toArray() ){
            QJsonObject const item_object{ item.toObject() };
            QString const product_name { item_object.value( "product" ).toString() };
            qint64 const quantity{ item_object.value( "quantity" ).toInt() };
            double const price{ item_object.value( "price" ).toDouble() };
            order.items.push_back( { product_name, quantity, price } );
        }
        order.order_date = order_date;
        order.order_id = order_id;
        order.reference_id = object.value( "payment_reference" ).toString();
        order.staff_username = object.value( "staff" ).toString();
        orders.push_back( order );
    }
}

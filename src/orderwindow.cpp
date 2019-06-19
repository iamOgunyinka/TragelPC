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

OrderWindow::OrderWindow( QWidget *parent ): QMainWindow( parent ),
    ui( new Ui::OrderWindow )
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

    QObject::connect( ui->search_limit_checkbox, &QCheckBox::toggled,
                      [=]( bool const is_checked )
    {
        ui->search_date_from->setEnabled( is_checked );
        ui->search_date_to->setEnabled( is_checked );
        ui->order_by_combo->setDisabled( is_checked );
    });
    QObject::connect( ui->tableView, &QTableView::customContextMenuRequested,
                      this, &OrderWindow::OnCustomContextMenuRequested );
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
    QStringList const search_criteria{
        "All orders", "Show today's orders",
        "From last 7 days", "This month's" };
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
    auto order_detail_dialog{ new OrderItemDetailDialog( order_items ) };

    auto window_title{
        QString( "Order: %1" ).arg( model->ReferenceIdAtIndex( index.row() ) )
    };
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
    QString const order_id{
        QString::number( model->OrderIdAtIndex( index.row() ) )
    };

    auto reason = QInputDialog::getText( this, "Delete order",
                                         "Why are you deleting this order?" )
            .trimmed();
    if( reason.isEmpty() ){
        ui->tableView->setEnabled( true );
        return;
    }
    QUrl address{ utilities::Endpoint::GetEndpoints().RemoveOrder() };
    QUrlQuery query{};
    query.addQueryItem( "order_id", order_id );
    query.addQueryItem( "reason", reason );
    address.setQuery( query );
    auto on_success = [=]( QJsonObject const & ){
        ui->tableView->setEnabled( true );
        model->removeRows( index.row(), 1, index.parent() );
    };
    auto on_error = [=]{
        ui->tableView->setEnabled( true );
    };

    utilities::SendNetworkRequest( address, on_success, on_error, this, true,
                                   utilities::SimpleRequestType::Delete );
}

QUrl OrderWindow::GetUrlNewMetaFromUrl( QString const & old_url )
{
    QUrl url{ old_url };
    QUrlQuery const original_request_query{ request_.url().query() };
    QUrlQuery this_page_query{ url.query() };
    for( auto const & key_val: original_request_query.queryItems() ){
        if( !this_page_query.hasQueryItem( key_val.first ) ){
            this_page_query.addQueryItem( key_val.first, key_val.second );
        }
    }
    url.setQuery( this_page_query );
    return url;
}

void OrderWindow::OnFirstPageButtonClicked()
{
    auto const first_page_address{
        GetUrlNewMetaFromUrl( orders_page_query.first_url )
    };
    SendNetworkRequest( first_page_address );
}

void OrderWindow::OnLastPageButtonClicked()
{
    QUrl const last_page_address{
        GetUrlNewMetaFromUrl( orders_page_query.last_url )
    };
    SendNetworkRequest( last_page_address );
}

void OrderWindow::OnNextPageButtonClicked()
{
    QUrl const next_page_address{
        GetUrlNewMetaFromUrl( orders_page_query.other_url.next_url )
    };
    SendNetworkRequest( next_page_address );
}

void OrderWindow::OnPreviousPageButtonClicked()
{
    QUrl const next_page_address{
        GetUrlNewMetaFromUrl( orders_page_query.other_url.previous_url )
    };
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
        if( search_term_index == 0 ){ // all orders from the beginning
            query.addQueryItem( "all", "" );
        } else if( search_term_index == 1 ){
            query.addQueryItem( "from", DateToString( today ) );
        } else if( search_term_index == 2 ){
            QDate const last_seven_days{ today.addDays( -7 ) };
            query.addQueryItem( "from", DateToString( last_seven_days ) );
        } else {
            QDate const first_of_this_month{
                today.addDays( ( -1 * today.day() ) + 1 ) };
            query.addQueryItem( "from", DateToString( first_of_this_month ) );
        }
        query.addQueryItem( "to", DateToString( today ) );
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

    auto on_success = [=]( QJsonObject const & result ){
        ui->find_guest_button->setEnabled( true );
        DisplayOrderData( result );
    };
    auto on_error = [=]{
        ui->find_guest_button->setEnabled( true );
    };
    request_ = utilities::GetRequestInterface( address );
    utilities::SendNetworkRequest( address, on_success, on_error, this );
}

void OrderWindow::UpdatePageData()
{
    utilities::UrlData const& url_data{ orders_page_query.other_url };
    ui->page_label->setText( QString( "Page %1 of %2 (Total : %3 items)" )
                             .arg( url_data.page_number )
                             .arg( orders_page_query.number_of_pages )
                             .arg( orders_page_query.total_result ));

    auto const page_number = url_data.page_number;
    auto const number_of_pages = orders_page_query.number_of_pages;

    if( page_number < number_of_pages ){
        ui->next_button->setEnabled( true );
        ui->last_page_button->setEnabled( true );
    }
    if( page_number > 1 ){
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
    bool result_found{ utilities::ParsePageUrls( data, orders_page_query )};
    if( !result_found ){
        QMessageBox::information( this, "Orders",
                                  "No result matching that criteria" );
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
    QObject::connect( model,
                      SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                      ui->tableView,
                      SLOT(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    ui->tableView->setModel( model );
    ui->tableView->horizontalHeader()
            ->setSectionResizeMode( QHeaderView::Stretch );
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

        for( auto const & item: object.value( "items" ).toArray() ){
            QJsonObject const item_object{ item.toObject() };
            QString const product_name {
                item_object.value( "product" ).toString()
            };
            qint64 const quantity{ item_object.value( "quantity" ).toInt() };
            double const price{ item_object.value( "price" ).toDouble() };
            order.items.push_back( { product_name, quantity, price } );
        }
        auto const order_date{
            QDateTime::fromString( date_str, Qt::ISODateWithMs )
        };
        order.order_date = order_date;
        qint64 const order_id{ object.value( "id" ).toInt() };
        order.order_id = order_id;
        order.reference_id = object.value( "payment_reference" ).toString();
        order.staff_username = object.value( "staff" ).toString();
        orders.push_back( order );
    }
}

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

    QStringList const search_criteria {
        "Cash payments", "Order reference number", "Our staff", "Other users"
    };
    ui->order_by_combo->addItems( search_criteria );
    ui->search_date_from->setDate( QDate::currentDate().addDays( -7 ) );
    ui->search_date_to->setDate( QDate::currentDate() );

    QObject::connect( ui->tableView, &QTableView::customContextMenuRequested,
                      this, &OrderWindow::OnCustomContextMenuRequested );
    QObject::connect( ui->find_order_button, &QPushButton::clicked, this,
                      &OrderWindow::OnFindButtonClicked );
    QObject::connect( ui->next_button, &QToolButton::clicked, this,
                      &OrderWindow::OnNextPageButtonClicked );
    QObject::connect( ui->prev_button, &QToolButton::clicked, this,
                      &OrderWindow::OnPreviousPageButtonClicked );
    QObject::connect( ui->last_page_button, &QToolButton::clicked, this,
                      &OrderWindow::OnLastPageButtonClicked );
    QObject::connect( ui->first_page_button, &QToolButton::clicked, this,
                      &OrderWindow::OnFirstPageButtonClicked );
    QObject::connect( ui->action_all_orders, &QAction::triggered, this,
                      &OrderWindow::OnFindAllOrdersTriggered );
    QObject::connect( ui->action_last_seven_days, &QAction::triggered, this,
                      &OrderWindow::OnFindOrdersFromLastSevenDays );
    QObject::connect( ui->action_this_month, &QAction::triggered, this,
                      &OrderWindow::OnFindThisMonthsOrderTriggered );
    QObject::connect( ui->action_today, &QAction::triggered, this,
                      &OrderWindow::OnFindTodaysOrderTriggered );
    QObject::connect( ui->search_limit_checkbox, &QCheckBox::toggled,
                      [=]( bool const checked )
    {
        ui->search_date_from->setEnabled( checked );
        ui->search_date_to->setEnabled( checked );
    });
    QObject::connect( ui->order_by_combo,
                      QOverload<int>::of( &QComboBox::currentIndexChanged ),
                      [=]( int const index )
    {
        ui->criteria_line->setEnabled( index != 0 );
    });
}

OrderWindow::~OrderWindow()
{
    delete ui;
}

QString OrderWindow::DateToString( QDate const & date )
{
    return date.toString( "yyyy-MM-dd" );
}

void OrderWindow::OnConfirmOrderPaymentTriggered()
{
    QModelIndex const index{ ui->tableView->currentIndex() };
    utilities::OrderData const & order{ orders_[ index.row() ]};

    int reply = QMessageBox::warning( this, "Confirm order",
                                      "This is an irreversible operation! Are "
                                      "you absolutely sure that the payment "
                                      "has been received?", QMessageBox::Yes |
                                      QMessageBox::No );
    if( reply == QMessageBox::No ) return;
    ui->tableView->setEnabled( false );

    QString const order_id { QString::number( order.order_id ) };
    QString const date_time {
        QDateTime::currentDateTime().toString( Qt::ISODateWithMs )
    };
    QByteArray const payload {
        QString( order_id + "@" + date_time ).toLocal8Bit().toBase64()
    };
    QUrlQuery query{};
    query.addQueryItem( "payload", payload );
    QUrl address { utilities::Endpoint::GetEndpoints().ConfirmPayment() };
    address.setQuery( query );

    auto on_success = [=]( QJsonObject const & result )
    {
        QJsonObject const confirmation_detail{
            result.value( "status" ).toObject()
        };
        utilities::OrderData &order{ orders_[index.row()] };
        order.confirmation_data.confirmed = true;
        order.confirmation_data.confirmed_by =
                confirmation_detail.value( "by" ).toString();
        order.confirmation_data.date_of_confirmation = QDateTime::fromString(
                    confirmation_detail.value( "date" ).toString(),
                    Qt::ISODateWithMs );
        ui->tableView->setEnabled( true );
        OrderModel* model{ qobject_cast<OrderModel*>( ui->tableView->model() ) };
        model->setData( model->index( index.row(), 4 ), 1 );
    };

    auto on_error = [=]
    {
        ui->tableView->setEnabled( true );
    };

    utilities::SendNetworkRequest( address, on_success, on_error, this );
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

        utilities::OrderData const & order{ orders_[index.row()] };
        if( !order.confirmation_data.confirmed ){
            QAction* action_confirm{ new QAction( "Confirm" ) };
            QObject::connect( action_confirm, &QAction::triggered, this,
                              &OrderWindow::OnConfirmOrderPaymentTriggered );
            custom_menu.addAction( action_confirm );
        }

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
    utilities::OrderData const &order{ orders_[index.row() ] };
    auto order_detail_dialog{ new OrderItemDetailDialog( order.items, this ) };
    if( order.confirmation_data.confirmed ){
        order_detail_dialog->SetConfirmationStatus(
                    order.confirmation_data.confirmed_by,
                    order.confirmation_data.date_of_confirmation );
    }
    auto window_title{ QString( "Order: %1" ).arg( order.reference_id ) };
    order_detail_dialog->setWindowTitle( window_title );
    order_detail_dialog->setAttribute( Qt::WA_DeleteOnClose );
    order_detail_dialog->show();
}

void OrderWindow::OnRemoveItemActionClicked()
{
    QModelIndex const index{ ui->tableView->currentIndex() };
    if( !index.isValid() ) return;

    ui->tableView->setEnabled( false );
    auto* model = qobject_cast<OrderModel*>( ui->tableView->model() );
    QString const order_id{ QString::number( orders_[index.row()].order_id ) };

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

void OrderWindow::OnFindAllOrdersTriggered()
{
    QUrlQuery query{};
    query.addQueryItem( "all", QString::number( 1 ) );
    QUrl address{ utilities::Endpoint::GetEndpoints().GetOrders() };
    address.setQuery( query );
    SendNetworkRequest( address );
}

void OrderWindow::OnFindTodaysOrderTriggered()
{
    QDate const today{ QDate::currentDate() };
    QUrlQuery query{};
    query.addQueryItem( "from", DateToString( today ) );
    query.addQueryItem( "to", DateToString( today ) );
    QUrl address{ utilities::Endpoint::GetEndpoints().GetOrders() };
    address.setQuery( query );
    SendNetworkRequest( address );
}

void OrderWindow::OnFindOrdersFromLastSevenDays()
{
    QDate const today{ QDate::currentDate() };
    QDate const last_seven_days{ today.addDays( -7 ) };
    QUrlQuery query{};
    query.addQueryItem( "from", DateToString( last_seven_days ) );
    query.addQueryItem( "to", DateToString( today ) );
    QUrl address{ utilities::Endpoint::GetEndpoints().GetOrders() };
    address.setQuery( query );
    SendNetworkRequest( address );
}

void OrderWindow::OnFindThisMonthsOrderTriggered()
{
    QDate const today{ QDate::currentDate() };
    QDate const first_of_this_month{today.addDays( ( -1 * today.day() ) + 1 ) };

    QUrlQuery query{};
    query.addQueryItem( "from", DateToString( first_of_this_month ) );
    query.addQueryItem( "to", DateToString( today ) );
    QUrl address{ utilities::Endpoint::GetEndpoints().GetOrders() };
    address.setQuery( query );
    SendNetworkRequest( address );
}

void OrderWindow::OnFindButtonClicked()
{
    ui->find_order_button->setDisabled( true );

    if( !ui->search_limit_checkbox->isChecked() ){
        QMessageBox::information( this, "Limit",
                                  "You might want to limit your search between"
                                  " dates." );
        ui->search_limit_checkbox->setChecked( true );
        ui->find_order_button->setEnabled( true );
        return;
    }
    int const search_type { ui->order_by_combo->currentIndex() };
    QString const criteria{ ui->criteria_line->text().trimmed() };
    if( search_type != 0 && criteria.isEmpty() ){
        QMessageBox::information( this, "Search", "Enter a valid search term" );
        ui->criteria_line->setFocus();
        ui->find_order_button->setEnabled( true );
        return;
    }
    QUrlQuery query {};
    query.addQueryItem( "search_type", QString::number( search_type ));
    if( search_type == 0 ){ // all cash payments
        query.addQueryItem( "ref_id", "cash" );
    } else if( search_type == 1 ){ //search by payment reference
        query.addQueryItem( "ref_id", criteria );
    } else if( search_type == 2 || search_type == 3 ){//by username
        QString const company_id = search_type == 2 ?
                    utilities::Endpoint::GetEndpoints().CompanyID(): "0";
        QString const username{ criteria + "@" + company_id };
        query.addQueryItem( "username", username );
    }
    if( ui->search_limit_checkbox->isChecked() ){
        auto const date_from { ui->search_date_from->date() };
        auto const date_to{ ui->search_date_to->date() };
        query.addQueryItem( "from", DateToString(  date_from ) );
        query.addQueryItem( "to", DateToString( date_to ) );
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
        ui->find_order_button->setEnabled( true );
        DisplayOrderData( result );
    };
    auto on_error = [=]{
        ui->find_order_button->setEnabled( true );
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
    while( !order_list.isEmpty() ){
        QJsonValue const value{ order_list[0] };
        if( value.isArray() ) order_list = value.toArray();
        else break;
    }
    ParseOrderResult( order_list, orders_ );
    if( orders_.isEmpty() ){
        QMessageBox::information( this, "Orders", "No result found" );
        return;
    }
    UpdatePageData();
    OrderModel *model{ new OrderModel( orders_, ui->tableView ) };
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
    orders.clear();
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
        order.payment_type = object.value( "payment_type" ).toInt();
        QJsonObject const confirmation_details{
            object.value( "confirmation" ).toObject()
        };
        order.confirmation_data.confirmed = confirmation_details.value(
                    "confirmed" ).toBool();
        if( order.confirmation_data.confirmed ){
            order.confirmation_data.confirmed_by = confirmation_details.
                    value( "by").toString();
            order.confirmation_data.date_of_confirmation =
                    QDateTime::fromString(
                        confirmation_details.value( "date").toString(),
                        Qt::ISODateWithMs );
        }
        orders.push_back( order );
    }
}

#include <QByteArray>
#include <QMdiSubWindow>
#include <QSettings>
#include <QVariant>
#include <QUrlQuery>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QProgressDialog>
#include <QJsonDocument>
#include <QJsonObject>

#include "widgets/addproductdialog.hpp"
#include "widgets/adminsetupdialog.hpp"
#include "widgets/allproductsdialog.hpp"
#include "widgets/centralwindow.hpp"
#include "widgets/createstaffdialog.hpp"
#include "widgets/orderwindow.hpp"
#include "widgets/userlogindialog.hpp"
#include "widgets/updateuserdialog.hpp"
#include "widgets/makeorderdialog.hpp"
#include "widgets/setupdialog.hpp"
#include "widgets/stockdialog.hpp"
#include "widgets/subscribedialog.hpp"
#include "widgets/allsubscriptionsdialog.hpp"
#include "utils/popupnotifier.hpp"
#include "utils/resources.hpp"
#include "framelesswindow.h"

#include "ui_centralwindow.h"

CentralWindow::CentralWindow( QWidget *parent ) :
    QMainWindow( parent ), ui( new Ui::CentralWindow ),
    workspace{ new QMdiArea },
    app_settings{ utilities::ApplicationSettings::GetAppSettings() },
    last_order_count{ 0 }, logged_in{ false }
{
    ui->setupUi( this );
    ui->actionExit->setIcon( QIcon( ":/darkstyle/icon_close.png" ) );
    setCentralWidget( workspace );

    ui->mainToolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

    QObject::connect( ui->actionExit, &QAction::triggered, this,
                      &CentralWindow::close );
    QObject::connect( ui->actionLogin, &QAction::triggered, this,
                      &CentralWindow::OnLoginButtonClicked );
    QObject::connect( ui->actionLogout, &QAction::triggered, this,
                      &CentralWindow::OnLogoutButtonClicked );
    QObject::connect( ui->actionShow_all_orders, &QAction::triggered, this,
                      &CentralWindow::OnOrderActionTriggered );
    QObject::connect( ui->actionAdd_products, &QAction::triggered, this,
                      &CentralWindow::OnAddProductTriggered );
    QObject::connect( ui->actionAdd_administrator, &QAction::triggered, this,
                      &CentralWindow::OnAddUserTriggered );
    QObject::connect( ui->actionAdd_user, &QAction::triggered, this,
                      &CentralWindow::OnAddUserTriggered );
    QObject::connect( ui->actionList_all_products, &QAction::triggered, this,
                      &CentralWindow::OnListAllProductsTriggered );
    QObject::connect( ui->actionUpdate_Change, &QAction::triggered, this,
                      &CentralWindow::OnListUsersTriggered );
    QObject::connect( ui->actionShow_all_users, &QAction::triggered, this,
                      &CentralWindow::OnListUsersTriggered );
    QObject::connect( ui->actionMakeOrders, &QAction::triggered, this,
                      &CentralWindow::OnMakeOrderTriggered );
    QObject::connect( ui->actionSettings, &QAction::triggered, this,
                      &CentralWindow::OnSettingsTriggered );
    QObject::connect( ui->actionSubscribe, &QAction::triggered, this,
                      &CentralWindow::OnSubscribeTriggered );
    QObject::connect( ui->actionStock, &QAction::triggered, this,
                      &CentralWindow::OnShowStockTriggered );
    QObject::connect( ui->actionList_our_subscriptions, &QAction::triggered,
                      this, &CentralWindow::OnListSubscriptionsTriggered );
    QObject::connect( ui->actionShow_expiry_date, &QAction::triggered,
                      this, &CentralWindow::OnShowExpiryTriggered );
    QObject::connect( ui->actionAbout, &QAction::triggered, qApp,
                      &QApplication::aboutQt );

    if( app_settings.Value( utilities::SettingsValue::AllowShortcut, false )
            .toBool() ){
        EnableShortcuts();
    }
}

CentralWindow::~CentralWindow()
{
    app_settings.SetValue( utilities::SettingsValue::OrderCount,
                           last_order_count );

    delete server_ping_timer;
    delete server_order_poll_timer;
    delete ui;
}

void CentralWindow::closeEvent( QCloseEvent* event )
{
    if( QMessageBox::question( this, "Close", "Are you sure you want to exit?" )
            == QMessageBox::Yes )
    {
        event->accept();
        emit closed();
    } else {
        event->ignore();
    }
}

void CentralWindow::EnableShortcuts()
{
    ui->actionExit->setShortcut( tr( "Ctrl+Q" ) );
    ui->actionAbout->setShortcut( tr( "Ctrl+A" ) );
    ui->actionLogin->setShortcut( tr( "Ctrl+L" ) );
    ui->actionLogout->setShortcut( tr( "Ctrl+G" ) );
    ui->actionHelp_me->setShortcut( tr( "F1" ) );
    ui->actionAdd_user->setShortcut( tr( "Ctrl+U" ) );
    ui->actionSettings->setShortcut( tr( "F2" ) );
    ui->actionSubscribe->setShortcut( tr( "Ctrl+B" ) );
    ui->actionMakeOrders->setShortcut( tr( "Ctrl+O" ) );
    ui->actionAdd_products->setShortcut( tr( "Ctrl+T" ));
    ui->actionUpdate_Change->setShortcut( tr( "Ctrl+E" ) );
    ui->actionShow_all_users->setShortcut( tr( "Ctrl+W" ));
    ui->actionShow_all_orders->setShortcut( tr( "Ctrl+H" ));
    ui->actionShow_expiry_date->setShortcut( tr( "Ctrl+X" ));
    ui->actionAdd_administrator->setShortcut( tr( "Ctrl+M" ));
    ui->actionList_all_products->setShortcut( tr( "Ctrl+I" ));
    ui->actionList_our_subscriptions->setShortcut( tr( "Ctrl+R" ) );
    ui->actionStock->setShortcut( tr( "Ctrl+K") );
}

void CentralWindow::OnShowStockTriggered()
{
    ui->actionStock->setEnabled( false );
    StockDialog* stock_dialog = new StockDialog( this );
    QMdiSubWindow* parent_window = workspace->addSubWindow( stock_dialog );
    parent_window->setWindowTitle( "Stock" );
    parent_window->setAttribute( Qt::WA_DeleteOnClose );
    QObject::connect( stock_dialog, &StockDialog::accepted, parent_window,
                      &QMdiSubWindow::close );
    QObject::connect( parent_window, &QMdiSubWindow::destroyed, [=]{
        if( logged_in ) ui->actionStock->setEnabled( true );
    });

    stock_dialog->show();
}

void CentralWindow::OnAddProductTriggered()
{
    ui->actionAdd_products->setEnabled( false );
    AddProductDialog *product_dialog{ new AddProductDialog };
    auto sub_window = workspace->addSubWindow( product_dialog );
    sub_window->setAttribute( Qt::WA_DeleteOnClose );
    sub_window->setWindowTitle( "Add new product(s)" );
    auto on_finished = [=]{
        sub_window->close();
        if( logged_in ) ui->actionAdd_products->setEnabled( true );
    };
    QObject::connect( product_dialog, &AddProductDialog::finished, on_finished );
    QObject::connect( sub_window, &QMdiSubWindow::destroyed, [=]{
        if( logged_in ) ui->actionAdd_products->setEnabled( true );
    });
    sub_window->setFixedHeight( workspace->height() );
    product_dialog->show();
}

void CentralWindow::OnShowExpiryTriggered()
{
    QUrl const address{ utilities::Endpoint::GetEndpoints().GetExpiryDate() };
    auto on_success = [this]( QJsonObject const & result ){
        QString const status = result.value( "status" ).toString();
        QDate const date = QDate::fromString( status, Qt::ISODate );
        QMessageBox::information( this, "Expiry date", date.toString() );
    };
    utilities::SendNetworkRequest( address, on_success, []{}, this );
}

void CentralWindow::OnListSubscriptionsTriggered()
{
    AllSubscriptionsDialog* subscription_dialog{
        new AllSubscriptionsDialog( this )
    };
    auto sub_window = workspace->addSubWindow( subscription_dialog );
    sub_window->setAttribute( Qt::WA_DeleteOnClose );
    sub_window->setWindowTitle( "List of subscriptions" );
    subscription_dialog->GetSubscriptions();
    subscription_dialog->show();
}

void CentralWindow::OnSubscribeTriggered()
{
    SubscribeDialog* subscribe_dialog{ new SubscribeDialog( this ) };
    auto sub_window = workspace->addSubWindow( subscribe_dialog );
    sub_window->setAttribute( Qt::WA_DeleteOnClose );
    sub_window->setWindowTitle( "Subscribe" );
    subscribe_dialog->show();
}

void CentralWindow::OnListUsersTriggered()
{
    ui->actionUpdate_Change->setDisabled( true );
    UpdateUserDialog* update_dialog{ new UpdateUserDialog( this ) };
    QMdiSubWindow* sub_window{ workspace->addSubWindow( update_dialog )};
    sub_window->setAttribute( Qt::WA_DeleteOnClose );
    sub_window->setFixedHeight( workspace->height() );
    QObject::connect( update_dialog, &UpdateUserDialog::finished, [=]{
        sub_window->close();
        if( logged_in ) ui->actionUpdate_Change->setEnabled( true );
    });
    update_dialog->show();
    update_dialog->GetAllUsers();
}

void CentralWindow::OnSettingsTriggered()
{
    ui->actionSettings->setDisabled( true );
    SetupDialog* setup_dialog{ new SetupDialog( this ) };
    QMdiSubWindow* parent_window{ workspace->addSubWindow( setup_dialog ) };
    parent_window->setAttribute( Qt::WA_DeleteOnClose );
    parent_window->setWindowTitle( "Application settings" );
    QObject::connect( setup_dialog, &SetupDialog::finished, [=]{
        if( logged_in ) ui->actionSettings->setEnabled( true );
        parent_window->close();
    });
    QObject::connect( parent_window, &QMdiSubWindow::destroyed, [=]{
        if( logged_in ) ui->actionSettings->setEnabled( true );
    });
    setup_dialog->show();
    parent_window->adjustSize();
}

void CentralWindow::OnAddUserTriggered()
{
    CreateStaffDialog *staff_dialog{ new CreateStaffDialog( this ) };
    QMdiSubWindow *sub_window{ workspace->addSubWindow( staff_dialog ) };
    sub_window->setAttribute( Qt::WA_DeleteOnClose );
    auto *const object_sender = qobject_cast<QAction*>( sender() );
    object_sender->setDisabled( true );
    if( object_sender == ui->actionAdd_administrator ){
        sub_window->setWindowTitle( "Create administrator" );
        staff_dialog->SetUserRole( UserRole::Administrator );
    } else {
        sub_window->setWindowTitle( "Create staff" );
        staff_dialog->SetUserRole( UserRole::BasicUser );
    }
    QObject::connect( staff_dialog, &CreateStaffDialog::finished, [=]{
        sub_window->close();
        if( logged_in ) object_sender->setEnabled( true );
    });
    QSize const min_max_size{ 516, 370 };
    sub_window->setMinimumSize( min_max_size );
    sub_window->setMaximumSize( min_max_size );
    staff_dialog->show();
}

void CentralWindow::OnOrderActionTriggered()
{
    ui->menuOrders->setEnabled( false );
    OrderWindow* order_window{ new OrderWindow };
    QMdiSubWindow *sub_window = workspace->addSubWindow( order_window );
    sub_window->setAttribute( Qt::WA_DeleteOnClose );
    sub_window->setWindowTitle( "Orders" );
    order_window->show();
    QObject::connect( order_window, &OrderWindow::destroyed, [=]{
        if( logged_in ) ui->menuOrders->setEnabled( true );
    });
    sub_window->adjustSize();
}

void CentralWindow::OnMakeOrderTriggered()
{
    ui->actionMakeOrders->setDisabled( true );
    MakeOrderDialog* order_dialog{ new MakeOrderDialog( this ) };
    QMdiSubWindow *parent_window{ workspace->addSubWindow( order_dialog ) };
    parent_window->setFixedHeight( workspace->height() );
    parent_window->setAttribute( Qt::WA_DeleteOnClose );
    parent_window->setWindowTitle( "Make order" );
    QObject::connect( order_dialog, &MakeOrderDialog::finished, [=]{
        if( logged_in ) ui->actionMakeOrders->setEnabled( true );
        parent_window->close();
    });
    QObject::connect( parent_window, &QMdiSubWindow::destroyed, [=]{
        if( logged_in ) ui->actionMakeOrders->setEnabled( true );
    });
    order_dialog->show();
    order_dialog->DownloadProducts();
}

void CentralWindow::OnListAllProductsTriggered()
{
    ui->actionList_all_products->setDisabled( true );
    AllProductsDialog *dialog{ new AllProductsDialog };
    QMdiSubWindow *parent_window{ workspace->addSubWindow( dialog ) };
    parent_window->setFixedHeight( workspace->height() );
    parent_window->setAttribute( Qt::WA_DeleteOnClose );
    parent_window->setWindowTitle( "Our items" );
    QObject::connect( dialog, &AllProductsDialog::finished, [=]{
        if( logged_in ) ui->actionList_all_products->setEnabled( true );
        parent_window->close();
    });
    QObject::connect( parent_window, &QMdiSubWindow::destroyed, [=]{
        if( logged_in ) ui->actionList_all_products->setEnabled( true );
    });
    dialog->show();
    dialog->DownloadProducts();
}

void CentralWindow::LogUserIn( QString const & username,
                               QString const & password )
{
    auto on_success = [=]( QJsonObject const &, QNetworkReply* reply ){
        SetEnableActionButtons( true );
        ui->actionLogin->setEnabled( false );
        ui->actionLogout->setEnabled( true );
        PopUpNotifier* notifier{ new PopUpNotifier( this ) };
        notifier->setAttribute( Qt::WA_DeleteOnClose );
        notifier->SetPopUpText( "You're logged in" );
        notifier->show();
        auto& session_cookie = utilities::NetworkManager::GetSessionCookie();
        utilities::NetworkManager::SetNetworkCookie( session_cookie, reply );
        StartOrderPolling();
        logged_in = true;
    };
    auto on_error = [=]{
        ui->actionLogin->setEnabled( true );
    };
    auto const & endpoints = utilities::Endpoint::GetEndpoints();
    QUrl const login_url { QUrl::fromUserInput( endpoints.LoginTo() ) };
    QString const token = endpoints.CompanyToken();
    QJsonObject const login_object{
        { "username", username }, { "password", password },{ "token", token }
    };
    QJsonDocument const document{ login_object };
    utilities::SendPostNetworkRequest( login_url, on_success, on_error, this,
                                       document.toJson() );
}

void CentralWindow::OnLoginButtonClicked()
{
    ui->actionLogin->setEnabled( false );
    auto *user_dialog = new UserLoginDialog;
    user_dialog->SetCompanyID( utilities::Endpoint::GetEndpoints().CompanyID() );
    QMdiSubWindow* subwindow = workspace->addSubWindow( user_dialog );
    subwindow->setAttribute( Qt::WA_DeleteOnClose );
    QObject::connect( user_dialog, &UserLoginDialog::accepted, [=]{
        auto const company_id = utilities::Endpoint::GetEndpoints().CompanyID();
        QString const username = user_dialog->GetUsername().trimmed() + "@" +
                company_id;
        QString const password = user_dialog->GetPassword();
        LogUserIn( username, password );
    });
    QObject::connect( user_dialog, &UserLoginDialog::accepted, subwindow,
                      &QMdiSubWindow::close );
    subwindow->setWindowTitle( "Login" );
    CentralizeDisplayWidget( subwindow, QSize{ 400, 260 });
}

void CentralWindow::OnLogoutButtonClicked()
{
    auto choice = QMessageBox::question( this, "Logout",
                                         "Are you sure you want to logout?" );
    if( choice == QMessageBox::No ) return;
    SetEnableActionButtons( false );
    ui->actionLogin->setEnabled( true );
    auto const & endpoints = utilities::Endpoint::GetEndpoints();
    auto& network_manager= utilities::NetworkManager::GetNetworkWithCookie();
    QNetworkRequest const request=
        utilities::GetRequestInterface( endpoints.LogoutFrom() );
    network_manager.get( request );
    // let's clear the cookies
    utilities::NetworkManager::GetSessionCookie().setCookiesFromUrl(
                QList<QNetworkCookie>{}, endpoints.LoginTo() );
    logged_in = false;
    foreach( QMdiSubWindow* const sub_window, workspace->subWindowList() ){
        sub_window->close();
    }
    SetEnableActionButtons( false );
    ui->actionLogin->setEnabled( true );

    server_order_poll_timer->stop();
    delete server_order_poll_timer;
    server_order_poll_timer = nullptr;

    QMessageBox::information( this, "Logout",
                              "You've been logged out successfully" );
}

void CentralWindow::SetEnableActionButtons( bool const enable )
{
    ui->actionAdd_administrator->setEnabled( enable );
    ui->actionAdd_products->setEnabled( enable );
    ui->actionAdd_user->setEnabled( enable );
    ui->actionList_all_products->setEnabled( enable );
    ui->actionList_our_subscriptions->setEnabled( enable );
    ui->actionLogin->setEnabled( enable );
    ui->actionLogout->setEnabled( enable );
    ui->actionMakeOrders->setEnabled( enable );
    ui->actionSettings->setEnabled( enable );
    ui->actionShow_all_users->setEnabled( enable );
    ui->actionShow_all_orders->setEnabled( enable );
    ui->actionShow_expiry_date->setEnabled( enable );
    ui->actionUpdate_Change->setEnabled( enable );
    ui->actionStock->setEnabled( enable );
}

void CentralWindow::StartApplication()
{
    SetEnableActionButtons( false );
    LoadSettingsFile();
    ActivatePingTimer();
}

void CentralWindow::ActivatePingTimer()
{
    if( !server_ping_timer ) {
        server_ping_timer = new QTimer( this );
    }
    QObject::connect( server_ping_timer, &QTimer::timeout, this,
                      &CentralWindow::PingServerNetwork );
    int const ping_interval_in_minutes {
        app_settings.Value( utilities::SettingsValue::PingInterval, 4 ).toInt()
                * 1000 * 60
    };
    server_ping_timer->setInterval( ping_interval_in_minutes );
    server_ping_timer->start();
}

void CentralWindow::PingServerNetwork()
{
    using utilities::SettingsValue;
    int const notification_interval { // in minutes
        app_settings.Value( SettingsValue::PingNotifInterval ).toInt() * 60
    };
    QString const ping_url{ utilities::Endpoint::GetEndpoints().PingAddress() };
    if( ping_url.isEmpty() ){
        ui->statusBar->showMessage( "Unable to ping server" );
        return;
    }
    auto const request = utilities::GetRequestInterface( QUrl( ping_url ) );
    auto& network_manager = utilities::NetworkManager::GetNetwork();
    QNetworkReply* const reply = network_manager.get( request );
    QObject::connect( reply, &QNetworkReply::sslErrors, reply,
                      QOverload<QList<QSslError> const &>::of(
                                    &QNetworkReply::ignoreSslErrors ));
    QObject::connect( reply, &QNetworkReply::finished, [=]{
        // we're pinging, we don't need the response, don't show error message
        auto const response = utilities::GetJsonNetworkData( reply, false );
        if( !response.isEmpty() ) return;
        QTime const current_time{ QTime::currentTime() };
        if( time_interval.isNull() || // is this the first time?
                // has it been up to an hour since we were last warned?
                time_interval.secsTo( current_time ) >= notification_interval )
        {
            time_interval = current_time;
            PopUpNotifier* notifier{ new PopUpNotifier( this ) };
            notifier->setAttribute( Qt::WA_DeleteOnClose );
            notifier->SetPopUpText( "Unable to ping server" );
            notifier->show();
        }
    });
}

void CentralWindow::LoadSettingsFile()
{
    QVariant const url_map{
        app_settings.Value( utilities::SettingsValue::UrlMap )
    };
    QByteArray const endpoint_obj_settings{ url_map.toByteArray() };
    // is everything already set up?
    if( url_map.isValid() && !endpoint_obj_settings.isEmpty() ){
        auto& endpoints = utilities::Endpoint::GetEndpoints();
        QJsonObject const settings{
            QJsonDocument::fromJson( endpoint_obj_settings ).object()
        };
        utilities::Endpoint::ParseEndpointsFromJson( endpoints, settings );
        SetEnableActionButtons( false );
        PingServerNetwork();
        ui->actionLogin->setEnabled( true );
        return;
    }
    QString const message {
        "There are some setup that needs to be made before the application can "
        "be useful, please contact your administrator for help"
    };
    QMessageBox::information( this, "SU Setup", message );
    auto *admin_setup = new AdminSetupDialog;
    QMdiSubWindow* sub_window = workspace->addSubWindow( admin_setup );
    sub_window->setAttribute( Qt::WA_DeleteOnClose );
    QObject::connect( admin_setup, &AdminSetupDialog::accepted, [=]{
        QString const username = admin_setup->GetSUUsername();
        QString const password = admin_setup->GetSUPassword();
        QString const endpoint_url = admin_setup->GetServerEnpointsURL();
        QString const company_id = admin_setup->GetCompanysID();
        GetEndpointsFromServer( endpoint_url, username, password, company_id );
    });
    QObject::connect( admin_setup, &AdminSetupDialog::finished,
                      sub_window, &QMdiSubWindow::close );
    sub_window->setWindowTitle( "SU Setup" );
    CentralizeDisplayWidget( sub_window, QSize{ 400, 260 });
}

void CentralWindow::GetEndpointsFromServer( QString const &url,
                                            QString const &username,
                                            QString const &password,
                                            QString const &company_id)
{
    QProgressDialog* progress_dialog{
        new QProgressDialog( "Getting endpoints", "Cancel", 1, 100, this )
    };
    progress_dialog->show();
    QUrlQuery company_id_query {};
    company_id_query.addQueryItem( "company_id", company_id );
    auto endpoint_url = QUrl( url );
    endpoint_url.setQuery( company_id_query );

    QByteArray const credentials = QString( username + ":" + password )
            .toLocal8Bit().toBase64();
    auto request = utilities::GetRequestInterface( endpoint_url );
    QByteArray const authorization_data{
        QString( "Basic " + credentials ).toLocal8Bit()
    };
    request.setRawHeader( "Authorization", authorization_data );

    auto& network_manager = utilities::NetworkManager::GetNetwork();
    QNetworkReply* reply = network_manager.get( request );

    QObject::connect( reply, &QNetworkReply::downloadProgress,
                      [=]( qint64 const received, qint64 const total )
    {
        progress_dialog->setMaximum( static_cast<int>( total + ( total * 0.25 ) ) );
        progress_dialog->setValue( static_cast<int>( received ) );
    });
    QObject::connect( reply, &QNetworkReply::sslErrors, reply,
                      QOverload<QList<QSslError> const &>::of(
                          &QNetworkReply::ignoreSslErrors ) );
    QObject::connect( reply, &QNetworkReply::finished, progress_dialog,
                      &QProgressDialog::close );
    QObject::connect( reply, &QNetworkReply::finished, [=]{
        // get the response from the server and show the error message if any
        QJsonObject const response{
            utilities::GetJsonNetworkData( reply, true )
        };
        if( response.isEmpty() ) return;
        auto& endpoints = utilities::Endpoint::GetEndpoints();
        utilities::Endpoint::ParseEndpointsFromJson( endpoints, response );
        WriteEndpointsToPersistenceStorage( endpoints );
        PingServerNetwork();
        QMessageBox::information( this, "Restart", "Please restart application");
        qApp->quit();
    });
}

void CentralWindow::WriteEndpointsToPersistenceStorage(
        utilities::Endpoint const & endpoint )
{
    QByteArray const url_map { QJsonDocument( endpoint.ToJson() ).toJson() };
    app_settings.SetValue( utilities::SettingsValue::UrlMap, url_map );
    app_settings.SetValue( utilities::SettingsValue::OrderCount, 0 );
}

void CentralWindow::CentralizeDisplayWidget( QWidget * const widget,
                                             QSize const &size )
{
    widget->setMaximumSize( size );
    widget->setMinimumSize( size );
    widget->setWindowIcon( QApplication::style()->standardIcon(
                               QStyle::SP_DesktopIcon ) );
    widget->show();
    widget->setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter,
                                              widget->size(),
                                              QApplication::desktop()
                                              ->availableGeometry()));
}

void CentralWindow::OnOrderPollResultObtained( QJsonObject const & result )
{
    qint64 const order_count = result.value( "status" ).toObject()
            .value( "count" ).toInt();
    if( order_count > last_order_count ){
        QString const text {
            tr( "You have %1 new orders" ).arg( order_count - last_order_count )
        };
        PopUpNotifier* notifier{ new PopUpNotifier( this ) };
        notifier->setAttribute( Qt::WA_DeleteOnClose );
        notifier->SetPopUpText( text );
        notifier->show();
        QMessageBox::information( this, "Orders", text );
    }
    last_order_count = order_count;
}

void CentralWindow::StartOrderPolling()
{
    using utilities::SettingsValue;
    if( server_order_poll_timer ) return;
    server_order_poll_timer = new QTimer( this );
    last_order_count=app_settings.Value( SettingsValue::OrderCount, 0 ).toInt();

    QUrl const address{ utilities::Endpoint::GetEndpoints().GetOrderCount() };
    QObject::connect( server_order_poll_timer, &QTimer::timeout, [=]
    {
        // we cannot use `utilities::SendNetworkRequest` here as it shows a
        // progress dialog for all requests && we have to be in stealth mode
        QNetworkRequest const request{
            utilities::GetRequestInterface( address )
        };
        auto& network_manager=
            utilities::NetworkManager::GetNetworkWithCookie();
        QNetworkReply* reply{ network_manager.get( request ) };
        QObject::connect( reply, &QNetworkReply::sslErrors, reply,
                          QOverload<QList<QSslError> const &>::of(
                              &QNetworkReply::ignoreSslErrors ) );
        QObject::connect( reply, &QNetworkReply::finished, [=]
        {
            QJsonObject const result {
                utilities::GetJsonNetworkData( reply, false )
            };
            if( result.isEmpty() ) return;
            OnOrderPollResultObtained( result );
        });
    });
    int const poll_interval_seconds {
        app_settings.Value( SettingsValue::OrderPollInterval, 25 ).toInt()* 1000
    };
    server_order_poll_timer->start( poll_interval_seconds );
}

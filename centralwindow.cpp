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

#include "addproductdialog.hpp"
#include "adminsetupdialog.hpp"
#include "allproductsdialog.hpp"
#include "centralwindow.hpp"
#include "createstaffdialog.hpp"
#include "framelesswindow.h"
#include "orderwindow.hpp"
#include "resources.hpp"
#include "userlogindialog.hpp"
#include "ui_centralwindow.h"


CentralWindow::CentralWindow( QWidget *parent ) :
    QMainWindow( parent ), ui( new Ui::CentralWindow ), workspace{ new QMdiArea }
{
    ui->setupUi( this );
    setCentralWidget( workspace );
    ui->mainToolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

    QObject::connect( ui->actionExit, &QAction::triggered, this, &CentralWindow::close );
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
}

CentralWindow::~CentralWindow()
{
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

void CentralWindow::OnAddProductTriggered()
{
    AddProductDialog *product_dialog{ new AddProductDialog };
    auto sub_window = workspace->addSubWindow( product_dialog );
    sub_window->setAttribute( Qt::WA_DeleteOnClose );
    sub_window->setWindowTitle( "Add new product(s)" );
    connect( product_dialog, &AddProductDialog::finished, sub_window, &QMdiSubWindow::close );
    sub_window->showMaximized();
}

void CentralWindow::OnAddUserTriggered()
{
    CreateStaffDialog *staff_dialog{ new CreateStaffDialog( this ) };
    QMdiSubWindow *sub_window{ workspace->addSubWindow( staff_dialog ) };
    sub_window->setAttribute( Qt::WA_DeleteOnClose );
    QAction *const object_sender = qobject_cast<QAction*>( sender() );
    object_sender->setDisabled( true );
    if( object_sender == ui->actionAdd_administrator ){
        sub_window->setWindowTitle( "Create administrator" );
        staff_dialog->SetUserRole( UserRole::Administrator );
    } else {
        sub_window->setWindowTitle( "Create staff" );
        staff_dialog->SetUserRole( UserRole::BasicUser );
    }
    connect( sub_window, &QMdiSubWindow::destroyed, [=]{ object_sender->setEnabled( true ); });
    QSize const min_max_size{ 516, 370 };
    sub_window->setMinimumSize( min_max_size );
    sub_window->setMaximumSize( min_max_size );
    sub_window->show();
}

void CentralWindow::OnOrderActionTriggered()
{
    QAction *const object_sender = qobject_cast<QAction*>( sender() );
    object_sender->setDisabled( true );
    ui->menuOrders->setEnabled( false );
    OrderWindow* order_window{ new OrderWindow };
    QMdiSubWindow *sub_window = workspace->addSubWindow( order_window );
    sub_window->setAttribute( Qt::WA_DeleteOnClose );
    sub_window->setWindowTitle( "Orders" );
    sub_window->showMaximized();
    QObject::connect( order_window, &OrderWindow::destroyed, [=]{
        if( ui->menuOrders ) ui->menuOrders->setEnabled( true );
        object_sender->setEnabled( true );
    });
}

void CentralWindow::OnListAllProductsTriggered()
{
    ui->actionList_all_products->setDisabled( true );
    AllProductsDialog *dialog{ new AllProductsDialog };
    QMdiSubWindow *parent_window{ workspace->addSubWindow( dialog ) };
    parent_window->setAttribute( Qt::WA_DeleteOnClose );
    parent_window->setWindowTitle( "Our items" );
    parent_window->show();
    dialog->DownloadProducts();
    QObject::connect( parent_window, &QMdiSubWindow::destroyed, [=]{
        if( ui->actionList_all_products ) ui->actionList_all_products->setEnabled( true );
    });
}

void CentralWindow::LogUserIn( QString const & username, QString const & password )
{
    QProgressDialog* progress_dialog{ new QProgressDialog( "Logging you in", "Cancel",
                                                           1, 100, this ) };
    progress_dialog->show();

    auto const & endpoints = utilities::Endpoint::GetEndpoints();
    QString const token = endpoints.CompanyToken();
    QUrl const login_url { QUrl::fromUserInput( endpoints.LoginTo() ) };
    QJsonObject const login_object { { "username", username }, { "password", password },
                                     { "token", token }};
    QJsonDocument const document{ login_object };
    auto const request = utilities::PostRequestInterface( login_url );
    auto& network_manager = utilities::NetworkManager::GetNetwork();
    QNetworkReply* reply = network_manager.post( request, document.toJson() );
    QObject::connect( reply, &QNetworkReply::downloadProgress, [=](qint64 received, qint64 total)
    {
        progress_dialog->setMaximum( total + ( total * 0.25 ) );
        progress_dialog->setValue( received );
    });
    QObject::connect( reply, &QNetworkReply::finished, progress_dialog, &QProgressDialog::close );
    QObject::connect( progress_dialog, &QProgressDialog::canceled, reply, &QNetworkReply::abort );
    QObject::connect( reply, &QNetworkReply::finished, [=]{
        // get the response from the server and show the error message if any
        QJsonObject const response = utilities::GetJsonNetworkData( reply, true );
        bool const logged_in = !response.isEmpty();
        if( logged_in ){
            SetEnableActionButtons( true );
            ui->actionLogin->setEnabled( !logged_in );
            ui->actionLogout->setEnabled( logged_in );
            QMessageBox::information( this, "Login", "You're successfully logged in" );
            auto& session_cookie = utilities::NetworkManager::GetSessionCookie();
            utilities::NetworkManager::SetNetworkCookie( session_cookie, reply );
        }
    });
}

void CentralWindow::OnLoginButtonClicked()
{
    ui->actionLogin->setEnabled( false );
    UserLoginDialog *user_dialog = new UserLoginDialog;
    user_dialog->SetCompanyID( utilities::Endpoint::GetEndpoints().CompanyID() );
    QMdiSubWindow* subwindow = workspace->addSubWindow( user_dialog );
    subwindow->setAttribute( Qt::WA_DeleteOnClose );
    QObject::connect( user_dialog, &UserLoginDialog::accepted, [=]{
        auto const company_id = utilities::Endpoint::GetEndpoints().CompanyID();
        QString const username = user_dialog->GetUsername().trimmed() + "@" + company_id;
        QString const password = user_dialog->GetPassword();
        LogUserIn( username, password );
    });
    QObject::connect( user_dialog, &UserLoginDialog::accepted, subwindow, &QMdiSubWindow::close );
    subwindow->setWindowTitle( "Login" );
    CentralizeDisplayWidget( subwindow, QSize{ 400, 260 });
}

void CentralWindow::OnLogoutButtonClicked()
{
    if( QMessageBox::question( this, "Logout", "Are you sure you want to logout?" ) ==
            QMessageBox::No )
    {
        return;
    }
    SetEnableActionButtons( false );
    ui->actionLogin->setEnabled( true );
    auto const & endpoints = utilities::Endpoint::GetEndpoints();
    QNetworkAccessManager& network_manager{ utilities::NetworkManager::GetNetwork() };
    auto& session_cookie = utilities::NetworkManager::GetSessionCookie();
    network_manager.setCookieJar( &session_cookie );
    // QNetworkAccessManager takes owner of its cookie jar, so let's snatch it from it
    session_cookie.setParent( nullptr );
    QNetworkRequest const request{ utilities::GetRequestInterface( endpoints.LogoutFrom() ) };
    network_manager.get( request );
    // let's clear the cookies
    session_cookie.setCookiesFromUrl( QList<QNetworkCookie>{}, endpoints.LoginTo() );

    foreach( QMdiSubWindow* const sub_window, workspace->subWindowList() ){
        sub_window->close();
    }
    SetEnableActionButtons( false );
    ui->actionLogin->setEnabled( true );
    QMessageBox::information( this, "Logout", "You've been logged out successfully" );
}

void CentralWindow::SetEnableCentralWindowBars( bool const enabled )
{
    ui->mainToolBar->setEnabled( enabled );
    ui->menuBar->setEnabled( enabled );
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
    ui->actionSettings->setEnabled( enable );
    ui->actionShow_all_users->setEnabled( enable );
    ui->actionShow_all_orders->setEnabled( enable );
    ui->actionShow_expiry_date->setEnabled( enable );
    ui->actionSubscribe->setEnabled( enable );
    ui->actionUpdate_Change->setEnabled( enable );
    ui->actionUpdate_product->setEnabled( enable );
    ui->actionOrders->setEnabled( enable );
}

void CentralWindow::StartApplication()
{
    SetEnableCentralWindowBars( false );
    LoadSettingsFile();
}

void CentralWindow::PingServerNetwork()
{
    QString const ping_url{ utilities::Endpoint::GetEndpoints().PingAddress() };
    if( ping_url.isEmpty() ){
        ui->statusBar->showMessage( "Unable to ping server" );
        return;
    }
    auto const request = utilities::GetRequestInterface( QUrl( ping_url ) );
    auto& network_manager = utilities::NetworkManager::GetNetwork();
    QNetworkReply* const reply = network_manager.get( request );
    QObject::connect( reply, &QNetworkReply::finished, [=]{
        // we're only pinging, we don't need the response, so don't show error message
        auto const response = utilities::GetJsonNetworkData( reply, false );
        if( response.isEmpty() ) ui->statusBar->showMessage( "Unable to ping server" );
    });
}

void CentralWindow::LoadSettingsFile()
{
    QSettings& app_settings = utilities::ApplicationSettings::GetAppSettings();
    QVariant const url_map{ app_settings.value( "url_map" ) };
    QByteArray const endpoint_obj_settings{ url_map.toByteArray() };
    if( url_map.isValid() && !endpoint_obj_settings.isEmpty() ){ // is everything already set up?
        auto &endpoints = utilities::Endpoint::GetEndpoints();
        QJsonObject const settings{ QJsonDocument::fromJson( endpoint_obj_settings ).object() };
        utilities::Endpoint::ParseEndpointsFromJson( endpoints, settings );
        PingServerNetwork();
        SetEnableCentralWindowBars( true );
        SetEnableActionButtons( false );
        ui->actionLogin->setEnabled( true );
        return;
    }
    QString const message { "There are some setup that needs to be made before the "
                            "application can be useful, please contact your administrator "
                            "for help" };
    QMessageBox::information( this, "SU Setup", message );
    AdminSetupDialog *admin_setup = new AdminSetupDialog;
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

void CentralWindow::GetEndpointsFromServer( QString const &url, QString const &username,
                                            QString const &password, QString const &company_id)
{
    QProgressDialog* progress_dialog{ new QProgressDialog( "Getting endpoints", "Cancel",
                                                           1, 100, this ) };
    progress_dialog->show();
    QUrlQuery company_id_query {};
    company_id_query.addQueryItem( "company_id", company_id );
    auto endpoint_url = QUrl( url );
    endpoint_url.setQuery( company_id_query );

    QByteArray const credentials = QString( username + ":" + password ).toLocal8Bit().toBase64();
    auto request = utilities::GetRequestInterface( endpoint_url );
    QByteArray const authorization_data{ QString( "Basic " + credentials ).toLocal8Bit() };
    request.setRawHeader( "Authorization", authorization_data );

    auto& network_manager = utilities::NetworkManager::GetNetwork();
    QNetworkReply* reply = network_manager.get( request );

    QObject::connect( reply, &QNetworkReply::downloadProgress, [=](qint64 received, qint64 total)
    {
        progress_dialog->setMaximum( total + ( total * 0.25 ) );
        progress_dialog->setValue( received );
    });
    QObject::connect( reply, &QNetworkReply::finished, progress_dialog, &QProgressDialog::close );
    QObject::connect( reply, &QNetworkReply::finished, [=]{
        // get the response from the server and show the error message if any
        QJsonObject const response = utilities::GetJsonNetworkData( reply, true );
        if( response.isEmpty() ) return;
        auto& endpoints = utilities::Endpoint::GetEndpoints();
        utilities::Endpoint::ParseEndpointsFromJson( endpoints, response );
        WriteEndpointsToPersistenceStorage( endpoints );
        PingServerNetwork();
        SetEnableCentralWindowBars( true );
        ui->actionSettings->setEnabled( true );
        ui->actionLogout->setDisabled( true );
    });
}

void CentralWindow::WriteEndpointsToPersistenceStorage( utilities::Endpoint const & endpoint )
{
    auto& app_settings = utilities::ApplicationSettings::GetAppSettings();
    QByteArray const url_map { QJsonDocument( endpoint.ToJson() ).toJson() };
    app_settings.setValue( "url_map", url_map );
}

void CentralWindow::CentralizeDisplayWidget( QWidget * const widget, QSize const &size )
{
    widget->setMaximumSize( size );
    widget->setMinimumSize( size );
    widget->setWindowIcon( QApplication::style()->standardIcon( QStyle::SP_DesktopIcon ) );
    widget->show();
    widget->setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter,
                                              widget->size(),
                                              QApplication::desktop()->availableGeometry()));
}

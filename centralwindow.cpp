#include "centralwindow.hpp"
#include "ui_centralwindow.h"
#include "resources.hpp"
#include "createstaffdialog.hpp"
#include "adminsetupdialog.hpp"

#include <QByteArray>
#include <QMdiSubWindow>
#include <QSettings>
#include <QVariant>
#include <QUrlQuery>
#include <QMessageBox>
#include <QDesktopWidget>

CentralWindow::CentralWindow( QWidget *parent ) :
    QMainWindow( parent ), ui( new Ui::CentralWindow ), workspace{ new QMdiArea }
{
    ui->setupUi( this );
    setCentralWidget( workspace );
    ui->mainToolBar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

    QObject::connect( ui->actionExit, &QAction::triggered, this, &CentralWindow::close );
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

void CentralWindow::SetCentralWindowBars( bool const enabled )
{
    ui->mainToolBar->setEnabled( enabled );
    ui->menuBar->setEnabled( enabled );
}

void CentralWindow::StartApplication()
{
    LoadSettingsFile();
    SetCentralWindowBars( false );
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
        else SetCentralWindowBars( true );
    });
}

void CentralWindow::LoadSettingsFile()
{
    QSettings& app_settings = utilities::ApplicationSettings::GetAppSettings();
    QVariant const url_map = app_settings.value( "url_map" );
    if( url_map.isValid() ){ // is everything already set up?
        auto &endpoints = utilities::Endpoint::GetEndpoints();
        utilities::Endpoint::ParseEndpointsFromJson( endpoints, url_map.toJsonObject() );
        PingServerNetwork();
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

    QSize const min_max_size{ 400, 260 };
    sub_window->setMaximumSize( min_max_size );
    sub_window->setMinimumSize( min_max_size );
    sub_window->setWindowTitle( "SU Setup" );
    sub_window->setWindowIcon( QApplication::style()->standardIcon( QStyle::SP_DesktopIcon ) );
    sub_window->show();
    sub_window->setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter,
                                                  sub_window->size(),
                                                  QApplication::desktop()->availableGeometry()));
}

void CentralWindow::GetEndpointsFromServer( QString const &url, QString const &username,
                                            QString const &password, QString const &company_id)
{
    QUrlQuery company_id_query {};
    company_id_query.addQueryItem( "company_id", company_id );
    auto endpoint_url = QUrl( url );
    endpoint_url.setQuery( company_id_query );

    QByteArray const credentials = QString( username + ":" + password ).toLocal8Bit().toBase64();
    auto request = utilities::GetRequestInterface( endpoint_url );
    request.setRawHeader( "Authorization", QString( "Basic " + credentials ).toLocal8Bit() );

    auto& network_manager = utilities::NetworkManager::GetNetwork();
    QNetworkReply* reply = network_manager.get( request );
    QObject::connect( reply, &QNetworkReply::finished, [=]{
        // get the response from the server and show the error message if any
        QJsonObject response = utilities::GetJsonNetworkData( reply, true );
        if( response.isEmpty() ) return;
        auto& endpoints = utilities::Endpoint::GetEndpoints();
        utilities::Endpoint::ParseEndpointsFromJson( endpoints, response );
        WriteEndpointsToPersistenceStorage( endpoints );
        PingServerNetwork();
    });
}

void CentralWindow::WriteEndpointsToPersistenceStorage( utilities::Endpoint const & endpoint )
{
    auto& app_settings = utilities::ApplicationSettings::GetAppSettings();
    app_settings.setValue( "url_map", endpoint.ToJson() );
}

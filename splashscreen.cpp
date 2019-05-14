#include <QPixmap>
#include <QSettings>
#include <QString>
#include <QByteArray>
#include <QNetworkReply>
#include <QPair>

#include "splashscreen.hpp"
#include "adminsetupdialog.hpp"
#include "resources.hpp"

SplashScreen::SplashScreen( QApplication& app, QObject *parent ) : QObject(parent),
    main_app{ app }, splash_screen{ nullptr }, has_error{ false }
{
    QPixmap splash_screen_pixmap{ ":/all_images/images/tragel.png" };
    splash_screen = new QSplashScreen{ splash_screen_pixmap.scaled( 600, 400 ) };
    tasks.insert( "Loading setup file", &SplashScreen::LoadSettingsFile );
    tasks.insert( "Pinging network for availability", &SplashScreen::PingServerNetwork );
}

void SplashScreen::DisplaySplash()
{
    for( QPair<QString const, void(SplashScreen::*)()> const & task: tasks ){
        splash_screen->showMessage( task.first, Qt::AlignBottom | Qt::AlignHCenter, Qt::blue );
        (this->*task)();
    }
}

void SplashScreen::PingServerNetwork()
{
    QString const pinging_address = utilities::Endpoint::GetEndpoints().PingAddress();
    if( pinging_address.isEmpty() ){
        has_error = true;
        emit done();
        return;
    }
    auto request = utilities::GetRequestInterface( pinging_address );
    auto reply = utilities::NetworkManager::GetNetwork().get( request );
    QObject::connect( reply, &QNetworkReply::finished, []{
        auto response = utilities::GetJsonNetworkData( qobject_cast<QNetworkReply*>( sender() ) );
        if( response.isEmpty() ) has_error = true;
        emit done();
    });
}
void SplashScreen::LoadSettingsFile()
{
    QSettings app_settings{ QSettings::UserScope, "Froist Inc.", "Tragel" };
    QVariant const url_map = app_settings.value( "url_map" );
    if( url_map.isNull() ){
        AdminSetupDialog setup_dialog{};
        if( setup_dialog.exec() == QDialog::Accepted ){
            QString const su_username = setup_dialog.GetSUUsername();
            QString const su_password = setup_dialog.GetSUPassword();
            QString const endpoint_url = setup_dialog.GetServerEnpointsURL();
            GetEndpointsFromServer( endpoint_url, su_username, su_password );
        } else {
            has_error = true;
            return;
        }
    } else {
        auto &endpoints = utilities::Endpoint::GetEndpoints();
        utilities::Endpoint::ParseEndpointsFromJson( endpoints, url_map );
    }
}

void SplashScreen::GetEndpointsFromServer( QString const & url,
                                           QString const & username,
                                           QString const & password )
{
    auto request = utilities::GetRequestInterface( url );
    auto authorization_data = QString( username + ":" + password ).toLocal8Bit().toBase64();
    request.setRawHeader( "Authorization", authorization_data );
    QNetworkReply* reply = utilities::NetworkManager::GetNetwork().get( request );
    QObject::connect( reply, &QNetworkReply::finished, [=]{
        auto response = utilities::GetJsonNetworkData( reply );
        if( response.isEmpty() ){
            has_error = true;
            return;
        }
        auto &endpoints = utilities::Endpoint::GetEndpoints();
        utilities::Endpoint::ParseEndpointsFromJson( endpoints, response );
        WriteEndpointsToPersistenceStorage( endpoints );
    });
}

void SplashScreen::WriteEndpointsToPersistenceStorage( utilities::Endpoint const & endpoints )
{
    QSettings app_settings{ QSettings::UserScope, "Froist Inc.", "Tragel" };
    app_settings.setValue( "url_map", QVariant{ endpoints.ToJson() } );
}

#include "resources.hpp"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QByteArray>
#include <QSsl>
#include <QSslCertificate>

namespace utilities {

ApplicationSettings& ApplicationSettings::GetAppSettings()
{
    static ApplicationSettings app_settings{};
    return app_settings;
}

QSettings& ApplicationSettings::GetSettings( QString const & organisation,
                                             QString const & application )
{
    static QSettings settings{ organisation, application };
    return settings;
}

ApplicationSettings::ApplicationSettings():
    settings{ GetSettings( "Froist Inc.", "LineCounter" ) }
{
}

QString SettingsValueToString( SettingsValue const val )
{
    switch( val ) {
    case SettingsValue::OrderCount:
        return "order_count";
    case SettingsValue::UrlMap:
        return "url_map";
    case SettingsValue::PingInterval:
        return "ping_interval";
    case SettingsValue::OrderPollInterval:
        return "order_poll_interval";
    case SettingsValue::AllowShortcut:
        return "allow_shortcut";
    case SettingsValue::ConfirmDeletion:
        return "confirm_deletion";
    case SettingsValue::ShowSplash:
        return "show_splash";
    case SettingsValue::PingNotifInterval:
        return "ping_ntf_interval";
    }
    abort();
}

void ApplicationSettings::SetValue( SettingsValue const data,
                                    QVariant const & value )
{
    auto const key{ SettingsValueToString( data ) };
    settings.setValue( key, value );
}

QVariant ApplicationSettings::Value( SettingsValue const value,
                                     QVariant const& default_ ) const
{
    auto const key{ SettingsValueToString( value ) };
    return settings.value( key, default_ );
}

bool ParsePageUrls( QJsonObject const &result, PageQuery & page_query )
{
    QJsonObject const page_info{ result.value( "pages" ).toObject() };
    uint& total_result = page_query.total_result;
    total_result = page_info.value( "total" ).toInt();
    uint& page_total = page_query.number_of_pages;
    page_total = page_info.value( "pages" ).toInt();

    if( total_result == 0 || page_total == 0 ) return false;

    page_query.first_url = page_info.value( "first_url" ).toString();
    page_query.last_url = page_info.value( "last_url" ).toString();
    page_query.result_per_page = page_info.value( "per_page" ).toInt();
    if( page_total > 0 ){
        QString const next_url = page_info.value( "next_url" ).toString();
        QString previous_url {};
        if( page_total != 0 ){
            previous_url = page_info.value( "prev_url" ).toString();
        }
        uint const page_number = page_info.value( "page" ).toInt();
        utilities::UrlData next_page_info{
            next_url, previous_url, page_number
        };
        page_query.other_url = std::move( next_page_info );
    }
    return true;
}

QNetworkRequest GetRequestInterface( QUrl const &address )
{
    QNetworkRequest request { address };
    if( address.scheme() == "https" ){
        auto ssl_config{ QSslConfiguration::defaultConfiguration() };
        ssl_config.setProtocol( QSsl::TlsV1_0OrLater );
        request.setSslConfiguration( ssl_config );
    }
    request.setHeader( QNetworkRequest::UserAgentHeader, USER_AGENT );
    return request;
}

QNetworkRequest PostRequestInterface( QUrl const &address )
{
    QNetworkRequest request{ address };
    if( address.scheme() == "https" ){
        auto ssl_config{ QSslConfiguration::defaultConfiguration() };
        ssl_config.setProtocol( QSsl::TlsV1_0OrLater );
        request.setSslConfiguration( ssl_config );
    }
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
    request.setHeader( QNetworkRequest::UserAgentHeader, USER_AGENT );
    return request;
}

QPair<QNetworkRequest, QByteArray> PostImageRequestInterface(
        QUrl const &address, QVector<QString> const & data )
{
    if( data.size() < 2 ) return {};
    QString const filename{ data[0] };
    QString const file_format{ data[1] };

    QNetworkRequest request{ address };
    request.setHeader( QNetworkRequest::UserAgentHeader, USER_AGENT );
    QString const boundary{ "--TragelBoundary7MA4YWxkTrZu7gW" }; // random string
    QByteArray payload{ QString( "--" + boundary + "\r\n" ).toLocal8Bit() };
    payload.append( QString( "Content-Disposition: form-data; name=\"photo\"; "
                             "filename=\"" + filename + "\"\r\nContent-Type: "
                                                        "image/" + file_format
                             + "\r\n\r\n" ).toLocal8Bit() );
    QFile file{ filename };
    if( !file.open( QIODevice::ReadOnly ) ) return {};
    payload.append( file.readAll() );
    payload.append( "\r\n" );
    payload.append( "--" + boundary + "--\r\n" );
    request.setRawHeader( "Content-Type",
                          QString( "multipart/form-data; boundary="
                                   + boundary ).toLocal8Bit() );
    request.setRawHeader( "Content-Length",
                          QByteArray::number( payload.length() ) );
    return { request, payload };
}

QJsonObject GetJsonNetworkData( QNetworkReply* network_reply,
                                bool show_error_message )
{
    if( !network_reply ) return {};
    if( network_reply->error() != QNetworkReply::NoError ){
        if( show_error_message ){
            QByteArray const network_response = network_reply->readAll();
            QJsonDocument const json_document{
                QJsonDocument::fromJson( network_response )
            };
            if( json_document.isNull() ){
                QMessageBox::critical( nullptr, "Server's response",
                                       network_reply->errorString() );
            } else {
                QJsonObject const server_error{ json_document.object() };
                QString error_message { server_error.contains( "status" ) ?
                                server_error.value( "status").toString():
                                server_error.value( "message" ).toString()
                                      };
                if( error_message.isEmpty() ){
                    error_message = network_reply->errorString();
                }
                QMessageBox::critical( nullptr, "Server says", error_message );
            }
        }
        return {};
    }
    QByteArray const network_response = network_reply->readAll();
    QJsonDocument const json_document{
        QJsonDocument::fromJson( network_response )
    };
    if( json_document.isNull() ) return {};
    return json_document.object();
}

QJsonObject ProductData::ToJson() const
{
    QJsonObject object{ { "price", price }, {"name", name },
                        { "thumbnail", thumbnail_location },
                        { "url", constant_url } };
    if( product_id != 0 ) object.insert( "id", product_id );
    return object;
}

bool operator==( ProductData const & a, ProductData const & b )
{
    return a.name == b.name && a.thumbnail_location == b.thumbnail_location
            && a.constant_url == b.constant_url && a.price == b.price;
}
}

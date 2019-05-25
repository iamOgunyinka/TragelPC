#include "resources.hpp"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>

namespace utilities {

QNetworkRequest GetRequestInterface( QUrl const &address )
{
    QNetworkRequest request { address };
    request.setHeader( QNetworkRequest::UserAgentHeader, USER_AGENT );
    return request;
}

QNetworkRequest PostRequestInterface( QUrl const &address )
{
    QNetworkRequest request{ address };
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
    request.setHeader( QNetworkRequest::UserAgentHeader, USER_AGENT );
    return request;
}

QJsonObject GetJsonNetworkData( QNetworkReply* network_reply, bool show_error_message )
{
    if( !network_reply ) return {};
    if( network_reply->error() != QNetworkReply::NoError ){
        if( show_error_message )
            QMessageBox::critical( nullptr, "Server's response", network_reply->errorString() );
        return {};
    }
    QByteArray const network_response = network_reply->readAll();
    QJsonDocument const json_document = QJsonDocument::fromJson( network_response );

    if( json_document.isNull() ) return {};
    return json_document.object();
}
}

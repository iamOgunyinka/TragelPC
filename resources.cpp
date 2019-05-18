#include "resources.hpp"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QDebug>

namespace utilities {

QNetworkRequest GetRequestInterface( QUrl const &address )
{
    qDebug() << address;
    return QNetworkRequest{ address };
}

QNetworkRequest PostRequestInterface( QString const &address )
{
    QNetworkRequest request{ QUrl{ address } };
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
    return request;
}

QJsonObject GetJsonNetworkData( QNetworkReply* network_reply, bool show_error_message )
{
    if( network_reply->error() != QNetworkReply::NoError ){
        if( show_error_message )
            QMessageBox::critical( nullptr, "Server's response", network_reply->errorString() );
        return QJsonObject{};
    }
    QByteArray const network_response = network_reply->readAll();
    QJsonDocument const json_document = QJsonDocument::fromBinaryData( network_response );
    if( json_document.isNull() ) return QJsonObject{};
    return json_document.object();
}
}

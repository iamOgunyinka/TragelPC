#include "resources.hpp"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>

namespace utilities {

QNetworkRequest GetRequestInterface( QString const &address )
{
    QNetworkRequest request{ QUrl{ address } };
    return request;
}

QNetworkRequest PostRequestInterface( QString const &address )
{
    QNetworkRequest request{ QUrl{ address } };
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
    return request;
}

QJsonObject GetJsonNetworkData( QNetworkReply* network_reply )
{
    if( network_reply->error() != QNetworkReply::NoError ){
        return QJsonObject{};
    }
    QByteArray const network_response = network_reply->readAll();
    QJsonDocument const json_document = QJsonDocument::fromBinaryData( network_response );
    if( json_document.isNull() ) return QJsonObject{};
    return json_document.object();
}
}

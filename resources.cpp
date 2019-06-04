#include "resources.hpp"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QByteArray>

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

QPair<QNetworkRequest, QByteArray> PostImageRequestInterface( QUrl const &address,
                                                              QList<QString> const & data )
{
    if( data.size() < 2 ) return {};
    QString const filename{ data[0] };
    QString const file_format{ data[1] };

    QNetworkRequest request{ address };
    request.setHeader( QNetworkRequest::UserAgentHeader, USER_AGENT );
    QString const boundary{ "--TragelBoundary7MA4YWxkTrZu7gW" }; // random string
    QByteArray payload{ QString( "--" + boundary + "\r\n" ).toLocal8Bit() };
    payload.append( QString( "Content-Disposition: form-data; name=\"photo\"; filename=\""
                             + filename + "\"\r\nContent-Type: image/" + file_format
                             + "\r\n\r\n" ).toLocal8Bit() );
    QFile file{ filename };
    if( !file.open( QIODevice::ReadOnly ) ) return {};
    payload.append( file.readAll() );
    payload.append( "\r\n" );
    payload.append( "--" + boundary + "--\r\n" );
    request.setRawHeader( "Content-Type",
                          QString( "multipart/form-data; boundary=" + boundary ).toLocal8Bit() );
    request.setRawHeader( "Content-Length", QByteArray::number( payload.length() ) );
    return { request, payload };
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

#include "productuploaddialog.hpp"
#include "ui_productuploaddialog.h"
#include "uploadwidget.hpp"

#include <QFileInfo>

ProductUploadDialog::ProductUploadDialog( QList<utilities::ProductData> &products,
                                          QWidget *parent ) :
    QDialog{ parent }, ui{ new Ui::ProductUploadDialog }, products_{ products }
{
    ui->setupUi( this );
    this->setWindowTitle( "Uploads" );
}

ProductUploadDialog::~ProductUploadDialog()
{
    delete ui;
}

void ProductUploadDialog::StartUpload()
{
    for( int index = 0; index != products_.size(); ++index ){
        UploadProductImagesByIndex( index );
    }
}

void ProductUploadDialog::UploadProductImagesByIndex( int const index )
{
    utilities::ProductData &product{ products_[ index ] };
    if( product.thumbnail_location.isEmpty() || product.thumbnail_location.startsWith( "http" ) )
        return;

    QNetworkAccessManager& network_manager{ utilities::NetworkManager::GetNetwork() };
    QNetworkCookieJar& session_cookie{ utilities::NetworkManager::GetSessionCookie() };
    network_manager.setCookieJar( &session_cookie );
    session_cookie.setParent( nullptr ); // don't take ownership of the session cookie

    QFileInfo const file_info{ product.thumbnail_location };
    QString const file_type{ file_info.suffix() };
    QList<QString> const metadata{ product.thumbnail_location, file_type };
    utilities::Endpoint& endpoint{ utilities::Endpoint::GetEndpoints() };
    QUrl const upload_address{ endpoint.UploadPhoto() };
    auto request_payload{ utilities::PostImageRequestInterface( upload_address, metadata ) };
    UploadWidget *upload_widget{ new UploadWidget };
    ui->widget_layout->addWidget( upload_widget );
    upload_widget->SetMessage( "Uploading image now" );
    if( request_payload.second.isNull() ){
        upload_widget->SetProgress( 100, 100 );
        upload_widget->SetMessage( "Unable to upload image" );
        return;
    }
    QNetworkReply* reply{ network_manager.post( request_payload.first, request_payload.second) };
    QObject::connect( reply, &QNetworkReply::uploadProgress, [=]( qint64 received, qint64 total )
    {
        upload_widget->SetProgress( received, total );
    });
    QObject::connect( reply, &QNetworkReply::finished, [=]{
        if( reply->error() != QNetworkReply::NoError ){
            upload_widget->SetMessage( reply->errorString() );
            upload_widget->SetProgress( 100, 100 );
        } else {
            QJsonObject const response{ utilities::GetJsonNetworkData( reply ) };
            this->products_[index].thumbnail_location = response.value( "url" ).toString();
            upload_widget->SetMessage( response.value( "message" ).toString() );
            upload_widget->setDisabled( true );
        }
        SendUploadCompleteSignalWhenDone();
    });
}

void ProductUploadDialog::SendUploadCompleteSignalWhenDone()
{
    for( int index = 0; index != products_.size(); ++index ){

    }
}

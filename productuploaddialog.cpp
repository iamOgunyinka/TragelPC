#include "productuploaddialog.hpp"
#include "ui_productuploaddialog.h"
#include "uploadwidget.hpp"

#include <QFileInfo>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonDocument>
#include <QProgressDialog>

ProductUploadDialog::ProductUploadDialog(
        QVector<utilities::ProductData> &products,
        QWidget *parent ):
    QDialog{ parent }, ui{ new Ui::ProductUploadDialog }, products_{ products },
    product_upload_status_( products_.size(), UploadStatus::PendingUpload )
{
    ui->setupUi( this );
    this->setWindowTitle( "Uploads" );
    QObject::connect( this, &ProductUploadDialog::image_upload_completed,
                      this, &ProductUploadDialog::OnUploadCompleted );
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

void ProductUploadDialog::OnUploadCompleted( bool const has_error )
{
    if( !has_error ){
        QJsonArray product_list {};
        for( auto const & product: products_ ){
            product_list.append( product.ToJson() );
        }
        UploadProducts( product_list );
        return;
    }
    QMessageBox::critical( this, "Upload",
                           "There were errors uploading some of the pictures" );
}

void ProductUploadDialog::UploadProducts( QJsonArray const & product_list )
{
    QNetworkAccessManager& network_manager{
        utilities::NetworkManager::GetNetworkWithCookie()
    };
    QUrl const address{ utilities::Endpoint::GetEndpoints().AddProduct() };
    QNetworkRequest const request{ utilities::PostRequestInterface( address ) };
    QByteArray const payload{ QJsonDocument( product_list ).toJson() };
    QNetworkReply* const reply{ network_manager.post( request, payload ) };
    QProgressDialog *progress_dialog{
        new QProgressDialog( "Uploading", "Cancel", 1, 100, this )
    };
    progress_dialog->setAttribute( Qt::WA_DeleteOnClose );
    this->connect( progress_dialog, &QProgressDialog::canceled, reply,
                   &QNetworkReply::abort );
    this->connect( reply, &QNetworkReply::uploadProgress,
                   [=]( qint64 received, qint64 total ){
        progress_dialog->setMaximum( total + ( total * 0.25 ) );
        progress_dialog->setValue( received );
    });
    this->connect( reply, &QNetworkReply::finished, progress_dialog,
                   &QProgressDialog::close );
    this->connect( reply, &QNetworkReply::finished, [=]{
        QJsonObject const response{
            utilities::GetJsonNetworkData( reply, true )
        };
        emit uploads_completed( response.isEmpty() );
    });
    progress_dialog->show();
}

void ProductUploadDialog::UploadProductImagesByIndex( int const index )
{
    utilities::ProductData &product{ products_[ index ] };
    if( product.thumbnail_location.isEmpty()
            || product.thumbnail_location.startsWith( "http" ) ){
        product_upload_status_[index] = UploadStatus::Uploaded;
        SendUploadCompleteSignalWhenDone();
        return;
    }

    QNetworkAccessManager& network_manager{
        utilities::NetworkManager::GetNetworkWithCookie()
    };
    QString const file_type{ QFileInfo{ product.thumbnail_location }.suffix() };
    QVector<QString> const metadata{ product.thumbnail_location, file_type };
    utilities::Endpoint& endpoint{ utilities::Endpoint::GetEndpoints() };
    QUrl const upload_address{ endpoint.UploadPhoto() };

    UploadWidget *upload_widget{ new UploadWidget };
    upload_widget->setMaximumHeight( 70 );
    upload_widget->SetMessage( "Uploading image now" );
    ui->widget_layout->addWidget( upload_widget );
    auto request_payload{
        utilities::PostImageRequestInterface( upload_address, metadata )
    };
    if( request_payload.second.isNull() ){
        upload_widget->SetProgress( 100, 100 );
        upload_widget->SetMessage( "Unable to upload image" );
        product_upload_status_[index] = UploadStatus::ErrorEncountered;
        SendUploadCompleteSignalWhenDone();
        return;
    }
    QNetworkReply* reply{
        network_manager.post( request_payload.first, request_payload.second )
    };
    QObject::connect( reply, &QNetworkReply::uploadProgress,
                      [=]( qint64 const received, qint64 const total )
    {
        upload_widget->SetProgress( received, total );
    });

    QObject::connect( reply, &QNetworkReply::finished, [=]{
        if( reply->error() != QNetworkReply::NoError ){
            upload_widget->SetMessage( reply->errorString() );
            upload_widget->SetProgress( 100, 100 );
            product_upload_status_[index] = UploadStatus::ErrorEncountered;
        } else {
            QJsonObject const response{
                utilities::GetJsonNetworkData( reply )
            };
            this->products_[index].thumbnail_location = response.value( "url" )
                    .toString();
            upload_widget->SetMessage( response.value( "message" ).toString() );
            upload_widget->setDisabled( true );
            product_upload_status_[index] = UploadStatus::Uploaded;
        }
        SendUploadCompleteSignalWhenDone();
    });
}

void ProductUploadDialog::SendUploadCompleteSignalWhenDone()
{
    bool has_error{ false };
    for( auto const & status: product_upload_status_ ){
        if( status == UploadStatus::PendingUpload ) return;
        else if( status == UploadStatus::ErrorEncountered ) has_error = true;
    }
    emit image_upload_completed( has_error );
}

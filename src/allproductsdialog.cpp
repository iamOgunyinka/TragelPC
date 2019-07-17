#include "allproductsdialog.hpp"
#include "ui_allproductsdialog.h"
#include "productmodel.hpp"
#include "productuploaddialog.hpp"
#include "productthumbnaildelegate.hpp"
#include "editproductdialog.hpp"

#include <QStandardItemModel>
#include <QInputDialog>
#include <QProgressDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QUrlQuery>
#include <QJsonDocument>

AllProductsDialog::AllProductsDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::AllProductsDialog ),
    app_settings{ utilities::ApplicationSettings::GetAppSettings() }
{
    ui->setupUi( this );
    ui->update_button->setDisabled( true );
    ui->next_button->setDisabled( true );
    ui->prev_button->setDisabled( true );
    ui->tableView->setContextMenuPolicy( Qt::CustomContextMenu );
    ui->tableView->setItemDelegateForColumn( 2, new ProductThumbnailDelegate );
    QObject::connect( ui->tableView, &QTableView::customContextMenuRequested,
                      this, &AllProductsDialog::OnCustomContextMenuRequested );
    QObject::connect( ui->next_button, &QToolButton::clicked, this,
                      &AllProductsDialog::OnNextPageButtonClicked );
    QObject::connect( ui->prev_button, &QToolButton::clicked, this,
                      &AllProductsDialog::OnPreviousPageButtonClicked );
    QObject::connect( ui->last_page_button, &QToolButton::clicked, this,
                      &AllProductsDialog::OnLastPageButtonClicked );
    QObject::connect( ui->first_page_button, &QToolButton::clicked, this,
                      &AllProductsDialog::OnFirstPageButtonClicked );
    QObject::connect( ui->update_button, &QPushButton::clicked, this,
                      &AllProductsDialog::OnUpdateButtonClicked );
}

AllProductsDialog::~AllProductsDialog()
{
    delete ui;
}

void AllProductsDialog::closeEvent(QCloseEvent *event)
{
    if( isWindowModified() ){
        if( QMessageBox::question( this, "Close",
                                   "You have unsaved changes, continue with "
                                   "closing?" ) == QMessageBox::Yes ){
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

void AllProductsDialog::OnRemoveItemButtonClicked()
{
    QModelIndex const index{ ui->tableView->currentIndex() };
    if( !index.isValid() ) return;

    auto const& app_settings = utilities::ApplicationSettings::GetAppSettings();
    using utilities::SettingsValue;
    bool const confirming_deletion {
        app_settings.Value( SettingsValue::ConfirmDeletion, true ).toBool()
    };
    if( confirming_deletion &&
            QMessageBox::question( this, "Remove item",
                                   "Removing this item deletes it completely, "
                                   "are you sure you want to continue with the "
                                   "removal?" ) == QMessageBox::No ){
        return;
    }
    QString const reason {
        QInputDialog::getText( this, "Reason",
                               "We would like to know the reason for deletion"
                               ).trimmed() };
    if( reason.isEmpty() ) {
        QMessageBox::information( this, "Deletion",
                                  "Cannot remove an item without a reason" );
        return;
    }
    utilities::ProductData& data{ products_[ index.row() ] };
    QUrlQuery query{};
    query.addQueryItem( "product_id", QString::number( data.product_id ) );
    query.addQueryItem( "reason", reason );
    QUrl product_removal_url{
        utilities::Endpoint::GetEndpoints().RemoveProduct()
    };
    product_removal_url.setQuery( query );
    auto on_removal_succeeded = [=]( QJsonObject const & ){
        ProductModel* const model{
            qobject_cast<ProductModel*>( ui->tableView->model() )
        };
        model->removeRows( index.row(), 1 );
        QMessageBox::information( this, "Removal", "Removal successful" );
    };
    using utilities::SimpleRequestType;

    utilities::SendNetworkRequest( product_removal_url, on_removal_succeeded,
                                   []{}, this, true, SimpleRequestType::Delete);
}

void AllProductsDialog::OnEditItemButtonClicked()
{
    QModelIndex const index{ ui->tableView->currentIndex() };
    if( !index.isValid() ) return;

    utilities::ProductData& data{ products_[ index.row() ] };
    EditProductDialog* edit_dialog{ new EditProductDialog( this ) };
    edit_dialog->SetName( data.name );
    edit_dialog->SetPrice( data.price );
    if( edit_dialog->exec() == QDialog::Accepted ){
        utilities::ProductData const new_data = edit_dialog->GetValue();
        if( data == new_data ) return; // no changes? OK!
        data.name = new_data.name;
        data.price = new_data.price;
        data.constant_url = "1"; // temporarily used to mark dirty cache
        setWindowModified( true );
        if( !new_data.thumbnail_location.isEmpty() ){
            data.thumbnail_location = new_data.thumbnail_location;
        }
        if( !ui->update_button->isEnabled() ){
            ui->update_button->setDisabled( false );
        }
    }
    edit_dialog->setAttribute( Qt::WA_DeleteOnClose );
}


void AllProductsDialog::OnUpdateButtonClicked()
{
    ProductUploadDialog *upload_dialog{
        new ProductUploadDialog( products_, this )
    };
    upload_dialog->setAttribute( Qt::WA_DeleteOnClose );
    QObject::connect( upload_dialog, &ProductUploadDialog::uploads_completed,
                      [=]( bool const has_error )
    {
        if( !has_error ){
            setWindowModified( false );
            QMessageBox::information( this, "Uploads",
                                      "Products successfully submitted" );
            upload_dialog->accept();
            ui->update_button->setEnabled( false );
            if ( QMessageBox::question( this, "Products", "Refresh list?" ) ==
                 QMessageBox::Yes )
            {
                DownloadProducts();
            } else {
                this->accept();
            }
        }
    });
    upload_dialog->show();
    upload_dialog->StartUpload();
}

void AllProductsDialog::OnFirstPageButtonClicked()
{
    QUrl const first_page_address{
        GetUrlWithQuery( product_query_data_.first_url )
    };
    DownloadProducts( first_page_address );
}

void AllProductsDialog::OnLastPageButtonClicked()
{
    QUrl const last_page_address{ product_query_data_.last_url };
    DownloadProducts( last_page_address );
}

void AllProductsDialog::OnNextPageButtonClicked()
{
    QUrl const next_page_address{
        GetUrlWithQuery( product_query_data_.other_url.next_url )
    };
    DownloadProducts( next_page_address );
}

void AllProductsDialog::OnPreviousPageButtonClicked()
{
    QUrl const next_page_address{
        GetUrlWithQuery( product_query_data_.other_url.previous_url )
    };
    DownloadProducts( next_page_address );
}

void AllProductsDialog::OnCustomContextMenuRequested( QPoint const &point )
{
    QModelIndex const index{ ui->tableView->indexAt( point )};
    if( !index.isValid() ) return;

    QMenu custom_menu( this );
    custom_menu.setWindowTitle( "Menu" );
    if( !index.parent().isValid() ){
        QAction* action_edit{ new QAction( "Edit" ) };
        QAction* action_remove{ new QAction( "Remove" ) };
        QObject::connect( action_edit, &QAction::triggered, this,
                          &AllProductsDialog::OnEditItemButtonClicked );
        QObject::connect( action_remove, &QAction::triggered, this,
                          &AllProductsDialog::OnRemoveItemButtonClicked );
        custom_menu.addAction( action_edit );
        custom_menu.addAction( action_remove );
    }
    custom_menu.exec( ui->tableView->mapToGlobal( point ) );
}

QUrl AllProductsDialog::GetUrlWithQuery( QString const & address )
{
    using utilities::SettingsValue;

    int const per_page{
        app_settings.Value( SettingsValue::ResultPerPage, 20 ).toInt()
    };
    QUrlQuery url_query{};
    url_query.addQueryItem( "per_page", QString::number( per_page ) );
    QUrl url{ address };
    url.setQuery( url_query );
    return url;
}

void AllProductsDialog::DownloadProducts( QUrl const & address )
{
    ui->first_page_button->setDisabled( true );
    ui->last_page_button->setDisabled( true );
    ui->prev_button->setDisabled( true );
    ui->next_button->setDisabled( true );

    auto on_success = std::bind( &AllProductsDialog::OnDownloadResultObtained,
                                 this, std::placeholders::_1 );
    // on error do nothing. on success, call this->OnDownloadResultObtained()
    utilities::SendNetworkRequest( address, on_success, []{}, this );
}

void AllProductsDialog::DownloadProducts()
{
    QUrl const address{
        GetUrlWithQuery( utilities::Endpoint::GetEndpoints().GetProducts() )
    };
    DownloadProducts( address );
}

void AllProductsDialog::UpdatePageData()
{
    utilities::UrlData const& url_data{ product_query_data_.other_url };
    ui->page_label->setText( QString( "Page %1 of %2 (Total : %3 items)" )
                             .arg( url_data.page_number )
                             .arg( product_query_data_.number_of_pages )
                             .arg( product_query_data_.total_result ));
    auto const page_number = url_data.page_number;
    if( page_number < product_query_data_.number_of_pages ){
        ui->next_button->setEnabled( true );
        ui->last_page_button->setEnabled( true );
    }
    if( page_number > 1 ){
        if( page_number == product_query_data_.number_of_pages ){
            ui->prev_button->setEnabled( true );
            ui->first_page_button->setEnabled( true );
        } else {
            ui->first_page_button->setEnabled( true );
            ui->last_page_button->setEnabled( true );
            ui->prev_button->setEnabled( true );
            ui->next_button->setEnabled( true );
        }
    }
}

void AllProductsDialog::OnDownloadResultObtained( QJsonObject const & result )
{
    bool const suceeds = utilities::ParsePageUrls( result, product_query_data_ );
    if( !suceeds ){
        QMessageBox::information( this, "Products", "No products found" );
        return;
    }
    products_.clear();
    QJsonArray product_array{ result.value( "products" ).toArray() };
    while( !product_array.isEmpty() ){
        QJsonValue const value{ product_array[0] };
        if( value.isArray() ){
            product_array = value.toArray();
        } else {
            for( auto const value: product_array ){
                QJsonObject const object = value.toObject();
                QString const name{ object.value( "name" ).toString() };
                QString const thumbnail_url{
                    object.value( "thumbnail" ).toString()
                };
                qint64 const product_id{ object.value( "id" ).toInt() };
                double const price{ object.value( "price" ).toDouble() };
                utilities::ProductData product {
                    name, thumbnail_url, "", price, product_id,
                };
                products_.push_back( std::move( product ) );
            }
            break;
        }
    }
    UpdatePageData();
    ProductModel *new_product_model {
        new ProductModel( products_, ui->tableView )
    };
    QObject::connect( new_product_model, &ProductModel::destroyed,
                      new_product_model, &ProductModel::deleteLater );
    ui->tableView->setVisible( false );
    ui->tableView->setModel( new_product_model );
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(
                QHeaderView::Stretch );
    ui->tableView->setVisible( true );
}

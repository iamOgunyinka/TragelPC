#include "allproductsdialog.hpp"
#include "ui_allproductsdialog.h"
#include "resources.hpp"
#include "productmodel.hpp"
#include "productthumbnaildelegate.hpp"
#include "editproductdialog.hpp"

#include <QStandardItemModel>
#include <QProgressDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QMenu>
#include <QAction>

AllProductsDialog::AllProductsDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::AllProductsDialog )
{
    ui->setupUi( this );
    ui->update_button->setDisabled( true );
    ui->next_button->setDisabled( true );
    ui->prev_button->setDisabled( true );
    ui->tableView->setContextMenuPolicy( Qt::CustomContextMenu );
    QObject::connect( ui->tableView, &QTableView::customContextMenuRequested, this,
                      &AllProductsDialog::OnCustomContextMenuRequested );
    ui->tableView->setItemDelegateForColumn( 2, new ProductThumbnailDelegate );
}

AllProductsDialog::~AllProductsDialog()
{
    delete ui;
}

void AllProductsDialog::OnEditItemButtonClicked()
{

}

void AllProductsDialog::OnRemoveItemButtonClicked()
{

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

void AllProductsDialog::DownloadProducts()
{
    QProgressDialog* progress_dialog{ new QProgressDialog( "Please wait", "Cancel",
                                                           1, 100, this ) };
    progress_dialog->show();
    QNetworkAccessManager& network_manager{ utilities::NetworkManager::GetNetwork() };
    QNetworkCookieJar& session_cookie{ utilities::NetworkManager::GetSessionCookie() };
    network_manager.setCookieJar( &session_cookie );
    session_cookie.setParent( nullptr ); // network manager doesn't own the session cookie
    QUrl const address{ utilities::Endpoint::GetEndpoints().GetProducts() };
    QNetworkRequest request{ utilities::GetRequestInterface( address ) };
    QNetworkReply* reply{ network_manager.get( request ) };

    QObject::connect( reply, &QNetworkReply::downloadProgress, [=]( qint64 received, qint64 total)
    {
        progress_dialog->setMaximum( total + ( total * 0.25 ));
        progress_dialog->setValue( received );
    });
    QObject::connect( progress_dialog, &QProgressDialog::canceled, reply, &QNetworkReply::abort );
    QObject::connect( reply, &QNetworkReply::finished, progress_dialog, &QProgressDialog::close );
    QObject::connect( reply, &QNetworkReply::finished, [=]
    {
        QJsonObject const result{ utilities::GetJsonNetworkData( reply, true ) };
        if( result.isEmpty() ) return;
        OnDownloadResultObtained( result );
    });
}

void AllProductsDialog::OnDownloadResultObtained( QJsonObject const & result )
{
    QJsonObject const page_information{ result.value( "pages" ).toObject() };
    uint& total_result = product_query_data.total_result;
    total_result = page_information.value( "total" ).toInt();
    uint& page_total = product_query_data.number_of_pages;
    page_total = page_information.value( "pages" ).toInt();

    if( total_result == 0 || page_total == 0 ){
        QMessageBox::information( this, "Products", "No products found" );
        return;
    }

    product_query_data.first_url = page_information.value( "first_url" ).toString();
    product_query_data.last_url = page_information.value( "last_url" ).toString();
    product_query_data.result_per_page = page_information.value( "per_page" ).toInt();
    if( page_total > 0 ){
        QString const next_url = page_information.value( "next_url" ).toString();
        QString previous_url {};
        if( page_total != 0 ){
            previous_url = page_information.value( "prev_url" ).toString();
        }
        uint page_number = page_information.value( "page" ).toInt();
        utilities::ProductPageUrlData const next_page_info{ next_url, previous_url, page_number };
        product_query_data.urls.push_back( next_page_info );
    }

    QJsonArray product_array{ result.value( "products" ).toArray() };
    QList<utilities::ProductData> products {};
    while( !product_array.isEmpty() ){
        QJsonValue const value{ product_array[0] };
        if( value.isArray() ){
            product_array = value.toArray();
        } else {
            for( auto const &value: product_array ){
                QJsonObject const & object = value.toObject();
                QString const name{ object.value( "name" ).toString() };
                QString const thumbnail_url{ object.value( "thumbnail" ).toString() };
                double const price{ object.value( "price" ).toDouble() };
                products.push_back( utilities::ProductData{ name, thumbnail_url, "", price } );
            }
            break;
        }
    }
    ProductModel *new_product_model { new ProductModel( std::move( products ), ui->tableView )};
    QObject::connect( new_product_model, &ProductModel::destroyed, new_product_model,
                      &ProductModel::deleteLater );
    ui->tableView->setModel( new_product_model );
    ui->tableView->resizeColumnsToContents();
}

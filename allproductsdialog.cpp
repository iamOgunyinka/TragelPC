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
    ui->tableView->setItemDelegateForColumn( 2, new ProductThumbnailDelegate );
    QObject::connect( ui->tableView, &QTableView::customContextMenuRequested, this,
                      &AllProductsDialog::OnCustomContextMenuRequested );
    QObject::connect( ui->next_button, &QToolButton::clicked, this,
                      &AllProductsDialog::OnNextPageButtonClicked );
    QObject::connect( ui->prev_button, &QToolButton::clicked, this,
                      &AllProductsDialog::OnPreviousPageButtonClicked );
    QObject::connect( ui->last_page_button, &QToolButton::clicked, this,
                      &AllProductsDialog::OnLastPageButtonClicked );
    QObject::connect( ui->first_page_button, &QToolButton::clicked, this,
                      &AllProductsDialog::OnFirstPageButtonClicked );
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

void AllProductsDialog::OnFirstPageButtonClicked()
{
    QUrl const first_page_address{ product_query_data.first_url };
    DownloadProducts( first_page_address );
}

void AllProductsDialog::OnLastPageButtonClicked()
{
    QUrl const last_page_address{ product_query_data.last_url };
    DownloadProducts( last_page_address );
}

void AllProductsDialog::OnNextPageButtonClicked()
{
    QUrl const& next_page_address{ product_query_data.other_url.next_url };
    DownloadProducts( next_page_address );
}

void AllProductsDialog::OnPreviousPageButtonClicked()
{
    QUrl const& next_page_address{ product_query_data.other_url.previous_url };
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

void AllProductsDialog::DownloadProducts( QUrl const & address )
{
    ui->first_page_button->setDisabled( true );
    ui->last_page_button->setDisabled( true );
    ui->prev_button->setDisabled( true );
    ui->next_button->setDisabled( true );

    QProgressDialog* progress_dialog{ new QProgressDialog( "Please wait", "Cancel",
                                                           1, 100, this ) };
    progress_dialog->show();
    QNetworkAccessManager& network_manager{ utilities::NetworkManager::GetNetwork() };
    QNetworkCookieJar& session_cookie{ utilities::NetworkManager::GetSessionCookie() };
    network_manager.setCookieJar( &session_cookie );
    session_cookie.setParent( nullptr ); // network manager doesn't own the session cookie
    QNetworkRequest request{ utilities::GetRequestInterface( address ) };
    QNetworkReply* reply{ network_manager.get( request ) };

    connect( reply, &QNetworkReply::downloadProgress, [=]( qint64 received, qint64 total )
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

void AllProductsDialog::DownloadProducts()
{
    QUrl const address{ utilities::Endpoint::GetEndpoints().GetProducts() };
    DownloadProducts( address );
}

void AllProductsDialog::UpdatePageData()
{
    utilities::UrlData const& url_data{ product_query_data.other_url };
    ui->page_label->setText( QString( "Page %1 of %2" ).arg( url_data.page_number )
                             .arg( product_query_data.number_of_pages ));
    auto& page_number = url_data.page_number;
    if( page_number < product_query_data.number_of_pages ){
        ui->next_button->setEnabled( true );
        ui->last_page_button->setEnabled( true );
    } else if( page_number > 1 ){
        if( page_number == product_query_data.number_of_pages ){
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
    bool const no_result = utilities::ParsePageUrls( result, product_query_data );
    if( no_result ){
        QMessageBox::information( this, "Products", "No products found" );
        return;
    }
    QJsonArray product_array{ result.value( "products" ).toArray() };
    QVector<utilities::ProductData> products {};
    while( !product_array.isEmpty() ){
        QJsonValue const value{ product_array[0] };
        if( value.isArray() ){
            product_array = value.toArray();
        } else {
            for( auto const &value: product_array ){
                QJsonObject const object = value.toObject();
                QString const name{ object.value( "name" ).toString() };
                QString const thumbnail_url{ object.value( "thumbnail" ).toString() };
                double const price{ object.value( "price" ).toDouble() };
                products.push_back( utilities::ProductData{ name, thumbnail_url, "", price } );
            }
            break;
        }
    }
    UpdatePageData();
    ProductModel *new_product_model { new ProductModel( std::move( products ), ui->tableView )};
    QObject::connect( new_product_model, &ProductModel::destroyed, new_product_model,
                      &ProductModel::deleteLater );
    ui->tableView->setVisible( false );
    ui->tableView->setModel( new_product_model );
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->tableView->setVisible( true );
}

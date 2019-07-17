#include "makeorderdialog.hpp"
#include "ui_makeorderdialog.h"


#include "productmodel.hpp"
#include "orderingitemmodel.hpp"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QMenu>

MakeOrderDialog::MakeOrderDialog( QWidget *parent ) : QDialog( parent ),
    ui( new Ui::MakeOrderDialog )
{
    ui->setupUi( this );
    ui->item_view->setContextMenuPolicy( Qt::CustomContextMenu );
    QObject::connect( ui->cash_radio, &QRadioButton::toggled,[=]( bool checked )
    {
        ui->ebank_radio->setChecked( !checked );
    });
    QObject::connect( ui->ebank_radio, &QRadioButton::toggled,
                      [=]( bool checked )
    {
        ui->cash_radio->setChecked( !checked );
        ui->lineEdit->setEnabled( checked );
        if( checked ) ui->lineEdit->setFocus();
    });
    ui->cash_radio->setChecked( true );
    QObject::connect( ui->item_view, &QTableView::customContextMenuRequested,
                     this, &MakeOrderDialog::OnItemCustomContextMenuRequested );
    QObject::connect( ui->add_button, &QPushButton::clicked, this,
                      &MakeOrderDialog::OnAddItemButtonClicked );
    QObject::connect( ui->make_order_button, &QPushButton::clicked, this,
                      &MakeOrderDialog::OnMakeOrderButtonClicked );
}

MakeOrderDialog::~MakeOrderDialog()
{
    delete ui;
}

void MakeOrderDialog::OnItemCustomContextMenuRequested( QPoint const & p )
{
    QModelIndex const index{ ui->item_view->indexAt( p )};
    if( !index.isValid() ) return;

    QMenu custom_menu( this );

    if( !index.parent().isValid() ){
        QAction* action_remove{ new QAction( "Remove" ) };

        QObject::connect( action_remove, &QAction::triggered, this,
                          &MakeOrderDialog::OnRemoveItemActionClicked );
        custom_menu.addAction( action_remove );
    }
    custom_menu.exec( ui->item_view->mapToGlobal( p ) );
}

void MakeOrderDialog::OnRemoveItemActionClicked()
{
    QModelIndex const index{ ui->item_view->currentIndex() };
    if( !index.isValid() ) return;

    auto const& app_settings = utilities::ApplicationSettings::GetAppSettings();
    using utilities::SettingsValue;
    bool const confirming_deletion {
        app_settings.Value( SettingsValue::ConfirmDeletion, true ).toBool()
    };

    if( confirming_deletion && QMessageBox::question( this, "Remove",
                                                      "Are you sure?" ) ==
            QMessageBox::No )
    {
        return;
    }
    OrderingItemModel* model{
        qobject_cast<OrderingItemModel*>( ui->item_view->model() )
    };
    model->removeRows( index.row(), 1 );
}

void MakeOrderDialog::OnAddItemButtonClicked()
{
    QModelIndex const index = ui->product_view->currentIndex();
    if( !index.isValid() ) return;

}

void MakeOrderDialog::OnMakeOrderButtonClicked()
{
    OrderingItemModel* const model {
        qobject_cast<OrderingItemModel*>( ui->item_view->model() )
    };
    if( model->rowCount( QModelIndex{} ) == 0 ){
        QMessageBox::information( this, "Make order",
                                  "You can't make an order with an empty cart");
        return;
    }
    QString ref_id{ ui->lineEdit->text().trimmed() };
    if( ui->cash_radio->isChecked() ){
        ref_id = "cash";
    } else if( ui->ebank_radio->isChecked() && ref_id.isEmpty() ){
        QMessageBox::information( this, "Ordering",
                                  "Enter the reference ID" );
        ui->lineEdit->setFocus();
        return;
    }

    auto const &cart = model->Items();
    QJsonArray items;
    for( auto const & item: cart ){
        items.append( item.ToJson() );
    }
    QJsonObject const order_payload {
        { "payment_reference_id", ref_id }, { "items", items }
    };
    QByteArray const payload{ QJsonDocument( order_payload ).toJson() };
    auto on_success = [=]( QJsonObject const &, QNetworkReply * ){
        QMessageBox::information( this, "Order", "Order made successfully" );
        this->accept();
    };

    QUrl const address{ utilities::Endpoint::GetEndpoints().AddOrder() };
    utilities::SendPostNetworkRequest( address, on_success, []{}, this,
                                       payload );

}

void MakeOrderDialog::OnDownloadResultObtained( QJsonObject const & result )
{
    utilities::PageQuery page_query{};
    bool const succeeds = utilities::ParsePageUrls( result, page_query );
    if( !succeeds ){
        QMessageBox::information( this, "Products", "No products found" );
        this->accept();
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
                utilities::ProductData product {
                    object.value( "name" ).toString(),
                    "", "", object.value( "price" ).toDouble(),
                    object.value( "id" ).toInt(),
                };
                products_.push_back( std::move( product ) );
            }
            break;
        }
    }
    OrderingProductModel *new_product_model {
        new OrderingProductModel( products_, ui->product_view )
    };
    QObject::connect( new_product_model, &OrderingProductModel::destroyed,
                      new_product_model, &OrderingProductModel::deleteLater );

    ui->product_view->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->product_view->setSelectionBehavior( QAbstractItemView::SelectRows );
    ui->product_view->setDragEnabled( true );
    ui->product_view->setDragDropMode( QAbstractItemView::DragOnly );
    ui->product_view->setDropIndicatorShown( true );
    ui->product_view->setModel( new_product_model );
    ui->product_view->resizeColumnsToContents();
    ui->product_view->horizontalHeader()->setSectionResizeMode(
                QHeaderView::Stretch );

    ui->item_view->setAcceptDrops( true );
    ui->item_view->setDragDropMode( QAbstractItemView::DropOnly );
    ui->item_view->setDropIndicatorShown( true );
    ui->item_view->setModel( new OrderingItemModel{ ui->item_view } );
    ui->item_view->resizeColumnsToContents();
    ui->item_view->horizontalHeader()->setSectionResizeMode(
                QHeaderView::Stretch );
    QMessageBox::information( this, "D&D", "Drag an item from the left to the "
                                           "right to add to the cart.");
}

void MakeOrderDialog::DownloadProducts()
{
    QUrl const address{ utilities::Endpoint::GetEndpoints().GetProducts() };
    auto on_success = std::bind( &MakeOrderDialog::OnDownloadResultObtained,
                                 this, std::placeholders::_1 );
    // on error do nothing. on success, call this->OnDownloadResultObtained()
    utilities::SendNetworkRequest( address, on_success, []{}, this );
}

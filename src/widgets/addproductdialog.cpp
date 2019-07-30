#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QStandardItem>

#include "widgets/addproductdialog.hpp"
#include "models/productthumbnaildelegate.hpp"
#include "widgets/productuploaddialog.hpp"
#include "widgets/newproductitemdialog.hpp"
#include "models/newproductmodel.hpp"

#include "ui_addproductdialog.h"

AddProductDialog::AddProductDialog(QWidget *parent) : QDialog(parent),
    ui( new Ui::AddProductDialog )
{
    ui->setupUi( this );

    ui->product_view->setDragDropMode( QAbstractItemView::NoDragDrop );
    ui->product_view->setContextMenuPolicy( Qt::CustomContextMenu );
    ui->product_view->setSelectionBehavior( QAbstractItemView::SelectRows );
    ui->product_view->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->product_view->resizeColumnsToContents();
    ui->product_view->horizontalHeader()->setSectionResizeMode(
                QHeaderView::Stretch );
    ui->upload_data_button->setEnabled( false );

    QObject::connect( ui->product_view, &QTableView::customContextMenuRequested,
                      this, &AddProductDialog::OnContextMenuItemRequested );
    QObject::connect( ui->add_item_button, &QPushButton::clicked, this,
                      &AddProductDialog::OnAddItemButtonClicked );
    QObject::connect( ui->remove_item_button, &QPushButton::clicked, this,
                      &AddProductDialog::OnRemoveItemButtonClicked );
    QObject::connect( ui->upload_data_button, &QPushButton::clicked, this,
                      &AddProductDialog::OnUploadDataToServerButtonClicked );
    ui->product_view->setModel( new NewProductModel( ui->product_view ) );
}

AddProductDialog::~AddProductDialog()
{
    delete ui;
}

void AddProductDialog::closeEvent( QCloseEvent *close_event )
{
    if( !isWindowModified() ) close_event->accept();
    else {
        if( QMessageBox::question( this, "Close",
                                   "You have products not yet saved, continue "
                                   "with closing?" ) == QMessageBox::Yes ){
            close_event->accept();
        } else {
            close_event->ignore();
        }
    }
}

void AddProductDialog::OnContextMenuItemRequested( QPoint const & point )
{
    QModelIndex const index{ ui->product_view->indexAt( point )};
    if( !index.isValid() ) return;

    QMenu custom_menu( this );
    custom_menu.setTitle( "Items" );
    if( !index.parent().isValid() ){
        QAction* action_remove{ new QAction( "Remove" ) };
        QObject::connect( action_remove, &QAction::triggered, this,
                          &AddProductDialog::OnRemoveItemButtonClicked );
        custom_menu.addAction( action_remove );
    }
    custom_menu.exec( ui->product_view->mapToGlobal( point ) );
}

void AddProductDialog::OnAddItemButtonClicked()
{
    NewProductModel* model = qobject_cast<NewProductModel*>(
                ui->product_view->model() );
    NewProductItemDialog* product_dialog = new NewProductItemDialog( this );
    product_dialog->setWindowTitle( "New product item" );

    if( product_dialog->exec() == QDialog::Accepted ){
        model->AddItem( product_dialog->GetProduct() );
        ui->upload_data_button->setEnabled( true );
    }
}

void AddProductDialog::OnRemoveItemButtonClicked()
{
    QModelIndex const index{ ui->product_view->currentIndex() };
    auto const& app_settings = utilities::ApplicationSettings::GetAppSettings();
    using utilities::SettingsValue;
    bool const confirming_deletion {
        app_settings.Value( SettingsValue::ConfirmDeletion, true ).toBool()
    };
    if( !index.isValid() ) return;
    if( !index.parent().isValid() ){
        if( confirming_deletion &&
                QMessageBox::question( this, "Delete", "Are you sure?" ) ==
                QMessageBox::No ){
            return;
        }
        int const row = index.row();
        NewProductModel* model{
            qobject_cast<NewProductModel*>(ui->product_view->model())
        };
        model->RemoveItem( row );
        ui->upload_data_button->setEnabled( model->rowCount() != 0 );
    }
}

void AddProductDialog::OnUploadDataToServerButtonClicked()
{
    auto model = qobject_cast<NewProductModel*>(ui->product_view->model() );
    auto& products = model->GetProducts();
    ProductUploadDialog *upload_dialog{
        new ProductUploadDialog( products, this )
    };
    upload_dialog->setAttribute( Qt::WA_DeleteOnClose );
    QObject::connect( upload_dialog, &ProductUploadDialog::uploads_completed,
                      [=]( bool const has_error )
    {
        upload_dialog->accept();
        if( !has_error ){
            QMessageBox::information( this, "Uploads",
                                      "Products successfully submitted" );
            this->setWindowModified( false );
            this->accept();
        }
    });
    upload_dialog->show();
    upload_dialog->StartUpload();
}

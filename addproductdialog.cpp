#include "addproductdialog.hpp"
#include "ui_addproductdialog.h"
#include "editproductdialog.hpp"
#include "productthumbnaildelegate.hpp"
#include "productuploaddialog.hpp"

#include <QFileDialog>
#include <QMessageBox>
#include <QMenu>
#include <QStandardItem>

AddProductDialog::AddProductDialog(QWidget *parent) : QDialog(parent),
    ui( new Ui::AddProductDialog ), data_model{ new QStandardItemModel( 0, 3 ) }
{
    ui->setupUi( this );
    ui->choose_image_button->setEnabled( false );
    ui->upload_checkbox->setChecked( false );
    QObject::connect( ui->upload_checkbox, &QCheckBox::toggled, [=]( bool const checked ){
        ui->choose_image_button->setEnabled( checked );
        if( checked ) ui->image_preview->clear();
        temp_file.clear();
    });

    ui->product_view->setDragDropMode( QAbstractItemView::NoDragDrop );
    ui->product_view->setContextMenuPolicy( Qt::CustomContextMenu );
    ui->product_view->setSelectionBehavior( QAbstractItemView::SelectRows );
    ui->product_view->setSelectionMode( QAbstractItemView::SingleSelection );
    ui->product_view->resizeColumnsToContents();
    ui->product_view->setItemDelegateForColumn( 2, new LocalProductUploadDelegate );

    data_model->setHorizontalHeaderItem( 0, new QStandardItem( "Product name" ) );
    data_model->setHorizontalHeaderItem( 1, new QStandardItem( "Product price" ) );
    data_model->setHorizontalHeaderItem( 2, new QStandardItem( "Picture" ) );

    ui->upload_data_button->setEnabled( false );
    QObject::connect( ui->product_view, &QTableView::customContextMenuRequested, this,
                      &AddProductDialog::OnContextMenuItemRequested );
    QObject::connect( ui->product_view, &QTableView::doubleClicked, this,
                      &AddProductDialog::OnEditItemButtonClicked );
    QObject::connect( ui->choose_image_button, &QToolButton::clicked, this,
                      &AddProductDialog::OnShowImageButtonClicked );
    QObject::connect( ui->add_item_button, &QPushButton::clicked, this,
                      &AddProductDialog::OnAddItemButtonClicked );
    QObject::connect( data_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                      ui->product_view, SLOT(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    QObject::connect( ui->edit_item_button, &QPushButton::clicked, this,
                      &AddProductDialog::OnEditItemButtonClicked );
    QObject::connect( ui->remove_item_button, &QPushButton::clicked, this,
                      &AddProductDialog::OnRemoveItemButtonClicked );
    QObject::connect( ui->upload_data_button, &QPushButton::clicked, this,
                      &AddProductDialog::OnUploadDataToServerButtonClicked );
    ui->name_edit->setFocus();
}

AddProductDialog::~AddProductDialog()
{
    delete ui;
}

void AddProductDialog::OnContextMenuItemRequested( QPoint const & point )
{
    QModelIndex const index{ ui->product_view->indexAt( point )};
    if( !index.isValid() ) return;

    QMenu custom_menu( this );
    custom_menu.setTitle( "Items" );
    if( !index.parent().isValid() ){
        QAction* action_edit{ new QAction( "Edit" ) };
        QAction* action_remove{ new QAction( "Remove" ) };
        QObject::connect( action_edit, &QAction::triggered, this,
                          &AddProductDialog::OnEditItemButtonClicked );
        QObject::connect( action_remove, &QAction::triggered, this,
                          &AddProductDialog::OnRemoveItemButtonClicked );
        custom_menu.addAction( action_edit );
        custom_menu.addAction( action_remove );
    }
    custom_menu.exec( ui->product_view->mapToGlobal( point ) );
}

void AddProductDialog::OnAddItemButtonClicked()
{
    QString const product_name{ ui->name_edit->text().trimmed() };
    if( product_name.isEmpty() ){
        QMessageBox::information( this, "Add item", "This field cannot be empty" );
        ui->name_edit->setFocus();
        return;
    }
    bool price_ok{ false };
    QString const product_price{ ui->price_line->text().trimmed() };
    double const price{ product_price.toDouble( &price_ok ) };
    if( product_price.isEmpty() || !price_ok ){
        QMessageBox::information( this, "Add item", "You must add a price for the product" );
        ui->price_line->clear();
        ui->price_line->setFocus();
        return;
    }
    utilities::ProductData const value{ product_name, temp_file, "", price };
    product_item_list.append( value );
    ui->upload_data_button->setEnabled( true );
    UpdateModel( value, product_item_list.size() );
    ui->product_view->setModel( data_model );

    ui->price_line->clear();
    ui->name_edit->clear();
    ui->name_edit->setFocus();
    ui->image_preview->clear();
    ui->upload_checkbox->setChecked( false );
}

void AddProductDialog::UpdateModel( utilities::ProductData const & data_item,
                                    qint64 const index )
{
    bool const editing_item = index < this->product_item_list.size();

    QStandardItem * name_item{ new QStandardItem( data_item.name ) };
    QStandardItem * price_item{ new QStandardItem( QString::number( data_item.price ) ) };
    QStandardItem * thumbnail_item{ new QStandardItem( "No thumbnail used" ) };
    name_item->setEditable( false );
    price_item->setEditable( false );
    thumbnail_item->setEditable( false );

    if( !data_item.thumbnail_location.isEmpty() ){
        thumbnail_item->setText( data_item.thumbnail_location );
    }

    if( editing_item ){ // when an edited question is used
        data_model->removeRow( index );
        data_model->insertRow( index, { name_item, price_item, thumbnail_item } );
    } else {
        data_model->appendRow( { name_item, price_item, thumbnail_item } );
    }
}

void AddProductDialog::OnRemoveItemButtonClicked()
{
    QModelIndex const index{ ui->product_view->currentIndex() };
    if( !index.isValid() ) return;
    if( !index.parent().isValid() ){
        int const response = QMessageBox::question( this, "Delete item", "Are you sure?" );
        if( response == QMessageBox::No ) return;
        int const row = index.row();
        this->product_item_list.removeAt( row );
        this->data_model->removeRow( row );
        ui->upload_data_button->setEnabled( !product_item_list.isEmpty() );
    }
}

void AddProductDialog::OnShowImageButtonClicked()
{
    QString const filename{ QFileDialog::getOpenFileName( this, "Choose custom preview file",
                                                          "", "PNG Files(*.png);;"
                                                              "JPEG Files(*.jpg)" ) };
    if( filename.isEmpty() || filename.isNull() ) return;
    qint64 const file_size{ QFileInfo( filename ).size() };
    // if file does not exist or it exceeds 150KB
    if( file_size == 0 || file_size > (150 * 1024) ){
        QMessageBox::warning( this, "Image upload", "The size of the thumbnail exceeds 150KB." );
        return;
    }
    ui->image_preview->clear();
    ui->image_preview->setPixmap( QPixmap( filename ).scaled( 100, 100 ) );
    temp_file = filename;
}

void AddProductDialog::OnEditItemButtonClicked()
{
    QModelIndex const index = ui->product_view->currentIndex();
    if( !index.isValid() ) return;

    int const current_row = index.row();
    utilities::ProductData& item = product_item_list[current_row];
    EditProductDialog *edit_dialog{ new EditProductDialog( this ) };
    edit_dialog->SetName( item.name );
    edit_dialog->SetPrice( item.price );
    if( !item.thumbnail_location.isEmpty() ){
        edit_dialog->SetThumbnail( item.thumbnail_location );
    }
    if( edit_dialog->exec() != QDialog::Accepted ) return;
    auto const result = edit_dialog->GetValue();
    product_item_list[current_row] = result;
    UpdateModel( result, current_row );
    ui->product_view->setModel( data_model );
}

void AddProductDialog::OnUploadDataToServerButtonClicked()
{
    ProductUploadDialog *upload_dialog{ new ProductUploadDialog( product_item_list, this ) };
    upload_dialog->StartUpload();
    upload_dialog->setWindowModality( Qt::WindowModal );
    upload_dialog->setAttribute( Qt::WA_DeleteOnClose );
    upload_dialog->exec();
}

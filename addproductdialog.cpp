#include "addproductdialog.hpp"
#include "ui_addproductdialog.h"
#include "editproductdialog.hpp"
#include "resources.hpp"

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

    QObject::connect( ui->product_view, &QTableView::customContextMenuRequested, this,
                      &AddProductDialog::OnContextMenuItemRequested );

    QObject::connect( ui->choose_image_button, &QToolButton::clicked, this,
                      &AddProductDialog::OnUploadButtonClicked );
    QObject::connect( ui->add_item_button, &QPushButton::clicked, this,
                      &AddProductDialog::OnAddItemButtonClicked );
    QObject::connect( data_model, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),
                      ui->product_view, SLOT(dataChanged(QModelIndex,QModelIndex,QVector<int>)));
    QObject::connect( ui->edit_item_button, &QPushButton::clicked, this,
                      &AddProductDialog::OnEditItemButtonClicked );
    QObject::connect( ui->remove_item_button, &QPushButton::clicked, this,
                      &AddProductDialog::OnRemoveItemButtonClicked );
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
    QJsonObject value { { "name", product_name }, { "price", price } };
    if( ui->upload_checkbox->isChecked() ){
        value.insert( "thumbnail", temp_file );
    }
    product_item_list.append( value );
    UpdateModel( value, product_item_list.size() );
    ui->product_view->setModel( data_model );

    ui->price_line->clear();
    ui->name_edit->clear();
    ui->image_preview->clear();
    ui->upload_checkbox->setChecked( false );
}

void AddProductDialog::UpdateModel( QJsonObject const & json_value, qint64 const index )
{
    bool const editing_item = index < this->product_item_list.size();

    QStandardItem * name_item{ new QStandardItem( json_value.value( "name" ).toString() ) };
    QString const price{ QString::number( json_value.value( "price" ).toDouble() ) };
    QStandardItem * price_item{ new QStandardItem( price ) };
    QStandardItem * thumbnail_item{ new QStandardItem( "No thumbnail used" ) };
    name_item->setEditable( false );
    price_item->setEditable( false );
    thumbnail_item->setEditable( false );

    if( json_value.contains( "thumbnail" ) ){
        thumbnail_item->setText( json_value.value( "thumbnail" ).toString() );
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
        this->data_model->removeRow( row );
        this->product_item_list.removeAt( row );
    }
}

void AddProductDialog::OnUploadButtonClicked()
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
    QJsonObject const& item = product_item_list[current_row].toObject();
    EditProductDialog *edit_dialog{ new EditProductDialog( this ) };
    edit_dialog->SetName( item.value( "name" ).toString() );
    edit_dialog->SetPrice( item.value( "price" ).toDouble() );
    if( item.contains( "thumbnail" ) ){
        edit_dialog->SetThumbnail( item.value( "thumbnail" ).toString() );
    }
    if( edit_dialog->exec() != QDialog::Accepted ) return;
    auto const result = edit_dialog->GetValue();
    product_item_list[current_row] = result;
    UpdateModel( result, current_row );
    ui->product_view->setModel( data_model );
}

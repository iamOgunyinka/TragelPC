#include "editproductdialog.hpp"
#include "ui_editproductdialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>

EditProductDialog::EditProductDialog( QWidget *parent ) :
    QDialog(parent), ui( new Ui::EditProductDialog )
{
    ui->setupUi( this );
    setAcceptDrops( true );
    QObject::connect( ui->change_button, &QPushButton::clicked, this,
                      &EditProductDialog::OnChangeButtonClicked );
    ui->choose_image_button->setEnabled( false );
    ui->upload_checkbox->setChecked( false );
    QObject::connect( ui->upload_checkbox, &QCheckBox::toggled,
                      [=]( bool const checked )
    {
        ui->choose_image_button->setEnabled( checked );
        ui->image_preview->setEnabled( checked );
    });
    QObject::connect( ui->choose_image_button, &QToolButton::clicked, [=]{
        QString const filename{
            QFileDialog::getOpenFileName( this, "Choose custom preview file",
                                          "", "PNG Files(*.png);;"
                                              "JPEG Files(*.jpg)" ) };
        ValidateImageData( filename );
    });
}

EditProductDialog::~EditProductDialog()
{
    delete ui;
}

void EditProductDialog::ValidateImageData( QString const &filename )
{
    if( filename.isEmpty() || filename.isNull() ) return;
    qint64 const file_size{ QFileInfo( filename ).size() };
    // if file does not exist or it exceeds 150KB
    if( file_size == 0 || file_size > ( 150 * 1024 ) ){
        QMessageBox::warning( this, "Image upload",
                              "The size of the thumbnail exceeds 150KB." );
        return;
    }
    ui->image_preview->clear();
    ui->image_preview->setPixmap( QPixmap( filename ).scaled( 100, 100 ) );
    this->temp_file = filename;
}

void EditProductDialog::OnChangeButtonClicked()
{
    QString const product_name{ ui->name_edit->text().trimmed() };
    if( product_name.isEmpty() ){
        QMessageBox::information( this, "Add item",
                                  "This field cannot be empty" );
        ui->name_edit->setFocus();
        return;
    }
    QString const product_price{ ui->price_line->text().trimmed() };
    bool price_ok{ false };
    ( void )product_price.toDouble( &price_ok );
    if( product_price.isEmpty() || !price_ok ){
        QMessageBox::information( this, "Add item",
                                  "You must add a valid price for the product");
        ui->price_line->clear();
        ui->price_line->setFocus();
        return;
    }
    this->accept();
}

utilities::ProductData EditProductDialog::GetValue() const
{
    double const price{ ui->price_line->text().trimmed().toDouble() };
    QString const product_name{ ui->name_edit->text().trimmed() };

    return utilities::ProductData{ product_name, temp_file, "", price };
}

void EditProductDialog::SetName( QString const & name )
{
    ui->name_edit->setText( name );
}

void EditProductDialog::SetPrice( double const price )
{
    ui->price_line->setText( QString::number( price ) );
}

void EditProductDialog::SetThumbnail( QString const & filename )
{
    ui->upload_checkbox->setChecked( true );
    QPixmap const preview{ QPixmap( filename ).scaled( QSize( 100, 100 ) ) };
    ui->image_preview->setPixmap( preview );
    temp_file = filename;
}

void EditProductDialog::dragEnterEvent( QDragEnterEvent * event )
{
    event->accept();
}

void EditProductDialog::dropEvent( QDropEvent *event )
{
    QMimeData const * mime_data{ event->mimeData() };
    if( !mime_data->hasImage() ) return;
    QString const file_path{ mime_data->urls()[0].toLocalFile() };
    ValidateImageData( file_path );
}

#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

#include "widgets/newproductitemdialog.hpp"
#include "ui_newproductitemdialog.h"
#include "utils/resources.hpp"

NewProductItemDialog::NewProductItemDialog( QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::NewProductItemDialog), src{}
{
    ui->setupUi( this );

    QObject::connect( ui->upload_button, &QPushButton::clicked, this,
                      &NewProductItemDialog::OnUploadButtonClicked );
    QObject::connect( ui->ok_button, &QPushButton::clicked, this,
                      &NewProductItemDialog::OnAddProductButtonClicked );
    QSize const min_max_size{ 520, 320 };
    setMaximumSize( min_max_size );
    setMinimumSize( min_max_size );
}

NewProductItemDialog::~NewProductItemDialog()
{
    delete ui;
}

void NewProductItemDialog::OnUploadButtonClicked()
{
    QString const filename = QFileDialog::getOpenFileName( this, "Open pictures",
                                QDir::home().path(),
                                "Images (*.png *.xpm *.jpg *.jpeg)" );
    qint64 const file_size = QFileInfo( filename ).size();
    int const max_size = 150 * 1024;
    bool const size_exceeded = file_size == 0 || file_size > max_size;
    if( filename.isEmpty() || size_exceeded ){
        if( size_exceeded ){
            QMessageBox::warning( this, "Size", "File exceeded" );
        }
        src = "";
        ui->preview->setPixmap( QPixmap( ":/all_images/images/no_preview.jpg" ));
    } else {
        src = filename;
        ui->preview->setPixmap( QPixmap( src ) );
    }
}

void NewProductItemDialog::OnAddProductButtonClicked()
{
    using Fields = QVector<QPair<QWidget*, QString>>;

    QString const product_name { ui->name_edit->text().trimmed() };
    QString const product_description{ ui->description_edit->text().trimmed() };
    QString const product_category{ ui->categories_edit->text().trimmed() };
    QString const product_price{ ui->price_edit->text().trimmed() };

    auto is_valid = [this]( Fields const & fields ){
        for( auto const & field: fields ){
            if( field.second.isEmpty() ){
                QMessageBox::warning( this, "Warning", "This field cannot be "
                                                       "empty" );
                field.first->setFocus();
                return false;
            }
        }
        return true;
    };
    Fields fields{ {ui->name_edit, product_name},
                   {ui->description_edit, product_description},
                   {ui->categories_edit, product_category},
                   {ui->price_edit, product_price}};
    if( !is_valid( fields ) ) return;

    bool price_ok{ true };
    product_price.toDouble( &price_ok );
    if(!price_ok){
        QMessageBox::warning( this, "Warning", "Invalid price" );
        ui->price_edit->setFocus();
        return;
    }
    accept();
}

utilities::Product NewProductItemDialog::GetProduct() const
{
    QString const product_name { ui->name_edit->text().trimmed() };
    QString const product_category{ ui->categories_edit->text().trimmed() };
    QString const product_description{ ui->description_edit->text().trimmed() };
    QString const product_price{ ui->price_edit->text().trimmed() };
    double const price{ product_price.toDouble() };
    return utilities::Product{ product_name, price, 0, src, product_category,
                product_description };
}

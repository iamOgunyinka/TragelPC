#include "uploadwidget.hpp"
#include "ui_uploadwidget.h"

UploadWidget::UploadWidget(QWidget *parent) :
    QWidget( parent ),
    ui( new Ui::UploadWidget )
{
    ui->setupUi( this  );
    ui->data_item_status_edit->setDisabled( true );
    ui->data_item_progress->setRange( 1, 100 );
    ui->data_item_progress->setValue( 1 );
}

UploadWidget::~UploadWidget()
{
    delete ui;
}

void UploadWidget::SetMessage( QString const & message )
{
    ui->data_item_status_edit->appendPlainText( message + "\n" );
}

void UploadWidget::SetProgress( qint64 current_value, qint64 total )
{
    ui->data_item_progress->setMaximum( total + ( total * 0.25 ) );
    ui->data_item_progress->setValue( current_value );
}

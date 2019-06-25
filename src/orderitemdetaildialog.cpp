#include "orderitemdetaildialog.hpp"
#include "ui_orderitemdetaildialog.h"

#include "ordermodel.hpp"

OrderItemDetailDialog::OrderItemDetailDialog(
        QVector<utilities::OrderData::Item> const &items, QWidget *parent ):
    QDialog( parent ), ui( new Ui::OrderItemDetailDialog )
{
    ui->setupUi( this );
    ui->tableView->setVisible( false );
    ui->tableView->setModel( new OrderDetailModel( items, this ) );
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(
                QHeaderView::Stretch );
    ui->tableView->setVisible( true );
    this->adjustSize();
    ui->confirmation_label->setText( "Payment status: <b>Unconfirmed</b>");
}

void OrderItemDetailDialog::SetConfirmationStatus( QString const & by,
                                                   QDateTime const &on )
{
    ui->confirmation_label->setText( "Confirmed by: <b>"
                                     + by + "</b>.<br>On: <b>"
                                     + on.toString() + "</b>." );
}

OrderItemDetailDialog::~OrderItemDetailDialog()
{
    delete ui;
}

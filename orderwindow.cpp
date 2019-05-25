#include "orderwindow.hpp"
#include "ui_orderwindow.h"


OrderWindow::OrderWindow(QWidget *parent) :
    QMainWindow( parent ), ui( new Ui::OrderWindow )
{
    ui->setupUi( this );
    ui->search_limit_checkbox->setChecked( false );
    ui->search_date_from->setEnabled( false );
    ui->search_date_to->setEnabled( false );

    QObject::connect( ui->search_limit_checkbox, &QCheckBox::toggled, [=]( bool is_checked ){
        ui->search_date_from->setEnabled( is_checked );
        ui->search_date_to->setEnabled( is_checked );
    });
    QObject::connect( ui->find_guest_button, &QPushButton::clicked, this,
                      &OrderWindow::OnFindButtonClicked );
    QStringList const search_criteria{ "Show today's order", "Show last 7 days",
                                       "Show this month's" };
    ui->order_by_combo->addItems( search_criteria );
    ui->search_date_from->setDate( QDate::currentDate() );
    ui->search_date_to->setDate( QDate::currentDate() );
}

OrderWindow::~OrderWindow()
{
    delete ui;
}

void OrderWindow::OnFindButtonClicked()
{

}

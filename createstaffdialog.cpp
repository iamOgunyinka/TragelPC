#include "createstaffdialog.hpp"
#include "ui_createstaffdialog.h"

CreateStaffDialog::CreateStaffDialog(QWidget *parent) :
    QDialog(parent),
    ui( new Ui::CreateStaffDialog )
{
    ui->setupUi( this );
}

CreateStaffDialog::~CreateStaffDialog()
{
    delete ui;
}

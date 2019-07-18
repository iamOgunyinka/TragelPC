#include "widgets/adminsetupdialog.hpp"
#include "ui_adminsetupdialog.h"

AdminSetupDialog::AdminSetupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdminSetupDialog)
{
    ui->setupUi( this );
}

QString AdminSetupDialog::GetCompanysID() const
{
    return ui->company_id_line->text();
}

QString AdminSetupDialog::GetServerEnpointsURL() const
{
    return ui->endpoint_url_line->text();
}

QString AdminSetupDialog::GetSUPassword() const
{
    return ui->su_password_line->text();
}

QString AdminSetupDialog::GetSUUsername() const
{
    return ui->su_username_line->text();
}

AdminSetupDialog::~AdminSetupDialog()
{
    delete ui;
}

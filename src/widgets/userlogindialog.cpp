#include "widgets/userlogindialog.hpp"
#include "ui_userlogindialog.h"

UserLoginDialog::UserLoginDialog( QWidget *parent ) :
    QDialog( parent ),
    ui( new Ui::UserLoginDialog )
{
    ui->setupUi(this);
    ui->company_line->setEnabled( false );
    ui->username_line->setFocus();
}

void UserLoginDialog::SetCompanyID( QString const &company_id )
{
    ui->company_line->setText( "0xB0A000" + company_id + "DC" );
}

QString UserLoginDialog::GetPassword() const
{
    return ui->password_line->text();
}

QString UserLoginDialog::GetUsername() const
{
    return ui->username_line->text();
}

UserLoginDialog::~UserLoginDialog()
{
    delete ui;
}

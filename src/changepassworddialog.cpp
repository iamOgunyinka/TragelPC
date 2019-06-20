#include "changepassworddialog.hpp"
#include "ui_changepassworddialog.h"


#include <QMessageBox>

ChangePasswordDialog::ChangePasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui( new Ui::ChangePasswordDialog )
{
    ui->setupUi( this );
    setWindowTitle( "Change password" );
    QObject::connect( ui->change_button, &QPushButton::clicked, this,
                      &ChangePasswordDialog::OnChangeButtonClicked );
}

ChangePasswordDialog::~ChangePasswordDialog()
{
    delete ui;
}

void ChangePasswordDialog::SetUsername( QString const &username )
{
    username_ = username;
}

QString ChangePasswordDialog::GetPassword() const
{
    return ui->password_line->text();
}

void ChangePasswordDialog::OnChangeButtonClicked()
{
    QString const username{ ui->username_line->text().trimmed() };
    if( username.isEmpty() || username != username_ ){
        QMessageBox::warning( this, "Error", "Username does not match" );
        ui->username_line->setFocus();
        return;
    }
    QString const password{ ui->password_line->text() };
    QString const repeated_password{ ui->repeat_password_line->text() };
    if( password.isEmpty() || password != repeated_password ){
        QMessageBox::warning( this, "Error", "Password do not match" );
        ui->password_line->setFocus();
        return;
    }
    emit data_validated();
}

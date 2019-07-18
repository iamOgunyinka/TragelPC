#include <QMessageBox>

#include "widgets/changepassworddialog.hpp"
#include "ui_changepassworddialog.h"

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

void ChangePasswordDialog::SetEmail( QString const &email_address )
{
    email_address_ = email_address;
}

QString ChangePasswordDialog::GetPassword() const
{
    return ui->password_line->text();
}

void ChangePasswordDialog::OnChangeButtonClicked()
{
    QString const email{ ui->email_line->text().trimmed() };
    if( email.isEmpty() || email != email_address_ ){
        QMessageBox::warning( this, "Error", "Email address do not match" );
        ui->email_line->setFocus();
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

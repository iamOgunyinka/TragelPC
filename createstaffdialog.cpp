#include "createstaffdialog.hpp"
#include "ui_createstaffdialog.h"

#include <QMessageBox>
#include <QByteArray>

CreateStaffDialog::CreateStaffDialog(QWidget *parent) :
    QDialog(parent),
    ui( new Ui::CreateStaffDialog )
{
    ui->setupUi( this );
    ui->role_combo->addItems( { "Administrator", "Basic" } );
    ui->role_combo->setDisabled( true );
    QObject::connect( ui->add_button, &QPushButton::clicked, this,
                      &CreateStaffDialog::OnAddStaffButtonClicked );
}

CreateStaffDialog::~CreateStaffDialog()
{
    delete ui;
}

void CreateStaffDialog::SetUserRole( UserRole role )
{
    if( role == UserRole::BasicUser ){
        ui->role_combo->setCurrentIndex( 1 );
    } else {
        ui->role_combo->setCurrentIndex( 0 );
    }
    user_role = role;
}

void CreateStaffDialog::OnAddStaffButtonClicked()
{
    QString const name{ ui->name_edit->text().trimmed() };
    QString const address{ ui->address_edit->text().trimmed() };
    QString const email{ ui->email_edit->text().trimmed() };
    QString const username{ ui->username_edit->text().trimmed() };
    QString const password{ ui->password_edit->text() };

    if( name.isEmpty() || address.isEmpty() || email.isEmpty() || username.isEmpty() || password.isEmpty() ){
        ui->name_edit->setFocus();
        return;
    }

    if( password != ui->repeat_password_edit->text() ){
        QMessageBox::critical( this, "Password", "The password fields do not match" );
        ui->repeat_password_edit->setFocus();
        return;
    }
    emit validated();
}

QJsonObject CreateStaffDialog::GetStaffData() const
{
    QString const name{ ui->name_edit->text().trimmed() };
    QString const address{ ui->address_edit->text().trimmed() };
    QString const email{ ui->email_edit->text().trimmed() };
    QString const username{ ui->username_edit->text().trimmed() };
    QString const password{ ui->password_edit->text() };

    QByteArray const user_pass{ QString( username + ":" + password ).toLocal8Bit().toBase64() };
    QJsonObject const value{ { "name", name }, { "address", address }, { "email", email },
                             { "username_password", QString( user_pass ) },
                             { "type", static_cast<int>( user_role ) } };
    return value;
}

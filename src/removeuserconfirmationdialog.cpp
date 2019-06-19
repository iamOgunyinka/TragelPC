#include "removeuserconfirmationdialog.hpp"
#include "ui_removeuserconfirmationdialog.h"

#include <QMessageBox>

RemoveUserConfirmationDialog::RemoveUserConfirmationDialog( QWidget *parent ):
    QDialog( parent ),
    ui( new Ui::RemoveUserConfirmationDialog )
{
    ui->setupUi( this );
    this->setWindowTitle( "Remove user" );
    QObject::connect( ui->delete_button, &QPushButton::clicked, this,
                      &RemoveUserConfirmationDialog::OnDeleteButtonClicked );
}

RemoveUserConfirmationDialog::~RemoveUserConfirmationDialog()
{
    delete ui;
}

void RemoveUserConfirmationDialog::OnDeleteButtonClicked()
{
    if( ui->recipient_username_line->text() != username_ ){
        QMessageBox::critical( this, "Error", "Username does not match" );
        ui->recipient_username_line->setFocus();
        return;
    }
    if( ui->deletion_reason_line->text().trimmed().isEmpty() ){
        QMessageBox::critical( this, "Error",
                               "State the reason for wanting to delete this "
                               "user" );
        ui->deletion_reason_line->setFocus();
        return;
    }
    if( ui->confirm_password_line->text().isEmpty() ){
        QMessageBox::critical( this, "Error",
                               "Enter your own password for confirmation" );
        ui->confirm_password_line->setFocus();
        return;
    }
    this->accept();
}

void RemoveUserConfirmationDialog::SetUsername( QString const & username )
{
    username_ = username;
}

QString RemoveUserConfirmationDialog::GetAdminPassword() const
{
    return ui->confirm_password_line->text();
}

QString RemoveUserConfirmationDialog::GetDeletionReason() const
{
    return ui->deletion_reason_line->text();
}

QString RemoveUserConfirmationDialog::GetUsername() const
{
    return ui->recipient_username_line->text();
}

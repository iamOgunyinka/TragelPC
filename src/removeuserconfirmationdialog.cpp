#include "removeuserconfirmationdialog.hpp"
#include "ui_removeuserconfirmationdialog.h"

#include <QMessageBox>

RemoveUserConfirmationDialog::RemoveUserConfirmationDialog( QWidget *parent ):
    QDialog( parent ),
    ui( new Ui::RemoveUserConfirmationDialog )
{
    ui->setupUi( this );
    this->setWindowTitle( "Mark inactive" );
    QObject::connect( ui->delete_button, &QPushButton::clicked, this,
                      &RemoveUserConfirmationDialog::OnDeleteButtonClicked );
}

RemoveUserConfirmationDialog::~RemoveUserConfirmationDialog()
{
    delete ui;
}

void RemoveUserConfirmationDialog::OnDeleteButtonClicked()
{
    if( ui->recipient_email_line->text().trimmed() != email_address_ ){
        QMessageBox::critical( this, "Error", "Email address do not match" );
        ui->recipient_email_line->setFocus();
        return;
    }
    if( ui->deletion_reason_line->text().trimmed().isEmpty() ){
        QMessageBox::critical( this, "Error",
                               "State the reason for wanting to mark this "
                               "user inactive" );
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

void RemoveUserConfirmationDialog::SetEmail( QString const & email_address )
{
    email_address_ = email_address;
}

QString RemoveUserConfirmationDialog::GetAdminPassword() const
{
    return ui->confirm_password_line->text();
}

QString RemoveUserConfirmationDialog::GetDeletionReason() const
{
    return ui->deletion_reason_line->text();
}

QString RemoveUserConfirmationDialog::GetEmail() const
{
    return ui->recipient_email_line->text().trimmed();
}

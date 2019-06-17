#include "createstaffdialog.hpp"
#include "ui_createstaffdialog.h"

#include <QMessageBox>
#include <QByteArray>
#include <QProgressDialog>
#include <QJsonDocument>

#include "resources.hpp"

CreateStaffDialog::CreateStaffDialog(QWidget *parent) :
    QDialog(parent),
    ui( new Ui::CreateStaffDialog )
{
    ui->setupUi( this );
    ui->role_combo->addItems( { "Administrator", "Basic" } );
    ui->role_combo->setDisabled( true );
    QObject::connect( ui->add_button, &QPushButton::clicked, this,
                      &CreateStaffDialog::OnAddStaffButtonClicked );
    QObject::connect( this, &CreateStaffDialog::validated, this,
                      &CreateStaffDialog::SendStaffRegistrationUserData );
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

    if( name.isEmpty() || address.isEmpty() || email.isEmpty() ||
            username.isEmpty() || password.isEmpty() )
    {
        ui->name_edit->setFocus();
        return;
    }

    if( password != ui->repeat_password_edit->text() ){
        QMessageBox::critical( this, "Password",
                               "The password fields do not match" );
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

    QByteArray const user_pass{
        QString( username + ":" + password ).toLocal8Bit().toBase64()
    };
    QJsonObject const value{
        { "name", name }, { "address", address }, { "email", email },
        { "username_password", QString( user_pass ) },
        { "type", static_cast<int>( user_role ) }
    };
    return value;
}

void CreateStaffDialog::SendStaffRegistrationUserData()
{
    ui->add_button->setEnabled( false );
    QProgressDialog *progress_dialog{ new QProgressDialog( "Sending information to server",
                                                           "Cancel", 1, 100, this )};
    QUrl const address{ utilities::Endpoint::GetEndpoints().CreateUser() };
    QNetworkRequest const request{ utilities::PostRequestInterface( address ) };
    auto& network_manager = utilities::NetworkManager::GetNetworkWithCookie();

    QByteArray const payload{ QJsonDocument( GetStaffData() ).toJson() };
    progress_dialog->show();
    QNetworkReply *reply{ network_manager.post( request, payload ) };
    connect( reply, &QNetworkReply::downloadProgress, [=]( qint64 received, qint64 total )
    {
        progress_dialog->setMaximum( total + ( total * 0.25 ) );
        progress_dialog->setValue( received );
    });
    connect( progress_dialog, &QProgressDialog::canceled, reply, &QNetworkReply::abort );
    connect( reply, &QNetworkReply::finished, progress_dialog, &QProgressDialog::close );
    connect( reply, &QNetworkReply::finished, [=]{
        ui->add_button->setEnabled( true );
        QJsonObject const response { utilities::GetJsonNetworkData( reply, true ) };
        if( response.isEmpty() ) return;
        QMessageBox::information( this, "Registration", "User added successfully" );
        ClearDataColumn();
    });
    QObject::connect( reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater );
}

void CreateStaffDialog::ClearDataColumn()
{
    ui->address_edit->clear();
    ui->email_edit->clear();
    ui->password_edit->clear();
    ui->repeat_password_edit->clear();
    ui->username_edit->clear();
    ui->name_edit->clear();
    ui->name_edit->setFocus();
}

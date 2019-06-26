#include "changeuserroledialog.hpp"
#include "ui_changeuserroledialog.h"

#include <QMessageBox>

ChangeUserRoleDialog::ChangeUserRoleDialog(QWidget *parent) :
    QDialog( parent ),
    ui( new Ui::ChangeUserRoleDialog )
{
    ui->setupUi( this );
    setWindowTitle( "Change role" );
    QStringList const roles{ "Administrator", "Basic staff" };
    ui->old_role_combo->addItems( roles );
    ui->new_role_combo->addItems( roles );
    ui->old_role_combo->setEnabled( false );
    QObject::connect( ui->new_role_combo,
                      QOverload<int>::of( &QComboBox::currentIndexChanged ),
                      [=]( int const index )
    {
        if( index == 0 ) user_role_ = Administrator;
        else user_role_ = BasicUser;
    });
    QObject::connect( ui->change_button, &QPushButton::clicked, this,
                      &ChangeUserRoleDialog::OnChangeButtonClicked );
}

ChangeUserRoleDialog::~ChangeUserRoleDialog()
{
    delete ui;
}

void ChangeUserRoleDialog::OnChangeButtonClicked()
{
    QString const username{ ui->username_line->text().trimmed() };
    if( username.isEmpty() || username != username_ ){
        QMessageBox::warning( this, "Error", "Username does not match" );
        ui->username_line->setFocus();
        return;
    }
    if( ui->new_role_combo->currentIndex() ==
            ui->old_role_combo->currentIndex() )
    {
        QMessageBox::warning( this, "Error", "The user already has this role "
                                             "assigned");
        return;
    }
    emit data_validated();
}

void ChangeUserRoleDialog::SetUsername( QString const &username )
{
    username_ = username;
}

void ChangeUserRoleDialog::SetCurrentUserRole( int const user_role )
{
    if( user_role == UserRole::Administrator ){
        ui->old_role_combo->setCurrentIndex( 0 );
        ui->new_role_combo->setCurrentIndex( 0 );
        user_role_ = UserRole::Administrator;
    } else {
        ui->old_role_combo->setCurrentIndex( 1 );
        ui->new_role_combo->setCurrentIndex( 1 );
        user_role_ = UserRole::BasicUser;
    }
}

int ChangeUserRoleDialog::GetNewRole() const
{
    return user_role_;
}

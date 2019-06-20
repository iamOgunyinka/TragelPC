#include "updateuserdialog.hpp"
#include "ui_updateuserdialog.h"
#include "resources.hpp"
#include "removeuserconfirmationdialog.hpp"
#include "changepassworddialog.hpp"
#include "changeuserroledialog.hpp"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProgressDialog>
#include <QJsonObject>
#include <QAction>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QUrlQuery>
#include <QJsonArray>
#include <functional>
#include <QJsonDocument>


UpdateUserDialog::UpdateUserDialog(QWidget *parent) :
    QDialog(parent),
    ui( new Ui::UpdateUserDialog ), page_query{ new utilities::PageQuery }
{
    ui->setupUi( this );
    ui->first_page_button->setDisabled( true );
    ui->prev_page_button->setDisabled( true );
    ui->next_page_button->setDisabled( true );
    ui->last_page_button->setDisabled( true );

    ui->tableView->setContextMenuPolicy( Qt::CustomContextMenu );
    QObject::connect( ui->tableView, &QTableView::customContextMenuRequested,
                      this, &UpdateUserDialog::OnCustomMenuRequested );
}

UpdateUserDialog::~UpdateUserDialog()
{
    delete page_query;
    delete ui;
}

void UpdateUserDialog::GetAllUsers()
{
    QUrl const url{ utilities::Endpoint::GetEndpoints().ListStaffs() };
    FetchUserData( url );
}

void UpdateUserDialog::OnCustomMenuRequested( QPoint const &point )
{
    QModelIndex const index{ ui->tableView->indexAt( point )};
    if( !index.isValid() ) return;

    QMenu custom_menu( this );
    custom_menu.setWindowTitle( "Menu" );
    if( !index.parent().isValid() ){
        QAction* action_change_role{ new QAction( "Change role" ) };
        QAction* action_change_password{ new QAction( "Change password" ) };
        QAction* action_remove_user{ new QAction( "Remove user" ) };

        QObject::connect( action_change_role, &QAction::triggered, this,
                          &UpdateUserDialog::OnChangeUserRoleTriggered );
        QObject::connect( action_change_password, &QAction::triggered, this,
                          &UpdateUserDialog::OnChangeUserPasswordTriggered );
        QObject::connect( action_remove_user, &QAction::triggered, this,
                          &UpdateUserDialog::OnRemoveUserTriggered );
        custom_menu.addAction( action_change_role );
        custom_menu.addAction( action_change_password );
        custom_menu.addAction( action_remove_user );
    }
    custom_menu.exec( ui->tableView->mapToGlobal( point ) );
}

void UpdateUserDialog::OnRemoveUserTriggered()
{
    QModelIndex const index{ ui->tableView->currentIndex() };
    if( !index.isValid() ) return;

    RemoveUserConfirmationDialog *remove_dialog {
        new RemoveUserConfirmationDialog( this )
    };
    auto const &user_data{ users_[index.row()] };
    int const last_index{ user_data.username.lastIndexOf( '@') };
    remove_dialog->SetUsername( user_data.username.left( last_index ) );
    QString confirm_username {}, reason{}, admin_password {};

    if( remove_dialog->exec() == QDialog::Accepted ){
        confirm_username = remove_dialog->GetUsername();
        reason = remove_dialog->GetDeletionReason();
        admin_password = remove_dialog->GetAdminPassword();
        remove_dialog->setAttribute( Qt::WA_DeleteOnClose );
    } else return;

    QString const query_payload {
        users_[index.row()].username + ":" + admin_password + ":" + reason
    };
    QUrlQuery query{};
    query.addQueryItem( "payload", query_payload.toLocal8Bit().toBase64() );
    QUrl address{ utilities::Endpoint::GetEndpoints().DeleteUser() };
    address.setQuery( query );

    auto on_success = [=]( QJsonObject const & result ){
        UserModel* model = qobject_cast<UserModel*>( ui->tableView->model() );
        model->removeRows( index.row(), 1, index );
        QMessageBox::information( this, "Delete user",
                                  result.value( "status" ).toString() );
    };
    auto on_error = []{};
    auto report_error = true;
    utilities::SendNetworkRequest( address, on_success, on_error, this,
                                   report_error,
                                   utilities::SimpleRequestType::Delete );
}

void UpdateUserDialog::OnChangeUserRoleTriggered()
{
    QModelIndex const index{ ui->tableView->currentIndex() };
    if( !index.isValid() ) return;
    ChangeUserRoleDialog* role_dialog{ new ChangeUserRoleDialog( this )};
    utilities::UserData const &user_data{ users_[index.row()] };
    int const last_index{ user_data.username.lastIndexOf( '@') };
    role_dialog->SetUsername( user_data.username.left( last_index ) );
    role_dialog->SetCurrentUserRole( user_data.user_role );

    QUrl address{ utilities::Endpoint::GetEndpoints().ChangeRole() };
    auto on_success = [role_dialog]( QJsonObject const &, QNetworkReply* ){
        role_dialog->accept();
    };

    QObject::connect( role_dialog, &ChangeUserRoleDialog::data_validated,
                      [=]() mutable
    {
        QString const a{ QString::number( user_data.user_id ) + ":" +
                            QString::number( role_dialog->GetNewRole() ) };
        QByteArray const payload{ a.toLocal8Bit().toBase64() };
        QUrlQuery url_query {};
        url_query.addQueryItem( "payload", payload );
        address.setQuery( url_query );
        utilities::SendPostNetworkRequest( address, on_success, []{},
                                            role_dialog, payload );
    });
    role_dialog->setAttribute( Qt::WA_DeleteOnClose );
    QObject::connect( role_dialog, &ChangeUserRoleDialog::finished, role_dialog,
                      &ChangeUserRoleDialog::deleteLater );
    role_dialog->exec();
}

void UpdateUserDialog::OnChangeUserPasswordTriggered()
{
    QModelIndex const index{ ui->tableView->currentIndex() };
    if( !index.isValid() ) return;

    auto const &user_data{ users_[index.row()] };
    int const last_index{ user_data.username.lastIndexOf( '@') };

    ChangePasswordDialog* password_dialog { new ChangePasswordDialog( this ) };
    password_dialog->SetUsername( user_data.username.left( last_index ) );
    QUrl const address{ utilities::Endpoint::GetEndpoints().ResetPassword() };
    auto on_sucess = [=]( QJsonObject const &, QNetworkReply* ){
        password_dialog->close();
        QMessageBox::information( this, "Password", "Password reset OK" );
    };
    QObject::connect( password_dialog, &ChangePasswordDialog::data_validated,[=]
    {
        QJsonObject const object {
            { "username", user_data.username },
            { "new_password", password_dialog->GetPassword() }
        };
        QByteArray const payload{ QJsonDocument{ object }.toJson() };
        utilities::SendPostNetworkRequest( address, on_sucess, []{},
                                            password_dialog, payload );
    });
    QObject::connect( password_dialog, &ChangePasswordDialog::finished,
                      password_dialog, &ChangePasswordDialog::deleteLater );
    password_dialog->exec();
}

void UpdateUserDialog::FetchUserData( QUrl const &address )
{
    auto on_error = []{};
    auto on_success{
        std::bind( &UpdateUserDialog::OnGetUsersData, this,
                   std::placeholders::_1 )
    };
    utilities::SendNetworkRequest( address, on_success, on_error, this );
}

void UpdateUserDialog::OnFirstPageButtonClicked()
{
    QUrl const first_page_address{ page_query->first_url };
    FetchUserData( first_page_address );
}

void UpdateUserDialog::OnLastPageButtonClicked()
{
    QUrl const last_page_address{ page_query->last_url };
    FetchUserData( last_page_address );
}

void UpdateUserDialog::OnNextPageButtonClicked()
{
    QUrl const& next_page_address{ page_query->other_url.next_url };
    FetchUserData( next_page_address );
}

void UpdateUserDialog::OnPreviousPageButtonClicked()
{
    QUrl const& next_page_address{ page_query->other_url.previous_url };
    FetchUserData( next_page_address );
}

void UpdateUserDialog::OnGetUsersData( QJsonObject const &result )
{
    bool const suceeds = utilities::ParsePageUrls( result, *page_query );
    if( !suceeds ){
        QMessageBox::information( this, "Users", "No users found" );
        this->accept();
        return;
    }
    QJsonArray users_array{ result.value( "users" ).toArray() };
    while( !users_array.isEmpty() ){
        QJsonValue const value{ users_array[0] };
        if( value.isArray() ){
            users_array = value.toArray();
        } else {
            ParseUserList( users_array );
            break;
        }
    }
    UpdatePageData();
    UserModel* user_model{ new UserModel( users_, ui->tableView ) };
    ui->tableView->setVisible( false );
    ui->tableView->setModel( user_model );
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(
                QHeaderView::Stretch );
    ui->tableView->setVisible( true );
}

void UpdateUserDialog::UpdatePageData()
{
    utilities::UrlData const& url_data{ page_query->other_url };
    auto const page_number = url_data.page_number;
    auto const number_of_pages = page_query->number_of_pages;

    if( page_number < number_of_pages ){
        ui->next_page_button->setEnabled( true );
        ui->last_page_button->setEnabled( true );
    }
    if( page_number > 1 ){
        if( page_number == number_of_pages ){
            ui->prev_page_button->setEnabled( true );
            ui->first_page_button->setEnabled( true );
        } else {
            ui->first_page_button->setEnabled( true );
            ui->last_page_button->setEnabled( true );
            ui->prev_page_button->setEnabled( true );
            ui->next_page_button->setEnabled( true );
        }
    }
}

void UpdateUserDialog::ParseUserList( QJsonArray const &list )
{
    users_.clear();
    for( QJsonValue const & value: list ){
        QJsonObject const user_data_object{ value.toObject() };
        QString username { user_data_object.value( "username" ).toString() };
        utilities::UserData user_data {
            user_data_object.value( "id" ).toInt(),
            user_data_object.value( "role" ).toInt(),
            user_data_object.value( "name" ).toString(), username
        };
        users_.push_back( std::move( user_data ) );
    }
}


UserModel::UserModel( QVector<utilities::UserData> &users, QObject *parent )
    : QAbstractTableModel( parent ), users_{ users }
{
}

Qt::ItemFlags UserModel::flags( QModelIndex const & ) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant UserModel::data( QModelIndex const &index, int role ) const
{
    utilities::UserData const & row_data{ users_[index.row()] };
    if( role == Qt::DisplayRole ){
        switch( index.column() ){
        case 0:
        {
            // usernames usually come like so: username@company_id,
            // and sometimes `username` could be josh20@yahoo.com, so the
            // real username is everything but the last part separated by @
            int const last_index{ row_data.username.lastIndexOf( '@') };
            return row_data.username.left( last_index );
        }
        case 1:
            return row_data.fullname;
        case 2:
            return RoleToString( row_data.user_role );
        default:
            return QVariant{};
        }
    } else if( role == Qt::ToolTipRole ){
        return QString( "Right click to edit user details" );
    }
    return QVariant{};
}

int UserModel::rowCount( QModelIndex const & ) const
{
    return users_.size();
}

int UserModel::columnCount( QModelIndex const &) const
{
    return 3;
}

bool UserModel::removeRows( int row, int count, QModelIndex const & )
{
    users_.remove( row, count );
    return true;
}

QVariant UserModel::headerData( int section, Qt::Orientation orientation,
                                int role ) const
{
    if( role != Qt::DisplayRole ) return QVariant{};
    if( orientation == Qt::Horizontal ){
        switch( section ){
        case 0:
            return QString( "Username" );
        case 1:
            return QString( "Fullname" );
        case 2:
            return QString( "Role" );
        default:
            return QVariant{};
        }
    }
    return section + 1;
}

QString UserModel::RoleToString( int const role ) const
{
    switch( role ){
    case UserRole::BasicUser:
        return "Basic User";
    case UserRole::Administrator:
        return "Administrator";
    default:
        return "Role unknown";
    }
}

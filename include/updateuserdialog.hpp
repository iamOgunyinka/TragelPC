#ifndef UPDATEUSERDIALOG_HPP
#define UPDATEUSERDIALOG_HPP

#include <QDialog>
#include <QVector>
#include <QAbstractItemModel>

namespace Ui {
class UpdateUserDialog;
}

namespace utilities
{
struct UserData
{
    qint64   user_id;
    qint64   user_role;
    QString  fullname;
    QString  username;

    QJsonObject ToJson() const;
};

struct PageQuery;
}

class UpdateUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateUserDialog( QWidget *parent = nullptr );
    ~UpdateUserDialog();
    void GetAllUsers();
private:
    void OnCustomMenuRequested( QPoint const & );
    void OnGetUsersData( QJsonObject const & result );
    void ParseUserList( QJsonArray const & list );
    void OnRemoveUserTriggered();
    void OnChangeUserPasswordTriggered();
    void OnChangeUserRoleTriggered();
    void UpdatePageData();
    void OnFirstPageButtonClicked();
    void OnPreviousPageButtonClicked();
    void OnNextPageButtonClicked();
    void OnLastPageButtonClicked();
    void FetchUserData( QUrl const & address );
private:
    Ui::UpdateUserDialog *ui;
    QVector<utilities::UserData> users_;
    utilities::PageQuery *page_query{};
};

class UserModel: public QAbstractTableModel
{
    Q_OBJECT
    enum UserRole {
        BasicUser = 0x4A,
        Administrator = 0x4B
    };
    QString RoleToString( int const role ) const;
public:
    UserModel( QVector<utilities::UserData>& users, QObject* parent = nullptr );
    int rowCount( QModelIndex const &parent ) const override;
    int columnCount( QModelIndex const &parent ) const override;
    QVariant data( QModelIndex const &index, int role ) const override;
    QVariant headerData( int section, Qt::Orientation orientation, int role )
        const override;
    Qt::ItemFlags flags( QModelIndex const &index ) const override;
    bool removeRows( int row, int count, QModelIndex const & ) override;
private:
    QVector<utilities::UserData> &users_;
};

#endif // UPDATEUSERDIALOG_HPP

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
struct UserData;
struct PageResultQuery;
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
    utilities::PageResultQuery *page_query{};
};
#endif // UPDATEUSERDIALOG_HPP

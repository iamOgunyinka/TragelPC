#ifndef CHANGEUSERROLEDIALOG_HPP
#define CHANGEUSERROLEDIALOG_HPP

#include <QDialog>

namespace Ui {
class ChangeUserRoleDialog;
}

class ChangeUserRoleDialog : public QDialog
{
    Q_OBJECT
    enum UserRole {
        BasicUser = 0x4A,
        Administrator = 0x4B
    };

signals:
    void data_validated();
public:
    explicit ChangeUserRoleDialog(QWidget *parent = nullptr);
    ~ChangeUserRoleDialog();
    void SetUsername( QString const & );
    void SetCurrentUserRole( int const user_role );
    int  GetNewRole() const;
private:
    void OnChangeButtonClicked();
private:
    Ui::ChangeUserRoleDialog *ui;
    QString     username_;
    UserRole    user_role_;
};

#endif // CHANGEUSERROLEDIALOG_HPP

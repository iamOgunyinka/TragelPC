#ifndef CREATESTAFFDIALOG_HPP
#define CREATESTAFFDIALOG_HPP

#include <QDialog>
#include <QJsonObject>

namespace Ui {
class CreateStaffDialog;
}

enum class UserRole {
    BasicUser = 0x4A,
    Administrator = 0x4B
};

class CreateStaffDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CreateStaffDialog(QWidget *parent = 0);
    ~CreateStaffDialog();
    void SetUserRole( UserRole role );
signals:
    // emitted instead of accepted,since we dont want the window to close(hence deleted)just yet
    void validated();
private:
    QJsonObject GetStaffData() const;
    void OnAddStaffButtonClicked();
    void SendStaffRegistrationUserData();
    void ClearDataColumn();
private:
    Ui::CreateStaffDialog *ui;
    UserRole user_role;
};

#endif // CREATESTAFFDIALOG_HPP

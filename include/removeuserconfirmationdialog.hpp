#ifndef REMOVEUSERCONFIRMATIONDIALOG_HPP
#define REMOVEUSERCONFIRMATIONDIALOG_HPP

#include <QDialog>

namespace Ui {
class RemoveUserConfirmationDialog;
}

class RemoveUserConfirmationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RemoveUserConfirmationDialog( QWidget *parent = nullptr );
    void SetEmail( QString const & email_address );
    QString GetEmail() const;
    QString GetDeletionReason() const;
    QString GetAdminPassword() const;
    ~RemoveUserConfirmationDialog();
private:
    void OnDeleteButtonClicked();
private:
    Ui::RemoveUserConfirmationDialog *ui;
    QString email_address_;
};

#endif // REMOVEUSERCONFIRMATIONDIALOG_HPP

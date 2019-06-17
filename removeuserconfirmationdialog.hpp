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
    void SetUsername( QString const & username );
    QString GetUsername() const;
    QString GetDeletionReason() const;
    QString GetAdminPassword() const;
    ~RemoveUserConfirmationDialog();
private:
    void OnDeleteButtonClicked();
private:
    Ui::RemoveUserConfirmationDialog *ui;
    QString username_;
};

#endif // REMOVEUSERCONFIRMATIONDIALOG_HPP

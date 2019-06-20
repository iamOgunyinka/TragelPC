#ifndef CHANGEPASSWORDDIALOG_HPP
#define CHANGEPASSWORDDIALOG_HPP

#include <QDialog>

namespace Ui {
class ChangePasswordDialog;
}

class ChangePasswordDialog : public QDialog
{
    Q_OBJECT

signals:
    void data_validated();
public:
    explicit ChangePasswordDialog(QWidget *parent = 0);
    ~ChangePasswordDialog();
    void SetUsername( QString const & username );
    QString GetPassword() const;
private:
    void OnChangeButtonClicked();
private:
    Ui::ChangePasswordDialog *ui;
    QString username_;
};

#endif // CHANGEPASSWORDDIALOG_HPP

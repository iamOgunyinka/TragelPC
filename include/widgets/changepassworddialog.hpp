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
    explicit ChangePasswordDialog(QWidget *parent = nullptr);
    ~ChangePasswordDialog();
    void SetEmail( QString const & email_address );
    QString GetPassword() const;
private:
    void OnChangeButtonClicked();
private:
    Ui::ChangePasswordDialog *ui;
    QString email_address_;
};

#endif // CHANGEPASSWORDDIALOG_HPP

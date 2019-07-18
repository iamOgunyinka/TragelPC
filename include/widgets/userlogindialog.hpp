#ifndef USERLOGINDIALOG_HPP
#define USERLOGINDIALOG_HPP

#include <QDialog>

namespace Ui {
class UserLoginDialog;
}

class UserLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserLoginDialog(QWidget *parent = nullptr);
    ~UserLoginDialog();
    void SetCompanyID( QString const & company_id );
    QString GetUsername() const;
    QString GetPassword() const;
private:
    Ui::UserLoginDialog *ui;
};

#endif // USERLOGINDIALOG_HPP

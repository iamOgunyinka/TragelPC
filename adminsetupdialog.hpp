#ifndef ADMINSETUPDIALOG_HPP
#define ADMINSETUPDIALOG_HPP

#include <QDialog>

namespace Ui {
class AdminSetupDialog;
}

class AdminSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AdminSetupDialog(QWidget *parent = 0);
    ~AdminSetupDialog();
    QString GetSUUsername() const;
    QString GetSUPassword() const;
    QString GetServerBaseURL() const;
    QString GetServerEnpointsURL() const;
private:
    Ui::AdminSetupDialog *ui;
};

#endif // ADMINSETUPDIALOG_HPP

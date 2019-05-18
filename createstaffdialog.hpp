#ifndef CREATESTAFFDIALOG_HPP
#define CREATESTAFFDIALOG_HPP

#include <QDialog>

namespace Ui {
class CreateStaffDialog;
}

class CreateStaffDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateStaffDialog(QWidget *parent = 0);
    ~CreateStaffDialog();

private:
    Ui::CreateStaffDialog *ui;
};

#endif // CREATESTAFFDIALOG_HPP

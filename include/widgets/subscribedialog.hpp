#ifndef SUBSCRIBEDIALOG_HPP
#define SUBSCRIBEDIALOG_HPP

#include <QDialog>

namespace Ui {
class SubscribeDialog;
}

class SubscribeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SubscribeDialog(QWidget *parent = nullptr);
    ~SubscribeDialog();
    void OnSubscribeButtonClicked();

private:
    Ui::SubscribeDialog *ui;
};

#endif // SUBSCRIBEDIALOG_HPP

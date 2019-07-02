#ifndef SETUPDIALOG_HPP
#define SETUPDIALOG_HPP

#include <QDialog>
#include <QSettings>

namespace Ui {
class SetupDialog;
}

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog( QWidget *parent = 0 );
    ~SetupDialog();

private:
    void OnFactoryResetButtonClicked();
private:
    Ui::SetupDialog *ui;
    QSettings& app_settings;
};

#endif // SETUPDIALOG_HPP

#ifndef SETUPDIALOG_HPP
#define SETUPDIALOG_HPP

#include <QDialog>
#include <QSettings>

namespace Ui {
class SetupDialog;
}

namespace utilities {
class ApplicationSettings;
}

class SetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SetupDialog( QWidget *parent = nullptr );
    ~SetupDialog();

private:
    void SetupPingInterval();
    void SetupOrderPoll();
    void SetupPerPageResult();
    void OnFactoryResetButtonClicked();
private:
    Ui::SetupDialog *ui;
    utilities::ApplicationSettings& app_settings;
};

#endif // SETUPDIALOG_HPP

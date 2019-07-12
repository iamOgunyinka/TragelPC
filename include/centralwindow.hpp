#ifndef CENTRALWINDOW_HPP
#define CENTRALWINDOW_HPP

#include <QMainWindow>
#include <QMdiArea>
#include <QCloseEvent>
#include <QTime>
#include <QTimer>


namespace Ui {
class CentralWindow;
}

namespace utilities {
class Endpoint;
class ApplicationSettings;
}

class CentralWindow : public QMainWindow
{
    Q_OBJECT
signals:
    void closed();
public:
    explicit CentralWindow( QWidget *parent = nullptr );
    void StartApplication();
    ~CentralWindow();
protected:
    void closeEvent( QCloseEvent *event ) override;
private:
    void CentralizeDisplayWidget( QWidget * const widget, QSize const & size);
    void LogUserIn( QString const & username, QString const & password );
    void PingServerNetwork();
    void StartOrderPolling();
    void EnableShortcuts();
    void OnOrderPollResultObtained( QJsonObject const &result );
    void ActivatePingTimer();
    void LoadSettingsFile();
    void OnLoginButtonClicked();
    void OnLogoutButtonClicked();
    void OnOrderActionTriggered();
    void OnAddProductTriggered();
    void OnAddUserTriggered();
    void OnSettingsTriggered();
    void OnListAllProductsTriggered();
    void OnListUsersTriggered();
    void OnMakeOrderTriggered();
    void OnSubscribeTriggered();
    void OnShowExpiryTriggered();
    void OnListSubscriptionsTriggered();
    void GetEndpointsFromServer( QString const & url, QString const & username,
                                 QString const & password,
                                 QString const & company_id );
    void WriteEndpointsToPersistenceStorage( utilities::Endpoint const & );
    void SetEnableCentralWindowBars( bool const enabled );
    void SetEnableActionButtons( bool const enable = true );
private:
    Ui::CentralWindow *ui;
    QMdiArea *workspace;
    QTimer*   server_ping_timer{};
    QTimer*   server_order_poll_timer{};
    utilities::ApplicationSettings& app_settings;
    QTime     time_interval{};
    qint64    last_order_count;
    bool      logged_in;
};

#endif // CENTRALWINDOW_HPP

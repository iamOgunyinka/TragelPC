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
}

class CentralWindow : public QMainWindow
{
    Q_OBJECT
signals:
    void closed();
public:
    explicit CentralWindow( QWidget *parent = 0 );
    void StartApplication();
    ~CentralWindow();
protected:
    void closeEvent( QCloseEvent *event ) override;
private:
    void CentralizeDisplayWidget( QWidget * const widget, QSize const & size);
    void LogUserIn( QString const & username, QString const & password );
    void PingServerNetwork();
    void StartOrderPolling();
    void OnOrderPollResultObtained( QJsonObject const &result );
    void ActivatePingTimer();
    void LoadSettingsFile();
    void OnLoginButtonClicked();
    void OnLogoutButtonClicked();
    void OnOrderActionTriggered();
    void OnAddProductTriggered();
    void OnAddUserTriggered();
    void OnListAllProductsTriggered();
    void OnListUsersTriggered();
    void OnReportsActionTriggered();
    void GetEndpointsFromServer( QString const & url, QString const & username,
                                 QString const & password,
                                 QString const & company_id );
    void WriteEndpointsToPersistenceStorage( utilities::Endpoint const & );
    void SetEnableCentralWindowBars( bool const enabled );
    void SetEnableActionButtons( bool const enable = true );

    static int const an_hour = 60 * 60; // 60 seconds x 60 minutes
private:
    Ui::CentralWindow *ui;
    QMdiArea *workspace;
    QTimer*   server_ping_timer{};
    QTimer*   server_order_poll_timer{};
    QTime     time_interval{};
    qint64    last_order_count;
};

#endif // CENTRALWINDOW_HPP

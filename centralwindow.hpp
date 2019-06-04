#ifndef CENTRALWINDOW_HPP
#define CENTRALWINDOW_HPP

#include <QMainWindow>
#include <QMdiArea>
#include <QCloseEvent>

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
    void LoadSettingsFile();
    void OnLoginButtonClicked();
    void OnLogoutButtonClicked();
    void OnOrderActionTriggered();
    void OnAddProductTriggered();
    void OnAddUserTriggered();
    void OnListAllProductsTriggered();
    void GetEndpointsFromServer( QString const & url, QString const & username,
                                 QString const & password, QString const & company_id );
    void SendStaffRegistrationUserData(QJsonObject const & data,
                                        QMdiSubWindow * const data_widget );
    void WriteEndpointsToPersistenceStorage( utilities::Endpoint const & );
    void SetEnableCentralWindowBars( bool const enabled );
    void SetEnableActionButtons( bool const enable = true );
private:
    Ui::CentralWindow *ui;
    QMdiArea *workspace;
};

#endif // CENTRALWINDOW_HPP

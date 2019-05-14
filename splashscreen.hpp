#ifndef SPLASHSCREEN_HPP
#define SPLASHSCREEN_HPP

#include <QApplication>
#include <QObject>
#include <QSplashScreen>
#include <QString>
#include <QMap>

namespace utilities {
class Endpoint;
}

class SplashScreen : public QObject
{
    Q_OBJECT
public:
    explicit SplashScreen(QApplication& app, QObject *parent = nullptr );
    void DisplaySplash();

signals:
    void done();
public slots:
private:
    void PingServerNetwork();
    void LoadSettingsFile();
    void GetEndpointsFromServer( QString const & url, QString const & username,
                                 QString const & password );
    void WriteEndpointsToPersistenceStorage( utilities::Endpoint const & );
private:
    QSplashScreen *splash_screen;
    QApplication& main_app;
    QMap<QString, void(SplashScreen::*)()> tasks {};
    bool has_error;
};

#endif // SPLASHSCREEN_HPP

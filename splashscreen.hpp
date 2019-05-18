#ifndef SPLASHSCREEN_HPP
#define SPLASHSCREEN_HPP

#include <QApplication>
#include <QObject>
#include <QSplashScreen>
#include <QString>

class SplashScreen : public QObject
{
    Q_OBJECT
public:
    explicit SplashScreen(QApplication& app, QObject *parent = nullptr );
    void DisplaySplash();

signals:
    void done();
public slots:
    void close( QWidget * );

private:
    QApplication& main_app;
    QSplashScreen *splash_screen;
};

#endif // SPLASHSCREEN_HPP

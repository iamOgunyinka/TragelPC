#include <QPixmap>
#include <QSettings>
#include <QString>
#include <QByteArray>
#include <QNetworkReply>
#include <QIcon>
#include <QPair>
#include <QWidget>
#include <QThread>

#include "splashscreen.hpp"
#include "adminsetupdialog.hpp"
#include "resources.hpp"
#include "framelesswindow.h"

SplashScreen::SplashScreen( QApplication& app, QObject *parent ) : QObject(parent),
    main_app{ app }, splash_screen{ nullptr }
{
    QPixmap splash_screen_pixmap{ ":/all_images/images/tragel.png" };
    splash_screen = new QSplashScreen{ splash_screen_pixmap.scaled( 600, 400 ) };
}

void SplashScreen::DisplaySplash()
{
    splash_screen->show();
    splash_screen->showMessage( "Loading application settings",
                                Qt::AlignBottom | Qt::AlignHCenter, Qt::blue );
    main_app.processEvents();
    this->thread()->sleep( 5 );
    emit done();
}

void SplashScreen::close( QWidget *main_window )
{
    splash_screen->finish( main_window );
}


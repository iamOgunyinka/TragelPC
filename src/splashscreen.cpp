#include <QPixmap>
#include <QThread>

#include "splashscreen.hpp"

SplashScreen::SplashScreen( QApplication& app, QObject *parent ) :
    QObject( parent ), main_app{ app }, splash_screen{ nullptr }
{
    QPixmap splash_screen_pixmap{ ":/all_images/images/tragel.png" };
    splash_screen = new QSplashScreen{ splash_screen_pixmap.scaled( 600, 400 ) };
}

void SplashScreen::DisplaySplash()
{
    splash_screen->show();
    splash_screen->showMessage( "Loading application settings",
                                Qt::AlignBottom | Qt::AlignHCenter, Qt::blue );
    QApplication::processEvents();
    thread()->sleep( 2 );
    emit done();
}

void SplashScreen::close( QWidget *main_window )
{
    splash_screen->finish( main_window );
}

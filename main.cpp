#include "centralwindow.hpp"
#include <QApplication>
#include "DarkStyle.h"
#include "framelesswindow.h"
#include "splashscreen.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(new DarkStyle);

    FramelessWindow frameless_window {};
    CentralWindow* w = new CentralWindow {};
    w->setWindowIcon( a.style()->standardIcon( QStyle::SP_DesktopIcon ) );
    frameless_window.setContent( w );
    frameless_window.setWindowTitle( "Tragel" );
    frameless_window.setWindowIcon( a.style()->standardIcon( QStyle::SP_DesktopIcon ) );
    frameless_window.setWindowTitle( "Tragel" );

    SplashScreen splash_screen {a};
    QObject::connect( &splash_screen, &SplashScreen::done, [&]{
        splash_screen.close( &frameless_window );
        frameless_window.showMaximized();
        w->StartApplication();
    });
    QObject::connect( &frameless_window, &FramelessWindow::is_closing, w, &CentralWindow::close );
    QObject::connect( w, &CentralWindow::closed, &frameless_window, &FramelessWindow::close );
    splash_screen.DisplaySplash();
    return a.exec();
}

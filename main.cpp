#include "mainwindow.hpp"
#include <QApplication>
#include "DarkStyle.h"
#include "framelesswindow.h"
#include "splashscreen.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(new DarkStyle);

    FramelessWindow frameless_window {};
    MainWindow* w = new MainWindow {};
    frameless_window.setContent( w );
    frameless_window.setWindowTitle( "Tragel" );
    frameless_window.setWindowIcon( QIcon( ":/all_images/images/logo.png" ));
    frameless_window.setWindowTitle( "Tragel" );

    SplashScreen splash_screen {&a};
    QObject::connect( &splash_screen, &SplashScreen::done, [&frameless_window]{
        frameless_window.show();
    });
    splash_screen.DisplaySplash();
    return a.exec();
}

#include <QTimer>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>

#include "utils/popupnotifier.hpp"

PopUpNotifier::PopUpNotifier(QWidget *parent) : QWidget{ parent },
    label{ new QLabel( this) },
    layout{ new QGridLayout( this ) },
    timer{ new QTimer( this ) }
{
    setWindowFlags( Qt::FramelessWindowHint | Qt::Tool
                    | Qt::WindowStaysOnTopHint );
    setAttribute( Qt::WA_TranslucentBackground );
    setAttribute( Qt::WA_ShowWithoutActivating );

    animation.setTargetObject( this );
    animation.setPropertyName( "popup_opacity" );
    label->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );
    label->setStyleSheet( "QLabel { color: white; "
                          "margin-top: 6px; "
                          "margin-bottom: 6px; "
                          "margin-left: 10px; "
                          "margin-right: 10px; }");
    layout->addWidget( label, 0, 0 );
    this->setLayout( layout );
    QObject::connect( &animation, &QAbstractAnimation::finished, this,
                      &PopUpNotifier::hide );
    QObject::connect( timer, &QTimer::timeout, this,
                      &PopUpNotifier::HideAnimation );
}

void PopUpNotifier::paintEvent( QPaintEvent * )
{
    QPainter painter{ this };
    painter.setRenderHint( QPainter::Antialiasing );

    QRect rounded_rect {};
    QRect const window_rect{ this->rect() };
    rounded_rect.setX( window_rect.x() + 5 );
    rounded_rect.setY( window_rect.y() + 5 );
    rounded_rect.setWidth( window_rect.width() - 10 );
    rounded_rect.setHeight( window_rect.height() - 10 );

    painter.setBrush( QBrush{ QColor{ 0, 0, 0, 180 } } );
    painter.setPen( Qt::NoPen );
    painter.drawRoundedRect( rounded_rect, 10, 10 );
}

void PopUpNotifier::SetPopUpText( QString const &text )
{
    label->setText( text );
    this->adjustSize();
}

void PopUpNotifier::show()
{
    setWindowOpacity( 0.0f );
    animation.setDuration( 150 );
    animation.setStartValue( 0.0 );
    animation.setEndValue( 1.0 );

    auto const pc_geometry = QApplication::desktop()->availableGeometry();
    int const x { pc_geometry.width() - 36 - this->width() + pc_geometry.x() };
    int const y {pc_geometry.height() - 36 - this->height() + pc_geometry.y() };

    this->setGeometry( x, y, this->width(), this->height() );
    QWidget::show();
    animation.start();
    timer->start( 3000 );
}

void PopUpNotifier::HideAnimation()
{
    timer->stop();
    animation.setDuration( 1000 );
    animation.setStartValue( 1.0 );
    animation.setEndValue( 0.0 );
    animation.start();
}

void PopUpNotifier::hide()
{
    if( GetPopUpOpacity() == 0.0 ) QWidget::hide();
}

void PopUpNotifier::SetPopUpOpacity(const float opacity)
{
    popup_opacity = opacity;
    setWindowOpacity( popup_opacity );
}

float PopUpNotifier::GetPopUpOpacity() const
{
    return popup_opacity;
}

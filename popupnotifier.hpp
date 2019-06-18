#ifndef POPUPNOTIFIER_HPP
#define POPUPNOTIFIER_HPP

#include <QWidget>
#include <QGridLayout>
#include <QTimer>
#include <QPropertyAnimation>
#include <QLabel>

class PopUpNotifier : public QWidget
{
    Q_OBJECT

    Q_PROPERTY( float popup_opacity READ GetPopUpOpacity WRITE SetPopUpOpacity )

    void SetPopUpOpacity( float const opacity );
    float GetPopUpOpacity() const;
public:
    explicit PopUpNotifier( QWidget *parent = nullptr );
protected:
    void paintEvent( QPaintEvent* ) override;
signals:

public slots:
    void SetPopUpText( QString const & text );
    void show();

private slots:
    void HideAnimation();
    void hide();
private:
    QLabel* label;
    QGridLayout* layout;
    QTimer* timer;
    float popup_opacity;
    QPropertyAnimation animation;
};

#endif // POPUPNOTIFIER_HPP

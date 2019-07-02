#include "setupdialog.hpp"
#include "ui_setupdialog.h"

#include "resources.hpp"

SetupDialog::SetupDialog( QWidget *parent ):
    QDialog( parent ), ui( new Ui::SetupDialog ),
    app_settings{ utilities::ApplicationSettings::GetAppSettings() }
{
    ui->setupUi( this );
    int const poll_value = app_settings.value( "order_poll_intvl", 25 ).toInt();
    ui->order_poll_interval_slider->setMinimum( 20 ); // 20 seconds
    ui->order_poll_interval_slider->setMaximum( 120 ); // 5 minutes
    ui->order_poll_interval_slider->setTickInterval( 5 );
    ui->order_poll_interval_slider->setValue( poll_value );

    int const ping_value = app_settings.value( "ping_nt_intvl", 1 ).toInt();
    ui->ping_noty_interval_slider->setMinimum( 1 );
    ui->ping_noty_interval_slider->setMaximum( 60 );
    ui->ping_noty_interval_slider->setTickInterval( 1 );
    ui->ping_noty_interval_slider->setValue( ping_value );

    bool const allow_shortcut = app_settings.value( "allow_st", false ).toBool();
    ui->allow_shortcut_checkbox->setChecked( allow_shortcut );
    bool const confirm_deletion = app_settings.value( "confirm_deletion", true )
            .toBool();
    ui->confirm_deletion_checkbox->setChecked( confirm_deletion );
    bool const show_splash = app_settings.value( "show_splash", true ).toBool();
    ui->show_splashscreen_checkbox->setChecked( show_splash );

    QObject::connect( ui->factory_reset_button, &QPushButton::clicked, this,
                      &SetupDialog::OnFactoryResetButtonClicked );

    QObject::connect( ui->order_poll_interval_slider, &QSlider::valueChanged,
                      [=]( int const new_value )
    {
        utilities::ApplicationSettings::SetOrderPollInterval( new_value );
    });
    QObject::connect( ui->ping_noty_interval_slider, &QSlider::valueChanged,
                      [=]( int const new_value )
    {
        utilities::ApplicationSettings::SetPingNotificationInterval( new_value );
    });
    QObject::connect( ui->confirm_deletion_checkbox, &QCheckBox::toggled,
                      [=]( bool checked )
    {
        app_settings.setValue( "confirm_deletion", checked );
        app_settings.sync();
    });
    QObject::connect( ui->show_splashscreen_checkbox, &QCheckBox::toggled,
                      [=]( bool checked )
    {
        app_settings.setValue( "show_splash", checked );
        app_settings.sync();
    });
    QObject::connect( ui->allow_shortcut_checkbox, &QCheckBox::toggled,
                      [=]( bool checked )
    {
        app_settings.setValue( "allow_st", checked );
        app_settings.sync();
    });
}

SetupDialog::~SetupDialog()
{
    delete ui;
}

void SetupDialog::OnFactoryResetButtonClicked()
{

}

#include "setupdialog.hpp"
#include "ui_setupdialog.h"

#include "resources.hpp"

#include <QMessageBox>

SetupDialog::SetupDialog( QWidget *parent ):
    QDialog( parent ), ui( new Ui::SetupDialog ),
    app_settings{ utilities::ApplicationSettings::GetAppSettings() }
{
    ui->setupUi( this );
    using utilities::SettingsValue;

    int const poll_value{
        app_settings.Value( SettingsValue::OrderPollInterval, 25 ).toInt()
    };
    QString const poll_text { "Check for new orders every: %1 %2 %3" };
    ui->order_poll_interval_slider->setMinimum( 20 ); // 20 seconds
    ui->order_poll_interval_slider->setMaximum( 120 ); // 5 minutes
    ui->order_poll_interval_slider->setTickInterval( 5 );
    ui->order_poll_interval_slider->setValue( poll_value );

    auto set_poll_text = [=]( int const value ){
        if( value < 60 ){
            ui->periodic_poll_label->setText( poll_text.arg( value )
                                              .arg( "seconds" ).arg( "" ) );
        } else {
            int minute = value / 60, seconds = value % 60;
            ui->periodic_poll_label->setText( poll_text.arg( minute )
                                              .arg( "minutes" ).arg( seconds )
                                              + "seconds" );
        }
    };
    set_poll_text( poll_value );

    int const ping_value {
        app_settings.Value( SettingsValue::PingNotifInterval, 1 ).toInt()
    };
    QString const ping_interval_label {
        "Notify me of failed server-pings every: %1 minutes(s)."
    };
    ui->ping_noty_interval_slider->setMinimum( 30 );
    ui->ping_noty_interval_slider->setMaximum( 120 );
    ui->ping_noty_interval_slider->setTickInterval( 5 );
    ui->ping_noty_interval_slider->setValue( ping_value );
    ui->ping_label->setText( ping_interval_label.arg( ping_value ));

    bool const allow_shortcut {
        app_settings.Value( SettingsValue::AllowShortcut, false ).toBool()
    };
    ui->allow_shortcut_checkbox->setChecked( allow_shortcut );
    bool const confirm_deletion{
        app_settings.Value( SettingsValue::ConfirmDeletion, true ).toBool()
    };
    ui->confirm_deletion_checkbox->setChecked( confirm_deletion );
    bool const show_splash{
        app_settings.Value( SettingsValue::ShowSplash, true ).toBool()
    };
    ui->show_splashscreen_checkbox->setChecked( show_splash );

    QObject::connect( ui->factory_reset_button, &QPushButton::clicked, this,
                      &SetupDialog::OnFactoryResetButtonClicked );

    QObject::connect( ui->order_poll_interval_slider, &QSlider::valueChanged,
                      [=]( int const new_value )
    {
        app_settings.SetValue( SettingsValue::OrderPollInterval, new_value );
        set_poll_text( new_value );
    });

    QObject::connect( ui->ping_noty_interval_slider, &QSlider::valueChanged,
                      [=]( int const new_value )
    {
        app_settings.SetValue( SettingsValue::PingNotifInterval, new_value );
        ui->ping_label->setText( ping_interval_label.arg( new_value ) );
    });
    QObject::connect( ui->confirm_deletion_checkbox, &QCheckBox::toggled,
                      [=]( bool checked )
    {
        app_settings.SetValue( SettingsValue::ConfirmDeletion, checked );
    });
    QObject::connect( ui->show_splashscreen_checkbox, &QCheckBox::toggled,
                      [=]( bool checked )
    {
        app_settings.SetValue( SettingsValue::ShowSplash, checked );
    });
    QObject::connect( ui->allow_shortcut_checkbox, &QCheckBox::toggled,
                      [=]( bool checked )
    {
        app_settings.SetValue( SettingsValue::AllowShortcut, checked );
    });
}

SetupDialog::~SetupDialog()
{
    delete ui;
}

void SetupDialog::OnFactoryResetButtonClicked()
{
    QMessageBox::information( this, "Reset", "Not implemented yet" );
}

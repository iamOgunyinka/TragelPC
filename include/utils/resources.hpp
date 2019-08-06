#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QNetworkReply>
#include <QSettings>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QList>
#include <QVector>
#include <QProgressDialog>
#include <QMutex>

namespace utilities {
    static QString const USER_AGENT{ "Mozilla/5.0 (X11; Linux i586; rv:31.0) "
                                     "Gecko/20100101 Firefox/31.0" };

    class PersistentCookieJar : public QNetworkCookieJar {
    public:
        PersistentCookieJar( QObject *parent = nullptr );
        ~PersistentCookieJar() override;

        QList<QNetworkCookie> cookiesForUrl( QUrl const &url ) const override;
        virtual bool setCookiesFromUrl( QList<QNetworkCookie> const &cookieList,
                                        QUrl const &url ) override;
    private:
        void save();
        void load();
    private:
        mutable QMutex mutex;
    };

    class NetworkManager
    {
        NetworkManager() = default;
    public:
        static QNetworkAccessManager& GetNetworkWithCookie();
        static QNetworkAccessManager& GetNetwork();
        static QNetworkCookieJar& GetSessionCookie();
        static bool SetNetworkCookie( QNetworkCookieJar& jar,
                                      QNetworkReply* network_reply );
    };

    enum class SettingsValue
    {
        OrderCount = 0,
        UrlMap,
        PingInterval,
        OrderPollInterval,
        PingNotifInterval,
        AllowShortcut,
        ConfirmDeletion,
        ShowSplash,
        ResultPerPage,
        Cookies,
        KeepMeLoggedIn
    };

    QString SettingsValueToString( SettingsValue const val );
    class ApplicationSettings
    {
        QSettings& settings;
    private:
        ApplicationSettings();
        static QSettings& GetSettings( QString const & organisation,
                                       QString const & application );
    public:
        ApplicationSettings( ApplicationSettings const & ) = delete;
        ApplicationSettings( ApplicationSettings && ) = delete;
        ApplicationSettings& operator=( ApplicationSettings const & ) = delete;
        ApplicationSettings& operator=( ApplicationSettings && ) = delete;

        void     SetValue( SettingsValue const data, QVariant const &value );
        QVariant Value( SettingsValue const value,
                        QVariant const & default_ = {} ) const;
        void     Remove( SettingsValue const value );
        static  ApplicationSettings& GetAppSettings();
    };

    class Endpoint
    {
        QJsonObject endpoint_object;
        Endpoint() = default;
    public:
        QString CompanyToken() const;
        QString CompanyID() const;
        QString LoginTo() const;
        QString LogoutFrom() const;
        QString CreateUser() const;
        QString DeleteUser() const;
        QString ResetPassword() const;
        QString ChangeRole() const;
        QString ConfirmPayment() const;
        QString GetSubscriptions() const;
        QString AddSubscription() const;
        QString GetProducts() const;
        QString GetProductByID () const;
        QString RemoveProduct() const;
        QString ListStaffs() const;
        QString UpdateProducts() const;
        QString AddProduct() const;
        QString GetOrders() const;
        QString GetOrderByID() const;
        QString GetOrderCount() const;
        QString AddOrder() const;
        QString RemoveOrder() const;
        QString PingAddress() const;
        QString GetExpiryDate() const;
        QString UploadPhoto() const;
    public:
        Endpoint( Endpoint && ) = delete;
        Endpoint( Endpoint const & ) = delete;
        Endpoint& operator=( Endpoint const &)= delete;
        Endpoint& operator=( Endpoint && ) = delete;
        QJsonObject ToJson() const;

        static Endpoint& GetEndpoints();
        static void ParseEndpointsFromJson( Endpoint& endpoint,
                                            QJsonObject const & json_object );
    };

    struct UrlData
    {
        QString next_url;
        QString previous_url;
        uint    page_number;
    };

    struct PageResultQuery
    {
        uint    number_of_pages {};
        uint    result_per_page {};
        uint    total_result {};
        QString first_url{};
        QString last_url{};
        UrlData other_url{};
    };

    struct Product
    {
        QString name;
        QString thumbnail_location;
        QString categories;
        QString description;
        double  price{0.0};
        qint64  product_id{0};

        Product() = default;
        Product( QString name, double p, qint64 prod_id,
                 QString thumbnail = QString(), QString cat = QString(),
                 QString desc = QString() );

        QJsonObject ToJson() const;
    };

    bool operator==( Product const & a, Product const & b );

    enum PaymentType
    {
        Cash,
        E_Banking
    };

    struct PaymentConfirmationData
    {
        bool confirmed;
        QDateTime date_of_confirmation;
        QString   confirmed_by;
    };

    struct MakeOrderItem
    {
        utilities::Product product;
        int quantity;

        QJsonObject ToJson() const;
    };

    struct OrderingResultItem {
        QString product_name;
        qint64  quantity;
        double  price;
    };

    struct OrderResultData
    {
        qint64      order_id;
        int         payment_type;
        QString     confirmed_by;//empty by default, unless payment is confirmed
        QString     reference_id;
        QString     staff_username;
        QDateTime   order_date;
        QVector<OrderingResultItem> items;
        PaymentConfirmationData confirmation_data;
    };

    enum class SimpleRequestType
    {
        Get,
        Delete,
    };

    bool ParsePageUrls( QJsonObject const & page_url,
                        PageResultQuery& page_information );
    QPair<QNetworkRequest, QByteArray> PostImageRequestInterface(
            QUrl const &address, QVector<QString> const &data );
    QNetworkRequest GetRequestInterface( QUrl const &address );
    QNetworkRequest PostRequestInterface( QUrl const &address );
    QJsonObject     GetJsonNetworkData( QNetworkReply *data,
                                        bool show_error_message = false );

    template<typename FuncOnSuccess, typename FuncOnError>
    void SendNetworkRequest( QUrl const & address, FuncOnSuccess && on_success,
                             FuncOnError && on_error, QWidget *parent,
                             bool report_error = true,
                             SimpleRequestType type = SimpleRequestType::Get )
    {
        QNetworkRequest const request = GetRequestInterface( address );
        auto& network_manager = NetworkManager::GetNetworkWithCookie();
        QProgressDialog* progress_dialog =
            new QProgressDialog( "Please wait", "Cancel", 1, 100, parent );
        progress_dialog->show();
        QNetworkReply* reply;
        if( type == SimpleRequestType::Get ){
            reply = network_manager.get( request );
        } else {
            reply = network_manager.deleteResource( request );
        }
        QObject::connect( reply, &QNetworkReply::downloadProgress,
                          [=]( qint64 const received, qint64 const total )
        {
            progress_dialog->setMaximum( total + ( total * 0.25 ));
            progress_dialog->setValue( received );
        });

        QObject::connect( reply, &QNetworkReply::sslErrors, reply,
                          QOverload<QList<QSslError> const &>::of(
                              &QNetworkReply::ignoreSslErrors ) );
        QObject::connect( progress_dialog, &QProgressDialog::canceled, reply,
                          &QNetworkReply::abort );
        QObject::connect( reply, &QNetworkReply::finished, progress_dialog,
                          &QProgressDialog::close );
        QObject::connect( reply, &QNetworkReply::finished, [=]
        {
            QJsonObject const result =
                utilities::GetJsonNetworkData( reply, report_error );
            if( result.isEmpty() ) on_error();
            else on_success( result );
        });
    }

    template<typename FuncOnSuccess, typename FuncOnError>
    void SendPostNetworkRequest( QUrl const & address,
                                 FuncOnSuccess && on_success,
                                 FuncOnError && on_error, QWidget *parent,
                                 QByteArray const &payload,
                                 bool report_error = true )
    {
        QNetworkRequest const request = PostRequestInterface( address );
        auto& network_manager = NetworkManager::GetNetworkWithCookie();
        QProgressDialog* progress_dialog =
            new QProgressDialog( "Please wait", "Cancel", 1, 100, parent );
        progress_dialog->show();
        QNetworkReply* reply = network_manager.post( request, payload );
        QObject::connect( reply, &QNetworkReply::downloadProgress,
                          [=]( qint64 const received, qint64 const total )
        {
            progress_dialog->setMaximum( total + ( total * 0.25 ));
            progress_dialog->setValue( received );
        });

        QObject::connect( reply, &QNetworkReply::sslErrors, reply,
                          QOverload<QList<QSslError> const &>::of(
                              &QNetworkReply::ignoreSslErrors ) );
        QObject::connect( progress_dialog, &QProgressDialog::canceled, reply,
                          &QNetworkReply::abort );
        QObject::connect( reply, &QNetworkReply::finished, progress_dialog,
                          &QProgressDialog::close );
        QObject::connect( reply, &QNetworkReply::finished, [=]
        {
            QJsonObject const result {
                utilities::GetJsonNetworkData( reply, report_error )
            };
            if( result.isEmpty() ) on_error();
            else on_success( result, reply );
        });
    }
}

#endif // RESOURCES_HPP

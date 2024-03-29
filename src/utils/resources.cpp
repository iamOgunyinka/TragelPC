#include "utils/resources.hpp"
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QByteArray>
#include <QSsl>
#include <QSslCertificate>
#include <QMutexLocker>

namespace utilities {

QNetworkAccessManager& NetworkManager::GetNetworkWithCookie()
{
    auto& network_manager = GetNetwork();
    auto& session_cookie = GetSessionCookie();
    network_manager.setCookieJar( &session_cookie );
    // don't take ownership of the session cookie
    session_cookie.setParent( nullptr );
    return network_manager;
}

QNetworkAccessManager& NetworkManager::GetNetwork()
{
    static QNetworkAccessManager network_manager {};
    network_manager.setStrictTransportSecurityEnabled( true );
    return network_manager;
}

QNetworkCookieJar& NetworkManager::GetSessionCookie()
{
    static PersistentCookieJar network_cookie{};
    return network_cookie;
}

bool NetworkManager::SetNetworkCookie( QNetworkCookieJar& jar,
                                       QNetworkReply* network_reply )
{
    using CookieList = QList<QNetworkCookie>;
    auto const cookie_variant = network_reply->header(
                QNetworkRequest::SetCookieHeader );
    CookieList const cookies = qvariant_cast<CookieList>(
                cookie_variant );
    return jar.setCookiesFromUrl( cookies,
                                  network_reply->request().url() );
}

PersistentCookieJar::PersistentCookieJar(QObject* parent):
    QNetworkCookieJar( parent )
{
    load();
}

PersistentCookieJar::~PersistentCookieJar()
{
    save();
}

QList<QNetworkCookie> PersistentCookieJar::cookiesForUrl( QUrl const &url )const
{
    QMutexLocker lock(&mutex);
    return QNetworkCookieJar::cookiesForUrl( url );
}

bool PersistentCookieJar::setCookiesFromUrl( QList<QNetworkCookie> const
                                             &cookieList, QUrl const &url )
{
    QMutexLocker lock(&mutex);
    return QNetworkCookieJar::setCookiesFromUrl( cookieList, url );
}

void PersistentCookieJar::save()
{
    QMutexLocker lock(&mutex);
    QList<QNetworkCookie> list = allCookies();
    QByteArray data;
    for( auto const & cookie: list ){
        data.append( cookie.toRawForm() );
        data.append( "\n" );
    }
    auto& app_settings = ApplicationSettings::GetAppSettings();
    app_settings.SetValue( SettingsValue::Cookies, data );
}

void PersistentCookieJar::load()
{
    QMutexLocker lock( &mutex );
    auto& app_settings = ApplicationSettings::GetAppSettings();
    QByteArray data = app_settings.Value( SettingsValue::Cookies ).toByteArray();
    setAllCookies( QNetworkCookie::parseCookies( data ) );
}

QString Endpoint::CompanyToken() const
{
    return endpoint_object.value( "company_token" ).toString();
}

QString Endpoint::CompanyID() const
{
    return QString::number( endpoint_object.value( "company_id" )
                            .toInt() );
}

QString Endpoint::LoginTo() const
{
    return endpoint_object.value( "login" ).toString();
}

QString Endpoint::LogoutFrom() const
{
    return endpoint_object.value( "logout" ).toString();
}

QString Endpoint::CreateUser() const {
    return endpoint_object.value( "create_user").toString();
}

QString Endpoint::DeleteUser() const {
    return endpoint_object.value( "delete_user" ).toString();
}

QString Endpoint::ResetPassword() const {
    return endpoint_object.value( "reset_password" ).toString();
}

QString Endpoint::ChangeRole() const {
    return endpoint_object.value( "change_role" ).toString();
}

QString Endpoint::ConfirmPayment() const {
    return endpoint_object.value( "confirm_order" ).toString();
}

QString Endpoint::GetSubscriptions() const {
    return endpoint_object.value( "get_subscriptions" ).toString();
}

QString Endpoint::AddSubscription() const {
    return endpoint_object.value( "add_subscription" ).toString();
}

QString Endpoint::GetProducts() const {
    return endpoint_object.value( "get_products" ).toString();
}

QString Endpoint::GetProductByID () const {
    return endpoint_object.value( "get_product" ).toString();
}

QString Endpoint::RemoveProduct() const {
    return endpoint_object.value( "remove_product" ).toString();
}

QString Endpoint::ListStaffs() const {
    return endpoint_object.value( "list_users" ).toString();
}

QString Endpoint::UpdateProducts() const {
    return endpoint_object.value( "update_product" ).toString();
}

QString Endpoint::AddProduct() const {
    return endpoint_object.value( "add_product" ).toString();
}

QString Endpoint::GetOrders() const {
    return endpoint_object.value( "get_orders" ).toString();
}

QString Endpoint::GetOrderByID() const {
    return endpoint_object.value( "get_customer_order" ).toString();
}

QString Endpoint::GetOrderCount() const {
    return endpoint_object.value( "count_orders" ).toString();
}

QString Endpoint::AddOrder() const {
    return endpoint_object.value( "add_order" ).toString();
}

QString Endpoint::RemoveOrder() const {
    return endpoint_object.value( "remove_order" ).toString();
}

QString Endpoint::PingAddress() const {
    return endpoint_object.value( "ping" ).toString();
}

QString Endpoint::GetExpiryDate() const {
    return endpoint_object.value( "get_expiry" ).toString();
}

QString Endpoint::UploadPhoto() const {
    return endpoint_object.value( "upload_images" ).toString();
}

QJsonObject Endpoint::ToJson() const {
    return endpoint_object;
}

void Endpoint::ParseEndpointsFromJson( Endpoint& endpoint,
                                       QJsonObject const & json_object )
{
    endpoint.endpoint_object = json_object;
}

Endpoint& Endpoint::GetEndpoints()
{
    static Endpoint endpoints {};
    return endpoints;
}

ApplicationSettings& ApplicationSettings::GetAppSettings()
{
    static ApplicationSettings app_settings;
    return app_settings;
}

QSettings& ApplicationSettings::GetSettings( QString const & organisation,
                                             QString const & application )
{
    static QSettings settings { organisation, application };
    return settings;
}

ApplicationSettings::ApplicationSettings():
    settings{ GetSettings( "Froist Inc.", "LineCounter" ) }
{
}

QString SettingsValueToString( SettingsValue const val )
{
    switch( val ) {
    case SettingsValue::OrderCount:
        return "order_count";
    case SettingsValue::UrlMap:
        return "url_map";
    case SettingsValue::PingInterval:
        return "ping_interval";
    case SettingsValue::OrderPollInterval:
        return "order_poll_interval";
    case SettingsValue::AllowShortcut:
        return "allow_shortcut";
    case SettingsValue::ConfirmDeletion:
        return "confirm_deletion";
    case SettingsValue::ShowSplash:
        return "show_splash";
    case SettingsValue::PingNotifInterval:
        return "ping_ntf_interval";
    case SettingsValue::ResultPerPage:
        return "result_per_page";
    case SettingsValue::Cookies:
        return "session_cookies";
    case SettingsValue::KeepMeLoggedIn:
        return "keep_logged_in";
    }
    abort();
}

void ApplicationSettings::SetValue( SettingsValue const data,
                                    QVariant const & value )
{
    auto const key = SettingsValueToString( data );
    settings.setValue( key, value );
}

void ApplicationSettings::Remove(const SettingsValue value)
{
    auto const key = SettingsValueToString( value );
    settings.remove( key );
}

QVariant ApplicationSettings::Value( SettingsValue const value,
                                     QVariant const& default_ ) const
{
    auto const key = SettingsValueToString( value );
    return settings.value( key, default_ );
}

QJsonObject MakeOrderItem::ToJson() const
{
    return QJsonObject{ { "quantity", static_cast<int>( quantity ) },
        { "product_id", product.product_id }};
}

bool ParsePageUrls( QJsonObject const &result, PageResultQuery & page_query )
{
    QJsonObject const page_info{ result.value( "pages" ).toObject() };
    uint& total_result = page_query.total_result;
    total_result = page_info.value( "total" ).toInt();
    uint& page_total = page_query.number_of_pages;
    page_total = page_info.value( "pages" ).toInt();

    if( total_result == 0 || page_total == 0 ) return false;

    page_query.first_url = page_info.value( "first_url" ).toString();
    page_query.last_url = page_info.value( "last_url" ).toString();
    page_query.result_per_page = page_info.value( "per_page" ).toInt();
    if( page_total > 0 ){
        QString const next_url = page_info.value( "next_url" ).toString();
        QString previous_url {};
        if( page_total != 0 ){
            previous_url = page_info.value( "prev_url" ).toString();
        }
        uint const page_number = page_info.value( "page" ).toInt();
        utilities::UrlData next_page_info{
            next_url, previous_url, page_number
        };
        page_query.other_url = std::move( next_page_info );
    }
    return true;
}

QNetworkRequest GetRequestInterface( QUrl const &address )
{
    QNetworkRequest request = QNetworkRequest( address );
    if( address.scheme() == "https" ){
        auto ssl_config = QSslConfiguration::defaultConfiguration();
        ssl_config.setProtocol( QSsl::TlsV1_0OrLater );
        request.setSslConfiguration( ssl_config );
    }
    request.setHeader( QNetworkRequest::UserAgentHeader, USER_AGENT );
    return request;
}

QNetworkRequest PostRequestInterface( QUrl const &address )
{
    QNetworkRequest request{ address };
    if( address.scheme() == "https" ){
        auto ssl_config = QSslConfiguration::defaultConfiguration();
        ssl_config.setProtocol( QSsl::TlsV1_0OrLater );
        request.setSslConfiguration( ssl_config );
    }
    request.setHeader( QNetworkRequest::ContentTypeHeader, "application/json" );
    request.setHeader( QNetworkRequest::UserAgentHeader, USER_AGENT );
    return request;
}

QPair<QNetworkRequest, QByteArray> PostImageRequestInterface(
        QUrl const &address, QVector<QString> const & data )
{
    if( data.size() < 2 ) return {};
    QString const filename = data[0];
    QString const file_format = data[1];

    QNetworkRequest request = QNetworkRequest( address );
    request.setHeader( QNetworkRequest::UserAgentHeader, USER_AGENT );
    QString const boundary= "--TragelBoundary7MA4YWxkTrZu7gW"; // random string
    QByteArray payload = QString( "--" + boundary + "\r\n" ).toLocal8Bit();
    payload.append( QString( "Content-Disposition: form-data; name=\"photo\"; "
                             "filename=\"" + filename + "\"\r\nContent-Type: "
                                                        "image/" + file_format
                             + "\r\n\r\n" ).toLocal8Bit() );
    QFile file{ filename };
    if( !file.open( QIODevice::ReadOnly ) ) return {};
    payload.append( file.readAll() );
    payload.append( "\r\n" );
    payload.append( "--" + boundary + "--\r\n" );
    request.setRawHeader( "Content-Type",
                          QString( "multipart/form-data; boundary="
                                   + boundary ).toLocal8Bit() );
    request.setRawHeader( "Content-Length",
                          QByteArray::number( payload.length() ) );
    return { request, payload };
}

QJsonObject GetJsonNetworkData( QNetworkReply* network_reply,
                                bool show_error_message )
{
    if( !network_reply ) return {};
    if( network_reply->error() != QNetworkReply::NoError ){
        if( show_error_message ){
            QByteArray const network_response = network_reply->readAll();
            QJsonDocument const json_document=
                    QJsonDocument::fromJson( network_response );
            if( json_document.isNull() ){
                QMessageBox::critical( nullptr, "Server's response",
                                       network_reply->errorString() );
            } else {
                QJsonObject const server_error = json_document.object();
                QString error_message = server_error.contains( "status" ) ?
                            server_error.value( "status" ).toString():
                            QString::number( server_error.value( "message" )
                                             .toInt() );
                if( error_message.isEmpty() ){
                    error_message = network_reply->errorString();
                }
                QMessageBox::critical( nullptr, "Server says", error_message );
            }
        }
        return QJsonObject();
    }
    QByteArray const network_response = network_reply->readAll();
    QJsonDocument const json_document=
            QJsonDocument::fromJson( network_response );
    if( json_document.isNull() ) return QJsonObject();
    return json_document.object();
}

Product::Product( QString name, double p, qint64 prod_id , QString thumbnail,
                  QString cat, QString desc ):
    name{ name }, thumbnail_location{ thumbnail }, categories{ cat },
    description{ desc }, price{ p }, product_id{ prod_id }
{
}


QJsonObject Product::ToJson() const
{
    QJsonObject object{ { "price", price }, {"name", name },
                        { "thumbnail", thumbnail_location },
                        { "desc", description },
                        { "cat", categories }};
    if( product_id != 0 ) object.insert( "id", product_id );
    return object;
}

bool operator==( Product const & a, Product const & b )
{
    return a.name == b.name && a.thumbnail_location == b.thumbnail_location &&
            a.price == b.price;
}
}

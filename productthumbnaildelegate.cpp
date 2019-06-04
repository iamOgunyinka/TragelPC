#include "productthumbnaildelegate.hpp"
#include "resources.hpp"
#include <QPainter>
#include <QPixmapCache>
#include <QPixmap>

ProductThumbnailDelegate::ProductThumbnailDelegate( QObject *parent ):
    QAbstractItemDelegate{ parent }
{
}

void ProductThumbnailDelegate::paint( QPainter *painter, QStyleOptionViewItem const &option,
                                      QModelIndex const &index ) const
{
    QString const thumbnail_url{ index.model()->data( index, Qt::DisplayRole ).toString() };
    if( thumbnail_url.isEmpty() ) return;
    if( option.state & QStyle::State_Selected ) painter->fillRect( option.rect,
                                                                   option.palette.highlight() );
    painter->save();
    QPixmap picture_thumbnail{};
    // have we cached this image recently? If yes, use it.
    if( QPixmapCache::find( thumbnail_url, picture_thumbnail ) ){
        painter->drawPixmap( option.rect, picture_thumbnail.scaled( QSize( 120, 60 ) ) );
        painter->restore();
        return;
    }

    QNetworkAccessManager& network_manager{ utilities::NetworkManager::GetNetwork() };
    QNetworkRequest const request{ utilities::GetRequestInterface( QUrl{ thumbnail_url } ) };
    QNetworkReply* reply{ network_manager.get( request ) };
    QObject::connect( reply, &QNetworkReply::finished, [=]{
        if( reply->error() != QNetworkReply::NoError ){
            qDebug() << reply->errorString();
            return;
        }
        QByteArray const & data{ reply->readAll() };
        QPixmap pixmap{};
        if( !pixmap.loadFromData( data ) ) return;
        QPixmapCache::insert( thumbnail_url, pixmap );
        painter->drawPixmap( option.rect, pixmap.scaled( QSize( 120, 60 ) ) );
        painter->restore();
    });
}

QSize ProductThumbnailDelegate::sizeHint( QStyleOptionViewItem const &,
                                          QModelIndex const & ) const
{
    return QSize{ 120, 60 };
}


LocalProductUploadDelegate::LocalProductUploadDelegate( QObject *parent ):
    QAbstractItemDelegate{ parent }
{
}

void LocalProductUploadDelegate::paint( QPainter *painter, QStyleOptionViewItem const &option,
                                      QModelIndex const &index ) const
{
    QString const filename{ index.model()->data( index, Qt::DisplayRole ).toString() };
    if( filename.isEmpty() ) return;
    if( option.state & QStyle::State_Selected ) painter->fillRect( option.rect,
                                                                   option.palette.highlight() );
    painter->save();
    QPixmap picture_thumbnail{};
    // have we cached this image recently? If yes, use it.
    if( !QPixmapCache::find( filename, picture_thumbnail ) ){
        picture_thumbnail = QPixmap( filename );
        QPixmapCache::insert( filename, picture_thumbnail );
    }
    painter->drawPixmap( option.rect, picture_thumbnail.scaled( QSize( 120, 60 ) ) );
    painter->restore();
}

QSize LocalProductUploadDelegate::sizeHint( QStyleOptionViewItem const &,
                                          QModelIndex const & ) const
{
    return QSize{ 120, 60 };
}

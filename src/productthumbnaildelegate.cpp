#include "productthumbnaildelegate.hpp"
#include "resources.hpp"
#include <QPainter>
#include <QPixmapCache>
#include <QPixmap>
#include <QRect>

ProductThumbnailDelegate::ProductThumbnailDelegate( QObject *parent ):
    QAbstractItemDelegate{ parent }
{
}

void ProductThumbnailDelegate::paint( QPainter *painter,
                                      QStyleOptionViewItem const &option,
                                      QModelIndex const &index ) const
{
    QString const thumbnail_url{
        index.model()->data( index, Qt::DisplayRole ).toString()
    };
    if( thumbnail_url.isEmpty() ) return;
    if( option.state & QStyle::State_Selected ){
        painter->fillRect( option.rect, option.palette.highlight() );
    }
    QPixmap picture_thumbnail;
    // have we cached this image recently? If yes, use it.
    if( QPixmapCache::find( thumbnail_url, picture_thumbnail ) ){
        auto const scaled_image = picture_thumbnail.scaled( QSize( 120, 120 ) );
        painter->drawPixmap( option.rect, scaled_image );
        return;
    }

    QNetworkAccessManager& network_manager{
        utilities::NetworkManager::GetNetwork()
    };
    QNetworkRequest const request{
        utilities::GetRequestInterface( QUrl{ thumbnail_url } )
    };
    QNetworkReply* reply{ network_manager.get( request ) };
    QObject::connect( reply, &QNetworkReply::finished, [=]
    {
        if( reply->error() != QNetworkReply::NoError ){
            qDebug() << reply->errorString();
            return;
        }
        QByteArray const data{ reply->readAll() };
        QPixmap pixmap{};
        if( !pixmap.loadFromData( data ) ) return;
        QPixmapCache::insert( thumbnail_url, pixmap );
        QPixmap const scaled_image{ pixmap.scaled( QSize( 120, 120 ) ) };
        if( painter ){
            //painter->drawPixmap( option.rect, scaled_image );
        }
    });
}

QSize ProductThumbnailDelegate::sizeHint( QStyleOptionViewItem const &,
                                          QModelIndex const & ) const
{
    return QSize{ 120, 120 };
}


LocalProductUploadDelegate::LocalProductUploadDelegate( QObject *parent ):
    QAbstractItemDelegate{ parent }
{
}

void LocalProductUploadDelegate::paint( QPainter *painter,
                                        QStyleOptionViewItem const &option,
                                        QModelIndex const &index ) const
{
    QString const filename{
        index.model()->data( index, Qt::DisplayRole ).toString()
    };
    if( filename.isEmpty() ) return;
    if( option.state & QStyle::State_Selected ) {
        painter->fillRect( option.rect, option.palette.highlight() );
    }
    painter->save();
    QPixmap picture_thumbnail;
    // have we cached this image recently? If yes, use it.
    if( !QPixmapCache::find( filename, picture_thumbnail ) ){
        picture_thumbnail = QPixmap( filename );
        QPixmapCache::insert( filename, picture_thumbnail );
    }
    auto const scaled_image = picture_thumbnail.scaled( QSize( 120, 120 ) );
    painter->drawPixmap( option.rect, scaled_image );
    painter->restore();
}

QSize LocalProductUploadDelegate::sizeHint( QStyleOptionViewItem const &,
                                            QModelIndex const & ) const
{
    return QSize{ 120, 120 };
}

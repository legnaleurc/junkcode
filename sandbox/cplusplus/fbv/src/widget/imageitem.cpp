/**
 * @file imageitem.cpp
 * @author Wei-Cheng Pan
 *
 * KomiX, a comics viewer.
 * Copyright (C) 2008  Wei-Cheng Pan <legnaleurc@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "imageitem_p.hpp"
#include "loader/deviceloader.hpp"

#include <QtWidgets/QGraphicsProxyWidget>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QLabel>

using fbv::widget::ImageItem;
using fbv::loader::DeviceLoader;

ImageItem::Private::Private( ImageItem * owner ):
QObject(),
owner( owner ),
item( nullptr ),
movie( nullptr ) {
}

void ImageItem::Private::onFinished( int id, QMovie * movie ) {
    QLabel * label = new QLabel;
    label->setMovie( movie );
    movie->setParent( label );
    movie->start();
    label->resize( movie->frameRect().size() );
    QGraphicsProxyWidget * item = new QGraphicsProxyWidget( this->owner );
    item->setWidget( label );

    this->movie = movie;
    this->item = item;
    emit this->changed();
}

void ImageItem::Private::onFinished( int id, const QPixmap & pixmap ) {
    QGraphicsPixmapItem * item = new QGraphicsPixmapItem( pixmap, this->owner );
    item->setTransformationMode( Qt::SmoothTransformation );

    this->item = item;
    emit this->changed();
}

ImageItem::ImageItem( const QList< QIODevice * > & devices ):
QGraphicsObject(),
p_( new Private( this ) ) {
    this->connect( this->p_.get(), SIGNAL( changed() ), SIGNAL( changed() ) );
    foreach( QIODevice * device, devices ) {
        DeviceLoader * loader = new DeviceLoader( -1, device );
        this->p_->connect( loader, SIGNAL( finished( int, QMovie * ) ), SLOT( onFinished( int, QMovie * ) ) );
        this->p_->connect( loader, SIGNAL( finished( int, const QPixmap & ) ), SLOT( onFinished( int, const QPixmap & ) ) );
        loader->start();
    }
}

void ImageItem::setPaused( bool paused ) {
    if( this->p_->movie ) {
        this->p_->movie->setPaused( paused );
    }
}

QRectF ImageItem::boundingRect() const {
    if( !this->p_->item ) {
        return QRectF();
    }
    return this->p_->item->boundingRect();
}

void ImageItem::paint( QPainter * /*painter*/, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/ ) {
}

/**
 * @file deviceloader_p.hpp
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
#ifndef KOMIX_WIDGET_DEVICELOADER_HPP_
#define KOMIX_WIDGET_DEVICELOADER_HPP_

#include "deviceloader.hpp"

namespace fbv {
namespace loader {

class DeviceLoader::Private: public QObject {
    Q_OBJECT
public:
    Private( int id, QIODevice * device );

    void read( QIODevice * device );

public slots:
    void onFinished( const QByteArray & data );

signals:
    void finished( int id, const QPixmap & pixmap );
    void finished( int id, QMovie * movie );

public:
    int id;
    QIODevice * device;
};

}
}

#endif

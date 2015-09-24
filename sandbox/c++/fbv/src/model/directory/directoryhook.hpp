/**
 * @file directoryhook.hpp
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
#ifndef KOMIX_MODEL_DIRECTORY_DIRECTORYHOOK_HPP
#define KOMIX_MODEL_DIRECTORY_DIRECTORYHOOK_HPP

#include <QtWidgets/QAction>

class QUrl;

namespace KomiX {
namespace model {
namespace directory {

/// Private menu hook
class DirectoryHook : public QAction {
    Q_OBJECT
public:
    /// Constructor
    DirectoryHook( QWidget * parent );
signals:
    /// emitted when user opened a url
    void opened( const QUrl & url );
private slots:
    void helper_();
};

}
}
} // end of namespace

#endif

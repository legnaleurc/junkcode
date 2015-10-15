/**
 * @file exception.hpp
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
#ifndef KOMIX_EXCEPTION_HPP
#define KOMIX_EXCEPTION_HPP

#include <QtCore/QString>

#include <exception>
#include <memory>
#include <string>

namespace fbv {
namespace exception {

class Exception: public std::exception {
public:
    explicit Exception( int code );
    explicit Exception( const char * msg );
    explicit Exception( const wchar_t * msg );
    explicit Exception( const std::string & msg );
    explicit Exception( const std::wstring & msg );
    explicit Exception( const QString & msg );
    virtual ~Exception() throw();

    virtual const char * what() const throw();
    const QString & getMessage() const;

private:
    class Private;
    std::shared_ptr< Private > p_;
};

}
} // end of namespace

#endif

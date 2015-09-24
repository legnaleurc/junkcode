/**
 * @file exception.cpp
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
#include "exception.hpp"

#include <cassert>

namespace fbv {
namespace exception {

class Exception::Private {
public:
    Private();
    explicit Private( const QString & msg );

    QString msg;
};

}
}

using fbv::exception::Exception;

Exception::Private::Private(): msg() {
}

Exception::Private::Private( const QString & msg ): msg( msg ) {
}

Exception::Exception( int code ): p_( new Private ) {
#ifdef _MSC_VER
    wchar_t tmp[1024];
    errno_t ret = _wcserror_s( tmp, code );
    assert( ret == 0 || !"invalid errno" );
    this->p_->msg.fromWCharArray( tmp );
#else
    this->p_->msg.fromLocal8Bit( strerror( code ) );
#endif
}

/**
 * @brief Constructor which accept a C-Style string
 * @note please encode to UTF-8
 */
Exception::Exception( const char * msg ): p_( new Private( QString::fromUtf8( msg ) ) ) {
}

Exception::Exception( const wchar_t * msg ): p_( new Private( QString::fromWCharArray( msg ) ) ) {
}

Exception::Exception( const std::string & msg ): p_( new Private( QString::fromStdString( msg ) ) ) {
}

Exception::Exception( const std::wstring & msg ): p_( new Private( QString::fromStdWString( msg ) ) ) {
}

/**
 * @brief Constructor which accept a QString
 */
Exception::Exception( const QString & msg ): p_( new Private( msg ) ) {
}

/**
 * @brief Destructor
 */
Exception::~Exception() throw() {
}

/**
 * @brief Get message for STL
 * @note encode as UTF-8
 */
const char * Exception::what() const throw() {
    return this->p_->msg.toLocal8Bit().constData();
}

/**
 * @brief Get message for Qt
 */
const QString & Exception::getMessage() const {
    return this->p_->msg;
}

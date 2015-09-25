/**
 * @file filemodel.hpp
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
#ifndef KOMIX_MODEL_FILEMODEL_HPP
#define KOMIX_MODEL_FILEMODEL_HPP

#include <QtCore/QAbstractItemModel>
#include <QtCore/QUrl>
#include <QtCore/QMetaType>
#include <QtCore/QIODevice>

#include <functional>
#include <memory>

namespace fbv {
namespace model {

/**
 * @brief Abstract file model
 *
 * Describe how to retrive files.
 */
class FileModel: public QAbstractItemModel {
    Q_OBJECT
public:
    /// Functor of key comparsion
    typedef std::function< bool ( const QUrl & ) > KeyFunctor;
    /// Functor of model creation
    typedef std::function< std::shared_ptr< FileModel > ( const QUrl & ) > ValueFunctor;

    /**
     * @brief Create concrete model
     * @param url opening url
     */
    static std::shared_ptr< FileModel > createModel( const QUrl & url );
    /**
     * @brief Register model
     * @param key compare function
     * @param value create function
     * @return always true
     */
    static bool registerModel( const KeyFunctor & key, const ValueFunctor & value );

    using QAbstractItemModel::index;
    /// Query the index @p url in the model
    virtual QModelIndex index( const QUrl & url ) const = 0;

    void initialize();

protected:
    virtual void doInitialize() = 0;

signals:
    void error( const QString & msg );
    void ready();
};

}
} // end namespace

//Q_DECLARE_METATYPE(QIODevice *)

#endif

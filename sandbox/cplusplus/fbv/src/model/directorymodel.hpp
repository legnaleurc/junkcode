/**
 * @file directorymodel.hpp
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
#ifndef KOMIX_MODEL_DIRECTORY_DIRECTORYMODEL_HPP
#define KOMIX_MODEL_DIRECTORY_DIRECTORYMODEL_HPP

#include "model/localfilemodel.hpp"

namespace fbv {
namespace model {

/**
 * @brief The model to open directory
 */
class DirectoryModel : public LocalFileModel {
public:
    /// Constructor open @p root as top-level directory
    DirectoryModel( const QFileInfo & root );
};

}
} // end of namespace

#endif

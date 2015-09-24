/**
 * @file archivemodel.hpp
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
#ifndef KOMIX_MODEL_ARCHIVE_ARCHIVEMODEL_HPP
#define KOMIX_MODEL_ARCHIVE_ARCHIVEMODEL_HPP

#include "localfilemodel.hpp"

namespace KomiX {
namespace model {
namespace archive {

/**
 * @brief The model using 7-Zip to open compressed file
 *
 * Supported file formats: 7z, zip, rar, tar.gz, tar.bz2.
 * Do not support password.
 */
class ArchiveModel: public LocalFileModel {
public:
    /// Check if 7-zip existed
    static bool IsRunnable();
    /// Check if temporary directory is prepared
    static bool IsPrepared();

    /**
     * @brief Constructor with given fileinfo
     * @param root top-level file
     */
    ArchiveModel( const QFileInfo & root );

protected:
    virtual void doInitialize();

private:
    friend class ArchiveHook;
    class Private;
    std::shared_ptr< Private > p_;
};

/// get supported archive formats
const QStringList & ArchiveFormats();
/// get supported archive formats name filter
const QStringList & ArchiveFormatsFilter();
/**
 * @brief check if archive is supported
 * @param path file path
 */
bool isArchiveSupported( const QString & path );

}
}
} // end of namespace

#endif

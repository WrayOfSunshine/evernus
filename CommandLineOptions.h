/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <QString>

namespace Evernus
{
    namespace CommandLineOptions
    {
        const auto forceVersionArg = QStringLiteral("force-version");
        const auto noUpdateArg = QStringLiteral("no-update");
        const auto clientIdArg = QStringLiteral("client-id");
        const auto clientSecretArg = QStringLiteral("client-secret");
        const auto maxLogFileSizeArg = QStringLiteral("max-log-file-size");
        const auto maxLogFilesArg = QStringLiteral("max-log-files");
    }
}

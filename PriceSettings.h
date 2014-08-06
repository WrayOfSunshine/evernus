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

namespace Evernus
{
    namespace PriceSettings
    {
        enum class CopyMode
        {
            DontCopy,
            CopySell,
            CopyBuy
        };

        enum class DataSource
        {
            Orders,
            ItemCost,
            Station
        };

        const auto minMarginDefault = 10.;
        const auto preferredMarginDefault = 30.;
        const auto priceDeltaDefault = 0.01;
        const auto priceMaxAgeDefault = 1;
        const auto importLogWaitTimeDefault = 1000;
        const auto marketOrderMaxAgeDefault = 7;

        const auto minMarginKey = "prices/margin/min";
        const auto preferredMarginKey = "prices/margin/preferred";
        const auto copyModeKey = "prices/copyMode";
        const auto priceDeltaKey = "prices/delta";
        const auto priceAltImportKey = "prices/import/alt";
        const auto priceMaxAgeKey = "prices/maxAge";
        const auto importLogWaitTimeKey = "prices/import/logWaitTime";
        const auto marketOrderMaxAgeKey = "prices/orders/maxAge";
        const auto autoAddCustomItemCostKey = "prices/orders/autoAddCustomItemCost";
        const auto costDataSourceKey = "prices/dataSource";
        const auto costSourceStationKey = "prices/costSourceStation";
    }
}

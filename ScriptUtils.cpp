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
#include <QScriptEngine>

#include "ExternalOrder.h"
#include "MarketOrder.h"

#include "ScriptUtils.h"

namespace Evernus
{
    namespace ScriptUtils
    {
        QScriptValue wrapMarketOrder(QScriptEngine &engine, const MarketOrder &order)
        {
            auto orderObject = engine.newObject();
            orderObject.setProperty("id", QString::number(order.getId()));
            orderObject.setProperty("characterId", QString::number(order.getCharacterId()));
            orderObject.setProperty("stationId", static_cast<quint32>(order.getStationId()));
            orderObject.setProperty("volumeEntered", order.getVolumeEntered());
            orderObject.setProperty("volumeRemaining", order.getVolumeRemaining());
            orderObject.setProperty("minVolume", order.getMinVolume());
            orderObject.setProperty("delta", order.getDelta());
            orderObject.setProperty("state", static_cast<int>(order.getState()));
            orderObject.setProperty("typeId", order.getTypeId());
            orderObject.setProperty("range", order.getRange());
            orderObject.setProperty("accountKey", order.getAccountKey());
            orderObject.setProperty("duration", order.getDuration());
            orderObject.setProperty("escrow", order.getEscrow());
            orderObject.setProperty("price", order.getPrice());
            orderObject.setProperty("type", static_cast<int>(order.getType()));
            orderObject.setProperty("issued", engine.newDate(order.getIssued()));
            orderObject.setProperty("firstSeen", engine.newDate(order.getFirstSeen()));
            orderObject.setProperty("lastSeen", engine.newDate(order.getLastSeen()));
            orderObject.setProperty("corporationId", QString::number(order.getCorporationId()));

            return orderObject;
        }

        QScriptValue wrapExternalOrder(QScriptEngine &engine, const ExternalOrder &order)
        {
            auto orderObject = engine.newObject();
            orderObject.setProperty("id", QString::number(order.getId()));
            orderObject.setProperty("type", static_cast<int>(order.getType()));
            orderObject.setProperty("typeId", order.getTypeId());
            orderObject.setProperty("stationId", static_cast<quint32>(order.getStationId()));
            orderObject.setProperty("solarSystemId", order.getSolarSystemId());
            orderObject.setProperty("regionId", order.getRegionId());
            orderObject.setProperty("range", order.getRange());
            orderObject.setProperty("updateTime", engine.newDate(order.getUpdateTime()));
            orderObject.setProperty("price", order.getPrice());

            return orderObject;
        }
    }
}

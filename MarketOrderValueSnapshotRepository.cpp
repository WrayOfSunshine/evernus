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
#include <QSqlRecord>
#include <QSqlQuery>

#include "MarketOrderValueSnapshotRepository.h"

namespace Evernus
{
    QString MarketOrderValueSnapshotRepository::getTableName() const
    {
        return "market_order_value_snapshots";
    }

    QString MarketOrderValueSnapshotRepository::getIdColumn() const
    {
        return "id";
    }

    MarketOrderValueSnapshot MarketOrderValueSnapshotRepository::populate(const QSqlRecord &record) const
    {
        auto dt = record.value("timestamp").toDateTime();
        dt.setTimeSpec(Qt::UTC);

        MarketOrderValueSnapshot marketOrderValueSnapshot{record.value("id").value<MarketOrderValueSnapshot::IdType>()};
        marketOrderValueSnapshot.setTimestamp(dt);
        marketOrderValueSnapshot.setCharacterId(record.value("character_id").value<Character::IdType>());
        marketOrderValueSnapshot.setBuyValue(record.value("buy_value").toDouble());
        marketOrderValueSnapshot.setSellValue(record.value("sell_value").toDouble());
        marketOrderValueSnapshot.setNew(false);

        return marketOrderValueSnapshot;
    }

    void MarketOrderValueSnapshotRepository::create(const Repository<Character> &characterRepo) const
    {
        exec(QString{R"(CREATE TABLE IF NOT EXISTS %1 (
            id INTEGER PRIMARY KEY ASC,
            timestamp DATETIME NOT NULL,
            character_id BIGINT NOT NULL REFERENCES %2(%3) ON UPDATE CASCADE ON DELETE CASCADE,
            buy_value DOUBLE NOT NULL,
            sell_value DOUBLE NOT NULL
        ))"}.arg(getTableName()).arg(characterRepo.getTableName()).arg(characterRepo.getIdColumn()));

        exec(QString{"CREATE INDEX IF NOT EXISTS %1_%2_index ON %1(character_id)"}.arg(getTableName()).arg(characterRepo.getTableName()));
        exec(QString{"CREATE UNIQUE INDEX IF NOT EXISTS %1_character_timestamp ON %1(character_id, timestamp)"}.arg(getTableName()));
    }

    MarketOrderValueSnapshotRepository::SnapshotList MarketOrderValueSnapshotRepository
    ::fetchRange(Character::IdType characterId, const QDateTime &from, const QDateTime &to) const
    {
        auto query = prepare(QString{"SELECT * FROM %1 WHERE character_id = ? AND timestamp BETWEEN ? AND ? ORDER BY timestamp ASC"}
            .arg(getTableName()));

        query.addBindValue(characterId);
        query.addBindValue(from);
        query.addBindValue(to);

        DatabaseUtils::execQuery(query);

        SnapshotList result;

        const auto size = query.size();
        if (size > 0)
            result.reserve(size);

        while (query.next())
            result.emplace_back(populate(query.record()));

        return result;
    }

    QStringList MarketOrderValueSnapshotRepository::getColumns() const
    {
        return QStringList{}
            << "id"
            << "timestamp"
            << "character_id"
            << "buy_value"
            << "sell_value";
    }

    void MarketOrderValueSnapshotRepository::bindValues(const MarketOrderValueSnapshot &entity, QSqlQuery &query) const
    {
        if (entity.getId() != MarketOrderValueSnapshot::invalidId)
            query.bindValue(":id", entity.getId());

        query.bindValue(":timestamp", entity.getTimestamp());
        query.bindValue(":character_id", entity.getCharacterId());
        query.bindValue(":buy_value", entity.getBuyValue());
        query.bindValue(":sell_value", entity.getSellValue());
    }

    void MarketOrderValueSnapshotRepository::bindPositionalValues(const MarketOrderValueSnapshot &entity, QSqlQuery &query) const
    {
        if (entity.getId() != MarketOrderValueSnapshot::invalidId)
            query.addBindValue(entity.getId());

        query.addBindValue(entity.getTimestamp());
        query.addBindValue(entity.getCharacterId());
        query.addBindValue(entity.getBuyValue());
        query.addBindValue(entity.getSellValue());
    }
}

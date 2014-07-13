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

#include "ItemCostRepository.h"

namespace Evernus
{
    QString ItemCostRepository::getTableName() const
    {
        return "item_costs";
    }

    QString ItemCostRepository::getIdColumn() const
    {
        return "id";
    }

    ItemCost ItemCostRepository::populate(const QSqlRecord &record) const
    {
        ItemCost itemCost{record.value("id").value<ItemCost::IdType>()};
        itemCost.setCharacterId(record.value("character_id").value<Character::IdType>());
        itemCost.setTypeId(record.value("character_id").value<EveType::IdType>());
        itemCost.setCost(record.value("cost").toDouble());
        itemCost.setNew(false);

        return itemCost;
    }

    void ItemCostRepository::create(const Repository<Character> &characterRepo) const
    {
        exec(QString{R"(CREATE TABLE IF NOT EXISTS %1 (
            id BIGINT PRIMARY KEY ASC,
            character_id BIGINT NOT NULL REFERENCES %2(%3) ON UPDATE CASCADE ON DELETE CASCADE,
            type_id INTEGER NOT NULL,
            cost NUMERIC NOT NULL
        ))"}.arg(getTableName()).arg(characterRepo.getTableName()).arg(characterRepo.getIdColumn()));

        exec(QString{"CREATE INDEX IF NOT EXISTS %1_%2_index ON %1(character_id)"}.arg(getTableName()).arg(characterRepo.getTableName()));
        exec(QString{"CREATE INDEX IF NOT EXISTS %1_character_type ON %1(character_id, type_id)"}.arg(getTableName()));
    }

    QSqlQuery ItemCostRepository::prepareQueryForCharacter(Character::IdType id) const
    {
        auto query = prepare(QString{"SELECT * FROM %1 WHERE character_id = ?"}.arg(getTableName()));
        query.bindValue(0, id);

        return query;
    }

    ItemCost ItemCostRepository::fetchForCharacterAndType(Character::IdType characterId, EveType::IdType typeId) const
    {
        auto query = prepare(QString{"SELECT * FROM %1 WHERE character_id = ? AND type_id = ?"}.arg(getTableName()));
        query.bindValue(0, characterId);
        query.bindValue(1, typeId);

        DatabaseUtils::execQuery(query);

        if (!query.next())
            throw NotFoundException{};

        return populate(query.record());
    }

    QStringList ItemCostRepository::getColumns() const
    {
        return QStringList{}
            << "id"
            << "character_id"
            << "type_id"
            << "cost";
    }

    void ItemCostRepository::bindValues(const ItemCost &entity, QSqlQuery &query) const
    {
        if (entity.getId() != ItemCost::invalidId)
            query.bindValue(":id", entity.getId());

        query.bindValue(":character_id", entity.getCharacterId());
        query.bindValue(":type_id", entity.getTypeId());
        query.bindValue(":cost", entity.getCost());
    }

    void ItemCostRepository::bindPositionalValues(const ItemCost &entity, QSqlQuery &query) const
    {
        if (entity.getId() != ItemCost::invalidId)
            query.addBindValue(entity.getId());

        query.addBindValue(entity.getCharacterId());
        query.addBindValue(entity.getTypeId());
        query.addBindValue(entity.getCost());
    }
}

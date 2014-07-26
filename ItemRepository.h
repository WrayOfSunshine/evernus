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

#include <QHash>

#include "Repository.h"
#include "Item.h"

namespace Evernus
{
    class AssetList;

    class ItemRepository
        : public Repository<Item>
    {
    public:
        typedef QHash<QString, QVariantList> PropertyMap;

        using Repository::Repository;
        virtual ~ItemRepository() = default;

        virtual QString getTableName() const override;
        virtual QString getIdColumn() const override;

        virtual EntityPtr populate(const QSqlRecord &record) const override;

        void create(const Repository<AssetList> &assetRepo) const;
        void batchStore(const PropertyMap &map) const;

        static void fillProperties(const Item &entity, PropertyMap &map);

    private:
        virtual QStringList getColumns() const override;
        virtual void bindValues(const Item &entity, QSqlQuery &query) const override;
        virtual void bindPositionalValues(const Item &entity, QSqlQuery &query) const override;
    };
}

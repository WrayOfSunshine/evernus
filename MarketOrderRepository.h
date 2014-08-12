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

#include <unordered_map>
#include <unordered_set>

#include "MarketOrder.h"
#include "Repository.h"

namespace Evernus
{
    class MarketOrderRepository
        : public Repository<MarketOrder>
    {
    public:
        struct SingleAggrData
        {
            uint mCount = 0;
            uint mVolume = 0;
            double mPriceSum = 0.;
        };

        struct AggrData
        {
            SingleAggrData mBuyData;
            SingleAggrData mSellData;
        };

        struct OrderState
        {
            MarketOrder::State mState = MarketOrder::State::Active;
            uint mVolumeRemaining = 0;
            QDateTime mFirstSeen, mLastSeen;
            int mDelta = 0;
        };

        enum class AggregateColumn
        {
            TypeId,
            LocationId
        };

        enum class AggregateOrderColumn
        {
            Id,
            Count,
            Price,
            Volume
        };

        typedef std::unordered_map<MarketOrder::IdType, OrderState> OrderStateMap;
        typedef std::unordered_set<MarketOrder::IdType> OrderIdList;

        typedef std::vector<std::pair<quint64, SingleAggrData>> CustomAggregatedData;

        MarketOrderRepository(bool corp, const QSqlDatabase &db);
        virtual ~MarketOrderRepository() = default;

        virtual QString getTableName() const override;
        virtual QString getIdColumn() const override;

        virtual EntityPtr populate(const QSqlRecord &record) const override;

        void create(const Repository<Character> &characterRepo) const;
        void dropIndexes(const Repository<Character> &characterRepo) const;
        void copyDataWithoutCorporationIdFrom(const QString &table) const;

        AggrData getAggregatedData(Character::IdType characterId) const;
        CustomAggregatedData getCustomAggregatedData(Character::IdType characterId,
                                                     AggregateColumn groupingColumn,
                                                     AggregateOrderColumn orderColumn,
                                                     int limit,
                                                     bool includeActive,
                                                     bool includeNotFulfilled) const;
        OrderStateMap getOrderStates(Character::IdType characterId) const;

        EntityList fetchForCharacter(Character::IdType characterId) const;
        EntityList fetchForCharacter(Character::IdType characterId, MarketOrder::Type type) const;
        EntityList fetchArchivedForCharacter(Character::IdType characterId) const;

        void archive(const std::vector<MarketOrder::IdType> &ids) const;

        OrderIdList getActiveIds() const;

    private:
        bool mCorp = false;

        virtual QStringList getColumns() const override;
        virtual void bindValues(const MarketOrder &entity, QSqlQuery &query) const override;
        virtual void bindPositionalValues(const MarketOrder &entity, QSqlQuery &query) const override;

        virtual size_t getMaxRowsPerInsert() const override;
    };
}

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

#include "MarketOrderSellModel.h"
#include "CharacterBoundWidget.h"

namespace Evernus
{
    class WalletTransactionRepository;
    class MarketOrderProvider;
    class CacheTimerProvider;
    class ItemCostProvider;
    class EveDataProvider;

    class MarketOrderWidget
        : public CharacterBoundWidget
    {
        Q_OBJECT

    public:
        MarketOrderWidget(const MarketOrderProvider &orderProvider,
                          const CacheTimerProvider &cacheTimerProvider,
                          const EveDataProvider &dataProvider,
                          const ItemCostProvider &itemCostProvider,
                          const WalletTransactionRepository &transactionsRepo,
                          QWidget *parent = nullptr);
        virtual ~MarketOrderWidget() = default;

    signals:
        void characterChanged(Character::IdType id);

    public slots:
        void updateData();

    private:
        MarketOrderSellModel mSellModel;

        virtual void handleNewCharacter(Character::IdType id) override;
    };
}

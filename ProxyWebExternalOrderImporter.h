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

#include "ESIIndividualExternalOrderImporter.h"
#include "ESIWholeExternalOrderImporter.h"
#include "ImportSettings.h"

namespace Evernus
{
    class CharacterRepository;
    class ESIInterfaceManager;
    class EveDataProvider;

    class ProxyWebExternalOrderImporter
        : public ExternalOrderImporter
    {
        Q_OBJECT

    public:
        ProxyWebExternalOrderImporter(QByteArray clientId,
                                      QByteArray clientSecret,
                                      const EveDataProvider &dataProvider,
                                      const CharacterRepository &characterRepo,
                                      ESIInterfaceManager &interfaceManager,
                                      QObject *parent = nullptr);
        virtual ~ProxyWebExternalOrderImporter() = default;

        virtual void fetchExternalOrders(Character::IdType id, const TypeLocationPairs &target) const override;

    signals:
        void ssoAuthRequested(Character::IdType charId);

    public slots:
        void processAuthorizationCode(Character::IdType charId, const QByteArray &code);
        void cancelSSOAuth(Character::IdType charId);

        void handleNewPreferences();

    private:
        const EveDataProvider &mDataProvider;

        std::unique_ptr<ESIIndividualExternalOrderImporter> mESIIndividualImporter;
        std::unique_ptr<ESIWholeExternalOrderImporter> mESIWholeImporter;

        ImportSettings::MarketOrderImportType mCurrentOrderImportType = ImportSettings::marketOrderImportTypeDefault;

        template<class T>
        void connectImporter(T &importer);
        void setCurrentImporter();
    };
}

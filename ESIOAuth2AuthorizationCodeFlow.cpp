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
#include <QJsonDocument>
#include <QNetworkReply>
#include <QJsonObject>

#include "CharacterRepository.h"
#include "EveDataProvider.h"
#include "ESIInterface.h"

#include "ESIOAuth2AuthorizationCodeFlow.h"

namespace Evernus
{
    ESIOAuth2AuthorizationCodeFlow::ESIOAuth2AuthorizationCodeFlow(Character::IdType charId,
                                                                   const CharacterRepository &characterRepo,
                                                                   const EveDataProvider &dataProvider,
                                                                   const QString &clientIdentifier,
                                                                   const QUrl &authorizationUrl,
                                                                   const QUrl &accessTokenUrl,
                                                                   QNetworkAccessManager *manager,
                                                                   QObject *parent)
        : QOAuth2AuthorizationCodeFlow{clientIdentifier, authorizationUrl, accessTokenUrl, manager, parent}
        , mCharacterId{charId}
        , mCharacterRepo{characterRepo}
        , mDataProvider{dataProvider}
    {
        connect(this, &ESIOAuth2AuthorizationCodeFlow::granted, this, &ESIOAuth2AuthorizationCodeFlow::checkCharacter);
    }

    void ESIOAuth2AuthorizationCodeFlow::resetStatus()
    {
        setRefreshToken({});
        setToken({});
        setStatus(Status::NotAuthenticated);
    }

    void ESIOAuth2AuthorizationCodeFlow::checkCharacter()
    {
        const auto reply = get(ESIInterface::esiUrl + "/verify/");
        connect(reply, &QNetworkReply::finished, this, [=] {
            reply->deleteLater();

            if (Q_UNLIKELY(reply->error() != QNetworkReply::NoError))
            {
                resetStatus();
                emit error(reply->errorString(), {}, reply->url());
                return;
            }

            const auto data = reply->readAll();
            qDebug() << "Character check for" << mCharacterId << data;

            const auto doc = QJsonDocument::fromJson(data);
            const auto object = doc.object();

            const Character::IdType realCharId = object[QStringLiteral("CharacterID")].toDouble(Character::invalidId);
            if (realCharId != mCharacterId)
            {
                resetStatus();
                emit error(tr("Please authorize access for character: %1").arg(getCharacterName()), {}, reply->url());
                return;
            }

            emit characterConfirmed();
        });
    }

    QString ESIOAuth2AuthorizationCodeFlow::getCharacterName() const
    {
        QString charName;

        try
        {
            charName = mCharacterRepo.getName(mCharacterId);
        }
        catch (const CharacterRepository::NotFoundException &)
        {
            charName = mDataProvider.getGenericName(mCharacterId);
        }

        return (charName.isEmpty()) ? (QString::number(mCharacterId)) : (charName);
    }
}

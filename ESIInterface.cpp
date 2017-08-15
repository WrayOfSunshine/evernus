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
#include <QCoreApplication>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <QDebug>
#include <QUrl>

#include "NetworkSettings.h"
#include "SecurityHelper.h"
#include "ReplyTimeout.h"

#include "ESIInterface.h"

namespace Evernus
{
    template<>
    struct ESIInterface::TaggedInvoke<ESIInterface::JsonTag>
    {
        template<class T>
        static inline void invoke(const QByteArray &data, const T &callback)
        {
            callback(QJsonDocument::fromJson(data), QString{});
        }

        template<class T>
        static inline void invoke(const QString &error, const T &callback)
        {
            callback(QJsonDocument{}, error);
        }
    };

    template<>
    struct ESIInterface::TaggedInvoke<ESIInterface::StringTag>
    {
        template<class T>
        static inline void invoke(const QByteArray &data, const T &callback)
        {
            callback(QString::fromUtf8(data), QString{});
        }

        template<class T>
        static inline void invoke(const QString &error, const T &callback)
        {
            callback(QString{}, error);
        }
    };

    const QString ESIInterface::esiUrl{"https://esi.tech.ccp.is"};

    void ESIInterface::fetchMarketOrders(uint regionId, EveType::IdType typeId, const JsonCallback &callback) const
    {
        qDebug() << "Fetching market orders for" << regionId << "and" << typeId;
        asyncGet(QStringLiteral("/v1/markets/%1/orders/").arg(regionId), QStringLiteral("type_id=%1").arg(typeId), callback, getNumRetries());
    }

    void ESIInterface::fetchMarketOrders(uint regionId, const PaginatedCallback &callback) const
    {
        qDebug() << "Fetching whole market for" << regionId;
        fetchPaginatedData(QStringLiteral("/v1/markets/%1/orders/").arg(regionId), 1, callback);
    }

    void ESIInterface::fetchMarketHistory(uint regionId, EveType::IdType typeId, const JsonCallback &callback) const
    {
        qDebug() << "Fetching market history for" << regionId << "and" << typeId;
        asyncGet(QStringLiteral("/v1/markets/%1/history/").arg(regionId), QStringLiteral("type_id=%1").arg(typeId), callback, getNumRetries());
    }

    void ESIInterface::fetchCitadelMarketOrders(quint64 citadelId, Character::IdType charId, const PaginatedCallback &callback) const
    {
        qDebug() << "Fetching orders from citadel" << citadelId;

        if (Q_UNLIKELY(charId == Character::invalidId))
        {
            callback({}, true, tr("Cannot fetch citadels with no character selected."));
            return;
        }

        checkAuth(charId, [=](const auto &error) {
            if (!error.isEmpty())
                callback(QJsonDocument{}, true, error);
            else
                fetchPaginatedData(charId, QStringLiteral("/v1/markets/structures/%1/").arg(citadelId), 1, callback, true);
        });
    }

    void ESIInterface::fetchAssets(Character::IdType charId, const JsonCallback &callback) const
    {
        qDebug() << "Fetching assets for" << charId;

        if (Q_UNLIKELY(charId == Character::invalidId))
        {
            callback({}, tr("Cannot fetch assets with no character selected."));
            return;
        }

        checkAuth(charId, [=](const auto &error) {
            if (!error.isEmpty())
                callback({}, error);
            else
                asyncGet(charId, QStringLiteral("/v1/characters/%1/assets/").arg(charId), {}, callback, getNumRetries());
        });
    }

    void ESIInterface::fetchCharacter(Character::IdType charId, const JsonCallback &callback) const
    {
        qDebug() << "Fetching character" << charId;

        if (Q_UNLIKELY(charId == Character::invalidId))
        {
            callback({}, tr("Cannot fetch character with no character selected."));
            return;
        }

        asyncGet(QStringLiteral("/v4/characters/%1/").arg(charId), {}, callback, getNumRetries());
    }

    void ESIInterface::fetchCharacterSkills(Character::IdType charId, const JsonCallback &callback) const
    {
        qDebug() << "Fetching character skills for" << charId;

        if (Q_UNLIKELY(charId == Character::invalidId))
        {
            callback({}, tr("Cannot fetch character skills with no character selected."));
            return;
        }

        checkAuth(charId, [=](const auto &error) {
            if (!error.isEmpty())
                callback({}, error);
            else
                asyncGet(charId, QStringLiteral("/v3/characters/%1/skills/").arg(charId), {}, callback, getNumRetries());
        });
    }

    void ESIInterface::fetchCorporation(quint64 corpId, const JsonCallback &callback) const
    {
        qDebug() << "Fetching corporation" << corpId;
        asyncGet(QStringLiteral("/v3/corporations/%1/").arg(corpId), {}, callback, getNumRetries());
    }

    void ESIInterface::fetchRaces(const JsonCallback &callback) const
    {
        qDebug() << "Fetching races";
        asyncGet(QStringLiteral("/v1/universe/races/"), {}, callback, getNumRetries());
    }

    void ESIInterface::fetchBloodlines(const JsonCallback &callback) const
    {
        qDebug() << "Fetching bloodlines";
        asyncGet(QStringLiteral("/v1/universe/bloodlines/"), {}, callback, getNumRetries());
    }

    void ESIInterface::fetchCharacterWallet(Character::IdType charId, const StringCallback &callback) const
    {
        qDebug() << "Fetching character wallet for" << charId;

        if (Q_UNLIKELY(charId == Character::invalidId))
        {
            callback({}, tr("Cannot fetch character wallet with no character selected."));
            return;
        }

        checkAuth(charId, [=](const auto &error) {
            if (!error.isEmpty())
                callback({}, error);
            else
                asyncGet<decltype(callback), StringTag>(charId, QStringLiteral("/v1/characters/%1/wallet/").arg(charId), {}, callback, getNumRetries());
        });
    }

    void ESIInterface::fetchCharacterMarketOrders(Character::IdType charId, const JsonCallback &callback) const
    {
        qDebug() << "Fetching character market orders for" << charId;

        if (Q_UNLIKELY(charId == Character::invalidId))
        {
            callback({}, tr("Cannot fetch character market orders with no character selected."));
            return;
        }

        checkAuth(charId, [=](const auto &error) {
            if (!error.isEmpty())
                callback({}, error);
            else
                asyncGet(charId, QStringLiteral("/v1/characters/%1/orders/").arg(charId), {}, callback, getNumRetries());
        });
    }

    void ESIInterface::openMarketDetails(EveType::IdType typeId, Character::IdType charId, const ErrorCallback &errorCallback) const
    {
        qDebug() << "Opening market details for" << typeId;

        if (Q_UNLIKELY(charId == Character::invalidId))
        {
            errorCallback(tr("Cannot open market window for invalid character. Check if there's a character associated with the item you wish to view."));
            return;
        }

        auto opener = [=](const auto &error) {
            if (Q_UNLIKELY(!error.isEmpty()))
            {
                errorCallback(error);
            }
            else
            {
                post(charId, QStringLiteral("/v1/ui/openwindow/marketdetails/"), QStringLiteral("type_id=%1").arg(typeId), std::move(errorCallback));
            }
        };

        checkAuth(charId, opener);
    }

    void ESIInterface::setDestination(quint64 locationId, Character::IdType charId, const ErrorCallback &errorCallback) const
    {
        qDebug() << "Setting destination:" << locationId;

        auto setter = [=](const auto &error) {
            if (Q_UNLIKELY(!error.isEmpty()))
            {
                errorCallback(error);
            }
            else
            {
                QUrlQuery query;
                query.addQueryItem(QStringLiteral("destination_id"), QString::number(locationId));
                query.addQueryItem(QStringLiteral("add_to_beginning"), QStringLiteral("false"));
                query.addQueryItem(QStringLiteral("clear_other_waypoints"), QStringLiteral("true"));

                post(charId, QStringLiteral("/v2/ui/autopilot/waypoint/"), query.query(), std::move(errorCallback));
            }
        };

        checkAuth(charId, setter);
    }

    void ESIInterface::updateTokenAndContinue(Character::IdType charId, QString token, const QDateTime &expiry)
    {
        mAccessTokens[charId].mToken = std::move(token);
        mAccessTokens[charId].mExpiry = expiry;

        const auto range = mPendingAuthRequests.equal_range(charId);
        for (auto it = range.first; it != range.second; ++it)
            it->second(QString{});

        mPendingAuthRequests.erase(range.first, range.second);
    }

    void ESIInterface::handleTokenError(Character::IdType charId, const QString &error)
    {
        const auto requests = mPendingAuthRequests.equal_range(charId);
        for (auto request = requests.first; request != requests.second; ++request)
            request->second(error);

        mPendingAuthRequests.erase(requests.first, requests.second);
    }

    void ESIInterface::processSslErrors(const QList<QSslError> &errors)
    {
        SecurityHelper::handleSslErrors(errors, *qobject_cast<QNetworkReply *>(sender()));
    }

    template<class T>
    void ESIInterface::checkAuth(Character::IdType charId, T &&continuation) const
    {
        if (mAccessTokens[charId].mExpiry < QDateTime::currentDateTime() || mAccessTokens[charId].mToken.isEmpty())
        {
            mPendingAuthRequests.insert(std::make_pair(charId, std::forward<T>(continuation)));
            if (mPendingAuthRequests.size() == 1)
                emit tokenRequested(charId);
        }
        else
        {
            std::forward<T>(continuation)(QString{});
        }
    }

    template<class T>
    void ESIInterface::fetchPaginatedData(const QString &url, uint page, T &&continuation) const
    {
        asyncGet(url, QStringLiteral("page=%1").arg(page), [=](auto &&response, const auto &error) {
            if (Q_UNLIKELY(!error.isEmpty()))
            {
                continuation({}, true, error);
                return;
            }

            const auto array = response.array();
            if (array.isEmpty())
            {
                continuation(std::move(response), true, QString{});
            }
            else
            {
                continuation(std::move(response), false, QString{});
                fetchPaginatedData(url, page + 1, continuation);
            }
        }, getNumRetries());
    }

    template<class T>
    void ESIInterface::fetchPaginatedData(Character::IdType charId, const QString &url, uint page, T &&continuation, bool suppressForbidden) const
    {
        asyncGet(charId, url, QStringLiteral("page=%1").arg(page), [=](auto &&response, const auto &error) {
            if (Q_UNLIKELY(!error.isEmpty()))
            {
                continuation({}, true, error);
                return;
            }

            const auto array = response.array();
            if (array.isEmpty())
            {
                continuation(std::move(response), true, QString{});
            }
            else
            {
                continuation(std::move(response), false, QString{});
                fetchPaginatedData(charId, url, page + 1, continuation, suppressForbidden);
            }
        }, getNumRetries(), suppressForbidden);
    }

    template<class T>
    void ESIInterface::asyncGet(const QString &url, const QString &query, T &&continuation, uint retries) const
    {
        qDebug() << "ESI request:" << url << ":" << query;
        qDebug() << "Retries" << retries;

        auto reply = mNetworkManager.get(prepareRequest(url, query));
        Q_ASSERT(reply != nullptr);

        new ReplyTimeout{*reply};

        connect(reply, &QNetworkReply::sslErrors, this, &ESIInterface::processSslErrors);
        connect(reply, &QNetworkReply::finished, this, [=] {
            reply->deleteLater();

            const auto error = reply->error();
            if (Q_UNLIKELY(error != QNetworkReply::NoError))
            {
                if (retries > 0)
                    asyncGet(url, query, continuation, retries - 1);
                else
                    continuation(QJsonDocument{}, getError(url, query, *reply));
            }
            else
            {
                continuation(QJsonDocument::fromJson(reply->readAll()), QString{});
            }
        });
    }

    template<class T, class ResultTag>
    void ESIInterface
    ::asyncGet(Character::IdType charId, const QString &url, const QString &query, const T &continuation, uint retries, bool suppressForbidden) const
    {
        qDebug() << "ESI request:" << url << ":" << query;
        qDebug() << "Retries" << retries;

        auto reply = mNetworkManager.get(prepareRequest(charId, url, query));
        Q_ASSERT(reply != nullptr);

        new ReplyTimeout{*reply};

        connect(reply, &QNetworkReply::sslErrors, this, &ESIInterface::processSslErrors);
        connect(reply, &QNetworkReply::finished, this, [=] {
            reply->deleteLater();

            const auto error = reply->error();
            if (Q_UNLIKELY(error != QNetworkReply::NoError))
            {
                if (error == QNetworkReply::AuthenticationRequiredError)
                {
                    // expired token?
                    tryAuthAndContinue(charId, [=](const auto &error) {
                        if (error.isEmpty())
                            asyncGet<T, ResultTag>(charId, url, query, continuation, retries, suppressForbidden);
                        else
                            TaggedInvoke<ResultTag>::invoke(error, continuation);
                    });
                }
                else
                {
                    if (error == QNetworkReply::ContentOperationNotPermittedError || error == QNetworkReply::ContentAccessDenied)
                    {
                        if (suppressForbidden)
                            TaggedInvoke<ResultTag>::invoke(QString{}, continuation);
                        else
                            TaggedInvoke<ResultTag>::invoke(getError(url, query, *reply), continuation);
                    }
                    else if (retries > 0)
                    {
                        asyncGet<T, ResultTag>(charId, url, query, continuation, retries - 1, suppressForbidden);
                    }
                    else
                    {
                        TaggedInvoke<ResultTag>::invoke(getError(url, query, *reply), continuation);
                    }
                }
            }
            else
            {
                TaggedInvoke<ResultTag>::invoke(reply->readAll(), continuation);
            }
        });
    }

    template<class T>
    void ESIInterface::post(Character::IdType charId, const QString &url, const QString &query, T &&errorCallback) const
    {
        qDebug() << "ESI request:" << url << ":" << query;

        auto request = prepareRequest(charId, url, query);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        auto reply = mNetworkManager.post(request, QByteArray{});
        Q_ASSERT(reply != nullptr);

        new ReplyTimeout{*reply};

        connect(reply, &QNetworkReply::sslErrors, this, &ESIInterface::processSslErrors);
        connect(reply, &QNetworkReply::finished, this, [=] {
            reply->deleteLater();

            const auto error = reply->error();
            if (Q_UNLIKELY(error != QNetworkReply::NoError))
            {
                if (error == QNetworkReply::AuthenticationRequiredError)
                {
                    // expired token?
                    tryAuthAndContinue(charId, [=](const auto &error) {
                        if (error.isEmpty())
                            post(charId, url, query, errorCallback);
                        else
                            errorCallback(error);
                    });
                }
                else
                {
                    errorCallback(getError(url, query, *reply));
                }
            }
            else
            {
                const auto errorText = reply->readAll();
                if (Q_UNLIKELY(!errorText.isEmpty()))
                    errorCallback(errorText);
            }
        });
    }

    template<class T>
    void ESIInterface::tryAuthAndContinue(Character::IdType charId, T &&continuation) const
    {
        mAccessTokens.erase(charId);
        checkAuth(charId, std::forward<T>(continuation));
    }

    QNetworkRequest ESIInterface::prepareRequest(const QString &url, const QString &query) const
    {
        QUrlQuery endQuery{query};
#ifdef EVERNUS_ESI_SISI
        endQuery.addQueryItem(QStringLiteral("datasource"), QStringLiteral("singularity"));
#else
        endQuery.addQueryItem(QStringLiteral("datasource"), QStringLiteral("tranquility"));
#endif

        QUrl endUrl{esiUrl + url};
        endUrl.setQuery(endQuery);

        QNetworkRequest request{endUrl};
        request.setHeader(QNetworkRequest::UserAgentHeader,
                          QStringLiteral("%1 %2").arg(QCoreApplication::applicationName()).arg(QCoreApplication::applicationVersion()));
        request.setRawHeader(QByteArrayLiteral("Accept"), QByteArrayLiteral("application/json"));

        return request;
    }

    QNetworkRequest ESIInterface::prepareRequest(Character::IdType charId, const QString &url, const QString &query) const
    {
        auto request = prepareRequest(url, query);
        request.setRawHeader(QByteArrayLiteral("Authorization"), QByteArrayLiteral("Bearer ") + mAccessTokens[charId].mToken.toLatin1());

        return request;
    }

    uint ESIInterface::getNumRetries() const
    {
        return mSettings.value(NetworkSettings::maxRetriesKey, NetworkSettings::maxRetriesDefault).toUInt();
    }

    QString ESIInterface::getError(const QString &url, const QString &query, QNetworkReply &reply)
    {
        // try to get ESI error
        const auto errorDoc = QJsonDocument::fromJson(reply.readAll());
        auto errorString = errorDoc.object().value(QStringLiteral("error")).toString();
        if (errorString.isEmpty())
            errorString = reply.errorString();

        return QStringLiteral("%1?%2: %3").arg(url).arg(query).arg(errorString);
    }
}

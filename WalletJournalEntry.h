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

#include <boost/optional.hpp>

#include <QDateTime>

#include "Character.h"

namespace Evernus
{
    class WalletJournalEntry
        : public Entity<quint64>
    {
    public:
        using TaxReceiverType = boost::optional<quint64>;
        using ExtraInfoIdType = boost::optional<quint64>;
        using PartyIdType = boost::optional<quint64>;
        using ISKType = boost::optional<double>;

        using Entity::Entity;

        WalletJournalEntry() = default;
        WalletJournalEntry(const WalletJournalEntry &) = default;
        WalletJournalEntry(WalletJournalEntry &&) = default;
        virtual ~WalletJournalEntry() = default;

        Character::IdType getCharacterId() const noexcept;
        void setCharacterId(Character::IdType id) noexcept;

        QDateTime getTimestamp() const;
        void setTimestamp(const QDateTime &dt);

        QString getRefType() const &;
        QString &&getRefType() && noexcept;
        void setRefType(const QString &type);
        void setRefType(QString &&type) noexcept;

        PartyIdType getFirstPartyId() const noexcept;
        void setFirstPartyId(PartyIdType id) noexcept;

        PartyIdType getSecondPartyId() const noexcept;
        void setSecondPartyId(PartyIdType id) noexcept;

        QString getFirstPartyType() const &;
        QString &&getFirstPartyType() && noexcept;
        void setFirstPartyType(const QString &type);
        void setFirstPartyType(QString &&type) noexcept;

        QString getSecondPartyType() const &;
        QString &&getSecondPartyType() && noexcept;
        void setSecondPartyType(const QString &type);
        void setSecondPartyType(QString &&type) noexcept;

        ExtraInfoIdType getExtraInfoId() const noexcept;
        void setExtraInfoId(ExtraInfoIdType id) noexcept;

        QString getExtraInfoType() const &;
        QString &&getExtraInfoType() && noexcept;
        void setExtraInfoType(const QString &type);
        void setExtraInfoType(QString &&type) noexcept;

        ISKType getAmount() const noexcept;
        void setAmount(ISKType value) noexcept;

        ISKType getBalance() const noexcept;
        void setBalance(ISKType value) noexcept;

        QString getReason() const &;
        QString &&getReason() && noexcept;
        void setReason(const QString &reason);
        void setReason(QString &&reason);

        TaxReceiverType getTaxReceiverId() const noexcept;
        void setTaxReceiverId(TaxReceiverType id) noexcept;

        ISKType getTaxAmount() const noexcept;
        void setTaxAmount(ISKType amount) noexcept;

        quint64 getCorporationId() const noexcept;
        void setCorporationId(quint64 id) noexcept;

        bool isIgnored() const noexcept;
        void setIgnored(bool flag) noexcept;

        WalletJournalEntry &operator =(const WalletJournalEntry &) = default;
        WalletJournalEntry &operator =(WalletJournalEntry &&) = default;

    private:
        Character::IdType mCharacterId = Character::invalidId;
        QDateTime mTimestamp;
        QString mRefType;
        PartyIdType mFirstPartyId;
        PartyIdType mSecondPartyId;
        QString mFirstPartyType;
        QString mSecondPartyType;
        ExtraInfoIdType mExtraInfoId;
        QString mExtraInfoType;
        ISKType mAmount;
        ISKType mBalance;
        QString mReason;
        TaxReceiverType mTaxReceiverId;
        ISKType mTaxAmount;
        quint64 mCorporationId = 0;
        bool mIgnored = false;
    };

    bool operator <(const WalletJournalEntry &a, const WalletJournalEntry &b);
}

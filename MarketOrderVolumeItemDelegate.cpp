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
#include <QPainter>

#include "MarketOrderVolumeItemDelegate.h"

namespace Evernus
{
    void MarketOrderVolumeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        const auto data = index.data(Qt::UserRole);
        if (static_cast<QMetaType::Type>(data.type()) == QMetaType::QVariantList)
        {
            const auto list = data.toList();
            if (list.size() != 2)
                return;

            const auto alpha = std::min(std::max(0., list[0].toDouble() / list[1].toDouble()), 1.);

            const QColor brightColor{192, 255, 192};
            const QColor darkColor{128, 255, 128};

            painter->save();

            painter->setPen(brightColor);
            painter->setBrush(brightColor);
            painter->drawRect(QRectF{static_cast<qreal>(option.rect.left()),
                                     static_cast<qreal>(option.rect.top()),
                                     option.rect.width() * (1. - alpha),
                                     static_cast<qreal>(option.rect.height())});

            painter->setPen(darkColor);
            painter->setBrush(darkColor);
            painter->drawRect(QRectF{option.rect.left() + option.rect.width() * (1. - alpha),
                                     static_cast<qreal>(option.rect.top()),
                                     option.rect.width() * alpha,
                                     static_cast<qreal>(option.rect.height())});

            painter->restore();
        }

        QStyledItemDelegate::paint(painter, option, index);
    }
}

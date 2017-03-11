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
#include <QTableWidgetItem>
#include <QApplication>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QClipboard>
#include <QSettings>
#include <QLabel>

#include "PriceSettings.h"

#include "CustomFPCDialog.h"

namespace Evernus
{
    CustomFPCDialog::CustomFPCDialog(QWidget *parent)
        : QDialog(parent)
    {
        auto mainLayout = new QVBoxLayout{this};

        auto helpLabel = new QLabel{tr(
            "You can copy raw data into the clipboard for use as a custom Fast Price Copy source. "
            "The first column should contain item type id, and the second its price to copy (optional)."
        ), this};
        mainLayout->addWidget(helpLabel);
        helpLabel->setWordWrap(true);

        auto pasteBtn = new QPushButton{tr("Paste data"), this};
        mainLayout->addWidget(pasteBtn);
        connect(pasteBtn, &QPushButton::clicked, this, &CustomFPCDialog::pasteData);

        mDataView = new QTableWidget{this};
        mainLayout->addWidget(mDataView);
        mDataView->setColumnCount(2);
        mDataView->setHorizontalHeaderLabels({ tr("Type"), tr("Price") });
        mDataView->setSelectionMode(QAbstractItemView::SingleSelection);
    }

    void CustomFPCDialog::executeFPC()
    {
        auto row = mDataView->currentRow();
        if (row >= 0)
        {
            const auto priceItem = mDataView->item(row, 1);
            if (priceItem != nullptr)
            {
                auto ok = false;
                const auto price = priceItem->text().toDouble(&ok);

                if (ok)
                    QApplication::clipboard()->setText(QString::number(price, 'f', 2));
            }

            QSettings settings;
            if (settings.value(PriceSettings::showInEveOnFpcKey, PriceSettings::showInEveOnFpcDefault).toBool())
            {
                const auto id = mDataView->item(row, 0)->text().toULongLong();
                if (id != EveType::invalidId)
                    emit showInEve(id);
            }

            ++row;
            if (row >= mDataView->rowCount())
                row = 0;

            mDataView->setCurrentCell(row, 0);
        }
    }

    void CustomFPCDialog::pasteData()
    {
        const auto data = QApplication::clipboard()->text();
        if (data.isEmpty())
            return;

        mDataView->clearContents();

        auto row = 0;

        const auto lines = data.split('\n', QString::SkipEmptyParts);
        mDataView->setRowCount(lines.size());

        for (const auto &line : lines)
        {
            const auto values = line.split('\t', QString::SkipEmptyParts);
            if (values.size() > 0)
            {
                mDataView->setItem(row, 0, new QTableWidgetItem{values[0]});
                if (values.size() > 1)
                    mDataView->setItem(row, 1, new QTableWidgetItem{values[1]});

                ++row;
            }
        }

        mDataView->setRowCount(row);
        mDataView->setCurrentCell(0, 0);
    }
}

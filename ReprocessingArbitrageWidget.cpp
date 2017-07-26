/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for m details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <QStackedWidget>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QHeaderView>
#include <QCheckBox>
#include <QSettings>
#include <QSpinBox>
#include <QAction>
#include <QLabel>
#include <QDebug>

#include "ReprocessingArbitrageModel.h"
#include "FavoriteLocationsButton.h"
#include "MarketAnalysisSettings.h"
#include "CalculatingDataWidget.h"
#include "AdjustableTableView.h"
#include "StationSelectButton.h"
#include "MarketDataProvider.h"
#include "EveDataProvider.h"
#include "RegionComboBox.h"
#include "FlowLayout.h"

#include "ReprocessingArbitrageWidget.h"

namespace Evernus
{
    ReprocessingArbitrageWidget::ReprocessingArbitrageWidget(const EveDataProvider &dataProvider,
                                                             const MarketDataProvider &marketDataProvider,
                                                             const RegionStationPresetRepository &regionStationPresetRepository,
                                                             const QString &viewConfigName,
                                                             QWidget *parent)
        : StandardModelProxyWidget(mDataProxy, parent)
        , mMarketDataProvider(marketDataProvider)
    {
        const auto mainLayout = new QVBoxLayout{this};

        auto toolBarLayout = new FlowLayout{};
        mainLayout->addLayout(toolBarLayout);

        QSettings settings;

        const auto srcStationPath = settings.value(MarketAnalysisSettings::reprocessingSrcStationKey).toList();
        if (srcStationPath.size() == 4)
            mSrcStation = srcStationPath[3].toULongLong();
        const auto dstStationPath = settings.value(MarketAnalysisSettings::reprocessingDstStationKey).toList();
        if (dstStationPath.size() == 4)
            mDstStation = dstStationPath[3].toULongLong();

        toolBarLayout->addWidget(new QLabel{tr("Source:"), this});
        mSourceRegionCombo = new RegionComboBox{dataProvider, MarketAnalysisSettings::reprocessingSrcRegionKey, this};
        toolBarLayout->addWidget(mSourceRegionCombo);

        const auto srcStationBtn = new StationSelectButton{dataProvider, srcStationPath, this};
        toolBarLayout->addWidget(srcStationBtn);
        connect(srcStationBtn, &StationSelectButton::stationChanged, this, [=](const auto &path) {
            changeStation(mSrcStation, path, MarketAnalysisSettings::reprocessingSrcStationKey);
        });

        toolBarLayout->addWidget(new QLabel{tr("Destination:"), this});
        mDestRegionCombo = new RegionComboBox{dataProvider, MarketAnalysisSettings::reprocessingDstRegionKey, this};
        toolBarLayout->addWidget(mDestRegionCombo);

        const auto dstStationBtn = new StationSelectButton{dataProvider, dstStationPath, this};
        toolBarLayout->addWidget(dstStationBtn);
        connect(dstStationBtn, &StationSelectButton::stationChanged, this, [=](const auto &path) {
            changeStation(mDstStation, path, MarketAnalysisSettings::reprocessingDstStationKey);
        });

        const auto locationFavBtn = new FavoriteLocationsButton{regionStationPresetRepository, dataProvider, this};
        toolBarLayout->addWidget(locationFavBtn);
        connect(locationFavBtn, &FavoriteLocationsButton::locationsChosen,
                this, [=](const auto &srcRegions, auto srcStationId, const auto &dstRegions, auto dstStationId) {
            mSourceRegionCombo->setSelectedRegionList(srcRegions);
            mDestRegionCombo->setSelectedRegionList(dstRegions);

            srcStationBtn->setSelectedStationId(srcStationId);
            dstStationBtn->setSelectedStationId(dstStationId);
        });

        toolBarLayout->addWidget(new QLabel{tr("Base yield:"), this});

        mStationEfficiencyEdit = new QDoubleSpinBox{this};
        toolBarLayout->addWidget(mStationEfficiencyEdit);
        mStationEfficiencyEdit->setRange(0., 100.);
        mStationEfficiencyEdit->setSuffix(locale().percent());
        mStationEfficiencyEdit->setToolTip(tr("The base yield of the station you are reprocessing in."));
        mStationEfficiencyEdit->setValue(
            settings.value(MarketAnalysisSettings::reprocessingStationEfficiencyKey, MarketAnalysisSettings::reprocessingStationEfficiencyDefault).toDouble()
        );
        connect(mStationEfficiencyEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](auto value) {
            QSettings settings;
            settings.setValue(MarketAnalysisSettings::reprocessingStationEfficiencyKey, value);
        });

        toolBarLayout->addWidget(new QLabel{tr("Volume limit selling with sell orders:"), this});

        mSellVolumeLimitEdit = new QSpinBox{this};
        toolBarLayout->addWidget(mSellVolumeLimitEdit);
        mSellVolumeLimitEdit->setRange(0, std::numeric_limits<int>::max());
        mSellVolumeLimitEdit->setSuffix(locale().percent());
        mSellVolumeLimitEdit->setToolTip(tr(
            "If you wish to sell using sell orders (not recommended due to fast nature of arbitrage and additional taxes), you should specify "
            "the percentage of current market volume you wish to sell. For example, using 100% means you wish to sell the exact amount of materials "
            "which is currently on the market. Using higher values will give more total profit, but assumes you can actually sell such amounts. "
            "Therefore you should set this percentage to a reasonable value, like 10%."
        ));
        mSellVolumeLimitEdit->setValue(
            settings.value(MarketAnalysisSettings::reprocessingSellVolumeLimitKey, MarketAnalysisSettings::reprocessingSellVolumeLimitDefault).toInt()
        );
        connect(mSellVolumeLimitEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](auto value) {
            QSettings settings;
            settings.setValue(MarketAnalysisSettings::reprocessingSellVolumeLimitKey, value);
        });

        mIncludeStationTaxBtn = new QCheckBox{tr("Include station tax"), this};
        toolBarLayout->addWidget(mIncludeStationTaxBtn);
        mIncludeStationTaxBtn->setChecked(
            settings.value(MarketAnalysisSettings::reprocessingIncludeStationTaxKey, MarketAnalysisSettings::reprocessingIncludeStationTaxDefault).toBool()
        );
        connect(mIncludeStationTaxBtn, &QCheckBox::stateChanged, this, [=](auto state) {
            QSettings settings;
            settings.setValue(MarketAnalysisSettings::reprocessingIncludeStationTaxKey, state == Qt::Checked);
        });

        mIgnoreMinVolumeBtn = new QCheckBox{tr("Ignore orders with min. volume > 1"), this};
        toolBarLayout->addWidget(mIgnoreMinVolumeBtn);
        mIgnoreMinVolumeBtn->setToolTip(tr("Ignore orders wich require minimum volume larger than 1."));
        mIgnoreMinVolumeBtn->setChecked(
            settings.value(MarketAnalysisSettings::reprocessingIgnoreMinVolumeKey, MarketAnalysisSettings::reprocessingIgnoreMinVolumeDefault).toBool()
        );
        connect(mIgnoreMinVolumeBtn, &QCheckBox::stateChanged, this, [=](auto state) {
            QSettings settings;
            settings.setValue(MarketAnalysisSettings::reprocessingIgnoreMinVolumeKey, state == Qt::Checked);
        });

        mOnlyHighSecBtn = new QCheckBox{tr("Only high-sec orders"), this};
        toolBarLayout->addWidget(mOnlyHighSecBtn);
        mOnlyHighSecBtn->setChecked(
            settings.value(MarketAnalysisSettings::reprocessingOnlyHighSecKey, MarketAnalysisSettings::reprocessingOnlyHighSecDefault).toBool()
        );
        connect(mOnlyHighSecBtn, &QCheckBox::stateChanged, this, [=](auto state) {
            QSettings settings;
            settings.setValue(MarketAnalysisSettings::reprocessingOnlyHighSecKey, state == Qt::Checked);
        });

        mCustomStationTaxBtn = new QCheckBox{tr("Custom station tax"), this};
        toolBarLayout->addWidget(mCustomStationTaxBtn);
        mCustomStationTaxBtn->setChecked(
            settings.value(MarketAnalysisSettings::reprocessingCustomStationTaxKey, MarketAnalysisSettings::reprocessingCustomStationTaxDefault).toBool()
        );
        connect(mCustomStationTaxBtn, &QCheckBox::stateChanged, this, [=](auto state) {
            QSettings settings;
            settings.setValue(MarketAnalysisSettings::reprocessingCustomStationTaxKey, state == Qt::Checked);
        });

        mCustomStationTaxEdit = new QDoubleSpinBox{this};
        toolBarLayout->addWidget(mCustomStationTaxEdit);
        mCustomStationTaxEdit->setRange(0., 100.);
        mCustomStationTaxEdit->setSuffix(locale().percent());
        mCustomStationTaxEdit->setToolTip(tr("When including station tax, use this value instead of cumputed one."));
        mCustomStationTaxEdit->setEnabled(mCustomStationTaxBtn->isChecked());
        mCustomStationTaxEdit->setValue(
            settings.value(MarketAnalysisSettings::reprocessingCustomStationTaxValueKey, MarketAnalysisSettings::reprocessingCustomStationTaxValueDefault).toDouble()
        );
        connect(mCustomStationTaxEdit, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [=](auto value) {
            QSettings settings;
            settings.setValue(MarketAnalysisSettings::reprocessingCustomStationTaxValueKey, value);
        });
        connect(mCustomStationTaxBtn, &QCheckBox::stateChanged, mCustomStationTaxEdit, &QDoubleSpinBox::setEnabled);

        auto filterBtn = new QPushButton{tr("Apply"), this};
        toolBarLayout->addWidget(filterBtn);
        connect(filterBtn, &QPushButton::clicked, this, &ReprocessingArbitrageWidget::recalculateData);

        toolBarLayout->addWidget(new QLabel{tr("Press \"Apply\" to show results. Additional actions are available via the right-click menu."), this});
        toolBarLayout->addWidget(new QLabel{tr("If you wish to make the fastest trade as possible, be sure to set the correct <b>destination price type</b>. <b>Buying always uses sell orders.</b>"), this});
        toolBarLayout->addWidget(new QLabel{tr("Due to the fast nature of arbitrage, prices are not based on volume percentile."), this});
        toolBarLayout->addWidget(new QLabel{tr("Remember to import the source and resulting items to reprocess, otherwise you won't see any data."), this});

        mDataStack = new QStackedWidget{this};
        mainLayout->addWidget(mDataStack);

        mDataStack->addWidget(new CalculatingDataWidget{this});

        mDataProxy.setSortRole(Qt::UserRole);

        mDataView = new AdjustableTableView{viewConfigName, this};
        mDataStack->addWidget(mDataView);
        mDataView->setSortingEnabled(true);
        mDataView->setAlternatingRowColors(true);
        mDataView->setModel(&mDataProxy);
        mDataView->setContextMenuPolicy(Qt::ActionsContextMenu);
        mDataView->restoreHeaderState();

        mDataStack->setCurrentWidget(mDataView);

        installOnView(mDataView);
    }

    void ReprocessingArbitrageWidget::setCharacter(std::shared_ptr<Character> character)
    {
        Q_ASSERT(mDataModel != nullptr);

        StandardModelProxyWidget::setCharacter((character) ? (character->getId()) : (Character::invalidId));
        mDataModel->setCharacter(std::move(character));
    }

    void ReprocessingArbitrageWidget::clearData()
    {
        Q_ASSERT(mDataModel != nullptr);
        mDataModel->reset();
    }

    void ReprocessingArbitrageWidget::setPriceType(PriceType dst) noexcept
    {
        mDstPriceType = dst;
    }

    void ReprocessingArbitrageWidget::recalculateData()
    {
        qDebug() << "Recomputing  reprocessing arbitrage data...";

        const auto orders = mMarketDataProvider.getOrders();
        if (orders == nullptr)
            return;

        mDataStack->setCurrentIndex(waitingLabelIndex);
        mDataStack->repaint();

        boost::optional<double> stationTax;
        if (mCustomStationTaxBtn->isChecked())
            stationTax = mCustomStationTaxEdit->value() / 100.;

        Q_ASSERT(mDataModel != nullptr);
        mDataModel->setOrderData(*orders,
                                 mDstPriceType,
                                 mSourceRegionCombo->getSelectedRegionList(),
                                 mDestRegionCombo->getSelectedRegionList(),
                                 mSrcStation,
                                 mDstStation,
                                 mIncludeStationTaxBtn->isChecked(),
                                 mIgnoreMinVolumeBtn->isChecked(),
                                 mOnlyHighSecBtn->isChecked(),
                                 mStationEfficiencyEdit->value() / 100.,
                                 mSellVolumeLimitEdit->value() / 100.,
                                 stationTax);

        mDataView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

        mDataStack->setCurrentWidget(mDataView);
        mDataStack->repaint();
    }

    void ReprocessingArbitrageWidget::setSourceModel(ReprocessingArbitrageModel *model)
    {
        mDataModel = model;
        mDataProxy.setSourceModel(mDataModel);
    }

    void ReprocessingArbitrageWidget::changeStation(quint64 &destination, const QVariantList &path, const QString &settingName)
    {
        QSettings settings;
        settings.setValue(settingName, path);

        destination = EveDataProvider::getStationIdFromPath(path);

        if (QMessageBox::question(this, tr("Station change"), tr("Changing station requires data recalculation. Do you wish to do it now?")) == QMessageBox::No)
            return;

        recalculateData();
    }
}

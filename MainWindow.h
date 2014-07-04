#pragma once

#include <QMainWindow>
#include <QPointer>

#include "ItemPriceImporter.h"
#include "Character.h"

class QLabel;

namespace Evernus
{
    template<class T>
    class Repository;
    class CharacterManagerDialog;
    class ActiveTasksDialog;
    class MarginToolDialog;
    class EveDataProvider;
    class MenuBarWidget;
    class AssetProvider;
    class APIManager;
    class Key;

    class MainWindow
        : public QMainWindow
    {
        Q_OBJECT

    public:
        MainWindow(const Repository<Character> &characterRepository,
                   const Repository<Key> &keyRepository,
                   const AssetProvider &assetProvider,
                   const EveDataProvider &eveDataProvider,
                   APIManager &apiManager,
                   QWidget *parent = nullptr,
                   Qt::WindowFlags flags = 0);
        virtual ~MainWindow() = default;

        void showAsSaved();

    signals:
        void refreshCharacters();
        void refreshConquerableStations();

        void conquerableStationsChanged();
        void charactersChanged();
        void assetsChanged();
        void itemPricesChanged();

        void newTaskInfoAdded(uint taskId, const QString &description);
        void newSubTaskInfoAdded(uint taskId, uint parentTask, const QString &description);
        void taskStatusChanged(uint taskId, const QString &error);

        void importCharacter(Character::IdType id);
        void importAssets(Character::IdType id);

        void importItemPricesFromWeb(const ItemPriceImporter::TypeLocationPairs &target);

    public slots:
        void showCharacterManagement();
        void showPreferences();
        void showMarginTool();
        void showAbout();
        void showError(const QString &info);

        void addNewTaskInfo(uint taskId, const QString &description);

        void updateStatus();

        void setCharacter(Character::IdType id);

        void refreshAssets();
        void refreshAll();

    protected:
        virtual void closeEvent(QCloseEvent *event) override;

    private:
        static const QString settingsMaximizedKey;
        static const QString settingsPosKey;
        static const QString settingsSizeKey;

        const Repository<Character> &mCharacterRepository;
        const Repository<Key> &mKeyRepository;
        const AssetProvider &mAssetProvider;

        const EveDataProvider &mEveDataProvider;

        APIManager &mApiManager;

        MenuBarWidget *mMenuWidget = nullptr;

        bool mShowMaximized = false;

        CharacterManagerDialog *mCharacterManagerDialog = nullptr;
        ActiveTasksDialog *mActiveTasksDialog = nullptr;
        QPointer<MarginToolDialog> mMarginToolDialog;

        QLabel *mStatusWalletLabel = nullptr;

        Character::IdType mCurrentCharacterId = Character::invalidId;

        void readSettings();
        void writeSettings();

        void createMenu();
        void createMainView();
        void createStatusBar();
    };
}

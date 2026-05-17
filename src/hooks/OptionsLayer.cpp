#include <Geode/Geode.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include "../layers/CreditsLayer.hpp"
#include "../layers/GDPSSettingsPopup.hpp"

using namespace geode::prelude;

class $modify(MyOptionsLayer, OptionsLayer) {
    void customSetup() {
        OptionsLayer::customSetup();

        auto menu = this->getChildByIDRecursive("options-menu");
        // menu->setScale(menu->getScale() * 0.85f);

        std::vector<std::string> idsToHide = {"how-to-play-button", "rate-button", "songs-button"};
        for (const auto& id : idsToHide) {
            if (auto buttonToHide = this->getChildByIDRecursive(id)) {
                buttonToHide->setVisible(false);
            }
        }

        {
            auto buttonSprite = ButtonSprite::create("GDPS Settings", "bigFont.fnt", "GJ_button_03.png", 0.5f);
            auto button = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(MyOptionsLayer::onGDPSSettings)
            );
            button->setID("settings-button"_spr);

            menu->addChild(button);
        }

        {
            auto buttonSprite = ButtonSprite::create("Credits", "bigFont.fnt", "GJ_button_03.png", 0.5f);
            auto button = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(MyOptionsLayer::onGDPSCredits)
            );
            button->setID("credits-button"_spr);

            menu->addChild(button);
        }
        
        menu->updateLayout();
    }

    void onGDPSSettings(CCObject* sender) {
        auto popup = GDPSSettingsPopup::create();
        popup->show();
    }

    void onGDPSCredits(CCObject* sender) {
        auto scene = CreditsLayer::scene();
        auto transition = CCTransitionFade::create(0.5f, scene);
        
        CCDirector::sharedDirector()->pushScene(transition);
    }
};

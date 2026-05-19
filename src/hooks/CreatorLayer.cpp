#include <Geode/Geode.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include "../layers/ClansLayer.hpp"
#include "../layers/AnnouncementsLayer.hpp"

using namespace geode::prelude;

class $modify(MyCreatorLayer, CreatorLayer) {
    bool init() {
        if (!CreatorLayer::init()) return false;

        auto menu = this->getChildByID("creator-buttons-menu");
        if (!menu) return true; 

        {
            auto buttonSpriteIcon = CCSprite::createWithSpriteFrameName("theTowerDoor_001.png");
            buttonSpriteIcon->setScale(0.5f);
            buttonSpriteIcon->setColor({ 150, 255, 150 });

            auto buttonSpriteCaption = CCLabelBMFont::create("Clans", "bigFont.fnt");
            buttonSpriteCaption->setScale(0.45f);

            auto buttonSpriteNode = CCNode::create();
            buttonSpriteNode->setContentSize({ 90.f, 90.f });
            buttonSpriteNode->setAnchorPoint({ 0.5f, 0.5f });

            buttonSpriteIcon->setPosition({ 45.f, 50.f });
            
            buttonSpriteCaption->setPosition({ 45.f, 15.f });

            buttonSpriteNode->addChild(buttonSpriteIcon);
            buttonSpriteNode->addChild(buttonSpriteCaption);

            auto buttonSprite = CategoryButtonSprite::create(
                buttonSpriteNode,
                CategoryBaseColor::Green,
                CategoryBaseSize::Big
            );
            if (!Loader::get()->isModLoaded("raydeeux.vanilla_pages")) {
                buttonSprite->setScale(0.83f);
            }
            
            auto button = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(MyCreatorLayer::onClans)
            );

            menu->addChild(button);
        }

        {
            auto buttonSpriteIcon = CCSprite::createWithSpriteFrameName("commentIcon.png"_spr);
            // buttonSpriteIcon->setScale(1.f);

            auto buttonSpriteCaption = CCLabelBMFont::create("Announcements", "bigFont.fnt");
            buttonSpriteCaption->setScale(0.35f);

            auto buttonSpriteNode = CCNode::create();
            buttonSpriteNode->setContentSize({ 90.f, 90.f });
            buttonSpriteNode->setAnchorPoint({ 0.5f, 0.5f });

            buttonSpriteIcon->setPosition({ 45.f, 50.f });
            
            buttonSpriteCaption->setPosition({ 45.f, 15.f });

            buttonSpriteNode->addChild(buttonSpriteIcon);
            buttonSpriteNode->addChild(buttonSpriteCaption);

            auto buttonSprite = CategoryButtonSprite::create(
                buttonSpriteNode,
                CategoryBaseColor::Green,
                CategoryBaseSize::Big
            );
            if (!Loader::get()->isModLoaded("raydeeux.vanilla_pages")) {
                buttonSprite->setScale(0.83f);
            }
            
            auto button = CCMenuItemSpriteExtra::create(
                buttonSprite,
                this,
                menu_selector(MyCreatorLayer::onAnnouncements)
            );

            menu->addChild(button);
        }
        
        menu->updateLayout();

        return true;
    }

    void onClans(CCObject* sender) {
        auto scene = ClansLayer::scene();
        auto transition = CCTransitionFade::create(0.5f, scene);
        CCDirector::sharedDirector()->pushScene(transition);
    }

    void onAnnouncements(CCObject* sender) {
        auto scene = AnnouncementsLayer::scene();
        auto transition = CCTransitionFade::create(0.5f, scene);
        CCDirector::sharedDirector()->pushScene(transition);
    }
};
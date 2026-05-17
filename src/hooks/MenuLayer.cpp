#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "../layers/ClansLayer.hpp"

using namespace geode::prelude;

class $modify(MyMenuLayer, MenuLayer) {
	bool init() {
        if (!MenuLayer::init()) return false;

        auto oldLogo = this->getChildByID("main-title");

        auto newLogo = CCSprite::createWithSpriteFrameName("gdpsLogo.png"_spr);
        newLogo->setScale(0.75f);
        newLogo->setAnchorPoint({ 1.f, 1.f });
        newLogo->setPositionX(oldLogo->getPositionX() + oldLogo->getContentWidth() / 2);
        newLogo->setPositionY(oldLogo->getPositionY() - oldLogo->getContentHeight() / 2);
        this->addChild(newLogo);

		auto buttonSprite = CCSprite::createWithSpriteFrameName("theTowerDoor_001.png");
		buttonSprite->setColor({ 150, 255, 150 });
		
		auto button = CCMenuItemSpriteExtra::create(
			CircleButtonSprite::create(
				buttonSprite,
				CircleBaseColor::DarkAqua,
				CircleBaseSize::MediumAlt
			),
			this,
			menu_selector(MyMenuLayer::onClans)
		);
		buttonSprite->setScale(buttonSprite->getScale() * 0.9f);

		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(button);

		button->setID("clans-button"_spr);

		menu->getChildByID("geode.loader/geode-button")->setVisible(false);

		menu->updateLayout();

		return true;
	}

	void onClans(CCObject*) {
		auto scene = ClansLayer::scene();
        auto transition = CCTransitionFade::create(0.5f, scene);
        CCDirector::sharedDirector()->pushScene(transition);
	}
};

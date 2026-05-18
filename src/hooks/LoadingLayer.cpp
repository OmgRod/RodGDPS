#include <Geode/Geode.hpp>
#include <Geode/modify/LoadingLayer.hpp>
#include "../layers/ClansLayer.hpp"
#include "../Utils.hpp"

using namespace geode::prelude;

class $modify(MyLoadingLayer, LoadingLayer) {
	bool init(bool refresh) {
        if (!LoadingLayer::init(refresh)) return false;

        auto SFC = CCSpriteFrameCache::get();
        auto searchPathRoot = dirs::getModRuntimeDir() / Mod::get()->getID() / "resources";

        CCFileUtils::sharedFileUtils()->addSearchPath(searchPathRoot.string().c_str());
        SFC->addSpriteFramesWithFile("GDPSSheet.plist"_spr);

        auto oldLogo = this->getChildByID("gd-logo");

        auto newLogo = CCSprite::createWithSpriteFrameName("gdpsLogo.png"_spr);
        newLogo->setScale(0.75f);
        newLogo->setAnchorPoint({ 1.f, 1.f });
        newLogo->setPositionX(oldLogo->getPositionX() + oldLogo->getContentWidth() / 2);
        newLogo->setPositionY(oldLogo->getPositionY() - oldLogo->getContentHeight() / 2);
        this->addChild(newLogo);

        auto robtopLogo = static_cast<CCSprite *>(getChildByID("robtop-logo"));
        if (robtopLogo)
        {
            auto teamLogo = CCSprite::createWithSpriteFrameName("omgrodLogo.png"_spr);
            robtopLogo->setDisplayFrame(teamLogo->displayFrame());
        };
		
		return true;
	}
};

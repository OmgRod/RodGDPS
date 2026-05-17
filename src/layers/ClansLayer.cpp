#include "ClansLayer.hpp"

bool ClansLayer::init() {
    if (!CCLayer::init()) return false;

    this->setKeyboardEnabled(true);
    this->setKeypadEnabled(true);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    addBackButton(this);
    auto bg = createLayerBG();
    this->addChild(bg);
    addSideArt(this, SideArt::Bottom, false);

    auto clansTitle = CCSprite::createWithSpriteFrameName("clansTitle.png"_spr);
    clansTitle->setPosition({ winSize.width * 0.5f, winSize.height * 0.9f });
    this->addChild(clansTitle);

    return true;
}

void ClansLayer::keyBackClicked() {
    onExit(nullptr);
}

void ClansLayer::onExit(CCObject* sender) {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

CCScene* ClansLayer::scene() {
    auto scene = CCScene::create();
    auto layer = ClansLayer::create();

    scene->addChild(layer);
    return scene;
}
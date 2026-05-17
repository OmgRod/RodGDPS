// Code taken from my other mod GD Surge (with a few fixes!)

#include "CreditsLayer.hpp"

void CreditsLayer::keyBackClicked() {
    GameManager::sharedState()->fadeInMenuMusic();
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

bool CreditsLayer::init() {
    if (!CCLayer::init()) return false;

    setKeypadEnabled(true);

    FMODAudioEngine::sharedEngine()->playMusic("CreditsTheme.ogg"_spr, false, 0.1f, 0);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto menu = CCMenu::create();
    menu->setPosition({ 0.f, 0.f });
    menu->setAnchorPoint({ 0.f, 0.f });
    addChild(menu);

    auto background = createLayerBG();
    background->setPosition({ -5.f, -5.f });
    addChild(background, -1);
    addSideArt(this, SideArt::Bottom, SideArtStyle::Layer, false);

    auto backButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(CreditsLayer::backWrapper)
    );
    backButton->setPosition({ 25, winSize.height - 25 });
    menu->addChild(backButton);

    auto containerNode = CCNode::create();
    containerNode->setPosition({ 0.f, 0.f - winSize.height });
    addChild(containerNode);

    auto gdLogo = CCSprite::createWithSpriteFrameName("GJ_logo_001.png");
    gdLogo->setPosition({ winSize.width / 2, winSize.height * 0.8f });
    containerNode->addChild(gdLogo);

    auto surgeLogo = CCSprite::createWithSpriteFrameName("gdpsLogo.png"_spr);
    surgeLogo->setScale(0.75f);
    surgeLogo->setAnchorPoint({ 1.f, 1.f });
    surgeLogo->setPositionX(gdLogo->getPositionX() + gdLogo->getContentWidth() / 2);
    surgeLogo->setPositionY(gdLogo->getPositionY() - gdLogo->getContentHeight() / 2);
    containerNode->addChild(surgeLogo);

    struct CreditEntry {
        std::string text;
        bool isTitle;
    };

    std::vector<CreditEntry> credits = {
        {"Programmers", true},
        {"OmgRod", false},
        {"Art Design", true},
        {"OmgRod", false},
        {"Songs", true},
        {"Stay Inside Me by OcularNebula (credits song)", false},
        {"Owner", true},
        {"OmgRod", false},
        {"Elder Moderators", true},
        {"Chroma", false},
        {"Gemini", false},
        {"Lumi", false},
        {"", false}, {"", false}, {"", false}, {"", false}, {"", false}, {"", false}, {"", false}, {"", false},
        {"Special thanks to RobTop", false},
        {"for creating Geometry Dash", false},
        {"and inspiring millions of players!", false},
        {"", false}, {"", false}, {"", false}, {"", false}, {"", false}, {"", false}, {"", false}, {"", false},
        {"And a big special thanks to the", false},
        {"community for creating awesome", false},
        {"levels and staying active all", false},
        {"these years!", false},
        {"", false}, {"", false}, {"", false}, {"", false}, {"", false}, {"", false}, {"", false}, {"", false},
        {"You're still here??", false},
        {"", false}, {"", false}, {"", false}, {"", false}, {"", false},
        {"Why?", false},
        {"", false}, {"", false}, {"", false}, {"", false}, {"", false},
        {"Perhaps it's because you like the music?", false},
        {"", false}, {"", false}, {"", false}, {"", false}, {"", false},
        {"Or are you AFK?", false},
        {"", false}, {"", false}, {"", false}, {"", false}, {"", false},
        {"Well, anyways, I gotta go...", false},
        {"I gotta finish coding this mod lol", false},
        {"", false}, {"", false}, {"", false}, {"", false}, {"", false},
        {"Btw", false},
        {"the", false},
        {"menu", false},
        {"doesn't", false},
        {"close", false},
        {"automatically", false},
        {"so", false},
        {"please", false},
        {"either", false},
        {"press", false},
        {"escape", false},
        {"or", false},
        {"click", false},
        {"the", false},
        {"back", false},
        {"button", false},
        {"in", false},
        {"the", false},
        {"top-left", false},
        {"corner", false},
    };

    float labelY = winSize.height * 0.5f;

    for (const auto& entry : credits) {
        if (entry.text.empty()) {
            labelY -= 40.f;
            continue;
        }

        auto label = CCLabelBMFont::create(entry.text.c_str(), "bigFont.fnt");
        label->setAnchorPoint({ 0.5f, 1.f });
        label->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        label->setPosition({ winSize.width / 2.f, labelY });

        float scale = entry.isTitle ? 1.f : 0.5f;
        label->setScale(scale);
        containerNode->addChild(label);

        float spacing = entry.isTitle
            ? (label->getContentSize().height + 16.0f)
            : (label->getContentSize().height * 0.5f + 4.0f);

        labelY -= spacing;
    }

    float totalLayoutHeight = (winSize.height * 0.8f) - labelY; 
    float finalScrollY = winSize.height + totalLayoutHeight + 100.f; 

    float musicDurationSec = FMODAudioEngine::sharedEngine()->getMusicLengthMS(0) / 1000.f + 30.f;
    if (musicDurationSec <= 0.f) musicDurationSec = 45.f;

    auto moveAction = CCMoveTo::create(musicDurationSec, { 0.f, finalScrollY });
    auto delayTime  = CCDelayTime::create(2.5f);
    
    auto callBack   = CCCallFunc::create(this, callfunc_selector(CreditsLayer::keyBackClicked));
    
    auto sequence = CCSequence::create(moveAction, delayTime, callBack, nullptr);
    
    containerNode->runAction(sequence);

    return true;
}

CreditsLayer* CreditsLayer::create() {
    auto layer = new CreditsLayer();
    if (layer && layer->init()) {
        layer->autorelease();
        return layer;
    }
    CC_SAFE_DELETE(layer);
    return nullptr;
}

CCScene* CreditsLayer::scene() {
    auto scene = CCScene::create();
    auto layer = CreditsLayer::create();
    scene->addChild(layer);
    return scene;
}

void CreditsLayer::backWrapper(CCObject* sender) {
    keyBackClicked();
}

void CreditsLayer::onExit() {
    CCLayer::onExit();
}

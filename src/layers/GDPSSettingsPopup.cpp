#include <Geode/ui/GeodeUI.hpp>
#include "GDPSSettingsPopup.hpp"
#include "../Utils.hpp"

inline static CCMenuItemSpriteExtra* createButton(const char* text, std::function<void(CCObject*)> callback) {
    auto btnSprite = ButtonSprite::create(text, "goldFont.fnt", "GJ_button_01.png", 0.8f);
    auto btnItem = CCMenuItemExt::createSpriteExtra(btnSprite, callback);
    return btnItem;
}

bool GDPSSettingsPopup::init() {
    if (!Popup::init(400.f, 280.f)) return false;

    auto mainLayer = CCLayer::create();
    mainLayer->setContentSize(this->m_mainLayer->getContentSize());

    {
        float layerWidth = this->m_mainLayer->getContentWidth();
        float layerHeight = this->m_mainLayer->getContentHeight();

        auto title = CCLabelBMFont::create("Rod GDPS Settings", "bigFont.fnt");
        title->setScale(0.6f);
        float titleY = layerHeight * 0.95f; 
        title->setPosition({ layerWidth / 2.f, titleY });
        mainLayer->addChild(title);

        float titleBottomBoundary = titleY - 15.f; 
        
        float edgePaddingSide = 12.f;
        float edgePaddingTopBottom = 12.f;
        float gapBetweenBoxes = 8.f;

        float boxHeight = titleBottomBoundary - edgePaddingTopBottom;

        float totalHorizontalGaps = (edgePaddingSide * 2) + (gapBetweenBoxes * 2);
        float boxWidth = (layerWidth - totalHorizontalGaps) / 3.f;

        float yPos = edgePaddingTopBottom + (boxHeight / 2.f);

        for (int i = 0; i < 3; i++) {
            auto categoryBg = CCScale9Sprite::create("square02b_001.png");
            categoryBg->setContentSize({ boxWidth, boxHeight });
            categoryBg->setColor({ 0, 0, 0 });
            categoryBg->setOpacity(100);

            float xPos = edgePaddingSide + (boxWidth / 2.f) + (i * (boxWidth + gapBetweenBoxes));
            
            categoryBg->setPosition({ xPos, yPos });
            mainLayer->addChild(categoryBg);

            auto categoryTitle = CCLabelBMFont::create("Unknown", "bigFont.fnt");
            categoryTitle->setScale(0.5f);

            float assignedTitleY = categoryBg->getPositionY() + (boxHeight / 2.f) - 10.f;
            categoryTitle->setPosition({ categoryBg->getPositionX(), assignedTitleY });
            
            switch (i) {
                case 0: {
                    categoryTitle->setString("Account");
                    break;
                }
                case 1: {
                    categoryTitle->setString("Utils");
                    break;
                }
                case 2: {
                    categoryTitle->setString("Debug");
                    auto menu = CCMenu::create();
                    menu->setPosition({ 0.f, 0.f });
                    menu->setContentSize(mainLayer->getContentSize());
                    mainLayer->addChild(menu);
                    auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
                    infoSpr->setScale(0.35f);
                    auto infoBtn = CCMenuItemExt::createSpriteExtra(infoSpr, [](CCObject*) {
                        auto popup = FLAlertLayer::create(
                            "Warning",
                            "The buttons in this category are meant for <cr>development/debugging</c> "
                            "purposes. <cy>Only use these if you know what you are doing</c> or if you "
                            "are instructed by one of the <cp>mod developers</c>.",
                            "OK"
                        );
                        popup->show();
                    });
                    infoBtn->setPosition({
                        categoryTitle->getPositionX() + ((categoryTitle->getContentWidth() / 2) * categoryTitle->getScale()),
                        categoryTitle->getPositionY() + ((categoryTitle->getContentHeight() / 2) * categoryTitle->getScale())
                    });
                    infoBtn->setID("debug-info-button"_spr);
                    menu->addChild(infoBtn);
                    break;
                }
            }

            mainLayer->addChild(categoryTitle);

            float padding = 10.f;
            
            float bgBottomY = categoryBg->getPositionY() - (boxHeight / 2.f); 
            float titleBottomY = assignedTitleY - (categoryTitle->getContentHeight() * categoryTitle->getScaleY() / 2.f);

            float menuBottomBound = bgBottomY + padding;
            float menuTopBound = titleBottomY - padding;

            float menuHeight = menuTopBound - menuBottomBound;
            float menuWidth = boxWidth - (padding * 2.f);
            float menuYPos = menuBottomBound + (menuHeight / 2.f); 

            auto layout = SimpleAxisLayout::create(Axis::Column);
            layout->setGap(5.f);
            layout->setCrossAxisScaling(AxisScaling::ScaleDownGaps);
            layout->setMainAxisAlignment(MainAxisAlignment::Start);

            auto menu = CCMenu::create();
            menu->setLayout(layout);
            menu->setContentSize({ menuWidth, menuHeight });
            menu->setPosition({ categoryBg->getPositionX(), menuYPos });
            menu->ignoreAnchorPointForPosition(false); 

            switch (i) {
                // ACCOUNT
                case 0: {
                    auto changePassBtn = createButton("Reset Pass", [this](CCObject*) {
                        createQuickPopup(
                            "Change Password",
                            "By clicking the <cg>\"Yes\"</c> button below, you will be "
                            "taken to the <cr>forgot password</c> page. If you are "
                            "already logged in <cy>(in the website)</c>, log out and "
                            "follow the instructions in the page.",
                            "No",
                            "Yes",
                            [](FLAlertLayer*, bool btn2) {
                                if (btn2) {
                                    CCApplication::sharedApplication()->openURL("https://rod.ps.fhgdps.com/dashboard/login/forgotPassword.php");
                                }
                            }
                        );
                    });
                    menu->addChild(changePassBtn);
                    break;
                }
                // TOOLS
                case 1: {
                    auto gdBtn = createButton("Return to GD", [=](CCObject*) {
                        gdpsutils::returnToGD();
                    });
                    menu->addChild(gdBtn);
                    break;
                }
                // UTILS
                case 2: {
                    auto geodeBtn = createButton("Open Geode", [=](CCObject*) {
                        openModsList();
                    });
                    menu->addChild(geodeBtn);
                    break;
                }
            }

            menu->updateLayout();
            mainLayer->addChild(menu);
        }
    }

    auto multiplex = CCLayerMultiplexR::create({ mainLayer });
    multiplex->setPosition({ 0.f, 0.f });
    multiplex->setContentSize(this->m_mainLayer->getContentSize());
    multiplex->setTag(69); 
    this->m_mainLayer->addChild(multiplex);

    return true;
}

GDPSSettingsPopup* GDPSSettingsPopup::create() {
    auto ret = new GDPSSettingsPopup();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
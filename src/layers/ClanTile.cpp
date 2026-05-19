#include "ClanTile.hpp"
#include <charconv>

ClanTile* ClanTile::create(int id, std::string name, std::string tag, int members, bool isOwner, bool isMember, std::string color) {
    auto ret = new ClanTile();
    if (ret && ret->init(id, name, tag, members, isOwner, isMember, color)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ClanTile::init(int id, std::string name, std::string tag, int members, bool isOwner, bool isMember, std::string color) {
    if (!CCLayer::init()) return false;

    m_clanID = id;
    m_clanName = name;
    m_clanTag = tag;
    m_members = members;
    m_isOwner = isOwner;
    m_isMember = isMember;
    m_color = color;

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    this->setContentSize({ winSize.width * 0.65f, 45.f });

    auto bg = CCScale9Sprite::create("GJ_square06.png");
    bg->setContentSize(this->getContentSize());
    bg->setAnchorPoint({ 0.f, 0.f });

    int r = 255, g = 255, b = 255;
    if (!color.empty()) {
        if (color[0] == '#') {
            int hexValue = 0;
            if (std::from_chars(color.data() + 1, color.data() + color.size(), hexValue, 16).ec == std::errc()) {
                r = (hexValue >> 16) & 0xFF;
                g = (hexValue >> 8) & 0xFF;
                b = hexValue & 0xFF;
            }
        } else if (color.length() == 6 && color.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos) {
            int hexValue = 0;
            if (std::from_chars(color.data(), color.data() + color.size(), hexValue, 16).ec == std::errc()) {
                r = (hexValue >> 16) & 0xFF;
                g = (hexValue >> 8) & 0xFF;
                b = hexValue & 0xFF;
            }
        } else {
            size_t pos1 = color.find(',');
            if (pos1 != std::string::npos) {
                size_t pos2 = color.find(',', pos1 + 1);
                if (pos2 != std::string::npos) {
                    int pr = 255, pg = 255, pb = 255;
                    auto res1 = std::from_chars(color.data(), color.data() + pos1, pr);
                    auto res2 = std::from_chars(color.data() + pos1 + 1, color.data() + pos2, pg);
                    auto res3 = std::from_chars(color.data() + pos2 + 1, color.data() + color.size(), pb);
                    if (res1.ec == std::errc() && res2.ec == std::errc() && res3.ec == std::errc()) {
                        r = pr; g = pg; b = pb;
                    }
                }
            }
        }
    }
    bg->setColor({ (GLubyte)r, (GLubyte)g, (GLubyte)b });

    this->addChild(bg);

    std::string titleDisplay = fmt::format("{} [{}]", m_clanName, m_clanTag);
    auto titleLabel = CCLabelBMFont::create(titleDisplay.c_str(), "bigFont.fnt");
    titleLabel->setAnchorPoint({0.f, 0.5f});
    titleLabel->setPosition({ 15.f, this->getContentSize().height / 2.f });
    titleLabel->setScale(0.45f);
    bg->addChild(titleLabel);

    std::string memberDisplay = fmt::format("Members: {}", m_members);
    auto countLabel = CCLabelBMFont::create(memberDisplay.c_str(), "goldFont.fnt");
    countLabel->setAnchorPoint({1.f, 0.5f});
    countLabel->setPosition({ this->getContentSize().width - 120.f, this->getContentSize().height / 2.f });
    countLabel->setScale(0.55f);
    bg->addChild(countLabel);

    auto menu = CCMenu::create();
    menu->setPosition({0, 0});
    bg->addChild(menu);

    m_actionBtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Join", "goldFont.fnt", "GJ_button_01.png", 0.6f), this, nullptr);
    m_actionBtn->setAnchorPoint({0.5f, 0.5f});
    m_actionBtn->setPosition({this->getContentSize().width - 60.f, this->getContentSize().height / 2.f});
    menu->addChild(m_actionBtn);

    updateActionBtn();

    if (m_isOwner) {
        auto deleteBtnSprite = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
        deleteBtnSprite->setScale(0.6f);
        auto deleteBtn = CCMenuItemSpriteExtra::create(deleteBtnSprite, this, menu_selector(ClanTile::onDelete));
        deleteBtn->setAnchorPoint({0.5f, 0.5f});
        deleteBtn->setPosition({this->getContentSize().width - 25.f, this->getContentSize().height / 2.f});
        menu->addChild(deleteBtn);
        
        m_actionBtn->setPosition({this->getContentSize().width - 80.f, this->getContentSize().height / 2.f});
    }

    return true;
}

void ClanTile::updateActionBtn() {
    if (m_isMember) {
        auto leaveBtnSprite = ButtonSprite::create("Leave", "goldFont.fnt", "GJ_button_06.png", 0.6f);
        m_actionBtn->setNormalImage(leaveBtnSprite);
        m_actionBtn->setTarget(this, menu_selector(ClanTile::onLeave));
    } else {
        auto joinBtnSprite = ButtonSprite::create("Join", "goldFont.fnt", "GJ_button_01.png", 0.6f);
        m_actionBtn->setNormalImage(joinBtnSprite);
        m_actionBtn->setTarget(this, menu_selector(ClanTile::onJoin));
    }
}

void ClanTile::onJoin(CCObject* sender) {
    auto req = web::WebRequest();
    auto accountID = GJAccountManager::sharedState()->m_accountID;
    auto gjp2 = GJAccountManager::sharedState()->m_GJP2;

    req.header("Content-Type", "application/x-www-form-urlencoded");
    req.bodyString(fmt::format("accountID={}&gjp2={}&tag={}", accountID, gjp2, m_clanTag));
    m_webListener.spawn(
        req.post("https://rod.ps.fhgdps.com/joinClan.php"),
        [this](web::WebResponse response) {
            if (response.ok()) {
                auto respStr = response.string().unwrapOr("");
                if (respStr == "1") {
                    m_isMember = true;
                    updateActionBtn();
                    Notification::create("You have joined the clan!", NotificationIcon::Success)->show();
                } else {
                    if (respStr == "-5") FLAlertLayer::create("Error", "You are already in a clan.", "OK")->show();
                    else if (respStr == "-6") FLAlertLayer::create("Error", "Clan does not exist.", "OK")->show();
                    else if (respStr == "-7") FLAlertLayer::create("Error", "Clan is closed.", "OK")->show();
                    else FLAlertLayer::create("Error", "Failed to join clan. " + respStr, "OK")->show();
                }
            } else {
                FLAlertLayer::create("Error", "Connection failed.", "OK")->show();
            }
        }
    );
}

void ClanTile::onLeave(CCObject* sender) {
    geode::createQuickPopup("Leave Clan", "Are you sure you want to leave your current clan?", "Cancel", "Yes", [this](auto, bool btn2) {
        if (btn2) {
            auto req = web::WebRequest();
            auto accountID = GJAccountManager::sharedState()->m_accountID;
            auto gjp2 = GJAccountManager::sharedState()->m_GJP2;

            req.header("Content-Type", "application/x-www-form-urlencoded");
            req.bodyString(fmt::format("accountID={}&gjp2={}&leave=1", accountID, gjp2));
            m_webListener.spawn(
                req.post("https://rod.ps.fhgdps.com/joinClan.php"),
                [this](web::WebResponse response) {
                    if (response.ok()) {
                        auto respStr = response.string().unwrapOr("");
                        if (respStr == "1") {
                            m_isMember = false;
                            updateActionBtn();
                            Notification::create("You have left your clan!", NotificationIcon::Success)->show();
                        } else {
                            if (respStr == "-3") FLAlertLayer::create("Error", "You are not in a clan.", "OK")->show();
                            else if (respStr == "-4") FLAlertLayer::create("Error", "Owners cannot leave. Delete the clan instead.", "OK")->show();
                            else FLAlertLayer::create("Error", "Failed to leave clan. " + respStr, "OK")->show();
                        }
                    } else {
                        FLAlertLayer::create("Error", "Connection failed.", "OK")->show();
                    }
                }
            );
        }
    });
}

void ClanTile::onDelete(CCObject* sender) {
    geode::createQuickPopup("Delete Clan", "Are you sure you want to delete your clan?", "Cancel", "Yes", [this](auto, bool btn2) {
        if (btn2) {
            auto req = web::WebRequest();
            auto accountID = GJAccountManager::sharedState()->m_accountID;
            auto gjp2 = GJAccountManager::sharedState()->m_GJP2;

            req.header("Content-Type", "application/x-www-form-urlencoded");
            req.bodyString(fmt::format("accountID={}&gjp2={}&tag={}", accountID, gjp2, m_clanTag));
            m_webListener.spawn(
                req.post("https://rod.ps.fhgdps.com/deleteClan.php"),
                [this](web::WebResponse response) {
                    if (response.ok()) {
                        auto respStr = response.string().unwrapOr("");
                        if (respStr == "1") {
                            Notification::create("Clan deleted. Refresh the list to see changes.", NotificationIcon::Success)->show();
                        } else {
                            FLAlertLayer::create("Error", "Failed to delete clan. " + respStr, "OK")->show();
                        }
                    } else {
                        FLAlertLayer::create("Error", "Connection failed.", "OK")->show();
                    }
                }
            );
        }
    });
}
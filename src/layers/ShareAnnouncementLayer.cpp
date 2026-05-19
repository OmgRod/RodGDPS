#include "ShareAnnouncementLayer.hpp"
#include "AnnouncementsLayer.hpp"

ShareAnnouncementLayer* ShareAnnouncementLayer::create(AnnouncementsLayer* parentLayer, std::string title, std::string content, int editID) {
    auto ret = new ShareAnnouncementLayer();
    if (ret && ret->init(parentLayer, title, content, editID)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ShareAnnouncementLayer::init(AnnouncementsLayer* parentLayer, std::string title, std::string content, int editID) {
    if (!Popup::init(300.f, 200.f)) return false;

    m_parentLayer = parentLayer;
    m_title = title;
    m_content = content;
    m_editID = editID;

    this->setTitle(m_title);

    m_input = geode::TextInput::create(260.f, "Announcement Text...");
    m_input->setPosition({m_mainLayer->getContentSize().width / 2, m_mainLayer->getContentSize().height / 2 + 10.f});
    m_input->setString(m_content);
    m_mainLayer->addChild(m_input);

    auto submitBtnSprite = ButtonSprite::create("Post");
    auto submitBtn = CCMenuItemSpriteExtra::create(submitBtnSprite, this, menu_selector(ShareAnnouncementLayer::onSubmit));
    submitBtn->setPosition({m_mainLayer->getContentSize().width / 2, 30.f});
    m_buttonMenu->addChild(submitBtn);

    return true;
}

void ShareAnnouncementLayer::onSubmit(CCObject* sender) {
    auto text = m_input->getString();
    if (text.empty()) return;

    auto req = web::WebRequest();
    auto accountID = GJAccountManager::sharedState()->m_accountID;
    auto gjp2 = GJAccountManager::sharedState()->m_GJP2;

    req.header("Content-Type", "application/x-www-form-urlencoded");
    if (m_editID > 0) {
        req.bodyString(fmt::format("accountID={}&gjp2={}&announcementID={}&content={}", accountID, gjp2, m_editID, text));
        m_webListener.spawn(
            req.post("https://rod.ps.fhgdps.com/editGJAnnouncement.php"),
            [this](web::WebResponse response) {
                if (response.ok() && response.string().unwrapOr("") == "1") {
                    Notification::create("Announcement updated!", NotificationIcon::Success)->show();
                    if (m_parentLayer) m_parentLayer->refreshList();
                    this->onClose(nullptr);
                } else {
                    FLAlertLayer::create("Error", "Failed to update announcement.", "OK")->show();
                }
            }
        );
    } else {
        req.bodyString(fmt::format("accountID={}&gjp2={}&content={}", accountID, gjp2, text));
        m_webListener.spawn(
            req.post("https://rod.ps.fhgdps.com/uploadGJAnnouncement.php"),
            [this](web::WebResponse response) {
                if (response.ok() && response.string().unwrapOr("") == "1") {
                    Notification::create("Announcement posted!", NotificationIcon::Success)->show();
                    if (m_parentLayer) m_parentLayer->refreshList();
                    this->onClose(nullptr);
                } else {
                    auto resp = response.string().unwrapOr("");
                    if (resp == "-1") FLAlertLayer::create("Error", "You do not have permission to post announcements.", "OK")->show();
                    else FLAlertLayer::create("Error", "Failed to post announcement. " + resp, "OK")->show();
                }
            }
        );
    }
}

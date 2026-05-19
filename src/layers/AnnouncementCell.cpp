#include "AnnouncementCell.hpp"
#include "AnnouncementsLayer.hpp"

AnnouncementCell* AnnouncementCell::create(GJComment* comment, AnnouncementsLayer* parentLayer, float width, float height) {
    auto ret = new AnnouncementCell("AnnouncementCell", width, height);
    if (ret && ret->init()) {
        ret->setParentLayer(parentLayer);
        ret->loadFromComment(comment);
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void AnnouncementCell::loadFromComment(GJComment* comment) {
    CommentCell::loadFromComment(comment);

    if (comment->m_accountID == GJAccountManager::sharedState()->m_accountID) {
        auto newMenu = CCMenu::create();
        newMenu->setID("utils-menu"_spr);
        newMenu->setPosition({this->getContentSize().width - 25.f, this->getContentSize().height / 2.f});
        this->addChild(newMenu);

        auto editSpr = CCSprite::createWithSpriteFrameName("edit.png"_spr);
        editSpr->setScale(0.8f);
        auto editBtn = CCMenuItemSpriteExtra::create(
            editSpr,
            this,
            menu_selector(AnnouncementCell::onEdit)
        );
        editBtn->setPosition({0, 15.f});
        newMenu->addChild(editBtn);

        auto deleteSpr = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
        deleteSpr->setScale(0.8f);
        auto deleteBtn = CCMenuItemSpriteExtra::create(
            deleteSpr,
            this,
            menu_selector(AnnouncementCell::onDelete)
        );
        deleteBtn->setPosition({0, -15.f});
        newMenu->addChild(deleteBtn);
    }
}

void AnnouncementCell::onEdit(CCObject* sender) {
    if (m_comment) {
        ShareAnnouncementLayer::create(m_parentLayer, "Edit Announcement", m_comment->m_commentString, m_comment->m_commentID)->show();
    }
}

void AnnouncementCell::onDelete(CCObject* sender) {
    if (!m_comment) return;

    geode::createQuickPopup("Delete Announcement", "Are you sure you want to delete this announcement?", "Cancel", "Yes", [this](auto, bool btn2) {
        if (btn2) {
            auto req = web::WebRequest();
            auto accountID = GJAccountManager::sharedState()->m_accountID;
            auto gjp2 = GJAccountManager::sharedState()->m_GJP2;

            req.bodyString(fmt::format("accountID={}&gjp2={}&announcementID={}", accountID, gjp2, m_comment->m_commentID));
            m_webListener.spawn(
                req.post("https://rod.ps.fhgdps.com/deleteGJAnnouncement.php"),
                [this](web::WebResponse response) {
                    if (response.ok() && response.string().unwrapOr("") == "1") {
                        Notification::create("Announcement deleted.", NotificationIcon::Success)->show();
                        if (m_parentLayer) m_parentLayer->refreshList();
                    } else {
                        FLAlertLayer::create("Error", "Failed to delete announcement.", "OK")->show();
                    }
                }
            );
        }
    });
}
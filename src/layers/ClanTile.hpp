#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class ClanTile : public cocos2d::CCLayer {
protected:
    int m_clanID;
    std::string m_clanName;
    std::string m_clanTag;
    int m_members;
    bool m_isOwner;
    bool m_isMember;
    std::string m_color;
    TaskHolder<web::WebResponse> m_webListener;
    CCMenuItemSpriteExtra* m_actionBtn;

    bool init(int id, std::string name, std::string tag, int members, bool isOwner, bool isMember, std::string color);

    void updateActionBtn();

    void onJoin(CCObject*);
    void onLeave(CCObject*);
    void onDelete(CCObject*);

public:
    static ClanTile* create(int id, std::string name, std::string tag, int members, bool isOwner, bool isMember, std::string color);
};
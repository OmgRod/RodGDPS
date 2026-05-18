#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ClanTile : public cocos2d::CCLayer {
protected:
    int m_clanID;
    std::string m_clanName;
    std::string m_clanTag;
    int m_members;

    bool init(int id, std::string name, std::string tag, int members);

public:
    static ClanTile* create(int id, std::string name, std::string tag, int members);
};
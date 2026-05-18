#include "ClanTile.hpp"

ClanTile* ClanTile::create(int id, std::string name, std::string tag, int members) {
    auto ret = new ClanTile();
    if (ret && ret->init(id, name, tag, members)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ClanTile::init(int id, std::string name, std::string tag, int members) {
    if (!CCLayer::init()) return false;

    m_clanID = id;
    m_clanName = name;
    m_clanTag = tag;
    m_members = members;

    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    this->setContentSize({ winSize.width * 0.65f, 45.f });

    auto bg = CCScale9Sprite::create("GJ_square04.png");
    bg->setContentSize(this->getContentSize());
    bg->setAnchorPoint({ 0.f, 0.f });
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
    countLabel->setPosition({ this->getContentSize().width - 15.f, this->getContentSize().height / 2.f });
    countLabel->setScale(0.55f);
    bg->addChild(countLabel);

    return true;
}
#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class AnnouncementsLayer;

class ShareAnnouncementLayer : public geode::Popup {
protected:
    AnnouncementsLayer* m_parentLayer;
    std::string m_title;
    std::string m_content;
    int m_editID = 0;
    
    geode::TextInput* m_input = nullptr;
    TaskHolder<web::WebResponse> m_webListener;

    bool init(AnnouncementsLayer* parentLayer, std::string title, std::string content, int editID);
    void onSubmit(CCObject* sender);

public:
    static ShareAnnouncementLayer* create(AnnouncementsLayer* parentLayer, std::string title, std::string content, int editID);
};

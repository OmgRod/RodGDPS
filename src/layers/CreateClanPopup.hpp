#pragma once

#include <Geode/Geode.hpp>
#include <Geode/ui/Popup.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/cocos/extensions/GUI/CCControlExtension/CCControlColourPicker.h>

using namespace geode::prelude;

class ClansLayer;

class CreateClanPopup : public geode::Popup {
protected:
    ClansLayer* m_layer;
    TextInput* m_nameInput;
    TextInput* m_tagInput;
    cocos2d::extension::CCControlColourPicker* m_colorPicker;
    TaskHolder<web::WebResponse> m_webListener;

    bool init(ClansLayer* layer);
    void onSubmit(CCObject*);

public:
    static CreateClanPopup* create(ClansLayer* layer);
};

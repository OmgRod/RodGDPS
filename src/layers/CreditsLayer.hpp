// Code taken from my other mod GD Surge (with a few fixes!)

#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class CreditsLayer : public CCLayer {
public:
    bool init();
    static CCScene* scene();
    static CreditsLayer* create();
    void keyBackClicked();
    void backWrapper(CCObject* sender);
    void onExit();
};

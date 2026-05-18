#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <chrono>

using namespace geode::prelude;

struct ClanData {
    int id;
    std::string name;
    std::string tag;
    std::string desc;
    int members;
};

class ClansLayer : public cocos2d::CCLayer, public SetIDPopupDelegate {
private:
    CCScale9Sprite* m_contentBox = nullptr;
    ScrollLayer* m_scrollLayer = nullptr;
    CCLayer* m_contentLayer = nullptr;

    CCLabelBMFont* m_errorLabel = nullptr;
    CCLabelBMFont* m_pageLabel = nullptr;
    CCLabelBMFont* m_pageButtonLabel = nullptr;
    LoadingCircle* m_loadingCircle = nullptr;

    CCMenuItemSpriteExtra* m_nextButton = nullptr;
    CCMenuItemSpriteExtra* m_prevButton = nullptr;
    CCMenuItemSpriteExtra* m_refreshButton = nullptr;

    int m_currentPage = 0;
    int m_totalPages = 1;
    int m_totalClansCount = 0;
    const int itemsPerPage = 10;

    bool m_isLoading = true;
    bool m_isError = false;
    bool m_didRefresh = false;

    TaskHolder<web::WebResponse> m_webListener;
    std::chrono::steady_clock::time_point m_lastRefresh = std::chrono::steady_clock::now();

    bool init();
    void keyBackClicked() override;

    void fetchClansPage(int page);
    void parseClansJSON(const std::string& jsonResponse);
    void displayClans(const std::vector<ClanData>& clans);

    void onBack(CCObject*);
    void onNext(CCObject*);
    void onPrev(CCObject*);
    void onRefresh(CCObject*);
    void onGoToPage(CCObject*);
    void onLastPage(CCObject*);
    void onFirstPage(CCObject*);

    void hideError();
    void showLoading();
    void hideLoading();
    void updatePageLabels();
    int getLastPage();

    virtual void setIDPopupClosed(SetIDPopup* popup, int id) override;

public:
    static ClansLayer* create();
    static cocos2d::CCScene* scene();
    void showError();
    void updateButtons();
};
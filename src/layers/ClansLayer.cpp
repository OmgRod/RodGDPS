#include "ClansLayer.hpp"
#include "ClanTile.hpp"
#include "DoubleArrow.h"

ClansLayer* ClansLayer::create() {
    ClansLayer* ret = new ClansLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCScene* ClansLayer::scene() {
    auto scene = CCScene::create();
    scene->addChild(ClansLayer::create());
    return scene;
}

bool ClansLayer::init() {
    if (!CCLayer::init()) return false;

    setKeypadEnabled(true);
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    CCMenu* menu = CCMenu::create();
    menu->setPosition({0, 0});
    menu->setZOrder(1);
    this->addChild(menu);

    CCSprite* background = createLayerBG();
    background->setZOrder(INT_MIN);
    this->addChild(background);

    addSideArt(this, SideArt::Bottom);

    CCMenuItemSpriteExtra* backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this,
        menu_selector(ClansLayer::onBack)
    );
    backBtn->setPosition({25.f, winSize.height - 25.f});
    menu->addChild(backBtn);

    auto clansTitle = CCSprite::createWithSpriteFrameName("clansTitle.png"_spr);
    clansTitle->setPosition({ winSize.width * 0.5f, winSize.height * 0.9f });
    this->addChild(clansTitle);

    m_loadingCircle = LoadingCircle::create();
    m_loadingCircle->setParentLayer(this);
    m_loadingCircle->retain();
    m_loadingCircle->show();

    m_errorLabel = CCLabelBMFont::create("Unable to load page", "bigFont.fnt");
    m_errorLabel->setPosition(winSize / 2.f);
    m_errorLabel->setVisible(false);
    m_errorLabel->setOpacity(125);
    m_errorLabel->setScale(0.73f);
    addChild(m_errorLabel, 2);

    m_pageLabel = CCLabelBMFont::create("", "goldFont.fnt");
    m_pageLabel->setPosition({winSize.width - 15.f, winSize.height - 15.f});
    m_pageLabel->setAnchorPoint({1.f, 1.f});
    m_pageLabel->setScale(0.56f);
    addChild(m_pageLabel);

    m_contentBox = CCScale9Sprite::create("square02b_001.png");
    m_contentBox->setColor({0, 0, 0});
    m_contentBox->setOpacity(60);
    m_contentBox->setContentSize({ winSize.width * 0.72f, winSize.height * 0.68f });
    m_contentBox->setPosition({ winSize.width / 2.f, winSize.height / 2.f - winSize.height * 0.04f });
    this->addChild(m_contentBox);

    m_scrollLayer = ScrollLayer::create(m_contentBox->getContentSize(), true, true);
    m_scrollLayer->setTouchEnabled(true);
    m_contentBox->addChild(m_scrollLayer);

    m_refreshButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"), this, menu_selector(ClansLayer::onRefresh)
    );
    m_refreshButton->setPosition({winSize.width - 27.f, 27.f});
    menu->addChild(m_refreshButton);

    CCSprite* arrowNext = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    arrowNext->setFlipX(true);
    m_nextButton = CCMenuItemSpriteExtra::create(arrowNext, this, menu_selector(ClansLayer::onNext));
    m_nextButton->setPosition({winSize.width - 24.f, winSize.height / 2.f});
    menu->addChild(m_nextButton);

    m_prevButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"), this, menu_selector(ClansLayer::onPrev)
    );
    m_prevButton->setPosition({24.f, winSize.height / 2.f});
    menu->addChild(m_prevButton);

    CCSprite* pageFrame = CCSprite::create("GJ_button_02.png");
    pageFrame->setScale(0.7f);
    m_pageButtonLabel = CCLabelBMFont::create("1", "bigFont.fnt");
    m_pageButtonLabel->setPosition(pageFrame->getContentSize() / 2.f);
    pageFrame->addChild(m_pageButtonLabel);

    CCMenuItemSpriteExtra* goToPageButton = CCMenuItemSpriteExtra::create(pageFrame, this, menu_selector(ClansLayer::onGoToPage));
    goToPageButton->setPosition({winSize.width - 21.f, winSize.height - 55.f});
    menu->addChild(goToPageButton);

    CCMenuItemSpriteExtra* lastPageButton = CCMenuItemSpriteExtra::create(
        DoubleArrow::create(true, "GJ_arrow_03_001.png"), this, menu_selector(ClansLayer::onLastPage)
    );
    lastPageButton->setPosition({winSize.width - 21.f, winSize.height - 88.f});
    menu->addChild(lastPageButton);

    CCMenuItemSpriteExtra* firstPageButton = CCMenuItemSpriteExtra::create(
        DoubleArrow::create(false, "GJ_arrow_03_001.png"), this, menu_selector(ClansLayer::onFirstPage)
    );
    firstPageButton->setPosition({21.f, winSize.height - 88.f});
    menu->addChild(firstPageButton);

    fetchClansPage(m_currentPage);

    return true;
}

void ClansLayer::fetchClansPage(int page) {
    showLoading();
    
    auto req = web::WebRequest();
    std::string targetUrl = fmt::format("https://rod.ps.fhgdps.com/searchClans.php?page={}", page + 1);

    m_webListener.spawn(
        req.get(targetUrl),
        [this](web::WebResponse response) {
            if (response.ok()) {
                this->parseClansJSON(response.string().unwrapOr(""));
            } else {
                log::error("HTTP endpoint connection failed with status code: {}", response.code());
                this->showError();
            }
        }
    );
}

void ClansLayer::parseClansJSON(const std::string& jsonResponse) {
    auto result = matjson::parse(jsonResponse);
    if (!result) {
        log::error("Failed to parse json: {}", result.unwrapErr());
        showError();
        return;
    }

    matjson::Value root = result.unwrap();

    try {
        if (!root["success"].asBool().unwrap()) {
            showError();
            return;
        }

        m_totalClansCount = root["pagination"]["total"].asInt().unwrap();
        m_totalPages = root["pagination"]["totalPages"].asInt().unwrap();
        if (m_totalPages < 1) m_totalPages = 1;

        std::vector<ClanData> parsedClans;
        auto clansArray = root["clans"];
        
        for (const auto& clanObj : clansArray) {
            ClanData clan;
            clan.id = clanObj["ID"].asInt().unwrap();
            clan.name = clanObj["name"].asString().unwrap();
            clan.tag = clanObj["tag"].asString().unwrap();
            clan.desc = clanObj["desc"].asString().unwrap();
            clan.members = clanObj["members"].asInt().unwrap();
            parsedClans.push_back(clan);
        }

        displayClans(parsedClans);
    } 
    catch (const std::exception& e) {
        log::error("Exception processing json field maps: {}", e.what());
        showError();
    }
}

void ClansLayer::displayClans(const std::vector<ClanData>& clans) {
    if (m_contentLayer) {
        m_contentLayer->removeFromParentAndCleanup(true);
    }

    m_contentLayer = CCLayer::create();
    m_contentLayer->setContentSize({ m_contentBox->getContentSize().width, 0.f });
    m_contentLayer->setAnchorPoint({ 0.f, 0.f });
    m_contentLayer->setPosition({ 0.f, 0.f });

    auto columnLayout = ColumnLayout::create();
    columnLayout->setAxisReverse(true)
                ->setAutoGrowAxis(m_scrollLayer->getContentHeight())
                ->setCrossAxisOverflow(false)
                ->setAxisAlignment(AxisAlignment::End)
                ->setGap(6.f);
    m_contentLayer->setLayout(columnLayout);

    for (const auto& clan : clans) {
        auto tile = ClanTile::create(clan.id, clan.name, clan.tag, clan.members);
        m_contentLayer->addChild(tile);
    }

    auto updateContentBounds = [](CCLayer* target) {
        float dynamicHeight = 0.f;
        auto items = target->getChildren();
        if (items) {
            for (unsigned int i = 0; i < items->count(); ++i) {
                if (auto node = typeinfo_cast<CCNode*>(items->objectAtIndex(i))) {
                    dynamicHeight += node->getContentSize().height + 6.f;
                }
            }
        }
        target->setContentSize({ target->getContentSize().width, dynamicHeight + 10.f });
    };

    updateContentBounds(m_contentLayer);
    m_scrollLayer->m_contentLayer->addChild(m_contentLayer);
    m_contentLayer->updateLayout();

    float totalLayerHeight = m_contentLayer->getContentSize().height;
    m_scrollLayer->m_contentLayer->setContentSize({ m_contentBox->getContentSize().width, totalLayerHeight });
    m_scrollLayer->m_contentLayer->setAnchorPoint({ 0.f, 1.f });
    
    m_scrollLayer->m_contentLayer->setPosition({ 0.f, m_scrollLayer->getContentSize().height });
    m_scrollLayer->moveToTop();

    hideLoading();
    hideError();
    updatePageLabels();
    updateButtons();
}

int ClansLayer::getLastPage() {
    return m_totalPages;
}

void ClansLayer::keyBackClicked() {
    onBack(nullptr);
}

void ClansLayer::onBack(CCObject* sender) {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void ClansLayer::onRefresh(CCObject*) {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_lastRefresh).count();

    if (elapsed < (3 + (std::rand() % 3)) && !m_isError && m_didRefresh) return;

    m_lastRefresh = currentTime;
    m_didRefresh = true;

    fetchClansPage(m_currentPage);
}

void ClansLayer::onNext(CCObject*) {
    if (m_currentPage >= getLastPage() - 1) return;
    m_currentPage++;
    fetchClansPage(m_currentPage);
}

void ClansLayer::onPrev(CCObject*) {
    if (m_currentPage <= 0) return;
    m_currentPage--;
    fetchClansPage(m_currentPage);
}

void ClansLayer::updatePageLabels() {
    int pageMax = m_currentPage * itemsPerPage + itemsPerPage;
    m_pageLabel->setString(fmt::format(
        "{} to {} of {}",
        (m_totalClansCount == 0) ? 0 : (m_currentPage * itemsPerPage + 1),
        pageMax > m_totalClansCount ? m_totalClansCount : pageMax,
        m_totalClansCount
    ).c_str());

    m_pageButtonLabel->setString(std::to_string(m_currentPage + 1).c_str());
    m_pageButtonLabel->limitLabelWidth(28.f, 0.8f, 0.001f);
}

void ClansLayer::updateButtons() {
    bool enableArrows = !m_isLoading;

    m_nextButton->setEnabled(enableArrows && m_currentPage < getLastPage() - 1);
    m_nextButton->setOpacity(enableArrows && m_currentPage < getLastPage() - 1 ? 255 : 125);

    m_prevButton->setEnabled(enableArrows && m_currentPage > 0);
    m_prevButton->setOpacity(enableArrows && m_currentPage > 0 ? 255 : 125);

    m_refreshButton->setEnabled(!m_isLoading);
    m_refreshButton->setOpacity(!m_isLoading ? 255 : 200);
}

void ClansLayer::showError() {
    m_errorLabel->setVisible(true);
    m_isError = true;
    hideLoading();
}

void ClansLayer::hideError() {
    m_errorLabel->setVisible(false);
    m_isError = false;
    updateButtons();
}

void ClansLayer::showLoading() {
    if (m_contentLayer) {
        m_contentLayer->removeFromParentAndCleanup(true);
        m_contentLayer = nullptr;
    }
    m_loadingCircle->setVisible(true);
    m_isLoading = true;
    hideError();
}

void ClansLayer::hideLoading() {
    m_loadingCircle->setVisible(false);
    m_isLoading = false;
    updateButtons();
}

void ClansLayer::onGoToPage(CCObject* sender) {
    if (m_isLoading) return;
    auto popup = SetIDPopup::create(m_currentPage + 1, 1, getLastPage(), "Go to Page", "Search", true, 0, 0.f, true, true);
    popup->m_delegate = this;
    popup->show();
}

void ClansLayer::setIDPopupClosed(SetIDPopup* popup, int id) {
    id = std::clamp(id, 1, getLastPage());
    m_currentPage = id - 1;
    fetchClansPage(m_currentPage);
}

void ClansLayer::onLastPage(CCObject* sender) {
    if (m_isLoading) return;
    m_currentPage = getLastPage() - 1;
    fetchClansPage(m_currentPage);
}

void ClansLayer::onFirstPage(CCObject* sender) {
    if (m_isLoading) return;
    m_currentPage = 0;
    fetchClansPage(m_currentPage);
}
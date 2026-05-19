#include "AnnouncementsLayer.hpp"
#include "AnnouncementCell.hpp"
#include "ShareAnnouncementLayer.hpp"
#include "DoubleArrow.h"

AnnouncementsLayer* AnnouncementsLayer::create() {
    AnnouncementsLayer* ret = new AnnouncementsLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

CCScene* AnnouncementsLayer::scene() {
    auto scene = CCScene::create();
    scene->addChild(AnnouncementsLayer::create());
    return scene;
}

bool AnnouncementsLayer::init() {
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
        menu_selector(AnnouncementsLayer::onBack)
    );
    backBtn->setPosition({25.f, winSize.height - 25.f});
    menu->addChild(backBtn);

    auto announcementsTitle = CCSprite::createWithSpriteFrameName("announcementsTitle.png"_spr);
    announcementsTitle->setPosition({ winSize.width * 0.5f, winSize.height * 0.9f });
    this->addChild(announcementsTitle);

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
        CCSprite::createWithSpriteFrameName("GJ_updateBtn_001.png"), this, menu_selector(AnnouncementsLayer::onRefresh)
    );
    m_refreshButton->setPosition({winSize.width - 27.f, 27.f});
    menu->addChild(m_refreshButton);

    CCSprite* arrowNext = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    arrowNext->setFlipX(true);
    m_nextButton = CCMenuItemSpriteExtra::create(arrowNext, this, menu_selector(AnnouncementsLayer::onNext));
    m_nextButton->setPosition({winSize.width - 24.f, winSize.height / 2.f});
    menu->addChild(m_nextButton);

    m_prevButton = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png"), this, menu_selector(AnnouncementsLayer::onPrev)
    );
    m_prevButton->setPosition({24.f, winSize.height / 2.f});
    menu->addChild(m_prevButton);

    CCSprite* pageFrame = CCSprite::create("GJ_button_02.png");
    pageFrame->setScale(0.7f);
    m_pageButtonLabel = CCLabelBMFont::create("1", "bigFont.fnt");
    m_pageButtonLabel->setPosition(pageFrame->getContentSize() / 2.f);
    pageFrame->addChild(m_pageButtonLabel);

    CCMenuItemSpriteExtra* goToPageButton = CCMenuItemSpriteExtra::create(pageFrame, this, menu_selector(AnnouncementsLayer::onGoToPage));
    goToPageButton->setPosition({winSize.width - 21.f, winSize.height - 55.f});
    menu->addChild(goToPageButton);

    CCMenuItemSpriteExtra* lastPageButton = CCMenuItemSpriteExtra::create(
        DoubleArrow::create(true, "GJ_arrow_03_001.png"), this, menu_selector(AnnouncementsLayer::onLastPage)
    );
    lastPageButton->setPosition({winSize.width - 21.f, winSize.height - 88.f});
    menu->addChild(lastPageButton);

    CCMenuItemSpriteExtra* firstPageButton = CCMenuItemSpriteExtra::create(
        DoubleArrow::create(false, "GJ_arrow_03_001.png"), this, menu_selector(AnnouncementsLayer::onFirstPage)
    );
    firstPageButton->setPosition({21.f, winSize.height - 88.f});
    menu->addChild(firstPageButton);

    auto addBtnSprite = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
    addBtnSprite->setScale(0.8f);
    auto addBtn = CCMenuItemSpriteExtra::create(addBtnSprite, this, menu_selector(AnnouncementsLayer::onAddAnnouncement));
    addBtn->setPosition({winSize.width - 25.f, winSize.height - 25.f});
    menu->addChild(addBtn);

    fetchAnnouncementsPage(m_currentPage);

    return true;
}

void AnnouncementsLayer::fetchAnnouncementsPage(int page) {
    showLoading();
    
    auto req = web::WebRequest();
    std::string targetUrl = fmt::format("https://rod.ps.fhgdps.com/getGJAnnouncements.php?page={}", page + 1);

    m_webListener.spawn(
        req.get(targetUrl),
        [this](web::WebResponse response) {
            if (response.ok()) {
                this->parseAnnouncementsJSON(response.string().unwrapOr(""));
            } else {
                log::error("HTTP endpoint connection failed with status code: {}", response.code());
                this->showError();
            }
        }
    );
}

void AnnouncementsLayer::parseAnnouncementsJSON(const std::string& jsonResponse) {
    auto result = matjson::parse(jsonResponse);
    if (!result) {
        log::error("Failed to parse json: {}", result.unwrapErr());
        showError();
        return;
    }

    matjson::Value root = result.unwrap();

    auto success = root["success"].asBool();
    if (!success || !success.unwrap()) {
        showError();
        return;
    }

    auto total = root["pagination"]["total"].asInt();
    auto totalPages = root["pagination"]["totalPages"].asInt();

    if (!total || !totalPages) {
        log::error("Missing pagination fields");
        showError();
        return;
    }

    m_totalAnnouncementsCount = total.unwrap();
    m_totalPages = totalPages.unwrap();

    if (m_totalPages < 1)
        m_totalPages = 1;

    std::vector<GJComment*> parsedAnnouncements;

    auto announcementsArray = root["announcements"];

    for (const auto& annObj : announcementsArray) {
        auto comment = GJComment::create();
        comment->m_commentID = annObj["ID"].asInt().unwrapOr(0);
        comment->m_accountID = annObj["accountID"].asInt().unwrapOr(0);
        comment->m_userID = annObj["userID"].asInt().unwrapOr(0);
        comment->m_userName = annObj["userName"].asString().unwrapOr("Unknown");
        comment->m_commentString = annObj["content"].asString().unwrapOr("");
        comment->m_likeCount = annObj["likes"].asInt().unwrapOr(0);
        
        comment->m_hasLevelID = false;
        
        parsedAnnouncements.push_back(comment);
    }

    displayAnnouncements(parsedAnnouncements);
}

void AnnouncementsLayer::displayAnnouncements(const std::vector<GJComment*>& announcements) {
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

    float cellWidth = m_contentBox->getContentSize().width;
    for (const auto& comment : announcements) {
        auto cell = AnnouncementCell::create(comment, this, cellWidth, 60.f);
        m_contentLayer->addChild(cell);
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

int AnnouncementsLayer::getLastPage() {
    return m_totalPages;
}

void AnnouncementsLayer::keyBackClicked() {
    onBack(nullptr);
}

void AnnouncementsLayer::onBack(CCObject* sender) {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void AnnouncementsLayer::onRefresh(CCObject*) {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_lastRefresh).count();

    if (elapsed < (3 + (std::rand() % 3)) && !m_isError && m_didRefresh) return;

    m_lastRefresh = currentTime;
    m_didRefresh = true;

    fetchAnnouncementsPage(m_currentPage);
}

void AnnouncementsLayer::onNext(CCObject*) {
    if (m_currentPage >= getLastPage() - 1) return;
    m_currentPage++;
    fetchAnnouncementsPage(m_currentPage);
}

void AnnouncementsLayer::onPrev(CCObject*) {
    if (m_currentPage <= 0) return;
    m_currentPage--;
    fetchAnnouncementsPage(m_currentPage);
}

void AnnouncementsLayer::updatePageLabels() {
    int pageMax = m_currentPage * itemsPerPage + itemsPerPage;
    m_pageLabel->setString(fmt::format(
        "{} to {} of {}",
        (m_totalAnnouncementsCount == 0) ? 0 : (m_currentPage * itemsPerPage + 1),
        pageMax > m_totalAnnouncementsCount ? m_totalAnnouncementsCount : pageMax,
        m_totalAnnouncementsCount
    ).c_str());

    m_pageButtonLabel->setString(std::to_string(m_currentPage + 1).c_str());
    m_pageButtonLabel->limitLabelWidth(28.f, 0.8f, 0.001f);
}

void AnnouncementsLayer::updateButtons() {
    bool enableArrows = !m_isLoading;

    m_nextButton->setEnabled(enableArrows && m_currentPage < getLastPage() - 1);
    m_nextButton->setOpacity(enableArrows && m_currentPage < getLastPage() - 1 ? 255 : 125);

    m_prevButton->setEnabled(enableArrows && m_currentPage > 0);
    m_prevButton->setOpacity(enableArrows && m_currentPage > 0 ? 255 : 125);

    m_refreshButton->setEnabled(!m_isLoading);
    m_refreshButton->setOpacity(!m_isLoading ? 255 : 200);
}

void AnnouncementsLayer::showError() {
    m_errorLabel->setVisible(true);
    m_isError = true;
    hideLoading();
}

void AnnouncementsLayer::hideError() {
    m_errorLabel->setVisible(false);
    m_isError = false;
    updateButtons();
}

void AnnouncementsLayer::showLoading() {
    if (m_contentLayer) {
        m_contentLayer->removeFromParentAndCleanup(true);
        m_contentLayer = nullptr;
    }
    m_loadingCircle->setVisible(true);
    m_isLoading = true;
    hideError();
}

void AnnouncementsLayer::hideLoading() {
    m_loadingCircle->setVisible(false);
    m_isLoading = false;
    updateButtons();
}

void AnnouncementsLayer::onGoToPage(CCObject* sender) {
    if (m_isLoading) return;
    auto popup = SetIDPopup::create(m_currentPage + 1, 1, getLastPage(), "Go to Page", "Search", true, 0, 0.f, true, true);
    popup->m_delegate = this;
    popup->show();
}

void AnnouncementsLayer::setIDPopupClosed(SetIDPopup* popup, int id) {
    id = std::clamp(id, 1, getLastPage());
    m_currentPage = id - 1;
    fetchAnnouncementsPage(m_currentPage);
}

void AnnouncementsLayer::onLastPage(CCObject* sender) {
    if (m_isLoading) return;
    m_currentPage = getLastPage() - 1;
    fetchAnnouncementsPage(m_currentPage);
}

void AnnouncementsLayer::onFirstPage(CCObject* sender) {
    if (m_isLoading) return;
    m_currentPage = 0;
    fetchAnnouncementsPage(m_currentPage);
}

void AnnouncementsLayer::onAddAnnouncement(CCObject* sender) {
    if (m_isLoading) return;
    ShareAnnouncementLayer::create(this, "Post Announcement", "", 0)->show();
}
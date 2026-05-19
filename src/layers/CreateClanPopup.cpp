#include "CreateClanPopup.hpp"
#include "ClansLayer.hpp"

CreateClanPopup* CreateClanPopup::create(ClansLayer* layer) {
    auto ret = new CreateClanPopup();
    if (ret && ret->init(layer)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool CreateClanPopup::init(ClansLayer* layer) {
    if (!Popup::init(460.f, 250.f)) return false;

    m_layer = layer;
    this->setTitle("Create Clan");

    auto bgSize = m_mainLayer->getContentSize();
    float leftColX = 135.f;
    float rightColX = bgSize.width - 120.f;

    m_nameInput = TextInput::create(210.f, "Clan Name", "bigFont.fnt");
    m_nameInput->setPosition({leftColX, bgSize.height - 85.f});
    m_nameInput->setMaxCharCount(20);
    m_mainLayer->addChild(m_nameInput);

    m_tagInput = TextInput::create(210.f, "Clan Tag (4 chars)", "bigFont.fnt");
    m_tagInput->setPosition({leftColX, bgSize.height - 135.f});
    m_tagInput->setMaxCharCount(4);
    m_mainLayer->addChild(m_tagInput);

    m_colorPicker = cocos2d::extension::CCControlColourPicker::colourPicker();
    m_colorPicker->setPosition({rightColX, bgSize.height / 2.f - 10.f});
    m_mainLayer->addChild(m_colorPicker);

    auto submitBtnSprite = ButtonSprite::create("Create", "goldFont.fnt", "GJ_button_01.png", 0.8f);
    auto submitBtn = CCMenuItemSpriteExtra::create(submitBtnSprite, this, menu_selector(CreateClanPopup::onSubmit));
    submitBtn->setPosition({leftColX, 45.f});
    m_buttonMenu->addChild(submitBtn);

    return true;
}

void CreateClanPopup::onSubmit(CCObject* sender) {
    auto name = m_nameInput->getString();
    auto tag = m_tagInput->getString();
    
    auto ccColor = m_colorPicker->getColorValue();
    auto color = fmt::format("{:02X}{:02X}{:02X}", ccColor.r, ccColor.g, ccColor.b);

    if (name.empty() || tag.empty()) {
        FLAlertLayer::create("Error", "Please fill in all fields.", "OK")->show();
        return;
    }

    auto req = web::WebRequest();
    auto accountID = GJAccountManager::sharedState()->m_accountID;
    auto gjp2 = GJAccountManager::sharedState()->m_GJP2;

    req.header("Content-Type", "application/x-www-form-urlencoded");
    req.bodyString(fmt::format("accountID={}&gjp2={}&name={}&tag={}&color={}", accountID, gjp2, name, tag, color));
    
    m_webListener.spawn(
        req.post("https://rod.ps.fhgdps.com/createClan.php"),
        [this](web::WebResponse response) {
            if (response.ok()) {
                auto respStr = response.string().unwrapOr("");
                if (respStr == "1") {
                    Notification::create("Clan created successfully!", NotificationIcon::Success)->show();
                    if (m_layer) m_layer->refreshList();
                    this->onClose(nullptr);
                } else {
                    if (respStr == "-3") FLAlertLayer::create("Error", "You are already in a clan.", "OK")->show();
                    else if (respStr == "-4") FLAlertLayer::create("Error", "Clan name or tag already exists.", "OK")->show();
                    else FLAlertLayer::create("Error", "Failed to create clan. Code: " + respStr, "OK")->show();
                }
            } else {
                int code = response.code();
                auto respStr = response.string().unwrapOr("");
                if (respStr.empty()) respStr = "No data returned";
                
                FLAlertLayer::create(
                    "Error", 
                    fmt::format("Connection failed.\nCode: {}\nResp: {}", code, respStr), 
                    "OK"
                )->show();
            }
        }
    );
}

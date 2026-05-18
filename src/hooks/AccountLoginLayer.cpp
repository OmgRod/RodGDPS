#include <Geode/Geode.hpp>
#include <Geode/modify/AccountLoginLayer.hpp>

using namespace geode::prelude;

class $modify(MyAccountLoginLayer, AccountLoginLayer) {
    void onForgotPassword(CCObject* sender) {
        geode::utils::web::openLinkInBrowser("https://rod.ps.fhgdps.com/dashboard/login/forgotPassword.php");
    }

    void onForgotUser(CCObject* sender) {
        auto popup = FLAlertLayer::create(
            "Sorry",
            "This GDPS doesn't support username recovery. Sorry for the inconvenience!",
            "OK"
        );
        popup->show();
    }
};

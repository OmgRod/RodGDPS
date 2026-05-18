#include <Geode/Geode.hpp>
#include <Geode/modify/AccountLayer.hpp>

using namespace geode::prelude;

class $modify(MyAccountLayer, AccountLayer) {
    void onHelp(CCObject* sender) {
        auto popup = createQuickPopup(
            "Account Management",
            "<co>This GDPS doesn't have a single account management "
            "page</c>, so clicking the <cg>\"Yes\"</c> button below will "
            "take you to the <cp>GDPS dashboard</c> instead. Are you sure "
            "you want to open the dashboard?",
            "No",
            "Yes",
            [](FLAlertLayer*, bool btn2) {
                if (btn2) {
                    CCApplication::sharedApplication()->openURL("https://rod.ps.fhgdps.com/dashboard/");
                }
            }
        );
    }
};

#include <Geode/Geode.hpp>
#include <Geode/modify/SupportLayer.hpp>

using namespace geode::prelude;

class $modify(MySupportLayer, SupportLayer) {
    void onEmail(CCObject* sender) {
        auto popup = MDPopup::create(
            "Send Email",
            "For help with any issues, please contact <cp>OmgRod</c> "
            "on [GitHub](https://github.com/OmgRod/RodGDPS/issues)",
            "OK"
        );
        popup->show();
    }
};

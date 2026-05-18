#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace gdpsutils {
    inline std::vector<std::string> getBannedList() {
        return Mod::get()->getSavedValue<std::vector<std::string>>("banned-list");
    }

    inline void addToBannedList(std::string modID) {
        auto list = getBannedList();
        list.push_back(modID);
        Mod::get()->setSavedValue("banned-list", list);
    }

    inline void clearBannedList() {
        Mod::get()->setSavedValue("banned-list", std::vector<std::string>());
    }

    inline void returnToGD() {
        createQuickPopup(
            "Return to GD",
            "Are you sure you want to return to <cy>Geometry Dash</c>? "
            "All the <cg>mods</c> that have been <co>disabled will be re-enabled</c>. "
            "To return to this mod, open the <cp>Geode</c> menu and click the <cf>checkbox</c> "
            "next to the mod name.",
            "No",
            "Yes",
            [](FLAlertLayer*, bool btn2) {
                if (btn2) {
                    auto list = getBannedList();
                    
                    for (const std::string& modID : list) {
                        if (auto mod = Loader::get()->getInstalledMod(modID)) {
                            mod->enable();
                        }
                    }

                    clearBannedList();
                    Mod::get()->saveData();
                    Mod::get()->disable();
                    game::restart(true);
                }
            }
        );
    }
}

#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "Utils.hpp"

using namespace geode::prelude;

inline void clearBlacklistedMods() {
    Mod::get()->getSaveContainer().clear();
}

$on_game(ModsLoaded) {
    std::vector<Mod*> modsList = Loader::get()->getAllMods();

    bool blacklistedFound = false;

    for (Mod* mod : modsList) {
        if (!mod) continue;

#ifdef GITHUB_ACTIONS
        if (mod->isLoaded()) {
            if (mod->getMetadata().getTags().contains("cheat")) {
                mod->disable();
                gdpsutils::addToBannedList(mod->getID());
                blacklistedFound = true;
            }
        }
#endif
    }

    if (blacklistedFound) {
        Mod::get()->saveData();
        game::restart(true);
    }
}

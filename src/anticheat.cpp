#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

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
                blacklistedFound = true;
            }
        }
#endif
    }

    if (blacklistedFound) {
        game::restart(true);
    }
}

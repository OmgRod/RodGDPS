#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

using namespace geode::prelude;

$on_game(ModsLoaded) {
    std::vector<Mod*> modsList = Loader::get()->getAllMods();

    std::vector<std::string> allowedModIDs = { "geode.loader", "geode.node-ids", "omgrod.rod-gdps", "km7dev.server_api", "geode.devtools", "cvolton.misc_bugfixes", "omgrod.bettergaragestats", "capeling.garage-stats-menu" };

    bool testFailed = false;

    for (Mod* mod : modsList) {
        if (!mod) continue;

        if (mod->isLoaded()) {
            if (std::ranges::contains(allowedModIDs, std::string(mod->getID()))) {
                log::info("Allowed mod found: {}", mod->getID());
            } else {
                log::error("Unauthorized mod found: {}", mod->getID());
                mod->disable();
                testFailed = true;
            }
        }
    }

    if (testFailed) game::restart(true);
}

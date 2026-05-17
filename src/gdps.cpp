#include <Geode/Geode.hpp>
#include <km7dev.server_api/include/ServerAPIEvents.hpp>

using namespace geode::prelude;

$on_game(Loaded) {
    auto server = ServerAPIEvents::registerServer("https://gdps.omgrod.me/", INT_MAX, Mod::get());
}
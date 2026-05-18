#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>

using namespace geode::prelude;

class $modify(MyGameManager, GameManager) {
#ifndef GEODE_IS_WINDOWS
    void followTwitch() {
        geode::utils::web::openLinkInBrowser("https://www.twitch.tv/0mgrod");
    }
#endif

#ifndef GEODE_IS_WINDOWS
    void followTwitter() {
        geode::utils::web::openLinkInBrowser("https://twitter.com/0mgRod");
    }
#endif

#ifndef GEODE_IS_WINDOWS
    void joinDiscord() {
        geode::utils::web::openLinkInBrowser("https://discord.gg/vK3DuqJwyW");
    }
#endif

#ifndef GEODE_IS_WINDOWS
    void joinReddit() {
        FLAlertLayer::create(
            "Reddit",
            "I don't have a Reddit lol",
            "OK"
        )->show();
    }
#endif

#ifndef GEODE_IS_WINDOWS
    void likeFacebook() {
        FLAlertLayer::create(
            "Reddit",
            "I'm not a boomer lol",
            "OK"
        )->show();
    }
#endif

#ifndef GEODE_IS_WINDOWS
    void subYouTube() {
        geode::utils::web::openLinkInBrowser("https://www.youtube.com/@OmgRodYT");
    }
#endif
};

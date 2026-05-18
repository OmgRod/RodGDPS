#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>

using namespace geode::prelude;

class $modify(MyGameManager, GameManager) {
#ifndef GEODE_IS_WINDOWS
    void followTwitch() {
        CCApplication::sharedApplication()->openURL("https://www.twitch.tv/0mgrod");
    }
#endif

#ifndef GEODE_IS_WINDOWS
    void followTwitter() {
        CCApplication::sharedApplication()->openURL("https://twitter.com/0mgRod");
    }
#endif

#ifndef GEODE_IS_WINDOWS
    void joinDiscord() {
        CCApplication::sharedApplication()->openURL("https://discord.gg/vK3DuqJwyW");
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
        CCApplication::sharedApplication()->openURL("https://www.youtube.com/@OmgRodYT");
    }
#endif
};

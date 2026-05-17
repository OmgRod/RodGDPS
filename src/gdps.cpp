#include <Geode/modify/CCHttpClient.hpp>
#include <string>

using namespace geode::prelude;

class $modify(CCHttpClient) {
    void send(CCHttpRequest* req) {
        std::string url = req->getUrl();
        
        const std::string baseUrl = "https://www.boomlings.com/database";
        const std::string secondaryUrl = "http://www.boomlings.com/database";
        std::string newUrl = "https://rod.ps.fhgdps.com";

        if (newUrl == "NONE_REGISTERED") {
            return CCHttpClient::send(req);
        }
        if (url.starts_with("https://www.newgrounds.com/audio/download/")
            && !Mod::get()->getSettingValue<bool>("audio-fix")
        ) {
            req->setUrl(url.replace(0, 41, fmt::format("{}/music/", newUrl)).c_str());
            return CCHttpClient::send(req);
        }
        if (url.starts_with(baseUrl)) {
            req->setUrl(url.replace(0, baseUrl.length(), newUrl).c_str());
        } else if (url.starts_with(secondaryUrl)) {
            req->setUrl(url.replace(0, secondaryUrl.length(), newUrl).c_str());
        }
        
        req->setConnectTimeout(30);

        CCHttpClient::send(req);
    }
};
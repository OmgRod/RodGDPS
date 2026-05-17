#include <Geode/Geode.hpp>
#include <Geode/modify/ProfilePage.hpp>

using namespace geode::prelude;

class $modify(MyProfilePage, ProfilePage) {
    struct Fields {
        int m_clanID;
    };
};

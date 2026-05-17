#include <Geode/Geode.hpp>

using namespace geode::prelude;

class ClansLayer : public CCLayer {
public:
    CREATE_FUNC(ClansLayer);
    static CCScene* scene();

private:
    bool init();
    void keyBackClicked();
    void onExit(CCObject*);
};
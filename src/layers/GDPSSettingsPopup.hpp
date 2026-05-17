#include <Geode/Geode.hpp>
#include "CCLayerMultiplexR.hpp"

using namespace geode::prelude;

class GDPSSettingsPopup : public Popup {
public:
    static GDPSSettingsPopup* create();
private:
    bool init();
};

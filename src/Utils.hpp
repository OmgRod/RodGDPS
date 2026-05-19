#pragma once

#include "Geode/cocos/label_nodes/CCLabelBMFont.h"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace gdpsutils {
    inline std::vector<std::string> getBannedList() {
        return Mod::get()->getSavedValue<std::vector<std::string>>("banned-list");
    }

    inline void addToBannedList(std::string modID) {
        auto list = getBannedList();
        list.push_back(modID);
        Mod::get()->setSavedValue("banned-list", list);
    }

    inline void clearBannedList() {
        Mod::get()->setSavedValue("banned-list", std::vector<std::string>());
    }

    inline void returnToGD() {
        createQuickPopup(
            "Return to GD",
            "Are you sure you want to return to <cy>Geometry Dash</c>? "
            "All the <cg>mods</c> that have been <co>disabled will be re-enabled</c>. "
            "To return to this mod, open the <cp>Geode</c> menu and click the <cf>checkbox</c> "
            "next to the mod name.",
            "No",
            "Yes",
            [](FLAlertLayer*, bool btn2) {
                if (btn2) {
                    auto list = getBannedList();
                    
                    for (const std::string& modID : list) {
                        if (auto mod = Loader::get()->getInstalledMod(modID)) {
                            (void)mod->enable();
                        }
                    }

                    clearBannedList();
                    (void)Mod::get()->saveData();
                    (void)Mod::get()->disable();
                    game::restart(true);
                }
            }
        );
    }

    /// GLOBED FUNCTIONS - THANKS DANK!!!

    constexpr uint8_t hexDigit(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        throw "invalid hex digit";
    }

    constexpr uint8_t hexByte(char hi, char lo) {
        return static_cast<uint8_t>((hexDigit(hi) << 4) | hexDigit(lo));
    }

    constexpr cocos2d::ccColor4B colorFromHex(const char* str) {
        if (str[0] == '#') str++;

        int len = 0;
        while (str[len] != '\0') ++len;
        if (len != 6 && len != 8) throw "Invalid hex color length";

        uint8_t r = hexByte(str[0], str[1]);
        uint8_t g = hexByte(str[2], str[3]);
        uint8_t b = hexByte(str[4], str[5]);
        uint8_t a = (len == 8) ? hexByte(str[6], str[7]) : 255;

        return {r, g, b, a};
    }

    constexpr cocos2d::ccColor3B color3FromHex(const char* str) {
        if (str[0] == '#') str++;

        int len = 0;
        while (str[len] != '\0') ++len;
        if (len != 6) throw "Invalid hex color length";

        uint8_t r = hexByte(str[0], str[1]);
        uint8_t g = hexByte(str[2], str[3]);
        uint8_t b = hexByte(str[4], str[5]);

        return {r, g, b};
    }

    static ccColor3B mapColor(char c) {
        switch (c) {
            case 'b': return color3FromHex("#4a52e1");
            case 'g': return color3FromHex("#40e348");
            case 'l': return color3FromHex("#60abef");
            case 'j': return color3FromHex("#32c8ff");
            case 'y': return color3FromHex("#ffff00");
            case 'o': return color3FromHex("#ffa54b");
            case 'r': return color3FromHex("#ff5a5a");
            case 'p': return color3FromHex("#ff00ff");
            case 'a': return color3FromHex("#9632ff");
            case 'd': return color3FromHex("#ff96ff");
            case 'c': return color3FromHex("#ffff96");
            case 'f': return color3FromHex("#96ffff");
            case 's': return color3FromHex("#ffdc41");
            default: return color3FromHex("#ff0000");
        }
    }

    inline void colorizeLabel(CCLabelBMFont* label, std::string_view text) {
        struct Run {
            size_t start, end;
            ccColor3B color;
        };

        std::string outText;
        std::vector<Run> runs;
        size_t i = 0;
        size_t childI = 0;

        auto skip = [&](size_t n = 1) {
            auto sv = text.substr(i, n);
            outText += sv;
            i += n;

            for (char c : sv) {
                if (c != ' ' && c != '\t' && c != '\n' && c != '\r') childI++;
            }
        };

        std::optional<Run> current;

        while ((int64_t)text.size() - (int64_t)i >= 4) {
            std::string_view slice = text.substr(i, 4);

            if (!current) {
                // Scan for beginning of a new run
                if (!slice.starts_with("<c") || slice[3] != '>') {
                    skip();
                    continue;
                }

                auto color = mapColor(slice[2]);
                current = Run{childI, 0, color};
                i += 4;
                continue;
            }

            // Scan for end of current run
            if (slice != "</c>") {
                skip();
                continue;
            }

            current->end = childI;
            runs.push_back(*current);
            current = std::nullopt;
            i += 4;
        }

        // append remaining text
        skip(text.size() - i);

        label->setString(outText.c_str());
        auto mainBatch = label->getChildByType<CCSpriteBatchNode>(0);
        auto mchildren = mainBatch->getChildrenExt<CCSprite>();

        for (auto& run : runs) {
            for (size_t i = run.start; i < run.end; i++) {
                mchildren[i]->setColor(run.color);
            }
        }
    }

    /// END GLOBED FUNCTIONS
}

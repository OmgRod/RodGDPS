#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "ShareAnnouncementLayer.hpp"

using namespace geode::prelude;

class AnnouncementsLayer;

class AnnouncementCell : public CommentCell {
protected:
    AnnouncementsLayer* m_parentLayer = nullptr;
    TaskHolder<web::WebResponse> m_webListener;

public:
    AnnouncementCell(const char* name, float width, float height) : CommentCell(name, width, height) {}

    void setParentLayer(AnnouncementsLayer* layer) { m_parentLayer = layer; }
    void onEdit(CCObject* sender);
    void onDelete(CCObject* sender);
    void loadFromComment(GJComment* comment);

    static AnnouncementCell* create(GJComment* comment, AnnouncementsLayer* parentLayer, float width, float height);
};

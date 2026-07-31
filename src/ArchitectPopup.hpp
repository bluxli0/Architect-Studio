#pragma once
#include <Geode/Geode.hpp>
#include "generation/LevelGraph.hpp"

using namespace geode::prelude;

namespace architect {

// The hub popup opened from the editor tab. Houses entry points to every
// tool rather than being a single-purpose settings panel - Generate stays
// here directly since it's the most common action, everything else opens
// its own popup.
class ArchitectPopup : public geode::Popup<> {
protected:
    bool setup() override;

    void onGenerate(CCObject*);
    void onOpenPalette(CCObject*);
    void onOpenGroups(CCObject*);
    void onOpenTimeline(CCObject*);
    void onImportGmd(CCObject*);

    GenerationSettings m_settings;
    CCLabelBMFont* m_statusLabel = nullptr;

public:
    static ArchitectPopup* create();
};

} // namespace architect

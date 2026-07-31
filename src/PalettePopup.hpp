#pragma once
#include <Geode/Geode.hpp>
#include "generation/PaletteGenerator.hpp"

using namespace geode::prelude;

namespace architect {

// Shows a grid of swatches from PaletteGenerator with a Regenerate
// button. The simplest possible concrete UI on top of the generator -
// intentionally not wired to Decorator's style presets yet (that's
// an "Apply to style" button away, left as a next step).
class PalettePopup : public geode::Popup<> {
protected:
    bool setup() override;

    void onRegenerate(CCObject*);
    void rebuildGrid();

    PaletteSettings m_settings;
    CCNode* m_gridContainer = nullptr;

public:
    static PalettePopup* create();
};

} // namespace architect

#include "PalettePopup.hpp"
#include <chrono>

namespace architect {

PalettePopup* PalettePopup::create() {
    auto ret = new PalettePopup();
    if (ret->initAnchored(320.f, 260.f)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool PalettePopup::setup() {
    setTitle("Harmonic Palette");

    m_settings.seed = static_cast<unsigned int>(
        std::chrono::system_clock::now().time_since_epoch().count()
    );
    m_settings.harmony = HarmonyType::HarmonicSeries;
    m_settings.count = 64;

    m_gridContainer = CCNode::create();
    m_gridContainer->setPosition({20.f, 200.f});
    m_mainLayer->addChild(m_gridContainer);

    rebuildGrid();

    auto regenBtn = CCMenuItemSpriteExtra::create(
        ButtonSprite::create("Regenerate"),
        this,
        menu_selector(PalettePopup::onRegenerate)
    );
    auto menu = CCMenu::createWithItem(regenBtn);
    menu->setPosition({m_mainLayer->getContentSize().width / 2, 30.f});
    m_mainLayer->addChild(menu);

    return true;
}

void PalettePopup::rebuildGrid() {
    m_gridContainer->removeAllChildren();

    auto swatches = PaletteGenerator::generate(m_settings);

    const float cellSize = 22.f;
    const int columns = 8;

    // CCLayerColor is the simplest thing that can show a flat color
    // block; a polished version would use sprites so cells can have
    // rounded corners/borders matching Geode's usual UI language.
    for (size_t i = 0; i < swatches.size(); i++) {
        auto const& sw = swatches[i];
        int col = static_cast<int>(i) % columns;
        int row = static_cast<int>(i) / columns;

        auto cell = CCLayerColor::create(ccc4(sw.r, sw.g, sw.b, 255), cellSize - 2.f, cellSize - 2.f);
        cell->setPosition({col * cellSize, -row * cellSize});
        m_gridContainer->addChild(cell);
    }
}

void PalettePopup::onRegenerate(CCObject*) {
    std::mt19937 rng(m_settings.seed + 1);
    std::uniform_int_distribution<unsigned int> reroll(0u, 0xffffffffu);
    m_settings.seed = reroll(rng);
    rebuildGrid();
}

} // namespace architect

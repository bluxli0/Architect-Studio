#include "ArchitectPopup.hpp"
#include "PalettePopup.hpp"
#include "ui/GroupBrowserPopup.hpp"
#include "ui/TimelinePopup.hpp"
#include "generation/ObjectPlacer.hpp"
#include "generation/Decorator.hpp"
#include "generation/TemplateLibrary.hpp"
#include "generation/StyleAnalyzer.hpp"
#include "io/GmdImporter.hpp"
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/utils/file.hpp>
#include <random>
#include <chrono>

namespace architect {

ArchitectPopup* ArchitectPopup::create() {
    auto ret = new ArchitectPopup();
    if (ret->initAnchored(320.f, 260.f)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ArchitectPopup::setup() {
    setTitle("Level Architect");

    m_settings.seed = static_cast<unsigned int>(
        std::chrono::system_clock::now().time_since_epoch().count()
    );
    m_settings.targetDifficulty = 0.5f;
    m_settings.lengthInUnits = 900.f;
    m_settings.stylePreset = "default";

    struct ButtonSpec { const char* label; SEL_MenuHandler handler; };
    // clang-format off
    std::vector<std::pair<std::string, SEL_MenuHandler>> buttons = {
        {"Generate",      menu_selector(ArchitectPopup::onGenerate)},
        {"Palette",       menu_selector(ArchitectPopup::onOpenPalette)},
        {"Groups",        menu_selector(ArchitectPopup::onOpenGroups)},
        {"Timeline",      menu_selector(ArchitectPopup::onOpenTimeline)},
        {"Import .gmd",   menu_selector(ArchitectPopup::onImportGmd)},
    };
    // clang-format on

    auto menu = CCMenu::create();
    float y = 190.f;
    for (auto& [label, handler] : buttons) {
        auto btn = CCMenuItemSpriteExtra::create(ButtonSprite::create(label.c_str()), this, handler);
        btn->setPosition({m_mainLayer->getContentSize().width / 2, y});
        menu->addChild(btn);
        y -= 36.f;
    }
    m_mainLayer->addChild(menu);

    m_statusLabel = CCLabelBMFont::create("Ready.", "chatFont.fnt");
    m_statusLabel->setScale(0.5f);
    m_statusLabel->setPosition({m_mainLayer->getContentSize().width / 2, 20.f});
    m_mainLayer->addChild(m_statusLabel);

    return true;
}

void ArchitectPopup::onGenerate(CCObject*) {
    auto* editor = LevelEditorLayer::get();
    if (!editor) { m_statusLabel->setString("No level open."); return; }

    LevelGraph graph(m_settings);
    graph.generate();

    ObjectPlacer placer(editor, ObjectIDs{});
    float endX = placer.place(graph, 0.f, 105.f);

    TemplateLibrary templates(Mod::get()->getResourcesDir().string() + "/templates");
    templates.load();

    DecorationStyle style;
    style.name = m_settings.stylePreset;
    style.density = 0.4f;
    Decorator decorator(editor, style, m_settings.seed);
    decorator.decorate(graph, 0.f, 105.f);

    (void)endX;
    m_statusLabel->setString(fmt::format("Generated {} segments.", graph.segments().size()).c_str());
}

void ArchitectPopup::onOpenPalette(CCObject*) {
    PalettePopup::create()->show();
}

void ArchitectPopup::onOpenGroups(CCObject*) {
    GroupBrowserPopup::create()->show();
}

void ArchitectPopup::onOpenTimeline(CCObject*) {
    TimelinePopup::create()->show();
}

void ArchitectPopup::onImportGmd(CCObject*) {
    // file::pick opens a native file picker and resolves a Result<path>
    // asynchronously via a promise/callback - the exact call shape
    // (browse vs pick, filter struct fields) should be checked against
    // Geode/utils/file.hpp in your SDK version before trusting this.
    auto filter = file::FilePickOptions::Filter{{"*.gmd"}, "Geometry Dash Level"};
    file::pick(file::PickMode::OpenFile, {std::filesystem::path(), {filter}})
        .listen([this](Result<std::filesystem::path>* result) {
            if (!result || result->isErr()) {
                m_statusLabel->setString("Import cancelled or failed.");
                return;
            }
            ParsedLevel level;
            if (!GmdImporter::importFile(result->unwrap().string(), level)) {
                m_statusLabel->setString("Could not decode .gmd file.");
                return;
            }
            auto dna = StyleAnalyzer::analyze(level);
            m_settings.seed = dna.suggestedPaletteSeed;
            m_statusLabel->setString(
                fmt::format("Imported '{}' - {} objects analyzed.", dna.sourceName, level.objects.size()).c_str()
            );
        });
}

} // namespace architect

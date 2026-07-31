#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "ArchitectPopup.hpp"

// NOTE: this include path is a best guess - alphalaneous.editortab_api's
// actual header layout wasn't visible from its docs page alone. Once the
// dependency is fetched by CMake/CPM, check its include/ folder and fix
// this path to match. The docs page itself was inconsistent between
// "EditorTabs::get()->addTab()" (prose) and "EditorTabs::addTab()" (code
// example) - the code below follows the code example verbatim; verify
// against whichever the installed version actually expects.
#include <EditorTab-API/include/EditorTabs.hpp>

using namespace geode::prelude;

class $modify(ArchitectEditorUI, EditorUI) {
    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;

        EditorTabs::addTab(
            this, TabType::EDIT, "architect-tab"_spr,
            [](EditorUI* ui, CCMenuItemToggler* toggler) -> CCNode* {
                auto arr = CCArray::create();
                // `ui` is passed as EditorUI*, but at runtime it's actually
                // our modified ArchitectEditorUI - Geode's $modify replaces
                // the class in place, so this cast is safe and is the usual
                // pattern for reaching a modify-class method from inside a
                // tab-content lambda like this.
                auto btn = CCMenuItemSpriteExtra::create(
                    ButtonSprite::create("Open Architect"),
                    ui,
                    menu_selector(ArchitectEditorUI::onOpenArchitect)
                );
                arr->addObject(btn);
                return EditorTabUtils::createEditButtonBar(arr, ui);
            },
            [](EditorUI*, bool, CCNode*) {
                // tab shown/hidden toggle - nothing needed here since the
                // button opens a popup rather than living in the tab itself
            }
        );

        return true;
    }

    void onOpenArchitect(CCObject*) {
        architect::ArchitectPopup::create()->show();
    }
};

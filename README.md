# Level Architect (Geode mod scaffold)

Procedural level generation inside the GD editor: settings in, a full
layout + auto-decoration + stitched template chunks out - plus a growing
toolset around it (palette generation, group locking, a timeline view,
style import from existing levels).

## Layout

```
src/
  main.cpp                    editor tab entry point (EditorTab API)
  ArchitectPopup.hpp/.cpp      hub popup - Generate + links to every tool
  PalettePopup.hpp/.cpp        harmonic palette viewer/regenerator
  generation/
    LevelGraph, ObjectPlacer, Decorator, TemplateLibrary   - core pipeline
    Strategies (Noise/Sine/Platformer), PaletteGenerator
    DifficultyCurve            timeline-editable curve + margins + fatigue
    StyleAnalyzer               Style DNA stats from imported levels
    DepthGradient                layer-tint + density-heatmap color math
    PatternValidator             independent anti-pattern checks
  rules/
    Rule, RuleSet                hard-constraint / soft-preference rules
  core/
    GroupRegistry                per-level lock tracking (save-data backed)
    TaskManager                  frame-budgeted job queue
  io/
    GmdImporter                  .gmd decode: plist -> base64 -> zlib -> objects
  ui/
    GroupBrowserPopup            group list + lock toggles
    TimelineTrack, TimelinePopup  first-cut timeline (no drag/zoom yet)
resources/
  templates/                     chunk .json + .txt pairs go here
```

## What's real vs. what needs your input

Real, working logic:
- Segment generation (Linear/Noise/Sine/Platformer strategies), difficulty
  curve sampling, fatigue tracking, rule evaluation
- `.gmd` decoding (plist extraction, URL-safe base64, zlib inflate,
  object-string parsing) and Style DNA statistics from the result
- Palette generation (harmonic hue math), depth/heatmap gradient math
- Chunk stitching math in `TemplateLibrary`
- `GroupRegistry`'s lock persistence via Geode's real `setSavedValue`/
  `getSavedValue` API (verified against current docs)

Needs your input before it's visible/functional in-game:
- **`ObjectIDs`** in `ObjectPlacer.hpp` - still all `-1`, fill in from
  Geode DevTools' object browser
- **Template chunks** - `resources/templates/` is empty
- **`GroupBrowserPopup::scanLevelGroups`** - returns empty; needs the
  real accessor for a `GameObject`'s group membership, which wasn't
  confirmed against current bindings
- **`main.cpp`'s EditorTab API include path** - a best guess; confirm
  against the dependency's actual headers once fetched
- **`ArchitectPopup::onImportGmd`'s `file::pick` call shape** - verify
  against `Geode/utils/file.hpp` in your SDK version

## Dependency change this round

Switched from manually hunting `"edit-button-bar"` (which also required
depending on `geode.node-ids`, which mod.json never actually had) to
`alphalaneous.editortab_api`, now declared in `mod.json`. This gives the
whole toolset a proper tab instead of squeezing one button into a vanilla
menu - worth it now that there's more than one entry point.

## Known gaps, called out on purpose

- **Timeline UI** is tracks-as-rows with static markers - no drag, zoom,
  or click-to-edit yet. `TimelinePopup::onEventMarkerClicked` is a stub.
- **Strategies don't read the timeline's `DifficultyCurve` yet** - Noise/
  Sine currently compute their own internal difficulty. Wiring the
  curve in as the actual source of truth is the next integration step,
  not done yet.
- **Genome export/import** (portable seed+rules+palette+templates as one
  shareable file) - discussed, not built this round.
- **Playtest heatmap** (real death-position data feeding difficulty) -
  needs a `PlayLayer` hook, not built this round.
- **Live-adaptive difficulty** (level changes during actual play) is a
  different system entirely from everything here (editor-time
  generation vs. runtime mutation) - flagged, not attempted.

## Build

Geode CLI + SDK, `GEODE_SDK` set, then from this folder: `geode build`.
Change `mod.json`'s `id`/`developer` from the `yourname` placeholder
first. If zlib linking fails, see the comment in `CMakeLists.txt`.

## Suggested next steps

1. Fill in `ObjectIDs`, confirm generation places real objects
2. Confirm the EditorTab API include path and dependency version once
   fetched locally
3. Wire `scanLevelGroups` to real object data so Group Browser has
   something to show
4. Wire `DifficultyCurve` into the strategies as their actual difficulty
   source, replacing their internal computation

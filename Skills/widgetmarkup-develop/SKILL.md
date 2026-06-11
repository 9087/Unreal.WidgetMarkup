---
name: widgetmarkup-develop
user-invocable: true
description: 'WidgetMarkup is an Unreal Engine 5 plugin providing a declarative XML-based UI framework with Python scripting. Use this skill when the user wants to: write .widgetmarkup XML files to define UMG widget blueprints; write Python components (WidgetMarkupComponent subclasses) for UI logic; set up reactive data bindings between Python properties and UMG widgets; create ListView/TileView entries with IUserObjectListEntry/IUserListEntry mixins; create regular (non-widget) Blueprints with variables; use style sheets; debug WidgetMarkup compilation or binding issues. When NOT to use: general UE5 C++ development, standard UMG widget coding outside WidgetMarkup.'
---

# WidgetMarkup Development

## 1. Blueprint Creation

### 1.1 Widget Blueprint

A Widget Blueprint (`.widgetmarkup`) compiles to a `UWidgetBlueprint` with a `WidgetTree`. It has a mandatory `Script` attribute pointing to the Python component module.

```xml
<WidgetBlueprint Script="Samples.MyComponent">
  <WidgetTree>
    <CanvasPanel>
      <TextBlock Text="Hello" />
    </CanvasPanel>
  </WidgetTree>
</WidgetBlueprint>
```

The `Script` attribute is a Python import path resolved against the **project's** `Content/Python/` directory:

| Path | Purpose |
|---|---|
| `<ProjectDir>/Content/Python/` | **Your Python components** — create/edit files here |
| `<ProjectDir>/Plugins/WidgetMarkup/Content/Python/` | Framework code (`WidgetMarkupComponent.py`, `ObservableCollection.py`) and test cases — do NOT put your files here |

When the same module name exists in both paths, the project-path copy takes precedence — the plugin copy is silently ignored. For example, `Script="Calculator"` imports `<ProjectDir>/Content/Python/Calculator.py`.

> **Finding the project directory:** The project root is NOT always `Game/`. Look for the `.uproject` file to determine the actual project directory. If you cannot find it, ask the user.
> ```powershell
> Get-ChildItem -Recurse -Filter "*.uproject" | Select-Object -First 1 DirectoryName
> ```

`.widgetmarkup` XML blueprints also live in the **project's** `Content/` directory — not in the plugin. The plugin directory is reserved for samples and test cases (both under the plugin's `Content/`).

> **Path prefix matters:** Project blueprints are referenced with `/Game/` (e.g. `/Game/ScientificCalculator`), while plugin samples use `/WidgetMarkup/` (e.g. `/WidgetMarkup/Samples/Counter`). See [§3.3 Class Path Resolution](#33-class-path-resolution) for details.

The parent class defaults to `UWidgetMarkupUserWidget`. Override with `Super` (must be a subclass of `UWidgetMarkupUserWidget`). Add interfaces with `Implements`:
```xml
<WidgetBlueprint Script="Samples.MyComponent"
                 Implements="MyInterface,OtherInterface" />
```

### 1.2 Regular (Non-Widget) Blueprint

A regular Blueprint has no WidgetTree. Use `<Blueprint>` instead of `<WidgetBlueprint>`:

```xml
<Blueprint Super="Actor" Implements="AbilitySystemInterface">
  <Variable Name="Health" Type="Float" Default="100.0" />
  <Variable Name="Name" Type="String" Default="Player" />
</Blueprint>
```

### 1.3 Blueprint Variables

Variables are defined with `<Variable>` elements. See [docs/blueprint-variables.md](docs/blueprint-variables.md) for complete reference.

**Quick reference** (PascalCase, case-sensitive):

| Category | Types |
|---|---|
| Basic | `Boolean`, `Integer`, `Integer64`, `Float`, `Double`, `Byte`, `String`, `Text`, `Name` |
| Struct | `Vector`, `Vector2D`, `Rotator`, `Transform`, `Color`, `LinearColor` |
| Container | `Array(Float)`, `Set(String)`, `Map(String,Integer)` |
| Object | `Object(Actor)`, `Class(Actor)`, `SoftObject(Texture2D)`, `SoftClass(Actor)` |
| Enum | `ECollisionChannel` (auto-detected) |

## 2. WidgetTree & Widgets

### 2.1 Structure

The `<WidgetTree>` element is required inside `<WidgetBlueprint>`. It contains one root widget. The blueprint is auto-compiled after XML processing finishes via `FKismetEditorUtilities::CompileBlueprint`.

**Widget class resolution:** Element names are resolved via `UClass::TryFindTypeSlow` — any non-abstract, non-UserWidget UMG class works.

| Constraint | Widgets |
|---|---|
| Single child | Button, Border, ScrollBox, SizeBox, ScaleBox |
| Multiple children | CanvasPanel, VerticalBox, HorizontalBox, GridPanel, WrapBox, Overlay, UniformGridPanel |
| No children | TextBlock, Image, ProgressBar, Slider, CheckBox, Spacer, EditableText, ComboBoxString |

### 2.2 Widgets & Slots

> **Any UPROPERTY on the UMG class works as an XML attribute** via `FPropertyChainHandle`, which resolves dot-separated paths through UE's reflection system. Both nested structs (e.g. `Slot.Size.SizeRule`) and object pointers (e.g. `Slot.Row`) are supported — the slot object is created by `OnAddChild` before attributes are processed.\n>\n> **Sub-property syntax:** Dot notation (`Font.Size=\"18\"`) and child-element syntax (`<Font><Size>18</Size></Font>`) both work — they go through the same `FPropertyPathResolver`. Child-element form is preferred for clarity and was validated in test files. For container properties like `ColumnFill` and `RowFill`, child-element syntax is required (these are array properties, not scalar).

**[Shared base properties](docs/widgets/shared-properties.md)** — `Visibility`, `IsEnabled`, `RenderOpacity`, `RenderTransform`, `Cursor`, `ToolTipText` (all widgets).

**Widget reference:**

| Widget | File | Key bindings |
|---|---|---|
| `<TextBlock>` | [textblock.md](docs/widgets/textblock.md) | `Text`, `ColorAndOpacity`, `Font.Size`, `Justification` |
| `<Button>` | [button.md](docs/widgets/button.md) | `OnClicked`, `OnPressed`, `OnReleased`, `OnHovered` |
| `<Border>` | [border.md](docs/widgets/border.md) | `BrushColor`, `Padding` |
| `<SizeBox>` | [sizebox.md](docs/widgets/sizebox.md) | `WidthOverride`, `HeightOverride` |
| `<Image>` | [image.md](docs/widgets/image.md) | `Brush.ResourceObject`, `Brush.ImageSize` |
| `<ProgressBar>` | [progressbar.md](docs/widgets/progressbar.md) | `Percent`, `FillColorAndOpacity`, `BarFillType` |
| `<CheckBox>` | [checkbox.md](docs/widgets/checkbox.md) | `IsChecked`, `OnCheckStateChanged` |
| `<Slider>` | [slider.md](docs/widgets/slider.md) | `Value`, `MinValue`, `MaxValue`, `OnValueChanged` |
| `<EditableText>` | [editable-text.md](docs/widgets/editable-text.md) | `Text`, `HintText`, `OnTextChanged` (4 variants) |
| `<ComboBoxString>` | [combobox-string.md](docs/widgets/combobox-string.md) | `DefaultOptions`, `OnSelectionChanged` |
| `<ListView>` / `<TileView>` | [listview.md](docs/widgets/listview.md) | `ListItems`, `EntryWidgetClass` |
| `<Spacer>` | [spacer.md](docs/widgets/spacer.md) | `Size` |
| `<CanvasPanel>` | [canvas-panel.md](docs/widgets/canvas-panel.md) | `Slot.LayoutData.Anchors`, `Slot.LayoutData.Offsets`, `Slot.bAutoSize`, `Slot.ZOrder` |
| `<VerticalBox>` / `<HorizontalBox>` | [vertical-horizontal-box.md](docs/widgets/vertical-horizontal-box.md) | `Slot.Size.SizeRule`, `Slot.Size.Value`, `Slot.Padding`, `Slot.Alignment` |
| `<GridPanel>` | [grid-panel.md](docs/widgets/grid-panel.md) | `Slot.Row`, `Slot.Column`, `Slot.RowSpan`, `Slot.ColumnSpan`, `RowFill`, `ColumnFill` |
| `<UniformGridPanel>` | [uniform-grid-panel.md](docs/widgets/uniform-grid-panel.md) | `Slot.Row`, `Slot.Column` |
| `<WrapBox>` | [wrap-box.md](docs/widgets/wrap-box.md) | `Slot.bFillEmptySpace`, `Slot.bForceNewLine` |
| `<ScrollBox>` | [scroll-box.md](docs/widgets/scroll-box.md) | `Slot.Size.SizeRule`, `Slot.Padding` |
| `<Overlay>` | [overlay.md](docs/widgets/overlay.md) | `Slot.Padding`, `Slot.Alignment` |

> **GridPanel tip:** Use `<GridPanel>` for grid layouts instead of nesting `<HorizontalBox>` inside `<VerticalBox>`. Children **must** use `Slot.Row` and `Slot.Column` (the `Slot.` prefix is required — bare `Row` causes a parse error because attributes are resolved via `FPropertyPathResolver` on the widget, not on the slot). Set `ColumnFill` and `RowFill` as child elements with `<Float>1.0</Float>` entries for equal sizing:
> ```xml
> <GridPanel>
>   <ColumnFill><Float>1.0</Float><Float>1.0</Float></ColumnFill>
>   <RowFill><Float>1.0</Float><Float>1.0</Float></RowFill>
>   <Button Slot.Row="0" Slot.Column="0" OnClicked="press_1">...</Button>
> </GridPanel>
> ```
> **Slot property timing:** Slot properties (e.g. `Slot.Row`, `Slot.Size.SizeRule`) are resolved AFTER `OnAddChild` creates the slot object (`UPanelWidget::AddChild`), so they work for any panel-managed widget. Root widgets (direct children of `<WidgetTree>`) have no slot — do NOT set `Slot.*` on them.
>
> **Slot vs container properties:** Properties on **child** widgets use `Slot.` prefix (they apply to the slot created by the parent panel). Properties on the **panel itself** (e.g. `RowFill`, `ColumnFill`, `bAutoSize`) use bare names — they belong to the container, not the slot.

**Struct types:** [docs/structs/](docs/structs/) — [FLinearColor](docs/structs/linear-color.md), [FSlateColor](docs/structs/slate-color.md), [FVector2D](docs/structs/vector2d.md), [FMargin](docs/structs/margin.md), [FSlateBrush](docs/structs/slate-brush.md), [FWidgetTransform](docs/structs/render-transform.md), [FSlateChildSize](docs/structs/slate-child-size.md), [FAnchors](docs/structs/anchors.md), [FAnchorData](docs/structs/anchor-data.md), [FSlateFontInfo](docs/structs/font-info.md), [ESlateVisibility](docs/structs/slate-visibility.md), [Alignment](docs/structs/alignment.md)

### 2.3 Naming Widgets

Use `Name="WidgetName"` only when you need to access the widget from Python via `get_editor_property()`. Most widgets (buttons, labels) don't need names — data binding handles updates automatically.

```xml
<!-- Name becomes the Blueprint variable name; needed for Python access or style targeting -->
<TextBlock Name="StatusLabel" Text="{status}" />
<Button OnClicked="on_submit">
  <TextBlock Text="Submit" />
</Button>
```

> **Note:** Button labels use a child `<TextBlock>`, not `Text="..."` on the Button itself. Only set `Name` when you need to reference the widget from Python; otherwise let the system auto-generate names.

Prefer data binding over `get_editor_property()` when possible.

## 3. Data Binding

### 3.1 Property Binding

Use `{source_expression}` to bind a widget property to a Python `@reactive` property:

```xml
<TextBlock Text="{display_value}" />
```

When `self.display_value = "new"` is set in Python, the TextBlock's `Text` updates automatically.

> **Important:** The `{expression}` in XML must match the `@reactive` property name **exactly**. Mismatched names cause `'Component' object has no attribute 'xxx'` at runtime.

Flow:
```
Python: self.display_value = "new"
  → @reactive setter → _notify()
    → on_property_changed("display_value", "new")
      → widget_markup.apply_property_binding(widget, binding, value)
        → C++ applies value to UMG property
```

### 3.2 Delegate Binding

Bind UMG widget events to Python methods:

```xml
<Button OnClicked="on_confirm">
  <TextBlock Text="Confirm" />
</Button>
<Button OnClicked="on_cancel">
  <TextBlock Text="Cancel" />
</Button>
```

Python:
```python
def on_confirm(self):
    self.status = "Confirmed"
    self.save_data()

def on_cancel(self):
    self.status = "Cancelled"
    self.close_panel()
```

> **Note:** `@reactive` setters must assign (e.g., `self.display = "0"`), not mutate in-place (no `self.display += "1"`).

### 3.3 Class Path Resolution

Paths starting with `/` are long package paths resolved via UE's mount point system:

| Prefix | Resolves to | Use for |
|---|---|---|
| `/Game/` | Project `Content/` directory | **Your project blueprints** |
| `/WidgetMarkup/` | Plugin `Content/` directory | Plugin samples and test cases |

Examples:
```xml
<!-- Project blueprint: Game/Content/ScientificCalculator.widgetmarkup -->
<ListView EntryWidgetClass="/Game/ScientificCalculator" ... />

<!-- Plugin sample: Game/Plugins/WidgetMarkup/Content/Samples/ListEntry.widgetmarkup -->
<ListView EntryWidgetClass="/WidgetMarkup/Samples/ListEntry" ... />
```

> **Note:** `/WidgetMarkup/` maps to the plugin's Content directory — for project-level `.widgetmarkup` files, always use `/Game/`.

### 3.4 Property Value Types

Literal (non-binding) values are converted by UE's property system:
- `"Hello"` → FText/FString/FName
- `"True"` / `"False"` → bool
- `"3.14"` → float/double
- `"42"` → int32/int64/byte
- `"/Path/To/Class"` → UClass/UObject reference
- Colors: `"1,0,0,1"` → FLinearColor; use `unreal.SlateColor()` in Python bindings
- Structs: `"0,0,100"` → FVector; `"4,4,4,4"` → FMargin

## 4. Python & ListView

See [docs/python-components.md](docs/python-components.md) for the complete Python component development guide and ListView setup.

- **`@reactive`** — settable properties (int, float, str, bool, list) that auto-push to bound widgets
- **`@computed`** — read-only derived properties with automatic dependency tracking
- **`ObservableCollection`** — reactive lists; `append`/`pop`/`clear` auto-trigger ListView refresh
- **ListView/TileView** — entry mixins (`IUserListEntry`, `IUserObjectListEntry`), `on_data_refresh(data)`, delegate binding
- **Accessing widgets** — prefer data binding over `get_editor_property()`

> **Use `typing.Optional` for nullable types:** Always import `Optional` from `typing` for type hints that can be `None`:
> ```python
> from typing import Optional
> def evaluate(expr: str) -> Optional[float]: ...
> ```

## 5. Style Sheets

Style sheets apply runtime property overrides to named widgets. Defined inline or as standalone assets, stored in `UWidgetMarkupBlueprintExtension`.

```xml
<WidgetBlueprint Script="Samples.MyComponent">
  <WidgetTree>
    <TextBlock Name="Title" Text="Default" />
  </WidgetTree>
  <!-- Style overrides are applied after widget construction -->
</WidgetBlueprint>
```

## 6. Build & Run

```powershell
# Build
cd C:\PROJECTS\UnrealEngine
.\Engine\Build\BatchFiles\Build.bat WidgetMarkupApp Win64 Development -Project=Game/Game.uproject

# Run a plugin sample (mounted at /WidgetMarkup/)
.\Game\Binaries\Win64\WidgetMarkupApp.exe /WidgetMarkup/Samples/Counter --project Game/Game.uproject

# Run a project blueprint (mounted at /Game/ — project Content/ directory)
.\Game\Binaries\Win64\WidgetMarkupApp.exe /Game/ScientificCalculator --project Game/Game.uproject

# Clear Python cache before testing (clears BOTH project and plugin paths)
Get-ChildItem <ProjectDir> -Recurse -Directory -Filter "__pycache__" | Remove-Item -Recurse -Force
```

> Always clear caches before testing — stale `.pyc` files can silently load old code.

## 7. Samples

Sample `.widgetmarkup` blueprints and Python components are in the plugin directory:

```
Game/Plugins/WidgetMarkup/Content/Samples/
```

Includes: Counter (increment/decrement), ListView with color-cycled entries, and more. See the samples for complete working examples of all concepts covered in this document.
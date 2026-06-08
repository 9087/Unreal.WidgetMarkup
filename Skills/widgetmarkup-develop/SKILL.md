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

The `Script` attribute follows Python import path convention, e.g. `Samples.MyComponent` maps to `Content/Python/Samples/MyComponent.py`.

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

> **Any UPROPERTY on the UMG class works as an XML attribute** via UE reflection (`FPropertyChainHandle`).

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
| `<CanvasPanel>` | [canvas-panel.md](docs/widgets/canvas-panel.md) | `LayoutData.Anchors`, `LayoutData.Offsets`, `bAutoSize`, `ZOrder` |
| `<VerticalBox>` / `<HorizontalBox>` | [vertical-horizontal-box.md](docs/widgets/vertical-horizontal-box.md) | `Size.SizeRule`, `Size.Value`, `Padding`, `Alignment` |
| `<GridPanel>` | [grid-panel.md](docs/widgets/grid-panel.md) | `Row`, `Column`, `RowSpan`, `ColumnSpan`, `RowFill`, `ColumnFill` |
| `<UniformGridPanel>` | [uniform-grid-panel.md](docs/widgets/uniform-grid-panel.md) | `Row`, `Column` |
| `<WrapBox>` | [wrap-box.md](docs/widgets/wrap-box.md) | `bFillEmptySpace`, `bForceNewLine` |
| `<ScrollBox>` | [scroll-box.md](docs/widgets/scroll-box.md) | `Size.SizeRule`, `Padding` |
| `<Overlay>` | [overlay.md](docs/widgets/overlay.md) | `Padding`, `Alignment` |

**Struct types:** [docs/structs/](docs/structs/) — [FLinearColor](docs/structs/linear-color.md), [FSlateColor](docs/structs/slate-color.md), [FVector2D](docs/structs/vector2d.md), [FMargin](docs/structs/margin.md), [FSlateBrush](docs/structs/slate-brush.md), [FWidgetTransform](docs/structs/render-transform.md), [FSlateChildSize](docs/structs/slate-child-size.md), [FAnchors](docs/structs/anchors.md), [FAnchorData](docs/structs/anchor-data.md), [FSlateFontInfo](docs/structs/font-info.md), [ESlateVisibility](docs/structs/slate-visibility.md), [Alignment](docs/structs/alignment.md)

### 2.3 Naming Widgets

Use `Name="WidgetName"` only when you need to access the widget from Python via `get_editor_property()`. Most widgets (buttons, labels) don't need names — data binding handles updates automatically.

```xml
<!-- Named: accessed from Python -->
<TextBlock Name="DisplayLabel" Text="{display}" />

<!-- Unnamed: data-bound or purely interactive -->
<Button OnClicked="on_digit" Text="7" />
```

Prefer data binding over `get_editor_property()` when possible.

## 3. Data Binding

### 3.1 Property Binding

Use `{source_expression}` to bind a widget property to a Python `@reactive` property:

```xml
<TextBlock Text="{display_value}" />
```

When `self.display_value = "new"` is set in Python, the TextBlock's `Text` updates automatically.

Flow:
```
Python: self.display_value = "new"
  → @reactive setter → _notify()
    → on_property_changed("display_value", "new")
      → widget_markup.apply_property_binding(widget, binding, value)
        → C++ applies value to UMG property
```

### 3.2 Delegate Binding

Bind UMG widget events to Python methods. Each button can have its own handler:

```xml
<Button OnClicked="on_add" Text="+" />
<Button OnClicked="on_subtract" Text="-" />
<Button OnClicked="on_clear" Text="C" />
```

Python:
```python
def on_add(self):
    self.operator = "+"
    self.operand1 = float(self.display)
    self.display = "0"

def on_clear(self):
    self.display = "0"
    self.operand1 = 0.0
    self.operator = ""
```

> **Note:** `@reactive` setters must assign (e.g., `self.display = "0"`), not mutate in-place (no `self.display += "1"`).

### 3.3 Class Path Resolution

Use `EntryWidgetClass="/WidgetMarkup/Samples/MyEntry"` for referencing other WidgetMarkup blueprints. Paths starting with `/` are resolved via the WidgetMarkup module's object registry and compiled on-demand.

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

# Run a sample
.\Game\Binaries\Win64\WidgetMarkupApp.exe /WidgetMarkup/Samples/Counter --project Game/Game.uproject

# Clear Python cache before testing
Get-ChildItem Game/Plugins/WidgetMarkup/Content/Python -Recurse -Dir __pycache__ | Remove-Item -Recurse -Force
```

## 7. Samples

Sample `.widgetmarkup` blueprints and Python components are in the plugin directory:

```
Game/Plugins/WidgetMarkup/Content/WidgetMarkup/Samples/
```

Includes: Counter (increment/decrement), ListView with color-cycled entries, and more. See the samples for complete working examples of all concepts covered in this document.


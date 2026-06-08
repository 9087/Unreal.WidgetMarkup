# Python Components

## Basic Component

```python
from WidgetMarkupComponent import WidgetMarkupComponent, reactive

class MyComponent(WidgetMarkupComponent):
    @reactive
    def count(self):
        return 0

    def on_button_clicked(self):
        self.count += 1
```

## Reactive Properties

`@reactive` wraps a method in `_ReactiveProperty`, a descriptor that stores values in per-instance state. The method body serves as the **default value** — called by `__get__` when no value has been explicitly set.

**The getter should return a plain constant**, not read other attributes. The descriptor itself is the store — `__set__` writes to internal state, `__get__` reads from it. If you need a value derived from other reactive properties, use `@computed` instead.

```python
@reactive
def label(self):
    return ""          # Plain default — the descriptor IS the store

# Assignment calls _ReactiveProperty.__set__:
self.label = "Hello"  # stores in state, triggers notify → apply_property_binding
```

**Important**: `@reactive` does not support the `.setter` decorator pattern. Writing `@label.setter` will crash with `'_ReactiveProperty' object has no attribute 'setter'`. Assign directly: `self.label = value`.

**Anti-pattern** — the getter should not reference instance fields:
```python
# Incorrect: this is what @computed is for
@reactive
def label(self):
    return self._label  # depends on another attribute
```

**Usage tips**:
- Keep internal state in plain instance attributes (for example `self._state`), not in reactive getters. Update the reactive property via assignment when the UI needs to reflect a change.
- When the property changes, `on_property_changed()` triggers `apply_property_binding`. The binding pipeline converts the Python value to the target UMG property type.

## Multi-Property State

Reactive properties can interact — changing one can trigger updates to others via `@computed`.

```python
class CalcComponent(WidgetMarkupComponent):
    @reactive
    def display(self):
        return "0"

    @reactive
    def operand1(self):
        return 0.0

    @reactive
    def operator(self):
        return ""

    @computed
    def result_text(self):
        """Auto-updates whenever display, operand1, or operator changes."""
        if self.operator and self.display:
            return f"{self.operand1} {self.operator} {self.display}"
        return self.display

    def on_digit(self, digit):
        if self.display == "0":
            self.display = str(digit)
        else:
            self.display += str(digit)
```

```xml
<TextBlock Text="{result_text}" Font.Size="32" Justification="Right" />
```

## Reactive Lists (ObservableCollection)

`@reactive` properties returning a `list` are auto-wrapped in `ObservableCollection`. Mutations (`append`, `pop`, `clear`, `insert`, `remove`) automatically trigger binding refresh:

```python
@reactive
def items(self):
    return []

def add_item(self):
    self.items.append(42)  # Auto-triggers ListView refresh
```

No need for `self.items = list(self.items)` — the ObservableCollection handles notification internally.

## Computed Properties

`@computed` creates a read-only property whose value is computed from other reactive properties. Dependencies are tracked automatically:

```python
@computed
def full_name(self):
    return f"{self.first_name} {self.last_name}"
```

## Accessing the User Widget

The owning `UUserWidget` is available as `self._widget_markup_user_widget`, but **prefer data binding whenever possible.** Only fall back to `get_editor_property()` when binding can't express the desired behavior:

```python
# Prefer data binding (automatic, reactive):
#   <TextBlock Text="{display_text}" />

# Only when binding is impractical:
label = self._widget_markup_user_widget.get_editor_property('MyLabel')
```

---

## Property Bindings (XML Syntax)

Reactive properties connect to UMG widgets via the `{propertyName}` binding syntax in XML attributes:

```xml
<TextBlock Name="Display" Text="{display}" />
<ProgressBar Name="Progress" Percent="{progress}" />
<CheckBox IsChecked="{checked}" />
```

- `{display}` → binding to `self.display` reactive property
- `"0"` → literal string value, no binding
- `{}literal` → escape `{` for literal text starting with `{`

Bindings are parsed during `apply_property_bindings()` in `WidgetMarkupComponent.__init__()`. The binding system matches `source_expression` against `@reactive` property names to push updates to target widget properties.

## Handler Dispatch

Button clicks are routed by **method name** on the Python component:

```xml
<Button OnClicked="on_digit_1"><TextBlock Text="1" /></Button>
```
→ calls `component.on_digit_1()`

Each button needs an individual handler method. The framework does not support tag-based dispatch or generic handler routing.

## Init Order

If `@reactive` getters return plain constants (as recommended), `super().__init__()` can go anywhere. The only ordering constraint exists when a getter reads an instance field — set those fields first:

```python
class MyComponent(WidgetMarkupComponent):
    @reactive
    def label(self):
        return self._data.get("label", "untitled")  # reads instance field

    def __init__(self):
        self._data = {"label": "Hello"}   # must come before super().__init__()
        super().__init__()                # triggers apply_property_bindings
```

Better: avoid this entirely by keeping getters to plain defaults and managing state separately.

## Module Import & Resolve Order

WidgetMarkup resolves `Script="ModuleName"` to a Python module import. `Game/Content/Python/` is on `sys.path`:

```
File location                    → Script attribute
Game/Content/Python/MyComponent.py    → Script="MyComponent"
Plugin/Content/Python/Samples/X  → Script="Samples.X"
```

Do not wrap the import in try/except — if the import fails, it should crash visibly:

```python
from WidgetMarkupComponent import WidgetMarkupComponent, reactive
# Do not:
# try:
#     from WidgetMarkupComponent import ...
# except ImportError:
#     ...  # silent fallback hides real errors
```

---

## Common Patterns

### Delegating to Private Helpers

Since each button maps to a named handler, delegate to private methods to reduce boilerplate:

```python
def on_color_red(self):   self._set_color("red")
def on_color_blue(self):  self._set_color("blue")
def on_color_green(self): self._set_color("green")

def _set_color(self, name: str):
    self.selected_color = name
```

This pattern is useful whenever multiple buttons share similar logic but need distinct handler names in XML.

## List View Setup

The list (parent) blueprint declares a `ListView` with `ListItems` binding and `EntryWidgetClass`:

```xml
<WidgetBlueprint Script="Samples.ListComponent">
  <WidgetTree>
    <CanvasPanel>
      <ListView ListItems="{items}"
                EntryWidgetClass="/WidgetMarkup/Samples/ListEntry" />
    </CanvasPanel>
  </WidgetTree>
</WidgetBlueprint>
```

## Entry Widget

The entry blueprint is a separate `.widgetmarkup` file:

```xml
<WidgetBlueprint Script="Samples.ListEntryComponent">
  <WidgetTree>
    <TextBlock Name="ItemLabel" Text="{display_text}" ColorAndOpacity="{color}" />
  </WidgetTree>
</WidgetBlueprint>
```

## Entry Python Component

```python
from WidgetMarkupComponent import WidgetMarkupComponent, reactive
from IUserObjectListEntry import IUserObjectListEntry

class ListEntryComponent(WidgetMarkupComponent, IUserObjectListEntry):
    @reactive
    def display_text(self):
        return ""

    @reactive
    def color(self):
        return unreal.SlateColor()

    def on_data_refresh(self, data):
        """data is the raw Python value from the list."""
        self.display_text = str(data)
        self.color = _COLORS[data % len(_COLORS)]

    def on_entry_released(self):
        """Entry recycled — cleanup here."""
        pass
```

## Interface Hierarchy

| Mixin | Provides |
|---|---|
| `IUserListEntry` | `on_entry_released()`, `on_item_selection_changed(b)`, `on_item_expansion_changed(b)`, `is_selected()`, `get_owning_list_view()` |
| `IUserObjectListEntry(IUserListEntry)` | `get_list_item()` (returns raw Python value) |

`on_data_refresh(data)` is defined in `WidgetMarkupComponent` — override it to receive list item data.

The Python mixin auto-binds to `UWidgetMarkupUserWidget`'s multicast delegates in `__init__`.

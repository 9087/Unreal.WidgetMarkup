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

`@reactive` creates a settable property that automatically pushes changes to bound UMG widgets. Works with `int`, `float`, `str`, `bool`, `list`, and `unreal.SlateColor`.

```python
@reactive
def display(self):
    return ""          # String: use for TextBlock.Text

@reactive
def progress(self):
    return 0.0         # Float: use for ProgressBar.Percent

@reactive
def checked(self):
    return False       # Bool: use for CheckBox.IsChecked
```

When the property changes, `on_property_changed()` triggers `apply_property_binding`. The binding pipeline converts the Python value to the target UMG property type.

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

# ListView & Entry Widgets

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

The entry blueprint is a separate `.unrealwidgetmarkup` file:

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

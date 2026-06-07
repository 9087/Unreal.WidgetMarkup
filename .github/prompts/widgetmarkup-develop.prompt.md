---
name: widgetmarkup-develop
user-invocable: true
description: 'WidgetMarkup is an Unreal Engine 5 plugin providing a declarative XML-based UI framework with Python scripting. Use this skill when the user wants to: write .unrealwidgetmarkup XML files to define UMG widget blueprints; write Python components (WidgetMarkupComponent subclasses) for UI logic; set up reactive data bindings between Python properties and UMG widgets; create ListView/TileView entries with IUserObjectListEntry/IUserListEntry mixins; define style sheets or delegate bindings; debug WidgetMarkup compilation or binding issues. When NOT to use: general UE5 C++ development, Blueprint scripting, standard UMG widget coding outside WidgetMarkup.'
---

# WidgetMarkup Development

## XML Blueprint (*.unrealwidgetmarkup)

```xml
<WidgetBlueprint Script="Samples.MyComponent">
  <WidgetTree>
    <CanvasPanel>
      <!-- Property binding: {source_expression} links to @reactive Python property -->
      <TextBlock Name="MyLabel" Text="{my_value}" />
      <!-- Delegate binding: OnClicked="method_name" links to Python method -->
      <Button Name="MyButton" OnClicked="on_clicked">
        <TextBlock Text="Click Me" />
      </Button>
      <!-- ListView with entry widget -->
      <ListView ListItems="{items}"
                EntryWidgetClass="/WidgetMarkup/Samples/MyEntry" />
    </CanvasPanel>
  </WidgetTree>
</WidgetBlueprint>
```

### Attribute Patterns

| Pattern | Example | Description |
|---|---|---|
| `Property="{expr}"` | `Text="{count}"` | Reactive property binding |
| `Property="value"` | `Text="Hello"` | Literal value via ClassConverter |
| `Name="WidgetName"` | `Name="MyButton"` | Names widget, creates Blueprint variable |
| `OnClicked="method"` | `OnClicked="increment"` | Delegate binding to Python method |
| `EntryWidgetClass="/Path"` | `EntryWidgetClass="/WidgetMarkup/Samples/MyEntry"` | ListView entry blueprint path |

## Python Component

```python
from WidgetMarkupComponent import WidgetMarkupComponent, reactive

class MyComponent(WidgetMarkupComponent):
    # Reactive property: auto-pushes to bound widgets when changed
    @reactive
    def my_value(self):
        return "default"

    def on_clicked(self):
        self.my_value = "clicked!"
```

### List Entry Component

```python
from WidgetMarkupComponent import WidgetMarkupComponent, reactive
from IUserObjectListEntry import IUserObjectListEntry
from IUserObjectListEntry import IUserObjectListEntry

class MyEntry(WidgetMarkupComponent, IUserObjectListEntry):
    @reactive
    def display_text(self):
        return ""

    def on_data_refresh(self, data):
        """Called when list item is assigned. data is the raw Python value."""
        self.display_text = str(data)

    def on_entry_released(self):
        """Called when entry is recycled."""
        pass
```

### ObservableCollection for List Items

```python
@reactive
def items(self):
    return []  # Auto-wrapped in ObservableCollection

def add_item(self):
    self.items.append(value)  # append/pop/clear auto-trigger binding refresh
```

## Common Patterns

### Color Binding
Use `unreal.SlateColor` (not bare `unreal.LinearColor`):
```python
_COLORS = [
    unreal.SlateColor(unreal.LinearColor(1, 0, 0, 1)),  # red
    unreal.SlateColor(unreal.LinearColor(0, 0, 1, 1)),  # blue
]

# In XML: <TextBlock ColorAndOpacity="{color}" />
```

### Accessing Blueprint-Created Widget Variables
Widgets with `Name="X"` create Blueprint variables accessible via:
```python
label = self._widget_markup_user_widget.get_editor_property('X')
```
Prefer data binding over direct widget access when possible.

## Build & Run

```powershell
# Build
cd C:\PROJECTS\UnrealEngine
.\Engine\Build\BatchFiles\Build.bat WidgetMarkupApp Win64 Development -Project=Game/Game.uproject

# Run
.\Game\Binaries\Win64\WidgetMarkupApp.exe /WidgetMarkup/Samples/List --project Game/Game.uproject

# Clear Python cache
Get-ChildItem Game/Plugins/WidgetMarkup/Content/Python -Recurse -Dir __pycache__ | Remove-Item -Recurse -Force
```

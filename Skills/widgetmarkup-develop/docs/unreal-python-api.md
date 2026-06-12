# unreal Python API (WidgetMarkupApp)

WidgetMarkup components run inside **WidgetMarkupApp**, which exposes a subset of UE's Python bindings. Names and availability differ from the full Editor — do not assume Editor documentation applies verbatim.

## ScriptName vs C++ class name

Blueprint function libraries are exported under their `ScriptName` meta, **not** the C++ class name:

| C++ class | Python module on `unreal` |
|---|---|
| `UKismetInputLibrary` | `unreal.InputLibrary` |
| `UWidgetBlueprintLibrary` | `unreal.WidgetLibrary` |
| `UKismetSystemLibrary` | `unreal.SystemLibrary` |

`unreal.KismetInputLibrary` and `unreal.WidgetBlueprintLibrary` are **`None`** in WidgetMarkupApp. `unreal.load_class(None, "/Script/Engine.KismetInputLibrary")` returns a `UClass` but does **not** expose static UFUNCTIONs as Python methods — use the `ScriptName` entry on `unreal` instead.

## PointerEvent — no direct field access

`unreal.PointerEvent` is passed to `OnMouseButtonDownEvent` handlers, but **`effecting_button` is not a readable Python property** (the underlying `FPointerEvent::EffectingButton` member is not exported to reflection).

Use `InputLibrary`:

```python
button_key = unreal.InputLibrary.pointer_event_get_effecting_button(mouse_event)
button_name = str(button_key.key_name)  # e.g. "LeftMouseButton", "RightMouseButton"
```

Do **not** use `unreal.KismetInputLibrary`, `widget_markup.pointer_event_get_effecting_button`, or `mouse_event.get_editor_property("effecting_button")`.

## FKey — no `LEFT_MOUSE_BUTTON` constants

`unreal.Key.LEFT_MOUSE_BUTTON` / `RIGHT_MOUSE_BUTTON` are **not** available in WidgetMarkupApp. Compare the string from `button_key.key_name`, or use `InputLibrary.equal_equal_key_key(a, b)` when you have two `FKey` values.

## WidgetLibrary for event replies

Return a handled reply from pointer-event handlers:

```python
return unreal.WidgetLibrary.handled()
```

Not `unreal.WidgetBlueprintLibrary.handled()`.

## Border: distinguishing mouse buttons

`Button` only supports `OnClicked` (primary button). For distinct left/right actions, use `Border` + `OnMouseButtonDownEvent`:

```xml
<Border Name="ItemTile" OnMouseButtonDownEvent="on_item_tile_mouse_down"
        BrushColor="0.2,0.2,0.22,1" Padding="8,8,8,8">
  <TextBlock Name="ItemTileLabel" Text="{item_label}" Visibility="HitTestInvisible"
             Font.Size="14" Justification="Center" />
</Border>
```

Set decorative child text to `Visibility="HitTestInvisible"` so it does not steal hit tests from the `Border`.

```python
def on_item_tile_mouse_down(self, geometry: unreal.Geometry, mouse_event: unreal.PointerEvent):
    button_name = str(unreal.InputLibrary.pointer_event_get_effecting_button(mouse_event).key_name)
    if button_name == "RightMouseButton":
        self.open_item_context_menu()
    elif button_name == "LeftMouseButton":
        self.select_item()
    return unreal.WidgetLibrary.handled()
```

`OnMouseButtonDownEvent` is a **single-cast** delegate (`bind_callable`); `OnClicked` on `Button` is multicast (`add_callable`).

## widget_markup native module

Project-specific helpers only:

| Function | Purpose |
|---|---|
| `widget_markup.find_widget_in_user_widget(user_widget, name)` | Find a named widget in the tree |
| `widget_markup.apply_property_binding(...)` | Internal binding pipeline |
| `widget_markup.request_shutdown()` | Exit WidgetMarkupApp (tests) |

Do not add C++ wrappers for APIs already on `unreal.InputLibrary` / `unreal.WidgetLibrary`.

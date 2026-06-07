# Button

`UButton : UContentWidget` — Clickable button with a single child.

Inherits from: [shared-properties.md](shared-properties.md)

| Attribute | Type | Description |
|---|---|---|
| `ColorAndOpacity` | [FLinearColor](../structs/linear-color.md) | Content color multiplier |
| `BackgroundColor` | [FLinearColor](../structs/linear-color.md) | Background color multiplier |
| `ClickMethod` | `EButtonClickMethod` | `DownAndUp`, `MouseDown`, `MouseUp`, `PreciseClick` |
| `TouchMethod` | `EButtonTouchMethod` | `DownAndUp`, `Down`, `PreciseTap` |
| `PressMethod` | `EButtonPressMethod` | `DownAndUp`, `ButtonPress`, `ButtonRelease` |
| `IsFocusable` | `bool` | Keyboard focusable |

**Delegates:**

| Delegate | Signature |
|---|---|
| `OnClicked` | `()` |
| `OnPressed` | `()` |
| `OnReleased` | `()` |
| `OnHovered` | `()` |
| `OnUnhovered` | `()` |

```xml
<Button Name="MyButton" OnClicked="on_button_clicked" OnHovered="on_hover">
  <TextBlock Text="Click Me" />
</Button>
```

**Slot:** Padding ([FMargin](../structs/margin.md)), [Alignment](../structs/alignment.md) — see [panels.md](panels.md#border--button--backgroundblur).

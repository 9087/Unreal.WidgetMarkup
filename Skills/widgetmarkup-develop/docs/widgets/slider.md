# Slider

`USlider : UWidget` — Numeric slider. No children.

Inherits from: [shared-properties.md](shared-properties.md)

| Attribute | Type | Description |
|---|---|---|
| `Value` | `float` | Current value, supports `{binding}` |
| `MinValue` | `float` | Minimum value |
| `MaxValue` | `float` | Maximum value |
| `StepSize` | `float` | Keyboard/controller step |
| `Orientation` | `EOrientation` | `Orient_Horizontal`, `Orient_Vertical` |
| `SliderBarColor` | [FLinearColor](../structs/linear-color.md) | Bar color |
| `SliderHandleColor` | [FLinearColor](../structs/linear-color.md) | Handle color |
| `Locked` | `bool` | Disable interaction |
| `IndentHandle` | `bool` | Indent for handle fit |

**Delegate:**

| Delegate | Signature |
|---|---|
| `OnValueChanged` | `(float Value)` |

```xml
<Slider Value="{volume}" MinValue="0.0" MaxValue="1.0" StepSize="0.05"
        OnValueChanged="on_volume_changed" />
```

# ProgressBar

`UProgressBar : UWidget` — Fill bar 0..1. No children.

Inherits from: [shared-properties.md](shared-properties.md)

| Attribute | Type | Description |
|---|---|---|
| `Percent` | `float` | Fill value 0.0–1.0, supports `{binding}` |
| `FillColorAndOpacity` | [FLinearColor](../structs/linear-color.md) | Fill color |
| `BarFillType` | `EProgressBarFillType` | `LeftToRight`, `RightToLeft`, `TopToBottom`, `BottomToTop` |
| `BarFillStyle` | `EProgressBarFillStyle` | `Mask`, `Scale` |
| `bIsMarquee` | `bool` | Indeterminate marquee animation |
| `BorderPadding` | [FVector2D](../structs/vector2d.md) | Inner padding |

```xml
<ProgressBar Percent="{progress}" FillColorAndOpacity="{bar_color}" />
```

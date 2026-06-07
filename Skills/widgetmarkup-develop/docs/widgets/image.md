# Image

`UImage : UWidget` — Displays a Slate brush, texture, or material. No children.

Inherits from: [shared-properties.md](shared-properties.md)

See [FSlateBrush](../structs/slate-brush.md) for brush attribute details.

| Attribute | Type | Description |
|---|---|---|
| `Brush.ResourceObject` | asset path | Texture2D, Material, etc. |
| `Brush.ImageSize` | [FVector2D](../structs/vector2d.md) | `"128,128"` |
| `Brush.TintColor` | [FSlateColor](../structs/slate-color.md) | Tint color |
| `Brush.DrawAs` | `ESlateBrushDrawType` | `NoDrawType`, `Box`, `Border`, `Image`, `RoundedBox` |
| `Brush.Margin` | [FMargin](../structs/margin.md) | 9-slice margins |
| `ColorAndOpacity` | [FLinearColor](../structs/linear-color.md) | Color multiplier |

```xml
<Image Brush.ResourceObject="/Game/Textures/MyIcon.MyIcon"
       Brush.ImageSize="64,64" ColorAndOpacity="{tint}" />
```

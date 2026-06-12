# FSlateBrush

Describes how to draw a widget background/image: what texture/material, how to stretch it, tint color, and margins.

**Attributes (set via dotted paths like `Brush.ResourceObject`):**

| Path | Type | Description |
|---|---|---|
| `Brush.ResourceObject` | asset path | Texture2D, Material, or other UObject |
| `Brush.ImageSize` | `"W,H"` | Desired image size in pixels ([FVector2D](vector2d.md)) |
| `Brush.TintColor` | `"R,G,B,A"` | Tint color ([FSlateColor](slate-color.md)) |
| `Brush.DrawAs` | enum | `NoDrawType`, `Box`, `Border`, `Image` (default), `RoundedBox` |
| `Brush.Margin` | `"L,T,R,B"` | 9-slice margins ([FMargin](margin.md)) |

```xml
<Image Brush.ResourceObject="/Game/Textures/MyIcon.MyIcon"
       Brush.ImageSize="64,64"
       Brush.TintColor="1,1,1,1" />
```

Used by: [Image](../widgets/image.md) (`Brush`), [Border](../widgets/border.md) (`Background`).

> **XML nesting:** Child-element syntax (`<Brush><ResourceObject>/Game/Textures/Icon</ResourceObject></Brush>`) is supported via `FPropertyPathResolver`. Dotted attributes (`Brush.ResourceObject="..."`) are typically more concise.

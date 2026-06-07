# FWidgetTransform (RenderTransform)

Applies a 2D transform to any widget. Available on all widgets via `RenderTransform.*` dotted paths.

| Path | Type | Default | Description |
|---|---|---|---|
| `RenderTransform.Translation` | `"X,Y"` | `"0,0"` | Position offset ([FVector2D](vector2d.md)) |
| `RenderTransform.Scale` | `"X,Y"` | `"1,1"` | Scale multiplier ([FVector2D](vector2d.md)) |
| `RenderTransform.Shear` | `"X,Y"` | `"0,0"` | Shear/skew ([FVector2D](vector2d.md)) |
| `RenderTransform.Angle` | `float` | `0` | Rotation in degrees |
| `RenderTransformPivot` | `"X,Y"` | `"0.5,0.5"` | Transform origin (0–1) ([FVector2D](vector2d.md)) |

```xml
<TextBlock Text="Rotated"
           RenderTransform.Angle="45"
           RenderTransformPivot="0.5,0.5" />
```

> **Note:** `RenderTransform` is purely visual — it does not affect layout. The widget's slot still occupies its original position.

> **XML nesting:** TODO. Nested `<RenderTransform Translation="10,0" />` as a child element is not supported. Use dotted attributes (`RenderTransform.Translation="10,0"`).

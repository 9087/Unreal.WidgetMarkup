# Shared Widget Properties

All widgets inherit from `UWidget`. These properties are available on every widget.

| Attribute | Type | Description |
|---|---|---|
| `Visibility` | [ESlateVisibility](../structs/slate-visibility.md) | `Visible`, `Collapsed`, `Hidden`, `HitTestInvisible`, `SelfHitTestInvisible` |
| `IsEnabled` | `bool` | Enable/disable interaction |
| `RenderOpacity` | `float` | 0.0–1.0 opacity |
| `Cursor` | `EMouseCursor` | `Default`, `Hand`, `TextEditBeam`, etc. |
| `ToolTipText` | `FText` | Tooltip text |

**RenderTransform** — See [FWidgetTransform](../structs/render-transform.md) for full details. All widgets support these dotted paths:

| Path | Type | Description |
|---|---|---|
| `RenderTransform.Translation` | [FVector2D](../structs/vector2d.md) | Position offset |
| `RenderTransform.Scale` | [FVector2D](../structs/vector2d.md) | Scale multiplier |
| `RenderTransform.Shear` | [FVector2D](../structs/vector2d.md) | Shear/skew |
| `RenderTransform.Angle` | `float` | Rotation in degrees |
| `RenderTransformPivot` | [FVector2D](../structs/vector2d.md) | Transform origin (0–1) |

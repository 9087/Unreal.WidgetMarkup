# FAnchors

Defines how a CanvasPanel slot anchors to its parent. Anchors are normalized 0–1 coordinates.

| Path | Type | Description |
|---|---|---|
| `LayoutData.Anchors.Minimum` | `"X,Y"` | Upper-left anchor point |
| `LayoutData.Anchors.Maximum` | `"X,Y"` | Lower-right anchor point |

| Values | Behavior |
|---|---|
| `"0,0"` / `"0,0"` | Anchored to top-left corner |
| `"0.5,0.5"` / `"0.5,0.5"` | Anchored to center |
| `"0,0"` / `"1,1"` | Stretched to fill parent |
| `"0,0"` / `"0,1"` | Stretched vertically, pinned left |

When Min == Max, the slot has a fixed anchor point. When Min ≠ Max, the slot stretches with the parent and the `Offsets` margins control the distance from each anchor edge.

See also: [FAnchorData](anchor-data.md), [CanvasPanel](../widgets/panels.md#canvaspanel)

> **XML nesting:** TODO. Nested `<Anchors Minimum="0,0" Maximum="1,1" />` is not supported. Use dotted attributes (`LayoutData.Anchors.Minimum="0,0"`).

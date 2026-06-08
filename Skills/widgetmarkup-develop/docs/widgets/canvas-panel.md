# CanvasPanel

Free-form positioning via `UCanvasPanelSlot`.

| Slot Property | Type | Description |
|---|---|---|
| `LayoutData.Anchors.Minimum` | `FVector2D` | Anchor corner (0–1) |
| `LayoutData.Anchors.Maximum` | `FVector2D` | Anchor corner (0–1) |
| `LayoutData.Alignment` | `FVector2D` | Pivot alignment (0–1) |
| `LayoutData.Offsets` | `FMargin` | Position X, Y, Width, Height in pixels |
| `bAutoSize` | `bool` | Slot size follows child's desired size (editor: "Size To Content") |
| `ZOrder` | `int32` | Render order, higher = on top |

```xml
<CanvasPanel>
  <TextBlock Text="Top Left"
    Slot.LayoutData.Anchors.Minimum="0,0"
    Slot.LayoutData.Anchors.Maximum="0,0"
    Slot.LayoutData.Alignment="0,0"
    Slot.LayoutData.Offsets="10,10,100,20" />
  <TextBlock Text="Auto-sized">
    <Slot bAutoSize="True"
          LayoutData.Anchors.Minimum="0.5,0.5"
          LayoutData.Anchors.Maximum="0.5,0.5"
          LayoutData.Alignment="0.5,0.5" />
  </TextBlock>
</CanvasPanel>
```

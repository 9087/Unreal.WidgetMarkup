# Panel Widgets & Slot Mechanism

Panel widgets contain multiple children. Each child gets a **Slot** that controls its layout, set via a `<Slot>` child element.

> **Auto-sizing pattern:** `bAutoSize="True"` on CanvasPanel; `Size.SizeRule="Automatic"` on VerticalBox/HorizontalBox/ScrollBox/StackBox. Slot dimensions follow child content instead of a fixed value.

> **Struct references:** [FMargin](../structs/margin.md) · [FVector2D](../structs/vector2d.md) · [FAnchors](../structs/anchors.md) · [FAnchorData](../structs/anchor-data.md) · [FSlateChildSize](../structs/slate-child-size.md) · [Alignment](../structs/alignment.md)

---

## CanvasPanel

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

---

## VerticalBox / HorizontalBox

Stack children vertically/horizontally via `UVerticalBoxSlot` / `UHorizontalBoxSlot`.

| Slot Property | Type | Description |
|---|---|---|
| `Size.Value` | `float` | Size when SizeRule is `Fill` |
| `Size.SizeRule` | `ESlateSizeRule` | `Automatic` (follow content) or `Fill` (use Value) |
| `Padding` | `FMargin` | `"Left,Top,Right,Bottom"` |
| `HorizontalAlignment` | `EHorizontalAlignment` | `Left`, `Center`, `Right`, `Fill` |
| `VerticalAlignment` | `EVerticalAlignment` | `Top`, `Center`, `Bottom`, `Fill` |

```xml
<VerticalBox>
  <TextBlock Text="Auto height">
    <Slot Size.SizeRule="Automatic" />
  </TextBlock>
  <TextBlock Text="Fill remainder">
    <Slot Size.SizeRule="Fill" Size.Value="1"
          Padding="4,2,4,2"
          HorizontalAlignment="Center" />
  </TextBlock>
</VerticalBox>
```

---

## GridPanel

Grid layout via `UGridSlot`.

| Slot Property | Type | Description |
|---|---|---|
| `Row` | `int32` | Zero-based row index |
| `Column` | `int32` | Zero-based column index |
| `RowSpan` | `int32` | Number of rows to span |
| `ColumnSpan` | `int32` | Number of columns to span |
| `Layer` | `int32` | Z-order layer within grid |
| `Nudge` | `FVector2D` | Pixel offset from cell position |
| `Padding` | `FMargin` | Internal spacing |
| `HorizontalAlignment` | `EHorizontalAlignment` | Content H-align |
| `VerticalAlignment` | `EVerticalAlignment` | Content V-align |

```xml
<GridPanel>
  <TextBlock Text="(0,0)">
    <Slot Row="0" Column="0" />
  </TextBlock>
  <TextBlock Text="Spans 2 cols">
    <Slot Row="0" Column="1" ColumnSpan="2" />
  </TextBlock>
</GridPanel>
```

---

## UniformGridPanel

Evenly-sized grid cells via `UUniformGridSlot`.

| Slot Property | Type | Description |
|---|---|---|
| `Row` | `int32` | Zero-based row index |
| `Column` | `int32` | Zero-based column index |
| `HorizontalAlignment` | `EHorizontalAlignment` | Content H-align |
| `VerticalAlignment` | `EVerticalAlignment` | Content V-align |

---

## WrapBox

Auto-wrapping flow layout via `UWrapBoxSlot`.

| Slot Property | Type | Description |
|---|---|---|
| `Padding` | `FMargin` | Internal spacing |
| `bFillEmptySpace` | `bool` | Fill remaining space on the line |
| `bForceNewLine` | `bool` | Start on a new line before this child |
| `FillSpanWhenLessThan` | `float` | Fill entire line if available space drops below this threshold (0 = never) |
| `HorizontalAlignment` | `EHorizontalAlignment` | Content H-align |
| `VerticalAlignment` | `EVerticalAlignment` | Content V-align |

---

## ScrollBox

Scrollable container via `UScrollBoxSlot`.

| Slot Property | Type | Description |
|---|---|---|
| `Size.SizeRule` | `ESlateSizeRule` | `Automatic` or `Fill` |
| `Size.Value` | `float` | Size when Fill |
| `Padding` | `FMargin` | Internal spacing |
| `HorizontalAlignment` | `EHorizontalAlignment` | Content H-align |
| `VerticalAlignment` | `EVerticalAlignment` | Content V-align |

---

## Overlay

Layers children via `UOverlaySlot`.

| Slot Property | Type | Description |
|---|---|---|
| `Padding` | `FMargin` | Internal spacing |
| `HorizontalAlignment` | `EHorizontalAlignment` | Content H-align |
| `VerticalAlignment` | `EVerticalAlignment` | Content V-align |

---

## Border / Button / BackgroundBlur

Single-child container slots (`UBorderSlot` / `UButtonSlot` / `UBackgroundBlurSlot`).

| Slot Property | Type | Description |
|---|---|---|
| `Padding` | `FMargin` | Internal spacing |
| `HorizontalAlignment` | `EHorizontalAlignment` | Content H-align |
| `VerticalAlignment` | `EVerticalAlignment` | Content V-align |

---

## SizeBox Slot

`USizeBoxSlot` — handles alignment/padding only; size constraints are widget-level attributes on `<SizeBox>`.

| Slot Property | Type | Description |
|---|---|---|
| `Padding` | `FMargin` | Internal spacing |
| `HorizontalAlignment` | `EHorizontalAlignment` | Content H-align |
| `VerticalAlignment` | `EVerticalAlignment` | Content V-align |

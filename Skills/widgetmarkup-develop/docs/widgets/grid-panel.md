# GridPanel

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

## Star-Sized Rows & Columns

Use `<ColumnFill>` and `<RowFill>` as direct children of `<GridPanel>` to give columns/rows proportional sizing. Each `<Float>` value represents a weight.

```xml
<GridPanel>
  <ColumnFill><Float>1</Float><Float>1</Float><Float>1</Float></ColumnFill>
  <RowFill><Float>1</Float><Float>1</Float><Float>2</Float></RowFill>
  <!-- 3 equal-width columns; row 2 is twice the height of rows 0-1 -->
  <TextBlock Text="Top Left">
    <Slot Row="0" Column="0" />
  </TextBlock>
</GridPanel>
```

> **Important**: The number of `<Float>` elements in `ColumnFill` and `RowFill` must match the actual number of columns and rows used in the grid. If a row index exceeds the `RowFill` count, that row gets a default (non-proportional) size instead of the intended star size. A grid with buttons at rows 0–7 needs exactly 8 `<Float>1</Float>` entries in `RowFill`.

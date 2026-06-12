# VerticalBox / HorizontalBox

Stack children vertically/horizontally via `UVerticalBoxSlot` / `UHorizontalBoxSlot`.

| Slot Property | Type | Description |
|---|---|---|
| `Size.Value` | `float` | Size when SizeRule is `Fill` |
| `Size.SizeRule` | `ESlateSizeRule` | `Automatic` (follow content) or `Fill` (use Value) |
| `Padding` | `FMargin` | `"Left,Top,Right,Bottom"` |
| `HorizontalAlignment` | `EHorizontalAlignment` | `HAlign_Left`, `HAlign_Center`, `HAlign_Right`, `HAlign_Fill` |
| `VerticalAlignment` | `EVerticalAlignment` | `VAlign_Top`, `VAlign_Center`, `VAlign_Bottom`, `VAlign_Fill` |

```xml
<VerticalBox>
  <TextBlock Text="Auto height">
    <Slot Size.SizeRule="Automatic" />
  </TextBlock>
  <TextBlock Text="Fill remainder">
    <Slot Size.SizeRule="Fill" Size.Value="1"
          Padding="4,2,4,2"
          HorizontalAlignment="HAlign_Center" />
  </TextBlock>
</VerticalBox>
```

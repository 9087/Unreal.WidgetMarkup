# FSlateChildSize

Controls how much space a slot occupies in the direction of its parent panel. Used by VerticalBox, HorizontalBox, ScrollBox, and StackBox slots.

| Property | Type | Description |
|---|---|---|
| `Size.SizeRule` | `ESlateSizeRule` | `Automatic` — follow child's desired size; `Fill` — use `Size.Value` |
| `Size.Value` | `float` | Proportional size when `Fill` (e.g., `1` = one share of remaining space) |

```xml
<VerticalBox>
  <!-- Auto height: slot = child height -->
  <TextBlock Text="Header">
    <Slot Size.SizeRule="Automatic" />
  </TextBlock>
  <!-- Fill: takes remaining space -->
  <TextBlock Text="Content">
    <Slot Size.SizeRule="Fill" Size.Value="1" />
  </TextBlock>
</VerticalBox>
```

> **XML nesting:** TODO. Nested `<Size SizeRule="Automatic" />` as a child element is not supported. Use dotted attributes (`Size.SizeRule="Automatic"`).

# EHorizontalAlignment / EVerticalAlignment

Control how content aligns within its slot.

**EHorizontalAlignment:**

| Value | Description |
|---|---|
| `Left` | Align to left edge |
| `Center` | Center horizontally |
| `Right` | Align to right edge |
| `Fill` | Stretch to fill width |

**EVerticalAlignment:**

| Value | Description |
|---|---|
| `Top` | Align to top edge |
| `Center` | Center vertically |
| `Bottom` | Align to bottom edge |
| `Fill` | Stretch to fill height |

```xml
<TextBlock Text="Centered">
  <Slot HorizontalAlignment="Center" VerticalAlignment="Center" />
</TextBlock>
```

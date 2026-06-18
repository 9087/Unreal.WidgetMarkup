# EHorizontalAlignment / EVerticalAlignment

Control how content aligns within its slot. Use the **UE enum entry names** (case-sensitive), not editor display names.

**EHorizontalAlignment:**

| Value | Description |
|---|---|
| `HAlign_Left` | Align to left edge |
| `HAlign_Center` | Center horizontally |
| `HAlign_Right` | Align to right edge |
| `HAlign_Fill` | Stretch to fill width |

**EVerticalAlignment:**

| Value | Description |
|---|---|
| `VAlign_Top` | Align to top edge |
| `VAlign_Center` | Center vertically |
| `VAlign_Bottom` | Align to bottom edge |
| `VAlign_Fill` | Stretch to fill height |

```xml
<TextBlock Text="Centered">
  <Slot HorizontalAlignment="HAlign_Center" VerticalAlignment="VAlign_Center" />
</TextBlock>
```

> **Reading back in Python:** Python enum members use UPPER_SNAKE_CASE:
> ```python
> slot.get_editor_property("HorizontalAlignment") == unreal.HorizontalAlignment.H_ALIGN_FILL
> slot.get_editor_property("VerticalAlignment") == unreal.VerticalAlignment.V_ALIGN_CENTER
> ```

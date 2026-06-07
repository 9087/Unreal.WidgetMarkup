# SizeBox

`USizeBox : UContentWidget` — Constrains child to fixed dimensions. Single child.

Inherits from: [shared-properties.md](shared-properties.md)

| Attribute | Type | Description |
|---|---|---|
| `WidthOverride` | `float` | Fixed width |
| `HeightOverride` | `float` | Fixed height |
| `MinDesiredWidth` | `float` | Minimum width |
| `MinDesiredHeight` | `float` | Minimum height |
| `MaxDesiredWidth` | `float` | Maximum width |
| `MaxDesiredHeight` | `float` | Maximum height |

```xml
<SizeBox WidthOverride="200" HeightOverride="100">
  <TextBlock Text="Constrained" />
</SizeBox>
```

**Slot:** Padding ([FMargin](../structs/margin.md)), [Alignment](../structs/alignment.md) — see [panels.md](panels.md#sizebox-slot).

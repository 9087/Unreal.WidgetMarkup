# ESlateVisibility

Controls widget visibility and hit-testing.

| Value | Visible | Hit-testable | Occupies space |
|---|---|---|---|
| `Visible` | Yes | Yes | Yes |
| `Collapsed` | No | No | No |
| `Hidden` | No | No | Yes |
| `HitTestInvisible` | Yes | No (children can) | Yes |
| `SelfHitTestInvisible` | Yes | No (self only) | Yes |

```xml
<TextBlock Text="Can be hidden" Visibility="Collapsed" />
```

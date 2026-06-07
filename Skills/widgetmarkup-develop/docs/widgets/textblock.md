# TextBlock

`UTextBlock : UTextLayoutWidget` — Static or bound text display. No children.

Inherits from: [shared-properties.md](shared-properties.md)

| Attribute | Type | Description |
|---|---|---|
| `Text` | `FText` | Display text, supports `{binding}` |
| `ColorAndOpacity` | [FSlateColor](../structs/slate-color.md) | Text color |
| `Font.FontObject` | asset path | Font face asset |
| `Font.Size` | `int32` | Font size in points. See [FSlateFontInfo](../structs/font-info.md) |
| `Font.TypefaceFontName` | `FName` | Typeface variant |
| `ShadowOffset` | [FVector2D](../structs/vector2d.md) | Drop shadow offset |
| `ShadowColorAndOpacity` | [FLinearColor](../structs/linear-color.md) | Drop shadow color |
| `MinDesiredWidth` | `float` | Minimum width before wrapping |
| `AutoWrapText` | `bool` | Enable text wrapping |
| `TextTransformPolicy` | `ETextTransformPolicy` | `None`, `ToLower`, `ToUpper` |
| `TextOverflowPolicy` | `ETextOverflowPolicy` | `Clip`, `Ellipsis` |
| `Justification` | `ETextJustify` | `Left`, `Center`, `Right` |

```xml
<TextBlock Name="Title" Text="{title}" ColorAndOpacity="{title_color}"
           Font.Size="24" Justification="Center" />
```

# FSlateFontInfo

Font configuration for text widgets.

| Path | Type | Description |
|---|---|---|
| `Font.FontObject` | asset path | Font face asset (e.g., `/Engine/EngineFonts/Roboto.Roboto`) |
| `Font.Size` | `int32` | Font size in points |
| `Font.TypefaceFontName` | `FName` | Typeface variant within the font (e.g., `"Bold"`, `"Italic"`) |

```xml
<TextBlock Text="Hello" Font.Size="24" Font.TypefaceFontName="Bold" />
```

Used by: [TextBlock](../widgets/textblock.md)

> **XML nesting:** Child-element syntax (`<Font><Size>18</Size></Font>`) is supported via `FPropertyPathResolver`.

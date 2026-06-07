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

> **Partial XML nesting:** Dotted attributes like `Font.Size="24"` work. Nested `<Font Size="24" />` as a child element is TODO.

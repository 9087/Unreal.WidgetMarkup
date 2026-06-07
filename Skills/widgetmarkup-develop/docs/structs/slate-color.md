# FSlateColor

A color that can be either a literal `FLinearColor` or a reference to a color from the widget style. Used for text and brush tint colors.

**Format:** Same as [FLinearColor](linear-color.md) — `"R,G,B,A"`.

```
"1,1,1,1"     → White (from FLinearColor)
"{tint}"      → Bind to @reactive property returning unreal.SlateColor
```

**Python:** Use `unreal.SlateColor(unreal.LinearColor(r, g, b, a))`. Do NOT use bare `unreal.LinearColor` — the binding expects `SlateColor`.

> **XML nesting:** TODO. Nested elements are not supported. Use the `"R,G,B,A"` string format.

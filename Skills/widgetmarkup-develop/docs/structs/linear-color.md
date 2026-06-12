# FLinearColor

RGBA color with float components (0–1). Used by most color properties.

**Format:** `"R,G,B,A"` or `"R,G,B"` (A defaults to 1.0).

```
"1,0,0,1"     → Red, fully opaque
"0,0.5,0"     → Dark green, opaque (A=1.0)
"{my_color}"  → Bind to @reactive property returning unreal.LinearColor
```

**Python:** Use `unreal.LinearColor(r, g, b, a)`.

> **XML nesting:** Child-element syntax (`<ColorAndOpacity><R>1</R><G>0</G><B>0</B><A>1</A></ColorAndOpacity>`) is supported via `FPropertyPathResolver`.

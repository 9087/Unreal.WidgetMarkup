# FMargin

4-way margin/padding with float components.

**Format:** `"Left,Top,Right,Bottom"`.

| Example | Meaning |
|---|---|
| `"4,4,4,4"` | Uniform 4px on all sides |
| `"8,2,8,2"` | 8px horizontal, 2px vertical |
| `"0.25,0.25,0.25,0.25"` | 9-slice margins for brush |

Also used as `FAnchorData.Offsets` on CanvasPanel slots, where values represent Position X, Y, Width, Height instead of margins.

> **XML nesting:** TODO. Nested `<Padding Left="4" Top="4" Right="4" Bottom="4" />` is not yet supported. Use the `"L,T,R,B"` string format.

# ComboBoxString

`UComboBoxString : UWidget` — Dropdown selector. No children.

Inherits from: [shared-properties.md](shared-properties.md)

| Attribute | Type | Description |
|---|---|---|
| `DefaultOptions` | comma-separated strings | Initial dropdown options |
| `SelectedOption` | `FString` | Pre-selected item |
| `MaxListHeight` | `float` | Dropdown max height |
| `HasDownArrow` | `bool` | Show arrow indicator |

**Delegate:**

| Delegate | Signature |
|---|---|
| `OnSelectionChanged` | `(FString SelectedItem, ESelectInfo::Type SelectionType)` |

```xml
<ComboBoxString DefaultOptions="Option A,Option B,Option C"
                SelectedOption="Option A"
                OnSelectionChanged="on_item_selected" />
```

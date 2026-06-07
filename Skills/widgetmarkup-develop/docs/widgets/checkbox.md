# CheckBox

`UCheckBox : UContentWidget` — Toggle checkbox/toggle button with optional child.

Inherits from: [shared-properties.md](shared-properties.md)

| Attribute | Type | Description |
|---|---|---|
| `CheckedState` | `ECheckBoxState` | `Unchecked`, `Checked`, `Undetermined` |
| `IsChecked` | `bool` | Shorthand (true=Checked, false=Unchecked), supports `{binding}` |
| `HorizontalAlignment` | [EHorizontalAlignment](../structs/alignment.md) | Content H-align |
| `ClickMethod` | `EButtonClickMethod` | See [Button](button.md) |
| `IsFocusable` | `bool` | Keyboard focusable |

**Delegate:**

| Delegate | Signature |
|---|---|
| `OnCheckStateChanged` | `(bool bIsChecked)` |

```xml
<CheckBox IsChecked="{checked}" OnCheckStateChanged="on_toggle">
  <TextBlock Text="Enable Feature" />
</CheckBox>
```

"""Verify inherited + override stylesheet properties at runtime."""
import unreal
import widget_markup
from WidgetMarkupComponent import WidgetMarkupComponent

_USER_WIDGET_ATTR = "_widget_markup_user_widget"


def _get_widget(uw, name):
    widget = uw.get_editor_property(name)
    if not widget:
        raise AssertionError(f"Widget '{name}' not found.")
    return widget


class TestStyleOverrideComponent(WidgetMarkupComponent):
    def __init__(self):
        super().__init__()
        uw = getattr(self, _USER_WIDGET_ATTR, None)
        if not uw:
            unreal.log_error("[TestStyleOverride] user_widget is None.")
            widget_markup.request_shutdown()
            return

        default_text = _get_widget(uw, "DefaultText")
        title_text = _get_widget(uw, "TitleText")

        errors = []

        sz = default_text.get_editor_property("Font").size
        if sz != 18:
            errors.append(f"DefaultText.Font.Size expected 18, got {sz}")

        c = default_text.get_editor_property("ColorAndOpacity").get_editor_property("SpecifiedColor")
        if abs(c.r - 0.5) > 0.01:
            errors.append(f"DefaultText.Color.R expected 0.5, got {c.r:.2f}")

        sz = title_text.get_editor_property("Font").size
        if sz != 24:
            errors.append(f"TitleText.Font.Size expected 24, got {sz}")

        c = title_text.get_editor_property("ColorAndOpacity").get_editor_property("SpecifiedColor")
        if abs(c.r - 1.0) > 0.01 or abs(c.g - 0.5) > 0.01:
            errors.append(f"TitleText.Color expected (1,0.5,0), got ({c.r:.2f},{c.g:.2f},{c.b:.2f})")

        if errors:
            for e in errors:
                unreal.log_error(f"[TestStyleOverride] FAIL: {e}")
        else:
            unreal.log_warning("[TestStyleOverride] ALL CHECKS PASSED.")
        widget_markup.request_shutdown()

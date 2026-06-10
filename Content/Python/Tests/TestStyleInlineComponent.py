"""Verify inline stylesheet properties at runtime."""
import unreal
import widget_markup
from WidgetMarkupComponent import WidgetMarkupComponent

_USER_WIDGET_ATTR = "_widget_markup_user_widget"


def _get_widget(uw, name):
    widget = uw.get_editor_property(name)
    if not widget:
        raise AssertionError(f"Widget '{name}' not found.")
    return widget


class TestStyleInlineComponent(WidgetMarkupComponent):
    def __init__(self):
        try:
            super().__init__()
            uw = getattr(self, _USER_WIDGET_ATTR, None)
            if not uw:
                unreal.log_error("[TestStyleInline] user_widget is None.")
                return

            default_text = _get_widget(uw, "DefaultText")
            title_text = _get_widget(uw, "TitleText")

            errors = []

            sz = default_text.get_editor_property("Font").size
            if sz != 16:
                errors.append(f"DefaultText.Font.Size expected 16, got {sz}")

            c = default_text.get_editor_property("ColorAndOpacity").get_editor_property("SpecifiedColor")
            if abs(c.r - 0.7) > 0.01:
                errors.append(f"DefaultText.Color.R expected 0.7, got {c.r:.2f}")

            sz = title_text.get_editor_property("Font").size
            if sz != 28:
                errors.append(f"TitleText.Font.Size expected 28, got {sz}")

            c = title_text.get_editor_property("ColorAndOpacity").get_editor_property("SpecifiedColor")
            if abs(c.r - 1.0) > 0.01 or abs(c.g - 0.8) > 0.01:
                errors.append(f"TitleText.Color expected (1,0.8,0.2), got ({c.r:.2f},{c.g:.2f},{c.b:.2f})")

            if errors:
                for e in errors:
                    unreal.log_error(f"[TestStyleInline] FAIL: {e}")
            else:
                unreal.log_warning("[TestStyleInline] ALL CHECKS PASSED.")
        finally:
            widget_markup.request_shutdown()

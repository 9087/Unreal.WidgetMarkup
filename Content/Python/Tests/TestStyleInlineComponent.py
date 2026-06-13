"""Verify inline stylesheet properties at runtime -- typography scale, semantics, cards, table layout."""
import unreal
import widget_markup
from WidgetMarkupComponent import WidgetMarkupComponent


class TestStyleInlineComponent(WidgetMarkupComponent):
    def __init__(self):
        try:
            super().__init__()
            uw = getattr(self, '_widget_markup_user_widget', None)
            if not uw:
                unreal.log_error("[TestStyleInline] user_widget is None.")
                return

            errors = []

            def fail(msg):
                errors.append(msg)

            # --- Implicit (default TextBlock style) ---
            w = uw.get_editor_property("PreviewImplicit")
            if w:
                if w.get_editor_property("Font").size != 14:
                    fail(f"Implicit.Size expected 14, got {w.get_editor_property('Font').size}")
                c = w.get_editor_property("ColorAndOpacity").get_editor_property("SpecifiedColor")
                if abs(c.r - 0.78) > 0.02:
                    fail(f"Implicit.Color.R expected 0.78, got {c.r:.2f}")
            else:
                fail("PreviewImplicit widget not found")

            # --- Header style ---
            w = uw.get_editor_property("HeaderText")
            if w:
                if w.get_editor_property("Font").size != 26:
                    fail(f"Header.Size expected 26, got {w.get_editor_property('Font').size}")
            else:
                fail("HeaderText widget not found")

            # --- Subtitle style (via PreviewSubtitle) ---
            w = uw.get_editor_property("PreviewSubtitle")
            if w:
                c = w.get_editor_property("ColorAndOpacity").get_editor_property("SpecifiedColor")
                if abs(c.b - 1.0) > 0.02:
                    fail(f"Subtitle.Color.B expected 1.0, got {c.b:.2f}")
            else:
                fail("PreviewSubtitle widget not found")

            # --- Body style ---
            w = uw.get_editor_property("PreviewBody")
            if w:
                c = w.get_editor_property("ColorAndOpacity").get_editor_property("SpecifiedColor")
                if abs(c.r - 0.95) > 0.02:
                    fail(f"Body.Color.R expected 0.95, got {c.r:.2f}")
            else:
                fail("PreviewBody widget not found")

            # --- Caption style ---
            w = uw.get_editor_property("CaptionText")
            if w:
                if w.get_editor_property("Font").size != 10:
                    fail(f"Caption.Size expected 10, got {w.get_editor_property('Font').size}")
            else:
                fail("CaptionText widget not found")

            # --- Link style ---
            w = uw.get_editor_property("PreviewLink")
            if w:
                c = w.get_editor_property("ColorAndOpacity").get_editor_property("SpecifiedColor")
                if abs(c.b - 1.0) > 0.02 or abs(c.g - 0.7) > 0.05:
                    fail(f"Link.Color expected blue-ish, got ({c.r:.2f},{c.g:.2f},{c.b:.2f})")
            else:
                fail("PreviewLink widget not found")

            # --- Badge style ---
            w = uw.get_editor_property("PreviewBadge")
            if w:
                c = w.get_editor_property("ColorAndOpacity").get_editor_property("SpecifiedColor")
                if abs(c.g - 0.9) > 0.05:
                    fail(f"Badge.Color.G expected ~0.9, got {c.g:.2f}")
            else:
                fail("PreviewBadge widget not found")

            # --- Warning style ---
            w = uw.get_editor_property("PreviewWarning")
            if w:
                c = w.get_editor_property("ColorAndOpacity").get_editor_property("SpecifiedColor")
                if abs(c.r - 1.0) > 0.02:
                    fail(f"Warning.Color.R expected 1.0, got {c.r:.2f}")
            else:
                fail("PreviewWarning widget not found")

            # --- Card border style ---
            w = uw.get_editor_property("PreviewCard")
            if w:
                bc = w.get_editor_property("BrushColor")
                if abs(bc.r - 0.12) > 0.02:
                    fail(f"Card.BrushColor.R expected 0.12, got {bc.r:.2f}")
            else:
                fail("PreviewCard widget not found")

            # --- AccentCard border style ---
            w = uw.get_editor_property("PreviewAccentCard")
            if w:
                bc = w.get_editor_property("BrushColor")
                if abs(bc.b - 0.22) > 0.03:
                    fail(f"AccentCard.BrushColor.B expected 0.22, got {bc.b:.2f}")
            else:
                fail("PreviewAccentCard widget not found")

            # --- Image tint style (ColorAndOpacity is FLinearColor, not FSlateColor) ---
            w = uw.get_editor_property("PreviewImage")
            if w:
                c = w.get_editor_property("ColorAndOpacity")
                if abs(c.b - 0.9) > 0.05:
                    fail(f"Image.Color.B expected 0.9, got {c.b:.2f}")
            else:
                fail("PreviewImage widget not found")

            # --- Button color style (ColorAndOpacity is FLinearColor) ---
            w = uw.get_editor_property("PreviewButton")
            if w:
                c = w.get_editor_property("ColorAndOpacity")
                if abs(c.b - 0.9) > 0.05:
                    fail(f"Button.Color.B expected 0.9, got {c.b:.2f}")
            else:
                fail("PreviewButton widget not found")

            # --- ProgressBar fill style ---
            w = uw.get_editor_property("PreviewProgressBar")
            if not w:
                fail("PreviewProgressBar widget not found")

            # --- System font style ---
            for name in ("PreviewSymbols", "PreviewArrows", "PreviewOperators", "PreviewDingbats"):
                w = uw.get_editor_property(name)
                if not w:
                    fail(f"{name} widget not found")
                else:
                    font = w.get_editor_property("Font")
                    if font is None:
                        fail(f"{name}: Font is None")
                    else:
                        font_size = font.size
                        if font_size <= 0:
                            fail(f"{name}: Font.size is {font_size}")

            if errors:
                for e in errors:
                    unreal.log_error(f"[TestStyleInline] FAIL: {e}")
            else:
                unreal.log_warning("[TestStyleInline] ALL CHECKS PASSED.")
        finally:
            if widget_markup.Application.get_extra_arguments() == "test": widget_markup.Application.request_shutdown()

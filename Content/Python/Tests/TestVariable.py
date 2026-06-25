import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestVariable(TestComponent):
    """Verify that Variable elements work correctly, including:
    - Normal default values (String, Boolean, Integer)
    - Brace-containing literal values NOT consumed as bindings"""

    def __init__(self):
        try:
            super().__init__("TestVariable")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")

            # --- Normal default values (sanity: variables exist) ---
            self.check_equal(
                uw.get_editor_property("PlayerName"), "Unknown",
                "PlayerName default = 'Unknown'")
            self.check_equal(
                uw.get_editor_property("IsAlive"), True,
                "IsAlive default = True")
            self.check_equal(
                uw.get_editor_property("Score"), 0,
                "Score default = 0")

            # --- Brace-containing values: MUST be literal strings ---
            self.check_equal(
                uw.get_editor_property("LiteralBraces"), "{some text}",
                "LiteralBraces = '{some text}' (NOT consumed as binding)")
            self.check_equal(
                uw.get_editor_property("LooksLikeBinding"), "{x.y.z}",
                "LooksLikeBinding = '{x.y.z}' (NOT consumed as binding)")
            self.check_equal(
                uw.get_editor_property("BracesInMiddle"), "before {x} after",
                "BracesInMiddle = 'before {x} after'")
            self.check_equal(
                uw.get_editor_property("SingleBrace"), "only open {",
                "SingleBrace = 'only open {'")
            self.check_equal(
                uw.get_editor_property("DoubleBraces"), "{{nested}}",
                "DoubleBraces = '{{nested}}'")

            # --- Empty default ---
            self.check_equal(
                uw.get_editor_property("EmptyDefault"), "",
                "EmptyDefault is empty string")

            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

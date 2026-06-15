import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestCanvasPanel(TestComponent):
    def __init__(self):
        try:
            super().__init__( "TestCanvasPanel")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")
            cp = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "TestCP")
            self.check_not_none(cp, "TestCP found")
            if cp:
                self.check_true("CanvasPanel" in str(cp.__class__), "TestCP is CanvasPanel")
            for name in ("CPChild1", "CPChild2"):
                c = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, name)
                self.check_not_none(c, f"{name} found")
            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

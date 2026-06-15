import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestOverlay(TestComponent):
    def __init__(self):
        try:
            super().__init__( "TestOverlay")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")
            ov = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "TestOV")
            self.check_not_none(ov, "TestOV found")
            if ov:
                self.check_true("Overlay" in str(ov.__class__), "TestOV is Overlay")
            for name in ("OVChild1", "OVChild2"):
                c = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, name)
                self.check_not_none(c, f"{name} found")
            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

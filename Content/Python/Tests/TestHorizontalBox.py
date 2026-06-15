import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestHorizontalBox(TestComponent):
    def __init__(self):
        try:
            super().__init__( "TestHorizontalBox")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")
            hb = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "TestHB")
            self.check_not_none(hb, "TestHB found")
            if hb:
                self.check_true("HorizontalBox" in str(hb.__class__), "TestHB is HorizontalBox")
            for name in ("HBChild1", "HBChild2"):
                c = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, name)
                self.check_not_none(c, f"{name} found")
            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

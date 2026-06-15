import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestVerticalBox(TestComponent):
    def __init__(self):
        try:
            super().__init__( "TestVerticalBox")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")
            vb = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "TestVB")
            self.check_not_none(vb, "TestVB found")
            if vb:
                self.check_true("VerticalBox" in str(vb.__class__), "TestVB is VerticalBox")
            for name in ("VBChild1", "VBChild2"):
                c = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, name)
                self.check_not_none(c, f"{name} found")
            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

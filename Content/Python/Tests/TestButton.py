import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestButton(TestComponent):
    def __init__(self):
        try:
            super().__init__( "TestButton")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")
            w = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "TestBtn")
            self.check_not_none(w, "TestBtn found in WidgetTree")
            if w:
                self.check_true("Button" in str(w.__class__), "TestBtn is Button type")
            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

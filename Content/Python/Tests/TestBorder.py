import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestBorder(TestComponent):
    def __init__(self):
        try:
            super().__init__( "TestBorder")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")
            w = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "TestBdr")
            self.check_not_none(w, "TestBdr found in WidgetTree")
            if w:
                self.check_true("Border" in str(w.__class__), "TestBdr is Border type")
            child = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "TestBorderChild")
            self.check_not_none(child, "TestBorderChild found")
            if child:
                self.check_true("TextBlock" in str(child.__class__), "child is TextBlock")
            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

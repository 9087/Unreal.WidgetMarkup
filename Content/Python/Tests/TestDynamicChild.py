import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestDynamicChild(TestComponent):
    def __init__(self):
        try:
            super().__init__("TestDynamicChild")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")

            exist = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "ExistingChild")
            self.check_not_none(exist, "ExistingChild found (static XML child)")

            comp = self.add_child("DynamicText", "TextBlock", "RootCanvas")
            self.check_true(comp is None, "add_child returns None for plain UMG widget")

            w = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "DynamicText")
            self.check_not_none(w, "DynamicText in WidgetTree after add_child")
            if w:
                self.check_true("TextBlock" in str(w.__class__), "DynamicText is TextBlock")

            static = self.get_child("ExistingChild")
            self.check_true(static is None, "get_child returns None for plain UMG widget (no component)")

            result = self.remove_child("DynamicText")
            self.check_true(result, "remove_child(name) returned True")
            gone = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "DynamicText")
            self.check_true(gone is None, "DynamicText gone after remove")

            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

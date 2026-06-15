import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestStaticChild(TestComponent):
    def __init__(self):
        try:
            super().__init__("TestStaticChild")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")

            # Add a TextBlock dynamically.
            comp = self.add_child("DynChild", "TextBlock", "RootCanvas")
            self.check_true(comp is None, "add_child returns None for plain UMG widget")

            w = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "DynChild")
            self.check_not_none(w, "DynChild in WidgetTree after add_child")

            # get_child on static XML child (plain UMG, no component).
            child = self.get_child("SiblingText")
            self.check_true(child is None, "get_child returns None for plain UMG widget")

            # remove_child by name.
            result = self.remove_child("DynChild")
            self.check_true(result, "remove_child(name) returned True")
            gone = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "DynChild")
            self.check_true(gone is None, "DynChild gone after remove_child")

            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

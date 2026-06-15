import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestStyleSheetFile(TestComponent):
    def __init__(self):
        try:
            super().__init__( "TestStyleSheetFile")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")

            styled = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "Styled")
            unstyled = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "Unstyled")
            self.check_not_none(styled, "Styled TextBlock found")
            self.check_not_none(unstyled, "Unstyled TextBlock found")

            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

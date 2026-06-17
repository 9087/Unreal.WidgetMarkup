import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestStyleSheetOverride(TestComponent):
    def __init__(self):
        try:
            super().__init__( "TestStyleSheetOverride")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")

            # The inherited TestStyleSheetFile defines StyledText (blue, size 20).
            styled = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "InheritedStyled")
            self.check_not_none(styled, "InheritedStyled found (style inherited from file)")

            unstyled = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "UnstyledRef")
            self.check_not_none(unstyled, "UnstyledRef found")

            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

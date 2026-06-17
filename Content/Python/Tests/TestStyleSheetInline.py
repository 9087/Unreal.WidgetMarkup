import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestStyleSheetInline(TestComponent):
    def __init__(self):
        try:
            super().__init__( "TestStyleSheetInline")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")

            inline = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "InlineStyled")
            self.check_not_none(inline, "InlineStyled TextBlock found")
            if inline:
                self.check_equal(inline.get_editor_property("Text"), "inline styled text", "InlineStyled.Text")

            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

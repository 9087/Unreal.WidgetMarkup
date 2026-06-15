import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestTextBlock(TestComponent):
    def __init__(self):
        try:
            super().__init__( "TestTextBlock")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")

            tb = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "TestText")
            self.check_not_none(tb, "TextBlock found in WidgetTree")

            if tb:
                # The Text property is set via binding, not directly on the widget.
                # Verify the widget exists with correct type.
                self.check_true("TextBlock" in str(tb.__class__), "widget is TextBlock type")

            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

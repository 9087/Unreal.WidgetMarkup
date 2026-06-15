import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestEmpty(TestComponent):
    def __init__(self):
        try:
            super().__init__("TestEmpty")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget is not None")
            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestStaticChildWidget(TestComponent):
    def __init__(self):
        try:
            super().__init__("TestStaticChildWidget")
            # This component just needs to load. Verification is done by parent.
            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

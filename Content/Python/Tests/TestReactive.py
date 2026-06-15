import unreal
import widget_markup
from WidgetMarkupComponent import reactive
from Tests.TestComponent import TestComponent


class TestReactive(TestComponent):
    def __init__(self):
        try:
            super().__init__( "TestReactive")

            # --- reactive defaults ---
            t = _ReactiveTarget()
            self.check_equal(t.a, 0, "reactive default: a == 0")
            self.check_equal(t.b, "hello", "reactive default: b == 'hello'")
            self.check_true(t.c, "reactive default: c is True")
            self.check_equal(t.d, 3.14, "reactive default: d == 3.14")

            # --- reactive setter ---
            t.a = 42
            self.check_equal(t.a, 42, "reactive setter: a == 42")
            t.b = "world"
            self.check_equal(t.b, "world", "reactive setter: b == 'world'")
            t.c = False
            self.check_equal(t.c, False, "reactive setter: c is False")

            # --- reactive notification ---
            t.a = 100
            self.check_equal(t.last_changed_name, "a", "on_property_changed received name='a'")
            self.check_equal(t.last_changed_value, 100, "on_property_changed received value=100")

            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()


class _ReactiveTarget(TestComponent):
    @reactive
    def a(self): return 0

    @reactive
    def b(self): return "hello"

    @reactive
    def c(self): return True

    @reactive
    def d(self): return 3.14

    def __init__(self):
        self.last_changed_name = None
        self.last_changed_value = None

    def on_property_changed(self, name, value):
        self.last_changed_name = name
        self.last_changed_value = value

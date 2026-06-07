import os
import sys


CURRENT_DIRECTORY = os.path.dirname(__file__)
PYTHON_ROOT = os.path.abspath(os.path.join(CURRENT_DIRECTORY, ".."))
if PYTHON_ROOT not in sys.path:
    sys.path.insert(0, PYTHON_ROOT)

from WidgetMarkupComponent import WidgetMarkupComponent, computed, reactive


class RecordingReactiveObject(WidgetMarkupComponent):
    def __init__(self):
        self.changes = []
        self.prime_computed_dependencies()

    def on_property_changed(self, property_name, value):
        self.changes.append((property_name, value))


class DemoComponent(RecordingReactiveObject):
    @reactive
    def left(self):
        return 0

    @reactive
    def right(self):
        return 0

    @reactive
    def use_left(self):
        return True

    @computed
    def total(self):
        return self.left + self.right

    @computed
    def doubled_total(self):
        return self.total * 2

    @computed
    def selected(self):
        if self.use_left:
            return self.left
        return self.right


class CyclicComponent(WidgetMarkupComponent):
    @computed
    def first(self):
        return self.second + 1

    @computed
    def second(self):
        return self.first + 1


class ListComponent(RecordingReactiveObject):
    @reactive
    def items(self):
        return []

    @computed
    def item_count(self):
        return len(self.items)


class ReentrantComponent(RecordingReactiveObject):
    @reactive
    def left(self):
        return 0

    @reactive
    def right(self):
        return 0

    @computed
    def total(self):
        return self.left + self.right

    def on_property_changed(self, property_name, value):
        super().on_property_changed(property_name, value)
        if property_name == "left" and self.right != value * 2:
            self.right = value * 2


class RecoveryComponent(RecordingReactiveObject):
    @reactive
    def numerator(self):
        return 10

    @reactive
    def denominator(self):
        return 1

    @computed
    def quotient(self):
        return self.numerator / self.denominator


class InitConfiguredComponent(WidgetMarkupComponent):
    @reactive
    def base(self):
        return 2

    @computed
    def total(self):
        return self.base * self.multiplier

    def __init__(self):
        self.multiplier = 3
        self.prime_computed_dependencies()


class UnprimedComponent(WidgetMarkupComponent):
    @reactive
    def left(self):
        return 0

    @computed
    def total(self):
        return self.left + self.value

    def __init__(self):
        self.changes = []
        self.value = 2

    def on_property_changed(self, property_name, value):
        self.changes.append((property_name, value))


class DemoWidgetMarkupComponent(WidgetMarkupComponent):
    @reactive
    def left(self):
        return 1

    @computed
    def doubled(self):
        return self.left * 2

    def __init__(self):
        self.changes = []
        self.prime_computed_dependencies()

    def on_property_changed(self, property_name, value):
        self.changes.append((property_name, value))

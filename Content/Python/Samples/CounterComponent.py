from WidgetMarkupComponent import WidgetMarkupComponent, reactive


class CounterComponent(WidgetMarkupComponent):
    @reactive
    def count(self):
        return 0

    def increment(self):
        self.count += 1

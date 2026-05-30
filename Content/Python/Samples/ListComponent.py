from WidgetMarkupComponent import WidgetMarkupComponent, reactive


class ListComponent(WidgetMarkupComponent):
    @reactive
    def items(self):
        return []

    _next_id = 0

    def _take_id(self):
        val = self._next_id
        self._next_id += 1
        return val

    def add_item(self):
        self.items.append(self._take_id())

    def delete_item(self):
        if self.items:
            self.items.pop()

    def clear(self):
        self.items = []

    def update_list(self):
        self.items = [self._take_id(), self._take_id(), self._take_id()]

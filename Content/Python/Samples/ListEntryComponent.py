from WidgetMarkupComponent import WidgetMarkupComponent, reactive
import unreal


_COLORS = [
    unreal.SlateColor(unreal.LinearColor(1, 0, 0, 1)),  # red
    unreal.SlateColor(unreal.LinearColor(1, 1, 0, 1)),  # yellow
    unreal.SlateColor(unreal.LinearColor(0, 0, 1, 1)),  # blue
]


class ListEntryComponent(WidgetMarkupComponent):
    @reactive
    def display_text(self):
        return ""

    @reactive
    def color(self):
        return unreal.SlateColor()

    def on_data_refresh(self, data):
        """Push list item value through data binding to the TextBlock."""
        self.display_text = str(data)
        self.color = _COLORS[data % 3]

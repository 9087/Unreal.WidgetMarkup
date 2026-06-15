import unreal
import widget_markup
from WidgetMarkupComponent import reactive
from ObservableCollection import ObservableCollection
from Tests.TestComponent import TestComponent


class TestListView(TestComponent):
    def __init__(self):
        try:
            super().__init__( "TestListView")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")

            lv = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "TestLV")
            self.check_not_none(lv, "ListView found")

            # --- ObservableCollection ---
            items = ObservableCollection(owner=None, property_name="test",
                                         on_changed=None, initial=list("ABC"))
            self.check_equal(len(items), 3, "initial length == 3")
            self.check_equal(items[0], "A", "items[0] == 'A'")
            self.check_equal(items[1], "B", "items[1] == 'B'")

            items.append("D")
            self.check_equal(len(items), 4, "after append length == 4")
            self.check_equal(items[3], "D", "items[3] == 'D'")

            items.pop(0)
            self.check_equal(len(items), 3, "after pop(0) length == 3")
            self.check_equal(items[0], "B", "items[0] == 'B'")

            items.clear()
            self.check_equal(len(items), 0, "after clear length == 0")

            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

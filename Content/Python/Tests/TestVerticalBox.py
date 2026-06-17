import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestVerticalBox(TestComponent):
    def __init__(self):
        try:
            super().__init__("TestVerticalBox")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")

            vertical_box = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "TestVertical")
            self.check_not_none(vertical_box, "TestVertical found")
            if vertical_box:
                self.check_equal(vertical_box.get_children_count(), 2, "VerticalBox has 2 children")

            # Child1: Fill slot
            vertical_child1 = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "VerticalChild1")
            self.check_not_none(vertical_child1, "VerticalChild1 found")
            if vertical_child1 and vertical_child1.slot:
                slot1 = vertical_child1.slot
                self.check_true("H_ALIGN_FILL" in str(slot1.get_editor_property("HorizontalAlignment")), "VerticalChild1.HAlign Fill")
                self.check_true("V_ALIGN_CENTER" in str(slot1.get_editor_property("VerticalAlignment")), "VerticalChild1.VAlign Center")
                pad1 = slot1.get_editor_property("Padding")
                self.check_not_none(pad1, "VerticalChild1.Padding set")
                if pad1:
                    self.check_almost_equal(pad1.left, 4.0, 0.001, "VerticalChild1.Padding.Left")
                    self.check_almost_equal(pad1.top, 2.0, 0.001, "VerticalChild1.Padding.Top")

            # Child2: Automatic slot
            vertical_child2 = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "VerticalChild2")
            self.check_not_none(vertical_child2, "VerticalChild2 found")
            if vertical_child2 and vertical_child2.slot:
                slot2 = vertical_child2.slot
                self.check_true("H_ALIGN_RIGHT" in str(slot2.get_editor_property("HorizontalAlignment")), "VerticalChild2.HAlign Right")
                self.check_true("V_ALIGN_BOTTOM" in str(slot2.get_editor_property("VerticalAlignment")), "VerticalChild2.VAlign Bottom")
                pad2 = slot2.get_editor_property("Padding")
                self.check_not_none(pad2, "VerticalChild2.Padding set")
                if pad2:
                    self.check_almost_equal(pad2.left, 8.0, 0.001, "VerticalChild2.Padding.Left")
                    self.check_almost_equal(pad2.top, 4.0, 0.001, "VerticalChild2.Padding.Top")

            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

import unreal
import widget_markup
from Tests.TestComponent import TestComponent


class TestTextBlock(TestComponent):
    def __init__(self):
        try:
            super().__init__("TestTextBlock")
            uw = getattr(self, "_widget_markup_user_widget", None)
            self.check_not_none(uw, "user widget loaded")

            text_block = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "TestText")
            self.check_not_none(text_block, "TestText found")
            if text_block:
                self.check_equal(text_block.get_editor_property("Text"), "Hello World", "Text")

            shared_text = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "SharedText")
            self.check_not_none(shared_text, "SharedText found")
            if shared_text:
                self.check_equal(shared_text.get_editor_property("Text"), "Shared", "SharedText.Text")
                self.check_almost_equal(shared_text.get_editor_property("RenderOpacity"), 0.8, 0.001, "RenderOpacity")

            wrapped_text = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "WrappedText")
            self.check_not_none(wrapped_text, "WrappedText found")
            if wrapped_text:
                self.check_equal(wrapped_text.get_editor_property("AutoWrapText"), True, "AutoWrapText")
                self.check_almost_equal(wrapped_text.get_editor_property("MinDesiredWidth"), 100.0, 0.001, "MinDesiredWidth")

            shadow_text = widget_markup.WidgetLibrary.find_widget_in_user_widget(uw, "ShadowText")
            self.check_not_none(shadow_text, "ShadowText found")
            if shadow_text:
                self.check_equal(shadow_text.get_editor_property("Text"), "With Shadow", "ShadowText.Text")
                # ShadowOffset = (2, 2)
                so = shadow_text.get_editor_property("ShadowOffset")
                self.check_not_none(so, "ShadowOffset set")
                if so:
                    self.check_almost_equal(so.x, 2.0, 0.001, "ShadowOffset.X")
                    self.check_almost_equal(so.y, 2.0, 0.001, "ShadowOffset.Y")
                # ShadowColorAndOpacity = (0, 0, 0, 0.5)
                sc = shadow_text.get_editor_property("ShadowColorAndOpacity")
                self.check_not_none(sc, "ShadowColorAndOpacity set")
                if sc:
                    self.check_almost_equal(sc.a, 0.5, 0.001, "ShadowColorAndOpacity.A")

            self.report()
        finally:
            if widget_markup.Application.get_extra_arguments() == "test":
                widget_markup.Application.request_shutdown()

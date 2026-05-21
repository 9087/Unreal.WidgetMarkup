import unittest

from _support import DemoWidgetMarkupComponent, WidgetMarkupComponent


class WidgetMarkupComponentTests(unittest.TestCase):
    def test_widget_markup_component_is_the_base_type(self):
        component = DemoWidgetMarkupComponent()

        self.assertIsInstance(component, WidgetMarkupComponent)

    def test_widget_markup_component_supports_reactive_behavior(self):
        component = DemoWidgetMarkupComponent()

        component.left = 4

        self.assertEqual(component.doubled, 8)
        self.assertIn(("doubled", 8), component.changes)


if __name__ == "__main__":
    unittest.main()
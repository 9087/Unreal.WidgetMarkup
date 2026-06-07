import unittest

from _support import (
    CyclicComponent,
    DemoComponent,
    InitConfiguredComponent,
    RecoveryComponent,
    ReentrantComponent,
    UnprimedComponent,
)


class ReactiveTests(unittest.TestCase):
    def _changes_for(self, component, name):
        return [c for c in component.changes if c[0] == name]

    def test_settable_property_notifies_on_value_change(self):
        component = DemoComponent()

        component.left = 10
        component.left = 10
        component.left = 12

        self.assertEqual(self._changes_for(component, "left"), [("left", 10), ("left", 12)])

    def test_computed_property_tracks_dependencies_and_notifies(self):
        component = DemoComponent()

        # First read builds dependency graph for total.
        self.assertEqual(component.total, 0)

        component.left = 3

        self.assertEqual(component.total, 3)
        self.assertIn(("total", 3), component.changes)

    def test_unread_computed_property_is_still_notified(self):
        component = DemoComponent()

        # Do not read component.total before updating its dependencies.
        component.left = 2

        self.assertIn(("total", 2), component.changes)

    def test_chained_computed_properties_propagate(self):
        component = DemoComponent()

        # Build dependency graph for both computed properties.
        self.assertEqual(component.total, 0)
        self.assertEqual(component.doubled_total, 0)

        component.left = 4

        self.assertEqual(component.total, 4)
        self.assertEqual(component.doubled_total, 8)
        self.assertIn(("total", 4), component.changes)
        self.assertIn(("doubled_total", 8), component.changes)

    def test_conditional_dependencies_are_rebuilt(self):
        component = DemoComponent()

        # Build dependency graph for selected; starts with left branch.
        self.assertEqual(component.selected, 0)

        component.left = 5
        self.assertIn(("selected", 5), component.changes)

        component.use_left = False
        self.assertEqual(component.selected, 0)
        self.assertIn(("selected", 0), component.changes)

        selected_change_count = len(self._changes_for(component, "selected"))
        component.left = 9
        # No selected update expected because selected now depends on right.
        self.assertEqual(len(self._changes_for(component, "selected")), selected_change_count)

        component.right = 7
        self.assertEqual(component.selected, 7)
        self.assertIn(("selected", 7), component.changes)

    def test_computed_property_is_read_only(self):
        component = DemoComponent()

        with self.assertRaises(AttributeError):
            component.total = 123

    def test_cyclic_computed_dependencies_raise_error(self):
        component = CyclicComponent()
        with self.assertRaises(RuntimeError) as error_context:
            component.prime_computed_dependencies()

        self.assertIn("Cyclic computed dependency detected", str(error_context.exception))

    def test_multiple_instances_keep_reactive_state_isolated(self):
        first_component = DemoComponent()
        second_component = DemoComponent()

        first_component.left = 5

        self.assertEqual(first_component.total, 5)
        self.assertEqual(second_component.total, 0)
        self.assertIn(("left", 5), first_component.changes)
        self.assertNotIn(("left", 5), second_component.changes)

    def test_reentrant_callback_can_update_another_property(self):
        component = ReentrantComponent()

        component.left = 2

        self.assertEqual(component.right, 4)
        self.assertEqual(component.total, 6)
        self.assertIn(("left", 2), component.changes)
        self.assertIn(("right", 4), component.changes)
        self.assertIn(("total", 6), component.changes)

    def test_computed_property_recovers_after_failed_recompute(self):
        component = RecoveryComponent()

        with self.assertRaises(ZeroDivisionError):
            component.denominator = 0

        component.denominator = 2

        self.assertEqual(component.quotient, 5)
        self.assertIn(("denominator", 2), component.changes)
        self.assertIn(("quotient", 5), component.changes)

    def test_computed_property_can_depend_on_init_assigned_fields(self):
        component = InitConfiguredComponent()

        self.assertEqual(component.total, 6)

    def test_unread_computed_property_requires_explicit_priming(self):
        component = UnprimedComponent()

        component.left = 3
        self.assertNotIn(("total", 5), component.changes)

        component.prime_computed_dependencies()
        component.left = 4
        self.assertIn(("total", 6), component.changes)


if __name__ == "__main__":
    unittest.main()
import unittest

from _support import ListComponent


class ObservableCollectionTests(unittest.TestCase):
    def test_list_append_triggers_reactive_notifications(self):
        component = ListComponent()
        component.changes.clear()

        component.items.append(1)

        item_notifications = [c for c in component.changes if c[0] == "items"]
        self.assertEqual(len(item_notifications), 1)
        self.assertIn(("item_count", 1), component.changes)

    def test_list_setitem_triggers_reactive_notifications(self):
        component = ListComponent()

        component.items = [10]
        component.changes.clear()
        component.items[0] = 11

        self.assertIn(("items", component.items), component.changes)

    def test_list_pop_triggers_computed_update(self):
        component = ListComponent()

        component.items = [1, 2, 3]
        component.changes.clear()
        component.items.pop()

        self.assertEqual(component.item_count, 2)
        self.assertIn(("item_count", 2), component.changes)


if __name__ == "__main__":
    unittest.main()
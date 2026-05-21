from __future__ import annotations

from typing import Any, Callable, TYPE_CHECKING

if TYPE_CHECKING:
    from WidgetMarkupComponent import WidgetMarkupComponent


class ObservableCollection(list):
    """List wrapper that emits change notifications on in-place mutation."""

    def __init__(
        self,
        owner: WidgetMarkupComponent,
        property_name: str,
        on_changed: Callable[[WidgetMarkupComponent, str, list[Any]], None],
        initial_values: list[Any],
    ) -> None:
        super().__init__(initial_values)
        self._owner = owner
        self._property_name = property_name
        self._on_changed = on_changed

    def _mark_changed(self) -> None:
        self._on_changed(self._owner, self._property_name, self)

    def append(self, item: Any) -> None:
        super().append(item)
        self._mark_changed()

    def extend(self, items: list[Any]) -> None:
        super().extend(items)
        self._mark_changed()

    def insert(self, index: int, item: Any) -> None:
        super().insert(index, item)
        self._mark_changed()

    def pop(self, index: int = -1) -> Any:
        item = super().pop(index)
        self._mark_changed()
        return item

    def remove(self, item: Any) -> None:
        super().remove(item)
        self._mark_changed()

    def clear(self) -> None:
        super().clear()
        self._mark_changed()

    def sort(self, *args: Any, **kwargs: Any) -> None:
        super().sort(*args, **kwargs)
        self._mark_changed()

    def reverse(self) -> None:
        super().reverse()
        self._mark_changed()

    def __setitem__(self, key: Any, value: Any) -> None:
        super().__setitem__(key, value)
        self._mark_changed()

    def __delitem__(self, key: Any) -> None:
        super().__delitem__(key)
        self._mark_changed()

    def __iadd__(self, other: list[Any]) -> ObservableCollection:
        result = super().__iadd__(other)
        self._mark_changed()
        return result

    def __imul__(self, other: int) -> ObservableCollection:
        result = super().__imul__(other)
        self._mark_changed()
        return result
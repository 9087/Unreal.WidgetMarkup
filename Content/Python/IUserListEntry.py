from __future__ import annotations

from typing import Any


class IUserListEntry:
    """Mixin for WidgetMarkupComponent subclasses used as ListView entry widgets.

    Provides callbacks and helpers matching the C++ IUserListEntry interface.
    Inherit from this alongside WidgetMarkupComponent to receive list events.

    Binds to UWidgetMarkupUserWidget's multicast delegates:
      - on_entry_released  →  on_entry_released()
      - on_item_selection_changed  →  on_item_selection_changed(b_is_selected)
      - on_item_expansion_changed  →  on_item_expansion_changed(b_is_expanded)
    """

    def __init__(self) -> None:
        """Bind the WidgetMarkupUserWidget delegates to this mixin's callbacks."""
        super().__init__()
        uw = self._widget_markup_user_widget
        uw.on_entry_released.add_callable(self.on_entry_released)
        uw.on_item_selection_changed.add_callable(self.on_item_selection_changed)
        uw.on_item_expansion_changed.add_callable(self.on_item_expansion_changed)

    def on_entry_released(self) -> None:
        """Called when this entry is released from the owning list view."""
        pass

    def on_item_selection_changed(self, b_is_selected: bool) -> None:
        """Called when the selection state of this entry changes."""
        pass

    def on_item_expansion_changed(self, b_is_expanded: bool) -> None:
        """Called when the expansion state of this entry changes (TreeView only)."""
        pass

    def is_selected(self) -> bool:
        """Return True if this entry is currently selected in its owning list view."""
        return self._widget_markup_user_widget.is_entry_selected()

    def get_owning_list_view(self) -> Any:
        """Return the owning UListViewBase for this entry widget."""
        return self._widget_markup_user_widget.get_owning_entry_list_view()

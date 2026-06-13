"""
TestBlueprintCompilationComponent - validates object-description scenarios across Variable and WidgetTree contexts.

Test1   Basic inline:      Integer Default
Test2   String default:    String Default
Test3   Struct inline:     Vector2D with X/Y children
Test4   Object path-ref:   Texture2D from asset path
Test5   Object inline:     Texture2D NewObject (empty)
Test6   Object sub-prop:   Texture2D inline with LODBias child
Test7   Basic attr:        TextBlock.Text
Test8   Struct inline:     Font with Size sub-element
Test9   Basic array:       ColumnFill + RowFill with float children
"""
import unreal
import widget_markup
from WidgetMarkupComponent import WidgetMarkupComponent


class TestBlueprintCompilationComponent(WidgetMarkupComponent):

    def __init__(self) -> None:
        try:
            if hasattr(super(), '__init__'):
                super().__init__()
            uw = getattr(self, '_widget_markup_user_widget', None)

            if not uw:
                unreal.log("[FAIL] No user_widget")
                return

            cls_name = uw.get_class().get_name()
            unreal.log(f"[PASS] Widget class: {cls_name}")

            self._passed = 0
            self._failed = 0
            self._uw = uw
            self._cdo = uw.get_class().get_default_object()

            # Run tests in order
            self._test1_basic_inline()
            self._test2_string_default()
            self._test3_struct_inline()
            self._test4_object_path_reference()
            self._test5_object_inline()
            self._test6_object_sub_property()
            self._test7_object_variable()
            self._test8_array_object_variable()
            self._test9_basic_attribute()
            self._test10_struct_inline_property()
            self._test11_basic_array()

            if self._failed == 0:
                unreal.log_warning("[TestBlueprintCompilation] ALL CHECKS PASSED.")
            else:
                unreal.log_error(f"[TestBlueprintCompilation] FAIL: {self._failed} checks failed, {self._passed} passed.")
        finally:
            if widget_markup.Application.get_extra_arguments() == "test": widget_markup.Application.request_shutdown()

    # =========================================================================
    # Variable tests (Blueprint CDO)
    # =========================================================================

    def _test1_basic_inline(self):
        self._check_cdo("Test1_BasicInline", "Test1_BasicInline", lambda v: (v == 42, f"Integer = 42" if v == 42 else f"expected 42, got {v}"))

    def _test2_string_default(self):
        self._check_cdo("Test2_StringDefault", "Test2_StringDefault",
                         lambda v: (str(v) == "hello", f"String = 'hello'" if str(v) == "hello" else f"expected 'hello', got '{v}'"))

    def _test3_struct_inline(self):
        def validate(v):
            x = getattr(v, 'x', None) if v else None
            y = getattr(v, 'y', None) if v else None
            ok = x == 1.0 and y == 2.0
            return ok, "Vector2D(1.0, 2.0)" if ok else f"expected (1.0,2.0), got ({x},{y})"
        self._check_cdo("Test3_StructInline", "Test3_StructInline", validate)

    def _test4_object_path_reference(self):
        self._check_cdo("Test4_ObjectPathReference", "Test4_ObjectPathReference", lambda v: (v is not None, "Texture2D loaded from path" if v is not None else "Texture2D is None"))

    def _test5_object_inline(self):
        self._check_cdo("Test5_ObjectInline", "Test5_ObjectInline", lambda v: (v is not None, "Texture2D NewObject created" if v is not None else "Texture2D is None"))

    def _test6_object_sub_property(self):
        def validate(v):
            if v is None:
                return False, "Texture2D is None"
            lod_bias = getattr(v, 'lod_bias', None)
            ok = lod_bias == 2
            return ok, "LODBias = 2" if ok else f"expected LODBias=2, got {lod_bias}"
        self._check_cdo("Test6_ObjectSubProperty", "Test6_ObjectSubProperty", validate)

    def _test7_object_variable(self):
        self._check_cdo("Test7_ObjectVariable", "Test7_ObjectVariable", lambda v: (v is not None, "Texture2D loaded from path" if v is not None else "Texture2D is None"))

    def _test8_array_object_variable(self):
        def validate(v):
            if v is None or len(v) != 2:
                return False, f"expected array of 2, got {len(v) if v else 0}"
            if v[0] is not None and v[1] is not None:
                return True, "Array has 2 Texture2D items"
            return False, f"item0={v[0]}, item1={v[1]}"
        self._check_cdo("Test8_ArrayObjectVariable", "Test8_ArrayObjectVariable", validate)

    # =========================================================================
    # Property tests (WidgetTree)
    # =========================================================================

    def _test9_basic_attribute(self):
        def validate(w):
            text = str(w.get_editor_property("Text"))
            ok = "Basic-ok" in text
            return ok, "Text = 'Basic-ok'" if ok else f"expected 'Basic-ok', got '{text}'"
        self._check_widget("Test9_BasicAttribute", "Test9_BasicAttribute", validate)

    def _test10_struct_inline_property(self):
        def validate(w):
            font = w.font
            if not font:
                return False, "font is None"
            ok = font.size == 18
            return ok, "Font.Size = 18" if ok else f"expected Font.Size=18, got {font.size}"
        self._check_widget("Test10_StructInline", "Test10_StructInline", validate)

    def _test11_basic_array(self):
        def validate(grid):
            col_fill = grid.get_editor_property("ColumnFill")
            row_fill = grid.get_editor_property("RowFill")
            col_ok = col_fill and len(col_fill) == 2
            row_ok = row_fill and len(row_fill) == 1
            ok = col_ok and row_ok
            return ok, "ColumnFill=2, RowFill=1" if ok else f"ColumnFill={len(col_fill) if col_fill else 0}, RowFill={len(row_fill) if row_fill else 0}"
        self._check_widget("Test11_BasicArray", "Test11_BasicArray", validate)

    # =========================================================================
    # Helpers
    # =========================================================================

    def _check_cdo(self, name, prop_name, validate):
        """Fetch a CDO property, validate it, and pass/fail."""
        if not self._uw:
            self._fail(name, "Cannot get user widget")
            return
        try:
            val = self._uw.get_editor_property(prop_name)
            ok, detail = validate(val)
        except Exception as e:
            self._fail(name, str(e))
            return
        self._pass(name, detail) if ok else self._fail(name, detail)

    def _check_widget(self, name, widget_name, validate):
        """Find a widget, validate it, and pass/fail."""
        w = self._find_widget(widget_name)
        if not w:
            self._fail(name, f"widget '{widget_name}' not found")
            return
        try:
            ok, detail = validate(w)
        except Exception as e:
            self._fail(name, str(e))
            return
        self._pass(name, detail) if ok else self._fail(name, detail)

    def _find_widget(self, name):
        """Find a widget by name using the native helper."""
        return widget_markup.WidgetLibrary.find_widget_in_user_widget(self._uw, name)

    def _pass(self, name, detail):
        """Record a PASS result silently."""
        self._passed += 1

    def _fail(self, name, detail):
        """Log a FAIL result."""
        unreal.log_error(f"[TestBlueprintCompilation] FAIL: {name}: {detail}")
        self._failed += 1

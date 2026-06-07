"""
TestCasesComponent - validates object-description scenarios across Variable and WidgetTree contexts.

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
try:
    import widget_markup
except ImportError:
    widget_markup = None
try:
    from WidgetMarkupComponent import WidgetMarkupComponent
except ImportError:
    WidgetMarkupComponent = object


class TestCasesComponent(WidgetMarkupComponent):

    def __init__(self) -> None:
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

        unreal.log(f"=== SUMMARY: {self._passed} PASS, {self._failed} FAIL ===")
        widget_markup.request_shutdown()

    # =========================================================================
    # Variable tests (Blueprint CDO)
    # =========================================================================

    def _test1_basic_inline(self):
        """Test1: Integer variable with Default attribute."""
        if not self._uw:
            self._fail("Test1_BasicInline", "Cannot get user widget")
            return
        try:
            val = self._uw.get_editor_property("Test1_BasicInline")
            if val == 42:
                self._pass("Test1_BasicInline", "Integer = 42")
            else:
                self._fail("Test1_BasicInline", f"expected 42, got {val}")
        except Exception as e:
            self._fail("Test1_BasicInline", str(e))

    def _test2_string_default(self):
        """Test2: String variable with Default attribute."""
        if not self._uw:
            self._fail("Test2_StringDefault", "Cannot get user widget")
            return
        try:
            val = self._uw.get_editor_property("Test2_StringDefault")
            if str(val) == "hello":
                self._pass("Test2_StringDefault", "String = 'hello'")
            else:
                self._fail("Test2_StringDefault", f"expected 'hello', got '{val}'")
        except Exception as e:
            self._fail("Test2_StringDefault", str(e))

    def _test3_struct_inline(self):
        """Test3: Vector2D struct variable with inline X/Y children."""
        if not self._uw:
            self._fail("Test3_StructInline", "Cannot get user widget")
            return
        try:
            val = self._uw.get_editor_property("Test3_StructInline")
            x = getattr(val, 'x', None) if val else None
            y = getattr(val, 'y', None) if val else None
            if x == 1.0 and y == 2.0:
                self._pass("Test3_StructInline", "Vector2D(1.0, 2.0)")
            else:
                self._fail("Test3_StructInline", f"expected (1.0,2.0), got ({x},{y})")
        except Exception as e:
            self._fail("Test3_StructInline", str(e))

    def _test4_object_path_reference(self):
        """Test4: Texture2D loaded from asset path (StaticLoadObject)."""
        if not self._uw:
            self._fail("Test4_ObjectPathReference", "Cannot get user widget")
            return
        try:
            val = self._uw.get_editor_property("Test4_ObjectPathReference")
            if val is not None:
                self._pass("Test4_ObjectPathReference", "Texture2D loaded from path")
            else:
                self._fail("Test4_ObjectPathReference", "Texture2D is None")
        except Exception as e:
            self._fail("Test4_ObjectPathReference", str(e))

    def _test5_object_inline(self):
        """Test5: Texture2D created via NewObject (empty element)."""
        if not self._uw:
            self._fail("Test5_ObjectInline", "Cannot get user widget")
            return
        try:
            val = self._uw.get_editor_property("Test5_ObjectInline")
            if val is not None:
                self._pass("Test5_ObjectInline", "Texture2D NewObject created")
            else:
                self._fail("Test5_ObjectInline", "Texture2D is None")
        except Exception as e:
            self._fail("Test5_ObjectInline", str(e))

    def _test6_object_sub_property(self):
        """Test6: Texture2D inline with LODBias child element."""
        if not self._uw:
            self._fail("Test6_ObjectSubProperty", "Cannot get user widget")
            return
        try:
            val = self._uw.get_editor_property("Test6_ObjectSubProperty")
            if val is not None:
                lod_bias = getattr(val, 'lod_bias', None)
                if lod_bias == 2:
                    self._pass("Test6_ObjectSubProperty", "LODBias = 2")
                else:
                    self._fail("Test6_ObjectSubProperty", f"expected LODBias=2, got {lod_bias}")
            else:
                self._fail("Test6_ObjectSubProperty", "Texture2D is None")
        except Exception as e:
            self._fail("Test6_ObjectSubProperty", str(e))

    def _test7_object_variable(self):
        """Test7: Object:Texture2D variable loaded from asset path."""
        if not self._uw:
            self._fail("Test7_ObjectVariable", "Cannot get user widget")
            return
        try:
            val = self._uw.get_editor_property("Test7_ObjectVariable")
            if val is not None:
                self._pass("Test7_ObjectVariable", "Texture2D loaded from path")
            else:
                self._fail("Test7_ObjectVariable", "Texture2D is None")
        except Exception as e:
            self._fail("Test7_ObjectVariable", str(e))

    def _test8_array_object_variable(self):
        """Test8: Array<Object:Texture2D> with path-ref and inline items."""
        if not self._uw:
            self._fail("Test8_ArrayObjectVariable", "Cannot get user widget")
            return
        try:
            val = self._uw.get_editor_property("Test8_ArrayObjectVariable")
            if val is not None and len(val) == 2:
                item0 = val[0]
                item1 = val[1]
                if item0 is not None and item1 is not None:
                    self._pass("Test8_ArrayObjectVariable", "Array has 2 Texture2D items")
                else:
                    self._fail("Test8_ArrayObjectVariable", f"item0={item0}, item1={item1}")
            else:
                self._fail("Test8_ArrayObjectVariable", f"expected array of 2, got {len(val) if val else 0}")
        except Exception as e:
            self._fail("Test8_ArrayObjectVariable", str(e))

    # =========================================================================
    # Property tests (WidgetTree)
    # =========================================================================

    def _find_widget(self, name):
        """Find a widget by name using the native helper."""
        return widget_markup.find_widget_in_user_widget(self._uw, name)

    def _test9_basic_attribute(self):
        """Test9: TextBlock.Text property set via attribute."""
        w = self._find_widget("Test9_BasicAttribute")
        if not w:
            self._fail("Test9_BasicAttribute", "widget not found")
            return
        try:
            text = w.get_editor_property("Text")
            actual = str(text)
            if "Basic-ok" in actual:
                self._pass("Test9_BasicAttribute", "Text = 'Basic-ok'")
            else:
                self._fail("Test9_BasicAttribute", f"expected 'Basic-ok', got '{actual}'")
        except Exception as e:
            self._fail("Test9_BasicAttribute", str(e))

    def _test10_struct_inline_property(self):
        """Test10: Font struct property with Size sub-element."""
        w = self._find_widget("Test10_StructInline")
        if not w:
            self._fail("Test10_StructInline", "widget not found")
            return
        font = w.font
        if not font:
            self._fail("Test10_StructInline", "font is None")
            return
        size = font.size
        if size == 18:
            self._pass("Test10_StructInline", "Font.Size = 18")
        else:
            self._fail("Test10_StructInline", f"expected Font.Size=18, got {size}")

    def _test11_basic_array(self):
        """Test11: GridPanel ColumnFill + RowFill float arrays."""
        grid = self._find_widget("Test11_BasicArray")
        if not grid:
            self._fail("Test11_BasicArray", "GridPanel not found")
            return
        col_fill = grid.get_editor_property("ColumnFill")
        row_fill = grid.get_editor_property("RowFill")
        col_ok = col_fill and len(col_fill) == 2
        row_ok = row_fill and len(row_fill) == 1
        if col_ok and row_ok:
            self._pass("Test11_BasicArray", "ColumnFill=2, RowFill=1")
        else:
            self._fail("Test11_BasicArray",
                       f"ColumnFill={len(col_fill) if col_fill else 0}, RowFill={len(row_fill) if row_fill else 0}")

    def _pass(self, name, detail):
        """Log a PASS result."""
        unreal.log(f"[PASS] {name}: {detail}")
        self._passed += 1

    def _fail(self, name, detail):
        """Log a FAIL result."""
        unreal.log(f"[FAIL] {name}: {detail}")
        self._failed += 1
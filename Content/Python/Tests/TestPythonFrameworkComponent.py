import unittest
import unreal

import widget_markup

from WidgetMarkupComponent import WidgetMarkupComponent
from Tests.TestReactive import ReactiveTests
from Tests.TestWidgetMarkupComponent import WidgetMarkupComponentTests
from Tests.TestObservableCollection import ObservableCollectionTests


class TestPythonFrameworkComponent(WidgetMarkupComponent):
    def __init__(self):
        super().__init__()
        unreal.log("=== Unit Tests ===")
        loader = unittest.TestLoader()
        suite = unittest.TestSuite()
        suite.addTests(loader.loadTestsFromTestCase(ReactiveTests))
        suite.addTests(loader.loadTestsFromTestCase(WidgetMarkupComponentTests))
        suite.addTests(loader.loadTestsFromTestCase(ObservableCollectionTests))
        runner = unittest.TextTestRunner(verbosity=2)
        result = runner.run(suite)
        if result.wasSuccessful():
            unreal.log_warning("[TestPythonFramework] ALL CHECKS PASSED.")
        else:
            unreal.log_error("[TestPythonFramework] FAIL: one or more unit tests failed.")
        widget_markup.request_shutdown()

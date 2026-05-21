from __future__ import annotations

import sys
from pathlib import Path

import unreal


initialized_attr = "_widget_markup_python_integration_initialized"
if getattr(sys, initialized_attr, False):
    unreal.log("WidgetMarkupPythonIntegration init_unreal.py: already initialized.")
else:
    setattr(sys, initialized_attr, True)

    python_dir = Path(__file__).resolve().parent
    python_path = python_dir.as_posix()
    if python_path not in sys.path:
        sys.path.insert(0, python_path)

    try:
        import WidgetMarkupComponent  # noqa: F401
    except Exception as error:
        unreal.log_warning(
            f"WidgetMarkupPythonIntegration init_unreal.py: failed to import WidgetMarkupComponent from '{python_path}': {error}"
        )
    else:
        unreal.log(
            f"WidgetMarkupPythonIntegration init_unreal.py: registered Python path '{python_path}'."
        )
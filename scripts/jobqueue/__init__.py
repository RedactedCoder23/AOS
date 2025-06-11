import importlib, sys
# Ensure stdlib 'queue' module remains available when this package is on the path
if 'queue' not in sys.modules or sys.modules['queue'].__spec__.origin.endswith(__file__):
    sys.modules['queue'] = importlib.import_module('queue')

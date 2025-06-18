/* AOS — src/api/events.py — Purpose: Python module */
# AOS — ./src/api/events.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
listeners = []


def register(fn):
    """Register event listener function."""
    listeners.append(fn)


def unregister(fn):
    try:
        listeners.remove(fn)
    except ValueError:
        pass


def emit(event):
    for fn in list(listeners):
        try:
            fn(event)
        except Exception:
            pass

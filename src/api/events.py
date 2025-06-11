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

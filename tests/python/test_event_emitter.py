import unittest

from src.api import events


class EventEmitterTest(unittest.TestCase):
    def test_emit_once(self):
        seen = []

        def listener(evt):
            seen.append(evt)

        events.register(listener)
        try:
            events.emit({"a": 1})
        finally:
            events.unregister(listener)
        self.assertEqual(len(seen), 1)
        self.assertEqual(seen[0]["a"], 1)


if __name__ == "__main__":
    unittest.main()

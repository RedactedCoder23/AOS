import unittest
from src.branch.merge import three_way_merge


class MergeTest(unittest.TestCase):
    def test_trivial(self):
        merged, conflicts = three_way_merge("a", "a", "a")
        self.assertEqual(merged.strip(), "a")
        self.assertFalse(conflicts)


if __name__ == "__main__":
    unittest.main()

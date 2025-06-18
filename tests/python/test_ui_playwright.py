/* AOS — tests/python/test_ui_playwright.py — Purpose: Python module */
# AOS — ./tests/python/test_ui_playwright.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import unittest
import os
import sys

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")))

try:
    from playwright.sync_api import sync_playwright
except Exception:  # pragma: no cover - optional dependency
    sync_playwright = None


class BranchUITest(unittest.TestCase):
    def test_create_and_merge(self):
        if sync_playwright is None:
            self.skipTest("playwright not installed")
        with sync_playwright() as p:
            browser = p.chromium.launch()
            page = browser.new_page()
            page.goto("http://localhost:8000")
            page.click("text=New Branch")
            page.wait_for_selector("svg")
            page.wait_for_selector("circle")
            page.click("text=Merge")
            page.wait_for_selector("text=Merge succeeded.")
            browser.close()

    def test_run_agents(self):
        if sync_playwright is None:
            self.skipTest("playwright not installed")
        with sync_playwright() as p:
            browser = p.chromium.launch()
            page = browser.new_page()
            page.goto("http://localhost:8000")
            page.click("text=Run Agents")
            page.wait_for_selector("text=success")
            page.wait_for_selector("text=failed")
            browser.close()


if __name__ == "__main__":
    unittest.main()

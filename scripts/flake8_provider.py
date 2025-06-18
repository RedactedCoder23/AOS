/* AOS — scripts/flake8_provider.py — Purpose: Python module */
# AOS — ./scripts/flake8_provider.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
import ast


class ProviderGenerateChecker:
    """Flake8 checker ensuring AI providers override generate()."""

    name = "provider-generate-checker"
    version = "0.1"

    def __init__(self, tree):
        self.tree = tree

    def run(self):
        for node in ast.walk(self.tree):
            if isinstance(node, ast.ClassDef):
                bases = [
                    getattr(base, "id", getattr(base, "attr", None))
                    for base in node.bases
                ]
                if "AIProvider" in bases:
                    if not any(
                        isinstance(n, ast.FunctionDef) and n.name == "generate"
                        for n in node.body
                    ):
                        msg = "PG001 provider missing generate() override"
                        yield (node.lineno, node.col_offset, msg, type(self))

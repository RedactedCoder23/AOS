# AOS — scripts/ai_providers/mock_provider.py — Purpose: Python module 
# (c) 2025 RedactedCoder23
# Brief: Source file.
from .base import AIProvider


class MockProvider(AIProvider):
    """Mock provider returning a fixed response."""

    def generate(self, prompt: str) -> str:  # pragma: no cover - simple value
        return "mock-response"

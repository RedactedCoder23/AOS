from .base import AIProvider


class MockProvider(AIProvider):
    """Mock provider returning a fixed response."""

    def generate(self, prompt: str) -> str:  # pragma: no cover - simple value
        return "mock-response"

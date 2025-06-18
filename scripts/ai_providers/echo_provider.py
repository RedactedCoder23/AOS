# AOS — scripts/ai_providers/echo_provider.py — Purpose: Python module 
# (c) 2025 RedactedCoder23
# Brief: Source file.
from .base import AIProvider


class EchoProvider(AIProvider):
    """Provider that simply echoes the prompt."""

    def generate(self, prompt: str) -> str:
        return prompt

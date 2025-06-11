from .base import AIProvider


class OpenAIProvider(AIProvider):
    """Dummy OpenAI provider that echoes the prompt."""

    def generate(self, prompt: str) -> str:
        return prompt

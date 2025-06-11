from scripts.ai_providers.base import AIProvider


class EchoProvider(AIProvider):
    """Simple provider that echoes the prompt."""

    def generate(self, prompt: str) -> str:
        return prompt

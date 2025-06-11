from .base import AIProvider


class EchoProvider(AIProvider):
    """Provider that simply echoes the prompt."""

    def generate(self, prompt: str) -> str:
        return prompt

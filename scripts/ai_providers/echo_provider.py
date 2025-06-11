<<<<<< codex/add-echo-and-openai-provider-plugins
from scripts.ai_providers.base import AIProvider


class EchoProvider(AIProvider):
    """Simple provider that echoes the prompt."""

    def generate(self, prompt: str) -> str:
=======
from .base import AIProvider


class EchoProvider(AIProvider):
    """Provider that simply echoes the prompt."""

    def generate(self, prompt: str) -> str:  # pragma: no cover - trivial
>>>>>> main
        return prompt

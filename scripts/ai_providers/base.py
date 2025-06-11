class AIProvider:
    """Base AI provider interface."""

    def __init__(self, name: str):
        self.name = name

    def generate(self, prompt: str) -> str:  # pragma: no cover - to be implemented
        raise NotImplementedError

from __future__ import annotations

"""Base classes for AI provider plugins."""

from abc import ABC, abstractmethod


class AIProvider(ABC):
    """Abstract provider interface."""

    def __init__(self, name: str):
        self.name = name

    @abstractmethod
    def generate(self, prompt: str) -> str:
        """Return text generated for ``prompt``."""
        raise NotImplementedError

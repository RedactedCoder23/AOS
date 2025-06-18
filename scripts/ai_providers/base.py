/* AOS — scripts/ai_providers/base.py — Purpose: Python module */
# AOS — ./scripts/ai_providers/base.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
from __future__ import annotations

"""Base classes for AI provider plugins."""

from abc import ABC, abstractmethod


class AIProvider(ABC):
    """Abstract provider interface."""

    def __init_subclass__(cls, **kwargs):
        super().__init_subclass__(**kwargs)
        # warn if subclass fails to override generate()
        if cls.generate is AIProvider.generate:
            import warnings

            warnings.warn(f"{cls.__name__} must override generate()", RuntimeWarning)

    def __init__(self, name: str):
        self.name = name

    @abstractmethod
    def generate(self, prompt: str) -> str:
        """Return text generated for ``prompt``."""
        raise NotImplementedError

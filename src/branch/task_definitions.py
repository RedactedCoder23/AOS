# AOS — ./src/branch/task_definitions.py
# (c) 2025 RedactedCoder23
# Brief: Source file.
from dataclasses import dataclass, field
from typing import List, Optional


@dataclass
class Task:
    agent_id: str
    command: str
    depends_on: List[str] = field(default_factory=list)
    priority: int = 100
    provider: Optional[str] = None

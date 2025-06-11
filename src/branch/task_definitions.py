from dataclasses import dataclass, field
from typing import List, Optional


@dataclass
class Task:
    agent_id: str
    command: str
    depends_on: List[str] = field(default_factory=list)
    priority: int = 100
    provider: Optional[str] = None

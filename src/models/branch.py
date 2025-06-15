from dataclasses import dataclass, field
from typing import List


@dataclass
class Branch:
    id: int
    name: str
    created_by: str = "unknown"
    created_at: float = 0.0
    tags: List[str] = field(default_factory=list)

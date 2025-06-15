from typing import List, Tuple, Dict


def three_way_merge(base: str, ours: str, theirs: str) -> Tuple[str, List[Dict]]:
    base_lines = base.splitlines()
    ours_lines = ours.splitlines()
    theirs_lines = theirs.splitlines()
    max_len = max(len(base_lines), len(ours_lines), len(theirs_lines))
    merged: List[str] = []
    conflicts: List[Dict] = []
    for i in range(max_len):
        b = base_lines[i] if i < len(base_lines) else ""
        o = ours_lines[i] if i < len(ours_lines) else ""
        t = theirs_lines[i] if i < len(theirs_lines) else ""
        if o == t:
            merged.append(o)
        elif o == b:
            merged.append(t)
        elif t == b:
            merged.append(o)
        else:
            merged.append(f"<<<<<< ours\n{o}\n======\n{t}\n>>>>>> theirs")
            conflicts.append({"line": i, "base": b, "ours": o, "theirs": t})
    return "\n".join(merged), conflicts

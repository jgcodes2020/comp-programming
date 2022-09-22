from pathlib import Path
import re
from typing import Any

_REGEX_CONFIG_LINE = re.compile(r"^([A-Za-z0-9_]+)\s*=\s*(.+)$", re.M)
_REGEX_COMMENT = re.compile(r"#.+$")

__config_dict: dict[str, str] = {}

with open(Path(__file__).parent / "settings.ini") as f:
    for line in f:
        stripped = _REGEX_COMMENT.sub("", line[0:-1])
        
        if len(stripped) == 0 or stripped.isspace():
            continue
            
        if (match := _REGEX_CONFIG_LINE.match(stripped)) is not None:
            __config_dict[match.group(1)] = match.group(2)
        else:
            print(f"Line: '{stripped}'")
            exit(50)
            
def get_cfg(name: str, default: Any = None) -> str | Any:
    return __config_dict[name] if (name in __config_dict) else default
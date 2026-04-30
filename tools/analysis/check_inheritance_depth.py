from __future__ import annotations

import argparse
import json
import re
from pathlib import Path
from typing import Any, Optional


INHERITANCE_PATTERN = re.compile(
    r"\b(?:class|struct)\s+(?P<derived>[A-Za-z_][A-Za-z0-9_:]*)\s*:\s*"
    r"(?:(?:public|private|protected)\s+)?(?P<base>[A-Za-z_][A-Za-z0-9_:]*)"
)


def scan_inheritance(paths: list[Path]) -> dict[str, str]:
    graph: dict[str, str] = {}
    for path in _iter_source_files(paths):
        text = path.read_text(encoding="utf-8", errors="ignore")
        for match in INHERITANCE_PATTERN.finditer(text):
            graph[_short_name(match.group("derived"))] = _short_name(match.group("base"))
    return graph


def analyze_inheritance_depth(graph: dict[str, str], *, max_depth: int) -> dict[str, Any]:
    depths = {name: _depth(name, graph, set()) for name in graph}
    violations = [
        {"class": name, "depth": depth, "base": graph[name]}
        for name, depth in sorted(depths.items())
        if depth > max_depth
    ]
    return {
        "max_allowed_depth": max_depth,
        "max_observed_depth": max(depths.values(), default=0),
        "class_count_with_bases": len(graph),
        "violations": violations,
        "passed": not violations,
    }


def _iter_source_files(paths: list[Path]):
    suffixes = {".hpp", ".h", ".cpp", ".cc", ".cxx"}
    for root in paths:
        if root.is_file() and root.suffix in suffixes:
            yield root
        elif root.is_dir():
            for path in root.rglob("*"):
                if path.is_file() and path.suffix in suffixes:
                    yield path


def _short_name(name: str) -> str:
    return name.split("::")[-1]


def _depth(name: str, graph: dict[str, str], seen: set[str]) -> int:
    if name in seen:
        return 1
    base = graph.get(name)
    if base is None:
        return 0
    seen.add(name)
    return 1 + _depth(base, graph, seen)


def main(argv: Optional[list[str]] = None) -> int:
    parser = argparse.ArgumentParser(description="Check C++ inheritance depth in the repository.")
    parser.add_argument("paths", nargs="*", type=Path, default=[Path("include"), Path("src")])
    parser.add_argument("--max-depth", type=int, default=3)
    parser.add_argument("--format", choices=("json", "text"), default="text")
    args = parser.parse_args(argv)

    analysis = analyze_inheritance_depth(scan_inheritance(args.paths), max_depth=args.max_depth)
    if args.format == "json":
        print(json.dumps(analysis, indent=2, sort_keys=True))
    else:
        print(
            "passed={passed}\nmax_allowed_depth={max_allowed_depth}\n"
            "max_observed_depth={max_observed_depth}\nclass_count_with_bases={class_count_with_bases}".format(
                **analysis
            )
        )
        for violation in analysis["violations"]:
            print(f"violation={violation['class']} depth={violation['depth']}")
    return 0 if analysis["passed"] else 1


if __name__ == "__main__":
    raise SystemExit(main())

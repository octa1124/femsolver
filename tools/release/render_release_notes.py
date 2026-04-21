#!/usr/bin/env python3
"""Render a draft release summary from changelog fragments."""

from __future__ import annotations

import argparse
from collections import defaultdict
from pathlib import Path
from typing import Any

import yaml


def load_fragments(directory: Path) -> list[dict[str, Any]]:
    fragments: list[dict[str, Any]] = []
    for path in sorted(directory.glob("*.y*ml")):
        fragments.append(yaml.safe_load(path.read_text(encoding="utf-8")))
    return fragments


def render_release_notes(version: str, fragments: list[dict[str, Any]]) -> str:
    grouped: dict[str, list[str]] = defaultdict(list)
    for fragment in fragments:
        grouped[str(fragment["type"])].append(f"- {fragment['scope']}: {fragment['summary']}")

    lines = [f"# {version} Release Draft", "", "## Completed", ""]
    for category in sorted(grouped):
        lines.append(f"### {category.capitalize()}")
        lines.extend(grouped[category])
        lines.append("")

    lines.extend(
        [
            "## Not Completed",
            "",
            "- Fill in version-specific deferred work before release.",
            "",
            "## Known Risks",
            "",
            "- Fill in known risks before release.",
            "",
            "## Compatibility Changes",
            "",
            "- Fill in compatibility notes before release.",
            "",
            "## Next Version Handoff",
            "",
            "- Fill in next-version handoff items before release.",
        ]
    )
    return "\n".join(lines) + "\n"


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--version", required=True, help="Version string to render.")
    parser.add_argument(
        "--fragment-dir",
        type=Path,
        default=Path("changes/unreleased"),
        help="Directory containing changelog fragments.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=None,
        help="Optional file to write. Defaults to stdout if omitted.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    fragments = load_fragments(args.fragment_dir)
    text = render_release_notes(args.version, fragments)
    if args.output is None:
        print(text, end="")
    else:
        args.output.write_text(text, encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

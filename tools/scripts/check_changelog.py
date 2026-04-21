#!/usr/bin/env python3
"""Validate changelog fragments stored under changes/unreleased/."""

from __future__ import annotations

import argparse
from pathlib import Path
from typing import Any

import yaml

ALLOWED_TYPES = {"added", "changed", "fixed", "removed", "docs", "test", "build"}
REQUIRED_KEYS = {"type", "scope", "summary", "breaking"}


def load_fragment(path: Path) -> dict[str, Any]:
    data = yaml.safe_load(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError(f"{path} must contain a mapping")
    return data


def validate_fragment(path: Path) -> list[str]:
    data = load_fragment(path)
    errors: list[str] = []
    missing = REQUIRED_KEYS - data.keys()
    if missing:
        errors.append(f"{path}: missing keys {sorted(missing)}")
    if data.get("type") not in ALLOWED_TYPES:
        errors.append(f"{path}: invalid type {data.get('type')!r}")
    if not isinstance(data.get("scope"), str) or not data["scope"].strip():
        errors.append(f"{path}: scope must be a non-empty string")
    if not isinstance(data.get("summary"), str) or not data["summary"].strip():
        errors.append(f"{path}: summary must be a non-empty string")
    if not isinstance(data.get("breaking"), bool):
        errors.append(f"{path}: breaking must be a boolean")
    return errors


def collect_fragment_paths(directory: Path) -> list[Path]:
    return sorted(path for path in directory.glob("*.y*ml") if path.is_file())


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--directory",
        type=Path,
        default=Path("changes/unreleased"),
        help="Directory containing changelog fragments.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    fragment_paths = collect_fragment_paths(args.directory)
    if not fragment_paths:
        print(f"No changelog fragments found in {args.directory}")
        return 1

    errors: list[str] = []
    for path in fragment_paths:
        errors.extend(validate_fragment(path))

    if errors:
        for error in errors:
            print(error)
        return 1

    print(f"Validated {len(fragment_paths)} changelog fragment(s) in {args.directory}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

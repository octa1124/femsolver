#!/usr/bin/env python3
"""Render or apply GitHub repository bootstrap actions."""

from __future__ import annotations

import argparse
import json
import subprocess
from pathlib import Path
from typing import Any

import yaml

REPO_SETTINGS_DIR = Path(".github/repository-settings")


def load_yaml_file(path: Path) -> dict[str, Any]:
    data = yaml.safe_load(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError(f"{path} must contain a mapping")
    return data


def load_json_file(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError(f"{path} must contain a JSON object")
    return data


def render_label_commands(repo: str, labels: list[dict[str, Any]]) -> list[list[str]]:
    commands: list[list[str]] = []
    for label in labels:
        commands.append(
            [
                "gh",
                "label",
                "create",
                label["name"],
                "--repo",
                repo,
                "--color",
                str(label["color"]),
                "--description",
                str(label["description"]),
                "--force",
            ]
        )
    return commands


def render_milestone_commands(repo: str, milestones: list[dict[str, Any]]) -> list[list[str]]:
    commands: list[list[str]] = []
    for milestone in milestones:
        commands.append(
            [
                "gh",
                "api",
                f"repos/{repo}/milestones",
                "--method",
                "POST",
                "--field",
                f"title={milestone['title']}",
                "--field",
                f"description={milestone['description']}",
            ]
        )
    return commands


def render_ruleset_command(repo: str, ruleset_path: Path) -> list[str]:
    return [
        "gh",
        "api",
        f"repos/{repo}/rulesets",
        "--method",
        "POST",
        "--input",
        str(ruleset_path),
    ]


def execute_commands(commands: list[list[str]]) -> None:
    for command in commands:
        subprocess.run(command, check=True)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", required=True, help="GitHub repository in owner/name form.")
    parser.add_argument("--apply", action="store_true", help="Run GitHub CLI commands.")
    parser.add_argument("--dry-run", action="store_true", help="Print commands without running them.")
    return parser.parse_args()


def main() -> int:
    args = parse_args()

    labels_cfg = load_yaml_file(REPO_SETTINGS_DIR / "labels.yaml")
    milestones_cfg = load_yaml_file(REPO_SETTINGS_DIR / "milestones.yaml")
    _ = load_json_file(REPO_SETTINGS_DIR / "ruleset-main.json")

    label_commands = render_label_commands(args.repo, labels_cfg["labels"])
    milestone_commands = render_milestone_commands(args.repo, milestones_cfg["milestones"])
    ruleset_command = render_ruleset_command(args.repo, REPO_SETTINGS_DIR / "ruleset-main.json")

    all_commands = label_commands + milestone_commands + [ruleset_command]

    if args.dry_run or not args.apply:
        for command in all_commands:
            print(" ".join(command))
        return 0

    execute_commands(label_commands + milestone_commands)
    print("Labels and milestones applied. Apply the ruleset command manually after verification:")
    print(" ".join(ruleset_command))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

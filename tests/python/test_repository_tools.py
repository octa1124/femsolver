from __future__ import annotations

import importlib.util
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]


def load_module(relative_path: str, module_name: str):
    module_path = REPO_ROOT / relative_path
    spec = importlib.util.spec_from_file_location(module_name, module_path)
    assert spec is not None
    assert spec.loader is not None
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def test_changelog_validator_accepts_bootstrap_fragment() -> None:
    module = load_module("tools/scripts/check_changelog.py", "check_changelog")
    fragment = REPO_ROOT / "changes/unreleased/0001-infra-bootstrap.yaml"

    errors = module.validate_fragment(fragment)
    assert errors == []


def test_release_renderer_groups_fragment_by_type() -> None:
    module = load_module("tools/release/render_release_notes.py", "render_release_notes")
    text = module.render_release_notes(
        "v0.1.0",
        [
            {
                "type": "added",
                "scope": "infra",
                "summary": "Bootstrap the repo",
                "breaking": False,
            }
        ],
    )

    assert "# v0.1.0 Release Draft" in text
    assert "### Added" in text
    assert "- infra: Bootstrap the repo" in text


def test_github_bootstrap_renders_expected_commands() -> None:
    module = load_module("tools/scripts/github_bootstrap.py", "github_bootstrap")
    labels_cfg = module.load_yaml_file(REPO_ROOT / ".github/repository-settings/labels.yaml")
    milestones_cfg = module.load_yaml_file(REPO_ROOT / ".github/repository-settings/milestones.yaml")

    label_commands = module.render_label_commands("octa1124/femsolver", labels_cfg["labels"])
    milestone_commands = module.render_milestone_commands("octa1124/femsolver", milestones_cfg["milestones"])
    ruleset_command = module.render_ruleset_command(
        "octa1124/femsolver", REPO_ROOT / ".github/repository-settings/ruleset-main.json"
    )

    assert label_commands[0][:3] == ["gh", "label", "create"]
    assert milestone_commands[0][:3] == ["gh", "api", "repos/octa1124/femsolver/milestones"]
    assert ruleset_command[:3] == ["gh", "api", "repos/octa1124/femsolver/rulesets"]

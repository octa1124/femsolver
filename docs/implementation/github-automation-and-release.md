# GitHub Automation And Release Flow

## Scope

This document explains how GitHub automation, repository bootstrap metadata, review bots, and release tooling are wired in `v0.1.0`.

## Implemented Files

- `.github/workflows/ci.yml`
- `.github/workflows/nightly.yml`
- `.github/workflows/release.yml`
- `.github/repository-settings/labels.yaml`
- `.github/repository-settings/milestones.yaml`
- `.github/repository-settings/ruleset-main.json`
- `.coderabbit.yaml`
- `sonar-project.properties`
- `tools/scripts/github_bootstrap.py`
- `tools/release/render_release_notes.py`

## CI Design

### PR And Mainline CI

The CI workflow runs:

- debug configure/build/test
- ASan configure/build/test
- Python tooling checks
- coverage generation and Codecov upload
- SonarQube scan when secrets are configured

This creates a baseline gate that is meaningful today and expandable later.

### Nightly

The nightly workflow currently runs the baseline checks plus release-note rendering. Later versions will replace this placeholder scope with benchmark regressions, convergence checks, and performance tracking.

### Release Workflow

The release workflow validates changelog fragments and renders a draft release note artifact. This is intentionally simple for `v0.1.0`, but it already matches the release-summary model documented in the roadmap.

## GitHub Bootstrap Metadata

Repository settings that need admin access are stored declaratively:

- labels
- milestones
- branch ruleset

`tools/scripts/github_bootstrap.py` can render or apply the corresponding GitHub CLI commands. This keeps repository intent under version control even when the final change must happen through GitHub APIs.

## Review And Quality Bots

- `CodeRabbit` is configured as the primary AI review bot.
- `SonarQube Cloud` is configured as the static quality gate.
- `Codecov` is configured in CI as the coverage reporter.

The bot configuration is constrained by `Agent.md`, so review guidance is repository-specific rather than generic.

## Deferred Work

- authenticated bootstrap execution against the live GitHub repository
- required-check tuning once CI names are finalized on GitHub
- release tagging automation tied to an authenticated publish step

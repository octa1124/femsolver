# Governance And Documentation Workflow

## Scope

This document explains how repository governance, implementation documentation, changelog management, and release summaries work together.

## Implemented Files

- `Agent.md`
- `CHANGELOG.md`
- `changes/unreleased/*.yaml`
- `docs/releases/v0.1.0.md`
- `.github/pull_request_template.md`
- `.github/ISSUE_TEMPLATE/release_checklist.yml`

## Process Design

### Engineering Rules

`Agent.md` is the highest-priority repository rule file. It defines:

- repository mission and version scope
- coding and testing expectations
- documentation obligations
- release and git-history requirements

This avoids scattering enforcement across many independent notes.

### Implementation Documentation

`docs/implementation/` is the required home for implementation-level technical notes.

Each note should explain:

- what subsystem changed
- why the implementation was shaped this way
- which files define the behavior
- what is not implemented yet
- how the work is tested or validated

### Changelog Flow

Each PR must add one fragment under `changes/unreleased/`. The fragment format is validated by `tools/scripts/check_changelog.py`. This allows release notes to be built incrementally rather than reconstructed later.

### Release Summary

Each version has a curated release note under `docs/releases/`. This is where completed work, deferred work, risks, compatibility changes, and handoff items are written for humans.

### Version Completion Rule

A version is only considered complete when:

- the code is merged
- the implementation docs match the shipped state
- the changelog is updated
- the release summary is updated
- the finished state is captured in git with a dedicated commit

## Why This Matters

The solver roadmap is long and physics-heavy. Without strict documentation hygiene, it becomes difficult to recover why a numerical strategy, workflow, or repository rule was chosen. This baseline makes future solver work auditable.

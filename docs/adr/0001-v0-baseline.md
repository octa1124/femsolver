# ADR 0001: Establish The v0.1.0 Repository Baseline

## Status

Accepted

## Context

The repository started effectively empty. The project needs a durable engineering baseline before production physics can be implemented safely.

## Decision

Create `v0.1.0` as a repository-baseline milestone that includes:

- governance documents
- changelog and release-note workflow
- target-based CMake bootstrap
- unit and tooling tests
- GitHub Actions CI
- declarative GitHub repository settings for labels, milestones, and rulesets
- CodeRabbit, SonarQube Cloud, and Codecov configuration

## Consequences

- The repository gains a stable foundation for future solver work.
- Production MFEM integration is deferred to `v1.0.0`.
- Some GitHub-side settings still require admin credentials and cannot be fully enforced from local files alone.

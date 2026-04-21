# GitHub Administration Notes

Some repository-management tasks require GitHub-side admin access. This repository stores the desired state locally and provides scripts to help apply it.

## Declarative Inputs

- Labels: `.github/repository-settings/labels.yaml`
- Milestones: `.github/repository-settings/milestones.yaml`
- Main-branch ruleset: `.github/repository-settings/ruleset-main.json`

## Bootstrap Script

Use the helper script in dry-run mode first:

```bash
python tools/scripts/github_bootstrap.py --repo octa1124/femsolver --dry-run
```

With GitHub CLI authentication and admin access, apply labels and milestones:

```bash
python tools/scripts/github_bootstrap.py --repo octa1124/femsolver --apply
```

## Required Apps And Secrets

- Install the `CodeRabbit` GitHub App on the repository.
- Configure `SONAR_TOKEN` and `SONAR_HOST_URL` if your SonarQube Cloud setup requires them.
- Configure `CODECOV_TOKEN` if tokenless upload is not available for your usage pattern.

## Required Rules

The main branch should require:

- pull-request-only merges
- all required status checks
- resolved review conversations
- no direct pushes
- changelog fragment enforcement

# Analysis Tooling

This directory owns non-visual analysis helpers for solver and regression outputs.

Current tools:

- `analyze_machine_report.py`: parses `motor_solve` or `motor_check` text output and emits pass/fail checks plus a completion score.
- `check_inheritance_depth.py`: checks C++ inheritance depth and enforces the current style preference of keeping inheritance shallow.
- `version_readiness.py`: converts feature/test evidence into `v1.0` through `v2.0` readiness scores and missing-feature lists.

The analysis layer is deliberately text and JSON oriented. It is meant for local runs, CI logs, and later release dashboards without requiring visualization.

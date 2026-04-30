from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path
from typing import Any, Optional


SUMMARY_PATTERN = re.compile(r"solution:\s*(?P<summary>.*)")


def parse_solution_summary(text: str) -> dict[str, Any]:
    summary = ""
    for line in text.splitlines():
        match = SUMMARY_PATTERN.search(line)
        if match:
            summary = match.group("summary")
    if not summary and "=" in text:
        summary = text.strip()
    if not summary:
        raise ValueError("No solution summary found in report text")

    result: dict[str, Any] = {}
    for part in summary.split(", "):
        if "=" not in part:
            continue
        key, raw_value = part.split("=", 1)
        result[key.strip()] = _coerce_value(raw_value.strip())
    return result


def analyze_solution_metrics(metrics: dict[str, Any], *, require_torque_surface: bool) -> dict[str, Any]:
    checks = [
        _check("linear_converged", metrics.get("linear_converged") is True),
        _check("positive_primary_dofs", _as_float(metrics.get("primary_dofs")) > 0.0),
        _check("positive_magnetic_energy", _as_float(metrics.get("magnetic_energy")) > 0.0),
        _check(
            "positive_average_flux_density",
            _as_float(metrics.get("average_flux_density_magnitude")) > 0.0,
        ),
        _check(
            "max_flux_density_bounds_average",
            _as_float(metrics.get("max_flux_density_magnitude"))
            >= _as_float(metrics.get("average_flux_density_magnitude")),
        ),
    ]

    torque_faces = _as_float(metrics.get("torque_surface_face_count"))
    torque_check_passed = torque_faces > 0.0
    checks.append(_check("torque_surface_available", torque_check_passed or not require_torque_surface))

    passed_checks = sum(1 for check in checks if check["passed"])
    completion_score = passed_checks / len(checks)
    return {
        "case_id": metrics.get("case_id", "unknown"),
        "status": metrics.get("status", "unknown"),
        "overall_pass": all(check["passed"] for check in checks),
        "completion_score": completion_score,
        "checks": checks,
        "metrics": metrics,
    }


def render_text_analysis(analysis: dict[str, Any]) -> str:
    lines = [
        f"case_id={analysis['case_id']}",
        f"status={analysis['status']}",
        f"overall_pass={str(analysis['overall_pass']).lower()}",
        f"completion_score={analysis['completion_score']:.3f}",
    ]
    for check in analysis["checks"]:
        lines.append(f"check.{check['name']}={str(check['passed']).lower()}")
    return "\n".join(lines)


def _coerce_value(value: str) -> Any:
    if value == "true":
        return True
    if value == "false":
        return False
    try:
        if any(marker in value for marker in (".", "e", "E")):
            return float(value)
        return int(value)
    except ValueError:
        return value


def _as_float(value: Any) -> float:
    if isinstance(value, bool):
        return 1.0 if value else 0.0
    if isinstance(value, (int, float)):
        return float(value)
    return 0.0


def _check(name: str, passed: bool) -> dict[str, Any]:
    return {"name": name, "passed": bool(passed)}


def main(argv: Optional[list[str]] = None) -> int:
    parser = argparse.ArgumentParser(
        description="Analyze text reports from motor_solve or motor_check without visualization."
    )
    parser.add_argument("--input", type=Path, help="Report text file. Reads stdin when omitted.")
    parser.add_argument("--format", choices=("json", "text"), default="json")
    parser.add_argument(
        "--require-torque-surface",
        action="store_true",
        help="Fail analysis when the torque surface is absent.",
    )
    args = parser.parse_args(argv)

    text = args.input.read_text(encoding="utf-8") if args.input else sys.stdin.read()
    analysis = analyze_solution_metrics(
        parse_solution_summary(text),
        require_torque_surface=args.require_torque_surface,
    )
    if args.format == "json":
        print(json.dumps(analysis, indent=2, sort_keys=True))
    else:
        print(render_text_analysis(analysis))
    return 0 if analysis["overall_pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())

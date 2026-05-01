from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any, Optional


VERSION_REQUIREMENTS: dict[str, list[str]] = {
    "v1.0": [
        "tetra_nedelec",
        "linear_magnetostatic",
        "machine_regression",
        "torque_summary",
    ],
    "v1.1": [
        "bh_curve",
        "orthotropic_bh",
        "consistent_tangent_check",
    ],
    "v1.2": [
        "rt_p0_tetra",
        "hdiv_face_flux_benchmark",
        "hdiv_divergence_benchmark",
    ],
    "v1.3": [
        "reference_hex",
        "hex_q1",
        "hex_q2",
        "hex_quadrature",
    ],
    "v2.0": [
        "field_state_offsets",
        "magnetostatic_operator_adapter",
        "magnetothermal_canonical_coupling",
        "analysis_readiness_report",
    ],
}


def analyze_version_readiness(evidence: dict[str, bool]) -> dict[str, Any]:
    versions: dict[str, Any] = {}
    for version, requirements in VERSION_REQUIREMENTS.items():
        checks = [{"name": name, "passed": bool(evidence.get(name, False))} for name in requirements]
        passed_count = sum(1 for check in checks if check["passed"])
        versions[version] = {
            "overall_pass": passed_count == len(checks),
            "completion_score": passed_count / len(checks),
            "missing": [check["name"] for check in checks if not check["passed"]],
            "checks": checks,
        }

    return {
        "overall_pass": all(version["overall_pass"] for version in versions.values()),
        "versions": versions,
    }


def render_text_report(analysis: dict[str, Any]) -> str:
    lines = [f"overall_pass={str(analysis['overall_pass']).lower()}"]
    for version, result in analysis["versions"].items():
        lines.append(f"{version}.pass={str(result['overall_pass']).lower()}")
        lines.append(f"{version}.completion_score={result['completion_score']:.3f}")
        lines.append(f"{version}.missing={','.join(result['missing'])}")
    return "\n".join(lines)


def _parse_evidence_item(item: str) -> tuple[str, bool]:
    if "=" not in item:
        return item, True
    key, raw_value = item.split("=", 1)
    normalized = raw_value.strip().lower()
    if normalized in {"1", "true", "yes", "passed"}:
        return key.strip(), True
    if normalized in {"0", "false", "no", "failed"}:
        return key.strip(), False
    raise ValueError(f"Unsupported evidence value: {item}")


def _load_evidence(path: Path | None, items: list[str]) -> dict[str, bool]:
    evidence: dict[str, bool] = {}
    if path is not None:
        raw = json.loads(path.read_text(encoding="utf-8"))
        if not isinstance(raw, dict):
            raise ValueError("Evidence JSON must be an object mapping feature names to booleans")
        evidence.update({str(key): bool(value) for key, value in raw.items()})
    for item in items:
        key, value = _parse_evidence_item(item)
        evidence[key] = value
    return evidence


def main(argv: Optional[list[str]] = None) -> int:
    parser = argparse.ArgumentParser(
        description="Analyze femsolver version readiness from feature/test evidence."
    )
    parser.add_argument("--evidence-file", type=Path)
    parser.add_argument(
        "--feature",
        action="append",
        default=[],
        help="Feature evidence as name, name=true, or name=false. Can be repeated.",
    )
    parser.add_argument("--format", choices=("json", "text"), default="json")
    args = parser.parse_args(argv)

    analysis = analyze_version_readiness(_load_evidence(args.evidence_file, args.feature))
    if args.format == "json":
        print(json.dumps(analysis, indent=2, sort_keys=True))
    else:
        print(render_text_report(analysis))
    return 0 if analysis["overall_pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))

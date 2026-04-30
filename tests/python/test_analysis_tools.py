from __future__ import annotations

import subprocess
import sys
from pathlib import Path

from tools.analysis.analyze_machine_report import analyze_solution_metrics, parse_solution_summary
from tools.analysis.check_inheritance_depth import analyze_inheritance_depth, scan_inheritance


REPO_ROOT = Path(__file__).resolve().parents[2]


def test_machine_report_analysis_scores_solution_summary() -> None:
    report = (
        "machine regression check\n"
        "solution: case_id=joint_type_i_12s10p, status=linear-magnetostatic-smoke-ok, "
        "magnetic_energy=1.5, torque_estimate=0.02, average_flux_density_magnitude=0.1, "
        "max_flux_density_magnitude=0.3, torque_surface_area=2.0, "
        "torque_surface_face_count=4, primary_dofs=10, linear_iterations=3, "
        "residual_norm=1e-12, linear_converged=true, warnings=2\n"
    )

    analysis = analyze_solution_metrics(
        parse_solution_summary(report),
        require_torque_surface=True,
    )

    assert analysis["overall_pass"] is True
    assert analysis["completion_score"] == 1.0
    assert analysis["case_id"] == "joint_type_i_12s10p"


def test_machine_report_analysis_cli_returns_json(tmp_path: Path) -> None:
    report_path = tmp_path / "machine_report.txt"
    report_path.write_text(
        "solution: case_id=joint_type_i_12s10p, status=linear-magnetostatic-smoke-ok, "
        "magnetic_energy=1, torque_estimate=0, average_flux_density_magnitude=1, "
        "max_flux_density_magnitude=2, torque_surface_area=1, torque_surface_face_count=1, "
        "primary_dofs=3, linear_iterations=1, residual_norm=0, linear_converged=true, warnings=0\n",
        encoding="utf-8",
    )

    completed = subprocess.run(
        [
            sys.executable,
            str(REPO_ROOT / "tools/analysis/analyze_machine_report.py"),
            "--input",
            str(report_path),
            "--require-torque-surface",
        ],
        cwd=REPO_ROOT,
        check=True,
        capture_output=True,
        text=True,
    )

    assert '"overall_pass": true' in completed.stdout
    assert '"completion_score": 1.0' in completed.stdout


def test_inheritance_depth_analysis_accepts_current_repo_style() -> None:
    graph = scan_inheritance([REPO_ROOT / "include", REPO_ROOT / "src"])
    analysis = analyze_inheritance_depth(graph, max_depth=3)

    assert analysis["passed"] is True
    assert analysis["max_observed_depth"] <= 3

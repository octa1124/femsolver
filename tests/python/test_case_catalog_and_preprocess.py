from __future__ import annotations

import subprocess
import sys
from pathlib import Path

import yaml

from tools.gmsh.case_catalog import case_paths, load_case_spec


REPO_ROOT = Path(__file__).resolve().parents[2]
GENERATOR = REPO_ROOT / "tools" / "gmsh" / "generate_joint_motor_case.py"


def test_case_catalog_contains_expected_joint_motor_cases() -> None:
    catalog = case_paths(REPO_ROOT)
    case_ids = [load_case_spec(path)["case_id"] for path in catalog]

    assert case_ids == [
        "exo_outer_rotor_36s40p",
        "joint_type_i_12s10p",
        "quadruped_joint_36s32p",
        "tbm76_envelope",
    ]


def test_joint_case_schema_is_slots_then_poles() -> None:
    case_data = load_case_spec(REPO_ROOT / "cases/machines/joint_type_i_12s10p/case.yaml")

    assert case_data["geometry_parameters"]["slot_count"] == 12
    assert case_data["geometry_parameters"]["pole_count"] == 10
    assert case_data["space_family"] == "nedelec"
    assert "section_2d" in case_data["geometry_variants"]
    assert "extruded_3d" in case_data["geometry_variants"]


def test_preprocess_generator_writes_joint_section_outputs(tmp_path: Path) -> None:
    case_path = REPO_ROOT / "cases/machines/joint_type_i_12s10p/case.yaml"

    completed = subprocess.run(
        [
            sys.executable,
            str(GENERATOR),
            "--case",
            str(case_path),
            "--variant",
            "section_2d",
            "--output-dir",
            str(tmp_path),
        ],
        cwd=REPO_ROOT,
        check=True,
        capture_output=True,
        text=True,
    )

    manifest = yaml.safe_load((tmp_path / "mesh_manifest.yaml").read_text(encoding="utf-8"))
    geometry_text = next(tmp_path.glob("*.geo")).read_text(encoding="utf-8")
    summary_text = (tmp_path / "preprocess_summary.txt").read_text(encoding="utf-8")

    assert manifest["case_id"] == "joint_type_i_12s10p"
    assert manifest["geometry_variant"] == "section_2d"
    assert manifest["slot_count"] == 12
    assert manifest["pole_count"] == 10
    assert manifest["space_family"] == "nedelec"
    assert manifest["mesh_generation"]["status"] in {"generated", "skipped_gmsh_not_available"}
    assert 'Physical Surface("stator_core")' in geometry_text
    assert "slot_count: 12" in geometry_text
    assert "case_id=joint_type_i_12s10p" in summary_text
    assert "mesh_status=" in completed.stdout


def test_preprocess_generator_supports_extruded_envelope_case(tmp_path: Path) -> None:
    case_path = REPO_ROOT / "cases/machines/tbm76_envelope/case.yaml"

    subprocess.run(
        [
            sys.executable,
            str(GENERATOR),
            "--case",
            str(case_path),
            "--variant",
            "extruded_3d",
            "--output-dir",
            str(tmp_path),
        ],
        cwd=REPO_ROOT,
        check=True,
        capture_output=True,
        text=True,
    )

    manifest = yaml.safe_load((tmp_path / "mesh_manifest.yaml").read_text(encoding="utf-8"))
    geometry_text = next(tmp_path.glob("*.geo")).read_text(encoding="utf-8")

    assert manifest["case_id"] == "tbm76_envelope"
    assert manifest["geometry_variant"] == "extruded_3d"
    assert manifest["slot_count"] is None
    assert manifest["space_family"] == "nedelec"
    assert manifest["mesh_generation"]["dimension"] == 3
    assert "airgap_torque_surface" in manifest["boundary_names"]
    assert 'Physical Volume("magnet_ring")' in geometry_text
    assert 'Physical Surface("outer_air_boundary") = {outer_air_extrude[2]' in geometry_text
    assert 'Physical Surface("airgap_torque_surface")' in geometry_text
    assert 'Physical Surface("axial_plus")' in geometry_text

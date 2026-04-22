from __future__ import annotations

from pathlib import Path
from typing import Any

import yaml


ALLOWED_VARIANTS = {"section_2d", "extruded_3d", "full_3d"}
ALLOWED_SOURCE_KINDS = {"paper", "industrial"}
REQUIRED_GEOMETRY_KEYS = {
    "outer_diameter_mm",
    "stator_inner_diameter_mm",
    "rotor_outer_diameter_mm",
    "rotor_inner_diameter_mm",
    "shaft_diameter_mm",
    "airgap_mm",
    "magnet_thickness_mm",
    "axial_length_mm",
    "air_buffer_mm",
}


def load_case_spec(path: Path) -> dict[str, Any]:
    data = yaml.safe_load(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError(f"Case file {path} must contain a YAML mapping")
    validate_case_spec(data, source_path=path)
    return data


def validate_case_spec(data: dict[str, Any], source_path: Path | None = None) -> None:
    missing_keys = [
        key
        for key in (
            "case_id",
            "source_kind",
            "source_url",
            "reconstruction_level",
            "geometry_variants",
            "cell_type",
            "space_family",
            "polynomial_order",
            "materials_profile",
            "excitation_profile",
            "validation_targets",
            "geometry_parameters",
        )
        if key not in data
    ]
    if missing_keys:
        raise ValueError(_prefix(source_path) + f"Missing required keys: {', '.join(missing_keys)}")

    if data["source_kind"] not in ALLOWED_SOURCE_KINDS:
        raise ValueError(_prefix(source_path) + "source_kind must be 'paper' or 'industrial'")
    for key in ("case_id", "source_url", "reconstruction_level", "cell_type", "space_family"):
        value = data[key]
        if not isinstance(value, str) or not value.strip():
            raise ValueError(_prefix(source_path) + f"{key} must be a non-empty string")
    if not data["source_url"].startswith(("http://", "https://")):
        raise ValueError(_prefix(source_path) + "source_url must start with http:// or https://")

    variants = data["geometry_variants"]
    if not isinstance(variants, list) or not variants:
        raise ValueError(_prefix(source_path) + "geometry_variants must be a non-empty list")
    unsupported_variants = [variant for variant in variants if variant not in ALLOWED_VARIANTS]
    if unsupported_variants:
        raise ValueError(
            _prefix(source_path)
            + f"Unsupported geometry variants: {', '.join(sorted(set(unsupported_variants)))}"
        )
    if "section_2d" not in variants or "extruded_3d" not in variants:
        raise ValueError(
            _prefix(source_path) + "Every case must include both section_2d and extruded_3d"
        )

    polynomial_order = data["polynomial_order"]
    if not isinstance(polynomial_order, int) or polynomial_order < 1:
        raise ValueError(_prefix(source_path) + "polynomial_order must be an integer >= 1")

    for mapping_name in ("materials_profile", "excitation_profile"):
        mapping_value = data[mapping_name]
        if not isinstance(mapping_value, dict) or not mapping_value:
            raise ValueError(_prefix(source_path) + f"{mapping_name} must be a non-empty mapping")

    validation_targets = data["validation_targets"]
    if not isinstance(validation_targets, list) or not validation_targets:
        raise ValueError(_prefix(source_path) + "validation_targets must be a non-empty list")

    geometry_parameters = data["geometry_parameters"]
    if not isinstance(geometry_parameters, dict):
        raise ValueError(_prefix(source_path) + "geometry_parameters must be a mapping")
    missing_geometry_keys = sorted(REQUIRED_GEOMETRY_KEYS.difference(geometry_parameters))
    if missing_geometry_keys:
        raise ValueError(
            _prefix(source_path)
            + f"Missing geometry parameters: {', '.join(missing_geometry_keys)}"
        )

    for key in REQUIRED_GEOMETRY_KEYS:
        value = geometry_parameters[key]
        if not isinstance(value, (int, float)) or value <= 0.0:
            raise ValueError(_prefix(source_path) + f"{key} must be a positive number")

    for key in ("slot_count", "pole_count"):
        if key in geometry_parameters and geometry_parameters[key] is not None:
            value = geometry_parameters[key]
            if not isinstance(value, int) or value <= 0:
                raise ValueError(_prefix(source_path) + f"{key} must be a positive integer when present")

    outer_diameter = float(geometry_parameters["outer_diameter_mm"])
    stator_inner_diameter = float(geometry_parameters["stator_inner_diameter_mm"])
    rotor_outer_diameter = float(geometry_parameters["rotor_outer_diameter_mm"])
    rotor_inner_diameter = float(geometry_parameters["rotor_inner_diameter_mm"])
    shaft_diameter = float(geometry_parameters["shaft_diameter_mm"])
    airgap = float(geometry_parameters["airgap_mm"])
    if not (
        outer_diameter > stator_inner_diameter > rotor_outer_diameter > rotor_inner_diameter > shaft_diameter
    ):
        raise ValueError(
            _prefix(source_path)
            + "Diameters must satisfy outer > stator_inner > rotor_outer > rotor_inner > shaft"
        )

    diameter_gap = stator_inner_diameter - rotor_outer_diameter
    if abs(0.5 * diameter_gap - airgap) > 1.0e-6:
        raise ValueError(
            _prefix(source_path)
            + "airgap_mm must match half of (stator_inner_diameter_mm - rotor_outer_diameter_mm)"
        )


def case_paths(repo_root: Path) -> list[Path]:
    return sorted((repo_root / "cases" / "machines").glob("*/case.yaml"))


def _prefix(source_path: Path | None) -> str:
    return f"{source_path}: " if source_path is not None else ""

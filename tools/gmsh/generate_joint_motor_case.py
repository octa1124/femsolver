from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any

import yaml

REPO_ROOT = Path(__file__).resolve().parents[2]
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

from tools.gmsh.case_catalog import load_case_spec


REGION_NAMES = [
    "outer_air",
    "stator_core",
    "airgap",
    "magnet_ring",
    "rotor_core",
    "shaft",
]


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Generate simplified joint-motor geometry, manifest, and optional mesh outputs."
    )
    parser.add_argument("--case", required=True, type=Path, help="Path to case.yaml")
    parser.add_argument(
        "--variant",
        required=True,
        choices=("section_2d", "extruded_3d", "full_3d"),
        help="Geometry variant to generate.",
    )
    parser.add_argument(
        "--output-dir",
        required=True,
        type=Path,
        help="Directory for generated geometry, manifest, and mesh outputs.",
    )
    parser.add_argument(
        "--skip-gmsh",
        action="store_true",
        help="Do not invoke gmsh even if it is available on PATH.",
    )
    args = parser.parse_args()

    case_spec = load_case_spec(args.case)
    if args.variant not in case_spec["geometry_variants"]:
        raise ValueError(
            f"Variant {args.variant} is not declared in {args.case}. "
            f"Available variants: {', '.join(case_spec['geometry_variants'])}"
        )

    args.output_dir.mkdir(parents=True, exist_ok=True)
    geo_path = args.output_dir / f"{case_spec['case_id']}_{args.variant}.geo"
    mesh_path = args.output_dir / f"{case_spec['case_id']}_{args.variant}.msh"
    manifest_path = args.output_dir / "mesh_manifest.yaml"
    summary_path = args.output_dir / "preprocess_summary.txt"

    geometry = build_geometry(case_spec, args.variant)
    geo_path.write_text(render_geo(case_spec, geometry, args.variant), encoding="utf-8")

    mesh_generation = maybe_run_gmsh(
        geo_path=geo_path,
        mesh_path=mesh_path,
        variant=args.variant,
        skip_gmsh=args.skip_gmsh,
    )
    manifest = build_manifest(case_spec, args.variant, geo_path, mesh_path, mesh_generation, geometry)
    manifest_path.write_text(yaml.safe_dump(manifest, sort_keys=False), encoding="utf-8")

    summary_path.write_text(render_summary(manifest), encoding="utf-8")
    print(render_summary(manifest))
    return 0


def build_geometry(case_spec: dict[str, Any], variant: str) -> dict[str, Any]:
    parameters = case_spec["geometry_parameters"].copy()
    if variant == "full_3d":
        parameters["variant_mode"] = "provisional_full_3d_concentric_placeholder"
    else:
        parameters["variant_mode"] = variant

    parameters["outer_air_diameter_mm"] = (
        float(parameters["outer_diameter_mm"]) + 2.0 * float(parameters["air_buffer_mm"])
    )
    parameters["magnet_inner_diameter_mm"] = (
        float(parameters["rotor_outer_diameter_mm"]) - 2.0 * float(parameters["magnet_thickness_mm"])
    )
    return parameters


def build_manifest(
    case_spec: dict[str, Any],
    variant: str,
    geo_path: Path,
    mesh_path: Path,
    mesh_generation: dict[str, Any],
    geometry: dict[str, Any],
) -> dict[str, Any]:
    boundary_names = ["outer_air_boundary", "shaft_boundary"]
    if variant != "section_2d":
        boundary_names.extend(["axial_minus", "axial_plus"])

    return {
        "case_id": case_spec["case_id"],
        "source_kind": case_spec["source_kind"],
        "source_url": case_spec["source_url"],
        "reconstruction_level": case_spec["reconstruction_level"],
        "geometry_variant": variant,
        "cell_type": case_spec["cell_type"],
        "space_family": case_spec["space_family"],
        "polynomial_order": case_spec["polynomial_order"],
        "materials_profile": case_spec["materials_profile"],
        "excitation_profile": case_spec["excitation_profile"],
        "validation_targets": case_spec["validation_targets"],
        "slot_count": geometry.get("slot_count"),
        "pole_count": geometry.get("pole_count"),
        "region_names": REGION_NAMES,
        "boundary_names": boundary_names,
        "geometry_parameters": geometry,
        "files": {
            "geometry_geo": str(geo_path),
            "mesh_msh": str(mesh_path),
        },
        "mesh_generation": mesh_generation,
    }


def render_summary(manifest: dict[str, Any]) -> str:
    mesh_generation = manifest["mesh_generation"]
    return (
        f"case_id={manifest['case_id']}\n"
        f"variant={manifest['geometry_variant']}\n"
        f"source_kind={manifest['source_kind']}\n"
        f"cell_type={manifest['cell_type']}\n"
        f"space_family={manifest['space_family']}\n"
        f"polynomial_order={manifest['polynomial_order']}\n"
        f"slot_count={manifest['slot_count']}\n"
        f"pole_count={manifest['pole_count']}\n"
        f"geometry_geo={manifest['files']['geometry_geo']}\n"
        f"mesh_msh={manifest['files']['mesh_msh']}\n"
        f"mesh_generated={mesh_generation['generated']}\n"
        f"mesh_status={mesh_generation['status']}"
    )


def maybe_run_gmsh(
    *,
    geo_path: Path,
    mesh_path: Path,
    variant: str,
    skip_gmsh: bool,
) -> dict[str, Any]:
    gmsh_executable = shutil.which("gmsh")
    if skip_gmsh:
        return {
            "attempted": False,
            "generated": False,
            "status": "skipped_by_flag",
            "gmsh_executable": gmsh_executable,
            "dimension": 2 if variant == "section_2d" else 3,
        }

    if gmsh_executable is None:
        return {
            "attempted": False,
            "generated": False,
            "status": "skipped_gmsh_not_available",
            "gmsh_executable": None,
            "dimension": 2 if variant == "section_2d" else 3,
        }

    dimension_flag = "-2" if variant == "section_2d" else "-3"
    command = [gmsh_executable, str(geo_path), dimension_flag, "-format", "msh2", "-o", str(mesh_path)]
    completed = subprocess.run(command, check=False, capture_output=True, text=True)
    return {
        "attempted": True,
        "generated": completed.returncode == 0 and mesh_path.exists(),
        "status": "generated" if completed.returncode == 0 else "gmsh_failed",
        "gmsh_executable": gmsh_executable,
        "dimension": 2 if variant == "section_2d" else 3,
        "returncode": completed.returncode,
        "stdout": completed.stdout.strip(),
        "stderr": completed.stderr.strip(),
    }


def render_geo(case_spec: dict[str, Any], geometry: dict[str, Any], variant: str) -> str:
    dimension = 2 if variant == "section_2d" else 3
    slot_count = geometry.get("slot_count")
    pole_count = geometry.get("pole_count")
    lines = [
        "// Generated by tools/gmsh/generate_joint_motor_case.py",
        f"// case_id: {case_spec['case_id']}",
        f"// source_url: {case_spec['source_url']}",
        f"// reconstruction_level: {case_spec['reconstruction_level']}",
        f"// slot_count: {slot_count}",
        f"// pole_count: {pole_count}",
        f"// variant: {variant}",
        'SetFactory("Built-in");',
        "Mesh.CharacteristicLengthMin = 1.5;",
        "Mesh.CharacteristicLengthMax = 6.0;",
        "",
    ]

    radii = {
        "outer_air": 0.5 * float(geometry["outer_air_diameter_mm"]),
        "stator_outer": 0.5 * float(geometry["outer_diameter_mm"]),
        "stator_inner": 0.5 * float(geometry["stator_inner_diameter_mm"]),
        "rotor_outer": 0.5 * float(geometry["rotor_outer_diameter_mm"]),
        "magnet_inner": 0.5 * float(geometry["magnet_inner_diameter_mm"]),
        "rotor_inner": 0.5 * float(geometry["rotor_inner_diameter_mm"]),
        "shaft": 0.5 * float(geometry["shaft_diameter_mm"]),
    }

    center_tag = 1
    lines.append(f"Point({center_tag}) = {{0.0, 0.0, 0.0, 1.0}};")

    next_point = 2
    next_curve = 1
    next_loop = 1
    next_surface = 1
    loops: dict[str, int] = {}
    curves_by_radius: dict[str, list[int]] = {}

    for name, radius in radii.items():
        point_tags = [next_point + index for index in range(4)]
        next_point += 4
        coordinates = [(radius, 0.0), (0.0, radius), (-radius, 0.0), (0.0, -radius)]
        for tag, (x_coordinate, y_coordinate) in zip(point_tags, coordinates):
            lines.append(f"Point({tag}) = {{{x_coordinate:.6f}, {y_coordinate:.6f}, 0.0, 1.0}};")

        circle_tags: list[int] = []
        for start, end in ((0, 1), (1, 2), (2, 3), (3, 0)):
            lines.append(
                f"Circle({next_curve}) = {{{point_tags[start]}, {center_tag}, {point_tags[end]}}};"
            )
            circle_tags.append(next_curve)
            next_curve += 1

        curves_by_radius[name] = circle_tags
        lines.append(f"Line Loop({next_loop}) = {{{', '.join(str(tag) for tag in circle_tags)}}};")
        loops[name] = next_loop
        next_loop += 1
        lines.append("")

    surfaces = {
        "outer_air": (loops["outer_air"], loops["stator_outer"]),
        "stator_core": (loops["stator_outer"], loops["stator_inner"]),
        "airgap": (loops["stator_inner"], loops["rotor_outer"]),
        "magnet_ring": (loops["rotor_outer"], loops["magnet_inner"]),
        "rotor_core": (loops["magnet_inner"], loops["rotor_inner"]),
        "shaft": (loops["shaft"], None),
    }

    surface_tags: dict[str, int] = {}
    for name, (outer_loop, inner_loop) in surfaces.items():
        if inner_loop is None:
            lines.append(f"Plane Surface({next_surface}) = {{{outer_loop}}};")
        else:
            lines.append(f"Plane Surface({next_surface}) = {{{outer_loop}, {inner_loop}}};")
        surface_tags[name] = next_surface
        next_surface += 1

    lines.append("")
    if dimension == 2:
        for name in REGION_NAMES:
            lines.append(f'Physical Surface("{name}") = {{{surface_tags[name]}}};')
        lines.append(
            f'Physical Curve("outer_air_boundary") = {{{", ".join(str(tag) for tag in curves_by_radius["outer_air"])}}};'
        )
        lines.append(
            f'Physical Curve("shaft_boundary") = {{{", ".join(str(tag) for tag in curves_by_radius["shaft"])}}};'
        )
    else:
        for name in REGION_NAMES:
            lines.append(
                f'{name}_extrude[] = Extrude {{0.0, 0.0, {float(geometry["axial_length_mm"]):.6f}}} '
                f'{{ Surface{{{surface_tags[name]}}}; }};'
            )
        lines.append("")
        for name in REGION_NAMES:
            lines.append(f'Physical Volume("{name}") = {{{name}_extrude[1]}};')
        lines.append("")
        outer_air_boundary_surfaces = ", ".join(f"outer_air_extrude[{index}]" for index in range(2, 6))
        shaft_boundary_surfaces = ", ".join(f"shaft_extrude[{index}]" for index in range(2, 6))
        lines.append(
            f'Physical Surface("outer_air_boundary") = {{{outer_air_boundary_surfaces}}};'
        )
        lines.append(
            f'Physical Surface("shaft_boundary") = {{{shaft_boundary_surfaces}}};'
        )
        axial_surfaces = ", ".join(f"{name}_extrude[0]" for name in REGION_NAMES)
        lines.append(f'Physical Surface("axial_plus") = {{{axial_surfaces}}};')
        lines.append(
            f'Physical Surface("axial_minus") = {{{", ".join(str(tag) for tag in surface_tags.values())}}};'
        )

    if variant == "full_3d":
        lines.append("// full_3d currently reuses the concentric extrusion baseline pending detailed OCC geometry.")

    return "\n".join(lines) + "\n"


if __name__ == "__main__":
    raise SystemExit(main())

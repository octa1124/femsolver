# Implementation Documents

This directory stores implementation-level technical documents for each meaningful workstream.

Rules:

- every non-trivial subsystem change must update one of these documents or add a new one
- repository and infrastructure changes live here even when no solver physics is implemented yet
- release completion requires that the shipped work is reflected here before the version is considered done

## `v0.1.0` Baseline Documents

- [Repository Baseline Overview](./v0.1.0-repository-baseline.md)
- [Governance And Documentation Workflow](./governance-and-docs.md)
- [Build System And Code Layout](./build-system-and-layout.md)
- [Application Skeleton And Core Types](./application-bootstrap.md)
- [Testing And Quality Gates](./testing-and-quality.md)
- [GitHub Automation And Release Flow](./github-automation-and-release.md)
- [v1 Solver Implementation Path](./v1.0.0-solver-implementation-path.md)
- [v1 To v2 Readiness Review](../architecture/v1-v2-readiness-review.md)

## `v0.2.0` Kernel Foundation Documents

- [Kernel Foundation Overview](./v0.2.0-kernel-foundation.md)
- [Kernel Mesh And Topology](./kernel-mesh-topology.md)
- [Kernel Reference, Quadrature, And Basis](./kernel-reference-basis-quadrature.md)
- [Kernel Assembly, Algebra, And Benchmark](./kernel-assembly-and-benchmark.md)

## `v0.3.0` Current Development Documents

- [Case Catalog And Preprocessing](./case-catalog-and-preprocessing.md)
- [Vector-Kernel Baseline](./v0.3.0-vector-kernel-baseline.md)
- [Linear Magnetostatic Smoke Path](./linear-magnetostatic-smoke-path.md)

## `v2.0.0` Coupling Documents

- [Coupling Foundation](./v2.0.0-coupling-foundation.md)

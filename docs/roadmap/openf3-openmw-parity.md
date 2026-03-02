# OpenF3 OpenMW-Parity Roadmap (FO3/FNV)

## Goal
Deliver an open engine/runtime for Fallout 3 and New Vegas with parity class comparable to OpenMW:

- stable long-session gameplay,
- deterministic content loading and validation,
- robust mod/plugin/archive support,
- reproducible diagnostics and release quality.

## Phase Plan

## Phase 0 — Foundation Alignment
- Freeze canonical runtime path (`sugarbomb + f3goaty`).
- Harden module loading contracts and diagnostics.
- Add architecture docs and phased feature flags.

### Exit Criteria
- Module loading uses safe opaque handles.
- Loader failures report module/symbol/API mismatch details.
- New contributors can identify runtime boundaries quickly.

---

## Phase 1 — Content Bootstrap
- Add startup bootstrap config parsing.
- Build deterministic content manifest:
  - install/data roots,
  - plugin list/load order,
  - archive list.
- Validate dependency consistency (e.g. missing masters) before game loop.
- Emit structured bootstrap report.

### Exit Criteria
- Bootstrap-only mode can run and print success/failure report.
- Invalid plugin graphs fail early with explicit diagnostics.
- Manifest output is deterministic across repeated runs.

---

## Phase 2 — Data Model + World Skeleton
- Normalize FO3/FNV record ingestion into runtime data model.
- Build world/cell index.
- Spawn non-interactive scene shell for representative interior/exterior cells.

## Phase 3 — Simulation Core
- Actor lifecycle, inventory/forms, combat baseline, timing model.
- Quest/state backbone sufficient for early quest progression.

## Phase 4 — Script Runtime
- Event dispatch and script VM bridge.
- Compatibility-focused script subsystem iteration.

## Phase 5 — UI/HUD/Pip-Boy Core
- Main menu, HUD shell, inventory/dialogue baseline, Pip-Boy scaffolding.

## Phase 6 — Save/Load + Determinism
- Save schema/versioning.
- Determinism checks and compatibility stress tests.

## Phase 7 — Performance + Release Pipeline
- Profiling and optimization passes.
- CI and packaging for supported targets.
- Compatibility and regression matrices.

## Implementation Notes
- `src/components/*` remains a capability reservoir for staged integration.
- No broad “big-bang merge” from historical trees; integrate behind active module boundaries.
- Each phase should ship with measurable acceptance checks and regression hooks.


# OpenF3 Runtime Boundaries

## Purpose
This document defines the **canonical runtime path** used for active OpenF3 engine work.
The repository includes multiple historical code strata; without explicit boundaries, contributors
can accidentally build against the wrong path.

## Canonical Runtime Path (Active)

The active runtime is:

- `f3goaty/` — game-facing application and game module entry points
  - `GameApp/` — executable startup path (`F3GOATY`)
  - `SbGame/` — game module loaded by framework
- `sugarbomb/` — engine/module host
  - `CoreLibs/` — system, rendering, sound abstractions and implementations
  - `AppFrameworks/` — app shell and dynamic library helpers
  - `SbGameFramework/` — game-framework orchestration, game/network module binding
  - `SbNetwork/` — network module interface/implementation
  - `SDK/Include/` — module ABI interfaces and import/export contracts

Root build currently wires these trees directly:

- `libs/`
- `sugarbomb/`
- `f3goaty/`

## Non-Canonical Runtime Trees (Reference Reservoirs)

These trees are valuable, but **not** the active runtime in the root build:

- `src/` — OpenMW-derived and legacy subsystems (`components/esm4`, `components/bsa`, `components/vfs`, etc.)
- `code/` — older alternate application/runtime/server branches

They are treated as:

1. reference implementations,
2. subsystem reservoirs for selective integration,
3. migration sources behind explicit adapter layers.

They are **not** assumed to be startup-critical or runtime-authoritative for `F3GOATY`.

## Integration Rule

When integrating capabilities from `src/` or `code/` into active runtime:

1. expose/use interfaces in `sugarbomb/SDK/Include`,
2. implement adapters inside `sugarbomb/` or `f3goaty/`,
3. gate incomplete functionality behind compile-time/runtime flags,
4. avoid direct cross-tree coupling that bypasses module boundaries.

## Build/Execution Contract

Before adding runtime features, confirm they are reachable from:

`f3goaty/GameApp/main.cpp -> sugarbomb modules -> f3goaty/SbGame`

If a change only affects `src/` or `code/` but is not connected to this path,
it is documentation/reference work unless a follow-up wiring change is included.



# LhAILa Blueprint 0.1

## Project

**Name:** `LhAILa`  
**Meaning:** **LhA Integration Layer**

`LhAILa` is a native integration layer for **classic LhA/LZH archives** on **AmigaOS 3.2.3**.

The project does not attempt to replace LhA or define a new archive format.  
It accepts **LhA as the de-facto archive standard on the Amiga** and focuses on a cleaner, smaller and more reusable system-level integration.

---

## Mission

The purpose of `LhAILa` is to make **classic LhA archives** available through a **small, deterministic, native archive core** suitable for:

- inspection
- integrity testing
- extraction
- CLI use
- ARexx use
- later optional Workbench-facing integration
- later optional DOS-facing projection

The central question is not:

> “Which archive format should AmigaOS use?”

but:

> “How can the existing LhA reality be integrated cleanly into AmigaOS 3.2.3?”

---

## Baseline

- **Target OS:** AmigaOS 3.2.3
- **Reference SDK:** NDK 3.2R4
- **Toolchain target:** vbcc, C89
- **Design style:** native, explicit, deterministic, small-footprint
- **Primary rule:** system integration before reinvention

---

## Scope

### In scope for 0.1

- read-only archive access
- archive listing
- archive integrity testing
- archive extraction
- single-entry extraction
- native library-first design
- CLI frontend
- ARexx frontend

### Out of scope for 0.1

- archive writing
- archive updating
- archive deletion
- multi-format abstraction
- public decoder plugin API
- XPK-like infrastructure
- filesystem/handler integration in the MVP
- large GUI tooling
- generic “archive framework” ambitions

---

## Core design statement

`LhAILa` is **first an archive domain core**, not first a filesystem projection.

The design order is:

1. archive parser
2. profile enforcement
3. method dispatch
4. decode pipeline
5. sinks for testing and extraction
6. native API
7. CLI and ARexx
8. optional later system projections

This avoids letting a DOS/handler-facing view dictate the archive model.

---

## Classic Profile 0.1

`LhAILa` uses a deliberately narrow **Classic Profile**.

### Accepted archive family

- classic **`.lha`**
- classic **`.lzh`**

File extension is only a hint.  
Actual acceptance depends on archive structure.

### Accepted header levels

- **level 0**
- **level 1**

### Recognized but unsupported in 0.1

- **level 2**
- unknown levels
- structurally related but non-profile archive variants

### Accepted methods

- `-lh0-`
- `-lh1-`
- `-lh4-`
- `-lh5-`

### Recognized but unsupported in 0.1

- `-lh2-`
- `-lh3-`
- `-lh6-`
- `-lh7-`
- LArc / `.lzs`
- PMarc / `.pma`

Anything outside the active profile is rejected **cleanly and explicitly**.

---

## Architecture overview

`LhAILa` is built around a strict internal separation of responsibilities.

### Parser Layer

Responsible for:

- reading archive headers
- determining header level
- locating entry boundaries
- extracting entry metadata
- determining method id
- determining packed data offset

Not responsible for:

- decompression
- profile decisions
- target path construction
- CLI or ARexx behavior

### Profile Gate

Responsible for:

- deciding whether a parsed entry belongs to the supported Classic Profile
- classifying entries as:
  - supported
  - recognized but outside profile
  - invalid / corrupt

Not responsible for:

- decompression
- file writing
- user-facing formatting

### Method Registry

Responsible for:

- mapping method ids to decoder units
- defining which internal decoder handles which method

Not responsible for:

- header parsing
- policy
- path handling

### Decoder Units

Responsible for:

- decoding entry payload data only

Not responsible for:

- archive parsing
- profile decisions
- path logic
- CLI / ARexx / Workbench concerns

### Sinks

Responsible for:

- consuming decoded output
- test flow
- file writing flow

Planned sink types:

- test sink
- file sink

Later optional:

- memory sink
- null sink

---

## Internal principle

> **Parser finds. Profile permits. Registry selects. Decoder transforms. Sink consumes.**

No layer may cross those boundaries.

---

## Public API direction

The first public system-facing core is planned as:

- **`lhaarchive.library`**

The API is intentionally narrow and read-only.

Primary operations:

- open archive
- close archive
- rewind archive
- iterate entries
- get entry info
- test archive / entry
- extract archive / entry

The public API must remain independent from any imported decoder backend.

---

## Frontends

### CLI

Planned tool:

- **`lhaila`**

Initial commands:

- `list`
- `test`
- `extract`

The CLI must remain thin and must not duplicate archive logic.

### ARexx

Planned port:

- **`LHAILA`**

Initial commands mirror the library semantics:

- `LIST`
- `TEST`
- `EXTRACT`
- `EXTRACTENTRY`

The ARexx layer must be scriptable, deterministic and minimal.

---

## Path policy

Extraction must follow strict path safety rules.

Allowed:

- relative entry paths
- normal directory hierarchies inside the destination root

Forbidden:

- parent escapes (`..`)
- absolute paths
- device/volume-like path escapes from archive content
- extraction outside the destination root

`LhAILa` does not silently “fix” dangerous archive paths.  
It rejects them.

---

## Error policy

`LhAILa` prefers clear failure over ambiguous tolerance.

### Principles

- no silent fallback for unsupported methods
- no silent acceptance of profile violations
- no silent CRC mismatch acceptance
- no partial success presented as full success
- deterministic cleanup on all failure paths

Errors are classified, not guessed away.

---

## Ownership model

The implementation follows strict ownership rules.

### Archive handle

Owned by the caller.  
Freed by archive close.

### Entry view

Owned by the archive context.  
Not independently freed.

### Parsed entry internals

Owned by the archive core or by local orchestration scope.

### Decoder context

Short-lived, function-local or stack-local.

### Sinks

Owned by the orchestrating code path.

This keeps the design small, explicit and reviewable.

---

## Internal repository layout

Current intended source structure:

```text
src/
  core/
  internal/
  library/
  methods/
  orchestration/
  rexx/
  sinks/
  tools/
    lhaila/
````

### Intended role of each area

- `core/`  
    archive opening, iteration, header parsing, profile checks, path handling, CRC, low-level I/O helpers
    
- `internal/`  
    private internal contracts and shared internal types
    
- `library/`  
    public native API façade
    
- `methods/`  
    decoder registry and decoder units
    
- `orchestration/`  
    controlled pipeline logic for test and extract flows
    
- `rexx/`  
    ARexx-facing command handling
    
- `sinks/`  
    decoded data consumers
    
- `tools/lhaila/`  
    CLI frontend
    

---

## Implementation strategy

The project is intentionally built in narrow vertical slices.

### Milestone 1 — scan

- open archive
    
- iterate entries
    
- read entry info
    
- enforce profile
    

### Milestone 2 — test-lh0

- implement `-lh0-`
    
- test sink
    
- CRC path
    
- entry/archive testing
    

### Milestone 3 — extract-lh0

- path sanitizer
    
- file sink
    
- full extraction for `-lh0-`
    

### Milestone 4 — classic-core

- add `-lh1-`
    
- add `-lh4-`
    
- add `-lh5-`
    

### Milestone 5 — usable-tool

- CLI frontend stable
    

### Milestone 6 — scriptable-tool

- ARexx frontend stable
    

---

## Imported code policy

If third-party decoder code is used internally, it must follow these rules:

- it does not define the project profile
    
- it does not define the public API
    
- it does not leak its own types into the external interface
    
- it remains an internal backend only
    
- broader capabilities of the backend do not automatically widen `LhAILa`
    

The profile defines the project.  
Implementation helpers do not.

---

## Non-goals

`LhAILa` is not meant to become:

- a universal archive framework
    
- a new compression ecosystem
    
- a replacement for all archive tools
    
- a public decoder plugin system
    
- a clone of datatypes or XPK
    
- a Unix compatibility layer disguised as an Amiga project
    

---

## Status

This blueprint describes the **0.1 architecture-first phase**.

At this stage the repository is expected to focus on:

- internal contracts
    
- directory structure
    
- parser-first implementation
    
- profile enforcement
    
- first end-to-end path with `-lh0-`
    

---

## Guiding sentence

`LhAILa` exists to make **classic LhA** a **clean native archive domain** on **AmigaOS 3.2.3**, using a small read-only core built from explicit contracts, narrow scope and deterministic behavior.

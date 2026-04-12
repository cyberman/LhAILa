# LhAILa

**LhA Integration Layer** for **AmigaOS 3.2.3**

`LhAILa` is a native, system-oriented integration layer for **classic LhA/LZH archives** on **AmigaOS 3.2.3**.

The project does **not** try to invent a new archive format. 

It accepts **LhA as the de-facto Amiga archive standard** and focuses on making it a better citizen of the operating system.

## Project goals

`LhAILa` aims to provide a small, clear and reusable native archive core for:

- archive inspection
- archive testing
- archive extraction
- CLI integration
- ARexx integration
- later optional Workbench and DOS-facing integration

The guiding idea is:

> **System integration before reinvention.**

## Baseline

- **Target OS:** AmigaOS 3.2.3
- **Reference SDK:** NDK 3.2R4
- **Toolchain target:** vbcc, C89
- **Style:** native, explicit, deterministic, small-footprint

## Scope of the first implementation

The initial implementation is intentionally narrow.

### In scope

- **read-only** archive access
- list archive contents
- test archive integrity
- extract archive contents
- extract individual entries
- native library-first design
- CLI frontend
- ARexx frontend

### Out of scope

- writing archives
- updating archives
- deleting from archives
- multi-format abstraction
- XPK-like plugin architecture
- handler/filesystem integration in the MVP
- GUI-heavy tooling in the MVP

## Classic Profile 0.1

`LhAILa` uses a deliberately narrow **Classic Profile**.

### Accepted archive family

- classic **`.lha` / `.lzh`** archives

### Accepted header levels

- **level 0**
- **level 1**

### Accepted methods

- `-lh0-`
- `-lh1-`
- `-lh4-`
- `-lh5-`

### Recognized but not supported in 0.1

Examples include:

- header level 2
- `-lh2-`
- `-lh3-`
- `-lh6-`
- `-lh7-`
- LArc / `.lzs`
- PMarc / `.pma`

Anything outside the active profile is rejected cleanly and explicitly.

## Architecture

`LhAILa` is built around a strict internal separation of responsibilities:

- **Parser Layer**  
  Reads archive headers, entry boundaries and metadata.

- **Profile Gate**  
  Decides whether an entry belongs to the supported Classic Profile.

- **Method Registry**  
  Maps method IDs to internal decoder units.

- **Decoder Units**  
  Perform decompression only.

- **Sinks**  
  Consume decoded output for testing or extraction.

In short:

> **Parser finds. Profile permits. Registry selects. Decoder transforms. Sink consumes.**

## Design principles

- **AmigaOS-first**
- **C89**
- **explicit ownership**
- **deterministic cleanup**
- **small modules**
- **clear contracts**
- **no hidden magic**
- **no shell tricks as infrastructure**

## Project status

This repository is currently in an **early skeleton / architecture-first phase**.

The current focus is:

1. internal type and contract definitions
2. archive open / iterate core
3. Classic Profile enforcement
4. first end-to-end decode path with `-lh0-`
5. test and extract pipeline
6. CLI
7. ARexx

## Planned components

- `lhaarchive.library`
- `lhaila` CLI tool
- `LHAILA` ARexx port

## Repository structure

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
````

## Non-goals

`LhAILa` is **not** intended to become:

- a universal archive framework
    
- a replacement for every archive tool
    
- a new compression ecosystem
    
- a clone of XPK, datatypes, or Unix archive tooling
    

## Why this project exists

On classic Amiga systems, **LhA is already the social and practical archive standard**.  
The interesting problem today is no longer “which archive format should win?” but:

> **How can LhA be integrated more cleanly and more natively into AmigaOS?**

`LhAILa` is one answer to that question.

## License

See `LICENSE`.

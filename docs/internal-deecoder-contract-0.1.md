
# LhAILa Internal Decoder Contract 0.1

## Purpose

This document defines the **internal contract** between the archive parser, the profile gate, the method registry, the decoder units and the sinks used by `LhAILa`.

This is an **internal architecture contract**.

It is:

- not public API
- not ABI-stable
- not a plugin interface
- not intended for third-party extension in 0.1

Its purpose is to enforce clean separation of responsibilities inside the implementation.

---

## Core principle

> **Parser finds. Profile permits. Registry selects. Decoder transforms. Sink consumes.**

No internal layer may cross those responsibilities.

This contract exists to preserve that rule.

---

## Layers and responsibilities

## 1. Parser Layer

The parser layer is responsible for archive container interpretation.

### Responsibilities

- read raw archive header data
- detect supported header levels
- parse entry metadata
- determine method id
- determine packed data offset
- determine entry boundaries
- normalize parsed metadata into internal form

### Not responsible for

- decompression
- profile policy decisions
- file extraction
- path sanitization for output targets
- CRC end verification
- CLI / ARexx behavior

---

## 2. Profile Gate

The profile gate is responsible for deciding whether a parsed entry belongs to the active supported profile.

### Responsibilities

- decide whether header level is allowed
- decide whether method is allowed
- decide whether the entry belongs to:
  - supported profile
  - known but outside profile
  - invalid / corrupt class

### Not responsible for

- decompression
- method selection
- file writing
- path creation
- frontend error text

---

## 3. Method Registry

The method registry maps method identifiers to decoder units.

### Responsibilities

- map method id → decoder unit
- distinguish present vs missing decoder implementations
- provide internal method metadata if needed

### Not responsible for

- parsing headers
- deciding profile policy
- opening files
- consuming decoded output

---

## 4. Decoder Units

A decoder unit transforms compressed entry payload into uncompressed bytes.

### Responsibilities

- consume packed entry data
- emit decoded bytes to a sink
- maintain decode-local state
- respect input and output limits
- fail cleanly on malformed payloads

### Not responsible for

- parsing archive headers
- deciding whether a method is allowed by profile
- creating output files
- target path logic
- frontend integration
- global resource management

---

## 5. Sinks

Sinks consume decoded output.

### Responsibilities

- receive decoded bytes
- optionally update running CRC
- optionally write to destination files
- finalize or abort output safely

### Not responsible for

- decompression
- archive parsing
- method dispatch
- profile decisions

---

## Separation rule

The following mixes are forbidden:

- parser + decoder logic in the same unit
- decoder + path creation logic in the same unit
- decoder + file opening logic in the same unit
- profile policy inside decoder code
- registry logic inside parser code
- frontend-specific behavior inside decoder or sink code

The implementation must keep these boundaries visible in code structure.

---

## Internal normalized entry model

All header-specific parsing must be converted into a single internal normalized entry structure before profile or decoder selection continues.

A typical normalized internal form includes:

- header level
- method id
- entry type
- header offset
- data offset
- packed size
- unpacked size
- CRC
- timestamp
- normalized entry path
- internal flags

The rest of the decode pipeline must not need to understand the original header-specific layout.

---

## Internal profile status

Each parsed entry must be classified into one of these states:

- `LHAPROFILE_MATCH`
- `LHAPROFILE_KNOWN_OUTSIDE`
- `LHAPROFILE_INVALID`

Optional internal reason codes may include:

- bad header level
- unsupported method
- bad entry type
- corrupt header
- inconsistent sizes

The decoder stage must only be entered for `LHAPROFILE_MATCH`.

---

## Method registry model

The method registry is a **static internal table**, not a runtime extension system.

Typical internal method definition fields:

- method id
- internal flags
- decode function pointer

The registry must remain:

- internal
- fixed at build time
- not user-extensible in 0.1
- not dynamically loaded

`LhAILa` does not expose a public decoder module system in 0.1.

---

## Supported method core for 0.1

The active Classic Profile method set is:

- `-lh0-`
- `-lh1-`
- `-lh4-`
- `-lh5-`

Other related methods may be recognized internally, but recognition alone does not imply decoder use.

The profile gate remains authoritative.

---

## Decode context

Each decoder receives a normalized decode context.

A decode context typically contains:

- owning archive context
- normalized parsed entry
- active sink
- remaining packed bytes
- remaining unpacked bytes
- running CRC
- I/O buffer reference
- method scratch buffer reference
- last internal error field

### Decode context rules

- it is short-lived
- it is local to a decode operation
- it owns no global state
- it must not outlive the current processing operation
- decoder units may modify only the fields intended for decode progress

---

## Sink contract

Each sink must provide a small and explicit lifecycle.

Typical sink operations:

- `Begin`
- `Write`
- `End`
- `Abort`

### `Begin`
Prepare consumption for a specific entry.

### `Write`
Consume a chunk of decoded bytes.

### `End`
Finalize a successful output path.

### `Abort`
Clean up a failed output path.

### Sink rules

- `Abort` must always be safe to call after partial progress
- sinks must not assume decoder-specific behavior
- sinks must remain method-agnostic
- file sinks must remove partial output on failure
- test sinks must not keep unnecessary payload data

---

## Decoder rules

Every decoder unit must follow these rules.

### 1. No archive parsing
Decoders do not inspect raw archive header layouts.

### 2. No profile decisions
Decoders do not decide whether a method should be accepted.

### 3. No path knowledge
Decoders do not see destination directory policy.

### 4. No file creation
Decoders do not open output files.

### 5. No global mutable state
Decoders must not rely on hidden global decode state.

### 6. No silent tolerance of corrupt payload
Malformed or inconsistent compressed data must produce a clean error.

### 7. No frontend behavior
Decoders do not emit user-facing messages and do not know CLI or ARexx semantics.

---

## CRC policy

CRC belongs to the decode/output pipeline, not to the parser.

### Parser responsibility
- read expected CRC from the archive entry metadata

### Decode pipeline responsibility
- update running CRC over decoded output bytes

### Orchestration responsibility
- compare expected CRC and actual CRC after a successful decode run
- fail cleanly if the CRC mismatches

This keeps header interpretation and payload validation separate.

---

## Orchestration role

A thin orchestration layer coordinates:

1. profile check
2. registry lookup
3. sink begin
4. decode context init
5. decoder call
6. CRC verification
7. sink end or abort

The orchestrator is the glue.  
It must not become a second parser, a second registry or a second decoder.

---

## Ownership rules

The internal contract follows strict ownership rules.

### Archive context
Owns long-lived resources such as:

- file handle
- I/O buffers
- scratch path buffers
- current entry view

### Parsed entry
Owned by:
- archive context or
- local orchestration scope

Never treated as an independently free-floating long-lived object in 0.1.

### Decode context
Short-lived, local to the processing path.

### Sink
Owned by the orchestrating call path.

### Decoder units
Own no long-lived resources across calls.

The contract prefers a small number of long-lived owners and many short-lived local views.

---

## Error flow rules

Internal failures must remain classifiable by layer.

### Parser errors
Examples:
- malformed header
- impossible sizes
- broken header chain

### Profile errors
Examples:
- unsupported header level
- unsupported method in current profile

### Registry errors
Examples:
- no decoder for an otherwise supported method
- internal registration inconsistency

### Decoder errors
Examples:
- malformed compressed data
- packed/unpacked inconsistency
- unexpected end of compressed payload

### Sink errors
Examples:
- create failure
- write failure
- finalize failure

The layer that detects the error reports it.  
Cleanup happens in the owning orchestration path.

---

## File and module structure intent

The source tree should reflect the contract directly.

Typical areas:

- `core/`
- `methods/`
- `sinks/`
- `orchestration/`
- `library/`
- `rexx/`
- `tools/`

### Mapping

- `core/`  
  parser, profile, path, CRC, low-level archive I/O

- `methods/`  
  static registry and decoder units

- `sinks/`  
  decoded byte consumers

- `orchestration/`  
  test/extract processing flow

- `library/`  
  public API façade

No directory should undermine the contract by mixing multiple unrelated responsibilities.

---

## Explicit non-goals

This contract does **not** define:

- a public decoder extension API
- runtime-loaded method modules
- a datatypes-like subsystem
- an XPK-like architecture
- a generic archive framework
- a public stable backend abstraction

The internal separation exists to improve correctness and maintainability, not to create a new subsystem ecosystem.

---

## Backend adoption rule

If third-party decoder code is used internally, it must be integrated under this contract.

That means:

- imported code must fit behind the method registry
- imported code must not define the public API
- imported code must not widen the profile automatically
- imported code must not force frontend behavior
- imported code must remain backend implementation material only

The internal contract stays authoritative.

---

## Review checklist

Any new decoder-related code should be reviewable with these questions:

1. Is raw header parsing kept out of decoder code?
2. Is profile policy kept out of decoder code?
3. Does the decoder write only through a sink?
4. Can the sink abort cleanly?
5. Is CRC verified outside the parser?
6. Does ownership remain explicit?
7. Does the code avoid hidden global mutable state?
8. Does the new code preserve the parser/profile/registry/decoder/sink separation?

If the answer to any of these is “no”, the design should be reconsidered.

---

## Summary

`LhAILa Internal Decoder Contract 0.1` defines a strict internal split into:

- parser layer
- profile gate
- method registry
- decoder units
- sinks
- thin orchestration

This contract exists to keep the implementation:

- small
- deterministic
- reviewable
- explicit
- AmigaOS-oriented
- protected against accidental subsystem sprawl
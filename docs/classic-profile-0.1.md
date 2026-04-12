# LhAILa Classic Profile 0.1


## Purpose

This document defines the **authoritative narrow format profile** supported by `LhAILa` in its first useful implementation phase.

It exists to answer one practical question:

> Which LhA/LZH archives are accepted by `LhAILa 0.1` as part of its supported classic Amiga scope?

This profile is intentionally **narrow**.  
Its purpose is not to represent everything that some LHA-family decoder may be able to parse, but to define the exact subset that belongs to `LhAILa 0.1`.

Implementation helpers do not define the profile.  
This document does.

---

## Scope

This profile applies to the first read-only implementation of:

- archive listing
- archive testing
- archive extraction
- single-entry extraction

It does **not** define:

- archive writing
- update operations
- archive deletion
- generic multi-format behavior

---

## Profile philosophy

The profile follows these rules:

- prefer **small supported scope** over broad fuzzy acceptance
- prefer **explicit rejection** over silent best-effort behavior
- prefer **classic Amiga relevance** over historical format completeness
- prefer **deterministic behavior** over “maybe this works too”

This means:

- support a clearly chosen classic core
- recognize but reject known outside-profile variants
- reject invalid or corrupt input cleanly

---

## Classification model

Every parsed archive entry must fall into one of three classes.

### 1. `PROFILE_MATCH`

The entry is fully inside the active profile and may be processed normally.

Allowed operations:

- list
- test
- extract

### 2. `PROFILE_KNOWN_OUTSIDE`

The entry is recognized as structurally related to the LHA-family, but it lies outside the supported profile.

Typical reasons:

- unsupported header level
- unsupported method
- recognized related family outside the classic profile

Behavior:

- clean rejection
- explicit error classification
- no silent fallback

### 3. `PROFILE_INVALID`

The entry is corrupt, inconsistent, or structurally invalid.

Typical reasons:

- malformed header
- impossible sizes
- broken header chains
- inconsistent archive structure

Behavior:

- clean rejection
- no recovery-by-guessing in 0.1

---

## Accepted archive family

`LhAILa Classic Profile 0.1` supports only **classic LhA/LZH archives**.

Accepted filename hints:

- `.lha`
- `.lzh`

The file extension is not authoritative.  
Acceptance depends on the internal archive structure.

---

## Accepted header levels

### Supported

- **Header level 0**
- **Header level 1**

### Recognized but unsupported

- **Header level 2**

### Invalid

- unknown or malformed header levels
- structurally inconsistent header variants

---

## Header level policy

### Header level 0

Status:

- **supported**

Reason:

- part of the traditional classic archive family
- acceptable for the 0.1 classic core

### Header level 1

Status:

- **supported**

Reason:

- belongs to the classic practical LhA world
- part of the intended 0.1 baseline

### Header level 2

Status:

- **recognized but unsupported**

Reason:

- structurally related
- outside the narrow 0.1 classic implementation scope
- intentionally deferred to avoid early parser and metadata complexity growth

`LhAILa 0.1` must not silently accept level 2 “because the backend can”.

---

## Accepted methods

The following methods are inside the active Classic Profile:

- `-lh0-`
- `-lh1-`
- `-lh4-`
- `-lh5-`

These form the supported method set for `LhAILa 0.1`.

---

## Recognized but unsupported methods

The following methods are **recognized as related**, but are not part of the active 0.1 profile:

- `-lh2-`
- `-lh3-`
- `-lh6-`
- `-lh7-`
- `-lzs-`

Additional related external families also remain outside the profile, including:

- PMarc / `.pma`

Recognition does not imply support.

If such methods are encountered, the entry must be classified as:

- `PROFILE_KNOWN_OUTSIDE`

and rejected cleanly.

---

## Invalid method cases

The following are considered invalid, not merely unsupported:

- unknown garbage method tags with no plausible family meaning
- malformed method fields
- inconsistent method/header combinations that break structural plausibility

Those cases belong to:

- `PROFILE_INVALID`

---

## Combined acceptance rule

An entry is a **`PROFILE_MATCH`** only if:

1. its **header level** is supported, and
2. its **method** is supported, and
3. its structure is otherwise valid

That means:

### Accepted combinations in 0.1

- level 0 + `-lh0-`
- level 0 + `-lh1-`
- level 0 + `-lh4-`
- level 0 + `-lh5-`
- level 1 + `-lh0-`
- level 1 + `-lh1-`
- level 1 + `-lh4-`
- level 1 + `-lh5-`

### Outside-profile combinations

Examples:

- level 2 + supported method tag
- level 0/1 + unsupported but recognized method
- related but non-profile family variants

These must not be treated as successful matches.

---

## Metadata expectations

For profile-matching entries, `LhAILa 0.1` expects to extract and use only the metadata needed for its read-only use cases.

### Required metadata

- entry path
- entry type
- packed size
- unpacked size
- method id
- CRC
- timestamp (mapped to internal Amiga-style representation where possible)
- packed data offset

### Optional / ignored in 0.1

These do not belong to the normative minimum profile:

- comments
- foreign OS metadata
- user/group ownership data
- UNIX permissions semantics
- non-essential extended metadata
- profile-external extension fields

The rule is:

> Support the smallest metadata set required for correct classic read-only operation.

---

## Path policy

Archive paths are part of profile handling and extraction safety.

### Accepted path behavior

- relative archive entry paths
- normal directory hierarchies inside the archive

### Rejected path behavior

- parent escapes (`..`)
- absolute paths
- archive-driven escape outside the destination root
- device-/volume-like path escapes
- any extraction path behavior that would leave the chosen destination tree

### Important note

Path rejection is not a “frontend choice”.  
It is part of the archive policy enforced by `LhAILa`.

---

## Entry types

### Supported

- regular file entries
- directory entries

### Outside 0.1 scope or invalid depending on structure

- unknown exotic entry kinds
- malformed type encodings
- entries requiring unsupported structural semantics

---

## Error policy

Profile classification must map cleanly into the internal error model.

### `PROFILE_MATCH`
Normal processing path.

### `PROFILE_KNOWN_OUTSIDE`
Reject with a clear supported/unsupported-style error, typically:

- unsupported method
- unsupported profile variant
- unsupported header level

### `PROFILE_INVALID`
Reject with a corruption/invalid-archive style error.

There must be no silent downgrade from:

- invalid → known outside
- known outside → match

---

## Backend policy

If a third-party backend or decoder subset is used internally, the following rule applies:

> Backend capability does not automatically widen the profile.

This means:

- the backend may parse more than the project supports
- the project still accepts only the combinations defined here
- broader recognition is allowed internally
- broader acceptance is not

The profile remains the project truth.

---

## Compatibility policy

`LhAILa Classic Profile 0.1` is a **frozen narrow contract** for the first implementation phase.

Future support growth must be done **additively**.

Valid later options include:

- `Classic Profile 0.2`
- `Extended Profile 0.1`

What must not happen:

- silently widening 0.1 behavior without updating the profile document
- accepting more variants just because test data happened to work

---

## Minimum conformance expectations

An implementation claiming conformance with `LhAILa Classic Profile 0.1` must:

- accept level 0 and level 1 classic entries
- support methods `-lh0-`, `-lh1-`, `-lh4-`, `-lh5-`
- reject known outside-profile methods cleanly
- reject level 2 cleanly
- reject invalid archives cleanly
- enforce safe extraction path behavior
- perform CRC-based integrity validation for supported entries

---

## Summary

`LhAILa Classic Profile 0.1` is the deliberately small supported classic core:

- **archive family:** classic LhA/LZH
- **header levels:** 0 and 1
- **methods:** `-lh0-`, `-lh1-`, `-lh4-`, `-lh5-`
- **read-only only**
- **explicit rejection outside profile**
- **no silent broadening based on backend capabilities**

This profile is the project’s format truth for 0.1.
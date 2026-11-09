# RaceBear SDK Release Process

This document defines the public release workflow for `RB_SDK`. The SDK
development source remains in the sibling `RaceBear_SDK` directory. The public
repository receives only the verified header, matching binaries, runtime
dependencies, resources, documentation, examples, tools, and release assets.

## Request Semantics

For an SDK release task, the words `push`, `提交并推送`, or equivalent mean a
complete versioned release by default. The complete workflow updates the
public tree, pushes `main`, pushes the matching `vX.Y.Z` tag, and creates or
updates the GitHub Release with the package and checksum attached.

Use a branch-only push only when the request explicitly says `仅推送分支` or
`不创建 Release`. A normal release must not be performed by manually pushing
`main` alone because GitHub's Releases page is driven by tags and Release
objects, not by README version text or the latest branch commit.

## Version Rules

1. Read the version from the SDK runtime and the source release notes. Do not
   invent a version from the previous GitHub Release.
2. Update the runtime version, tests, source documentation, public README, and
   public API documentation together.
3. Use one version for the DLL, LIB, header, documentation, archive, checksum,
   tag, and GitHub Release.
4. Never reuse an existing version tag for a different build. Increment the
   version first, even when the ABI is unchanged.
5. For 0.6.0 and later, verify that `RB_Runtime_GetVersion()` returns the exact
   `X.Y.Z` value expected by the release before packaging. The `vs2019-x64`
   suffix belongs only to archive naming and does not belong to the runtime
   version string.

## Preflight

Before running the release script:

- Read the workspace and SDK entry documents.
- Create the required timestamped backup under `Backups`.
- Check `RB_SDK` with `git status --short --branch` and inspect the complete
  diff. Do not overwrite unrelated user changes.
- Rebuild `RaceBear_SDK` with VS2019 Community `v142` for `Debug|x64` and
  `Release|x64`.
- Run the ABI, configuration, dynamic roundtrip, communication, dashboard,
  and other tests required by the change.
- Confirm that the public header, DLL, LIB, runtime dependencies, dashboard
  assets, docs, C++ examples, and tools all come from the same verified build.
- Check that private SDK source, tests, internal references, build outputs, and
  private project files cannot enter the public package. Only reviewed C++
  example projects under `examples` are allowed; Python and generated build
  outputs are rejected by the package allowlist.
- Build all C++ examples with VS2019/v142 x64 in Debug and Release, and verify
  their SDK imports belong to the 10 unified ABI exports. Example wrappers are
  host source code, not extra DLL exports or legacy ABI declarations.
- Compare the public header and exported DLL symbols against the approved public
  API list. Configuration package APIs and external `RB_CAN` realtime position
  APIs are public in 0.6.0. From 0.6.1, the approved list also includes the
  high-level motor maintenance, `.rbmotor` package, same-type synchronization,
  RB servo firmware update, and CAN idle-disable timeout APIs. The documented
  high-level `RB_SeatbeltCan_*` APIs, seatbelt instance configuration and binding
  state are public; 0.6.2 adds explicit seatbelt protection recovery. Raw SDO/PDO,
  firmware-internal commands and SimRacebear UI APIs remain private.
- Confirm the API manual, RB_CAN guide, frontend guide, README, runtime version,
  header, DLL/LIB and archive all identify the same release version.

## One-Command Publish

Run from `D:\AI_Workspace\RACEBEAR\RB_SDK`:

```powershell
pwsh -NoProfile -ExecutionPolicy Bypass -File .\tools\Publish-Release.ps1 -Version X.Y.Z
```

The script performs these operations in order:

1. Rebuilds the SDK Debug and Release configurations unless `-SkipBuild` is
   explicitly used after an equivalent verified build.
2. Copies only the public files from `RaceBear_SDK` into `RB_SDK`.
3. Copies only allowlisted C++ example sources and rejects unexpected package files.
4. Runs the private-source leakage checks.
5. Creates `RaceBearSDK-X.Y.Z-vs2019-x64.zip` and its SHA256 sidecar.
6. Extracts the actual ZIP into an isolated SDK `obj` directory, verifies every
   delivered file against the public tree and SDK build, checks complete
   documentation/Skill/resource inclusion, builds all four C++ programs plus the
   API lifecycle snippet and runtime probe in Debug/Release, checks exact DLL
   exports and client imports, and runs read-only version/protocol/defaults smokes.
   Any failure stops before a commit or network publication.
7. Commits the public tree with the release version.
8. Pushes `main` and the annotated `vX.Y.Z` tag.
9. Creates the GitHub Release if it does not exist, or replaces its package
   assets when the same Release already exists.

The script is the release boundary. Do not run a separate ordinary `git push`
for a release after it completes.

For a local delivery review, use `-PrepareOnly` (and `-SkipBuild` only after
verified matching builds). It copies the reviewed SDK delivery, builds and
validates the ZIP, then returns before any commit, push, tag or GitHub Release.
It permits the already reviewed preparation changes in the worktree; it does
not clean or approve unknown changes. Review the complete resulting diff,
commit the approved preparation, then run without `-PrepareOnly` from clean
`main` for the full release.

## Post-Release Cleanup

After a successful release, keep only the current version's archive and
checksum in the repository root. Remove or move older versioned archives,
checksums, temporary release staging directories, and historical build output
from `RB_SDK`; do not remove the public source tree, matching Debug/Release
package contents, or the current release assets. Before cleanup, confirm that
no script or documentation references the old paths and record the moved or
removed items.

## Release Assets

The archive and checksum are ignored by `.gitignore` so they do not become
permanent source-tree files. This is intentional. A successful GitHub Release
must show both assets:

```text
RaceBearSDK-X.Y.Z-vs2019-x64.zip
RaceBearSDK-X.Y.Z-vs2019-x64.zip.sha256
```

The checksum file must contain the SHA256 of the exact archive followed by two
spaces and the archive filename. Verify the uploaded asset names and checksum
after the Release is created.

## Completion Check

A release is complete only when all of these are true:

- `main` is pushed and clean locally;
- `vX.Y.Z` exists locally and on the remote;
- the GitHub Release for `vX.Y.Z` is visible;
- both package assets are attached;
- the Release notes identify the same version as `RB_Runtime_GetVersion()`;
- the archive contains no private source or development artifacts; and
- the published DLL/LIB/header/resources passed the matching-build checks; and
- the public API diff contains no private CAN maintenance or firmware protocol
  entry.

If a step fails, stop before claiming publication and record the exact failed
step, command, and artifact state.

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
  assets, docs, examples, and tools all come from the same verified build.
- Check that private SDK source, tests, internal references, build outputs, and
  private project files cannot enter the public package. The public
  `examples/RaceBearMotionStudio` sample may retain its own public solution and
  project files.

## One-Command Publish

Run from `D:\AI_Workspace\RACEBEAR\RB_SDK`:

```powershell
pwsh -NoProfile -ExecutionPolicy Bypass -File .\tools\Publish-Release.ps1 -Version X.Y.Z
```

The script performs these operations in order:

1. Rebuilds the SDK Debug and Release configurations unless `-SkipBuild` is
   explicitly used after an equivalent verified build.
2. Copies only the public files from `RaceBear_SDK` into `RB_SDK`.
3. Removes generated example output from the public package tree.
4. Runs the private-source leakage checks.
5. Creates `RaceBearSDK-X.Y.Z-vs2019-x64.zip` and its SHA256 sidecar.
6. Commits the public tree with the release version.
7. Pushes `main` and the annotated `vX.Y.Z` tag.
8. Creates the GitHub Release if it does not exist, or replaces its package
   assets when the same Release already exists.

The script is the release boundary. Do not run a separate ordinary `git push`
for a release after it completes.

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
- the published DLL/LIB/header/resources passed the matching-build checks.

If a step fails, stop before claiming publication and record the exact failed
step, command, and artifact state.

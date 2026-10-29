param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^\d+\.\d+\.\d+$')]
    [string]$Version,

    [string]$SdkSourceRoot = '',
    [string]$Repository = 'ming982566/RB_SDK',
    [switch]$SkipBuild,
    [switch]$PreflightOnly
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$publishRoot = Split-Path -Parent $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($SdkSourceRoot)) {
    $SdkSourceRoot = Join-Path (Split-Path -Parent $publishRoot) 'RaceBear_SDK'
}
$msbuild = 'C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\amd64\MSBuild.exe'
$project = Join-Path $SdkSourceRoot 'RaceBearSDK.vcxproj'
$tag = "v$Version"
$archiveName = "RaceBearSDK-$Version-vs2019-x64.zip"
$archivePath = Join-Path $publishRoot $archiveName
$hashPath = "$archivePath.sha256"

function Invoke-Checked {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Program,
        [Parameter(Mandatory = $true)]
        [string[]]$Arguments
    )

    & $Program @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "$Program failed with exit code $LASTEXITCODE."
    }
}

function Copy-SdkPackage {
    $debugSource = Join-Path $SdkSourceRoot 'bin\x64\Debug'
    $releaseSource = Join-Path $SdkSourceRoot 'bin\x64\Release'

    $directories = @(
        (Join-Path $publishRoot 'bin\x64\Debug'),
        (Join-Path $publishRoot 'bin\x64\Release'),
        (Join-Path $publishRoot 'include'),
        (Join-Path $publishRoot 'docs'),
        (Join-Path $publishRoot 'examples'),
        (Join-Path $publishRoot 'tools\DashboardDesigner'),
        (Join-Path $publishRoot 'tools\skills')
    )
    foreach ($directory in $directories) {
        New-Item -ItemType Directory -Path $directory -Force | Out-Null
    }

    Copy-Item (Join-Path $debugSource 'RaceBearSDK.dll') (Join-Path $publishRoot 'bin\x64\Debug') -Force
    Copy-Item (Join-Path $debugSource 'RaceBearSDK.lib') (Join-Path $publishRoot 'bin\x64\Debug') -Force
    Copy-Item (Join-Path $debugSource 'libusb-1.0.dll') (Join-Path $publishRoot 'bin\x64\Debug') -Force
    Copy-Item (Join-Path $releaseSource 'RaceBearSDK.dll') (Join-Path $publishRoot 'bin\x64\Release') -Force
    Copy-Item (Join-Path $releaseSource 'RaceBearSDK.lib') (Join-Path $publishRoot 'bin\x64\Release') -Force
    Copy-Item (Join-Path $releaseSource 'libusb-1.0.dll') (Join-Path $publishRoot 'bin\x64\Release') -Force

    foreach ($configuration in @('Debug', 'Release')) {
        $assetSource = Join-Path $SdkSourceRoot "bin\x64\$configuration\DashboardAssets\RaceBear"
        $assetTarget = Join-Path $publishRoot "bin\x64\$configuration\DashboardAssets\RaceBear"
        if (Test-Path -LiteralPath $assetTarget) {
            Remove-Item -LiteralPath $assetTarget -Recurse -Force
        }
        New-Item -ItemType Directory -Path (Split-Path -Parent $assetTarget) -Force | Out-Null
        Copy-Item -LiteralPath $assetSource -Destination $assetTarget -Recurse -Force
    }

    Copy-Item (Join-Path $SdkSourceRoot 'include\RaceBearSDK.h') (Join-Path $publishRoot 'include\RaceBearSDK.h') -Force
    Copy-Item (Join-Path $SdkSourceRoot 'docs\RaceBearSDK_API.md') (Join-Path $publishRoot 'docs\RaceBearSDK_API.md') -Force
    Copy-Item (Join-Path $SdkSourceRoot 'docs\RaceBearSDK_FRONTEND_GUIDE.md') (Join-Path $publishRoot 'docs\RaceBearSDK_FRONTEND_GUIDE.md') -Force
    Copy-Item (Join-Path $SdkSourceRoot 'docs\RaceBearSDK_RB_CAN_GUIDE.md') (Join-Path $publishRoot 'docs\RaceBearSDK_RB_CAN_GUIDE.md') -Force
    Copy-Item (Join-Path $SdkSourceRoot 'examples\README.md') (Join-Path $publishRoot 'examples\README.md') -Force
    # SDK language examples are flat source directories. Copy only files so local
    # CMake builds and Python caches can never enter the distribution package.
    foreach ($exampleName in @('cpp', 'python', 'qt')) {
        $exampleSource = Join-Path $SdkSourceRoot "examples\$exampleName"
        $exampleTarget = Join-Path $publishRoot "examples\$exampleName"
        if (Test-Path -LiteralPath $exampleTarget) {
            Remove-Item -LiteralPath $exampleTarget -Recurse -Force
        }
        New-Item -ItemType Directory -Path $exampleTarget -Force | Out-Null
        Get-ChildItem -LiteralPath $exampleSource -File | Copy-Item -Destination $exampleTarget -Force
    }

    foreach ($generatedExamplePath in @(
        (Join-Path $publishRoot 'examples\RaceBearMotionStudio\bin-app'),
        (Join-Path $publishRoot 'examples\RaceBearMotionStudio\obj-app'),
        (Join-Path $publishRoot 'examples\RaceBearMotionStudio\tests')
    )) {
        if (Test-Path -LiteralPath $generatedExamplePath) {
            Remove-Item -LiteralPath $generatedExamplePath -Recurse -Force
        }
    }

    $designerTarget = Join-Path $publishRoot 'tools\DashboardDesigner'
    if (Test-Path -LiteralPath $designerTarget) {
        Remove-Item -LiteralPath $designerTarget -Recurse -Force
    }
    New-Item -ItemType Directory -Path $designerTarget -Force | Out-Null
    Get-ChildItem -LiteralPath (Join-Path $SdkSourceRoot 'tools\DashboardDesigner') -File |
        Copy-Item -Destination $designerTarget -Force

    $skillTarget = Join-Path $publishRoot 'tools\skills\racebear-frontend-panel'
    if (Test-Path -LiteralPath $skillTarget) {
        Remove-Item -LiteralPath $skillTarget -Recurse -Force
    }
    Copy-Item -LiteralPath (Join-Path $SdkSourceRoot 'tools\skills\racebear-frontend-panel') `
        -Destination $skillTarget -Recurse -Force
}

function Assert-NoPrivateSdkSource {
    $forbiddenPaths = @(
        (Join-Path $publishRoot 'src'),
        (Join-Path $publishRoot 'tests'),
        (Join-Path $publishRoot 'InternalReferences'),
        (Join-Path $publishRoot 'RaceBearSDK.sln'),
        (Join-Path $publishRoot 'RaceBearSDK.vcxproj'),
        (Join-Path $publishRoot 'RaceBearSDK.vcxproj.filters'),
        (Join-Path $publishRoot 'RaceBearSDK.vcxproj.user')
    )
    foreach ($path in $forbiddenPaths) {
        if (Test-Path -LiteralPath $path) {
            throw "Private SDK source path found in publish repository: $path"
        }
    }

    foreach ($name in @('RaceBearSDK.cpp', 'RaceBearRuntimeForSDK.cpp', 'RaceBearSDKPrivateExports.h')) {
        $match = Get-ChildItem -LiteralPath $publishRoot -Recurse -File -Filter $name | Select-Object -First 1
        if ($match) {
            throw "Private SDK implementation file found: $($match.FullName)"
        }
    }
}

function Remove-ReleaseStage([string]$stage) {
    $tempRoot = [IO.Path]::GetFullPath($env:TEMP).TrimEnd('\') + '\'
    $target = [IO.Path]::GetFullPath($stage)
    if (-not $target.StartsWith($tempRoot, [StringComparison]::OrdinalIgnoreCase) -or
        (Split-Path -Leaf $target) -notmatch '^RaceBearSdkRelease-[0-9a-f]{32}$') {
        throw "Refusing to remove release staging outside the expected temporary directory: $target"
    }
    if (Test-Path -LiteralPath $target) {
        Remove-Item -LiteralPath $target -Recurse -Force
    }
}

function New-ReleaseStage {
    $stage = Join-Path $env:TEMP ("RaceBearSdkRelease-" + [Guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $stage -Force | Out-Null
    $allowed = @(
        '^README\.md$', '^LICENSE$',
        '^bin\\x64\\(Debug|Release)\\(RaceBearSDK\.(dll|lib)|libusb-1\.0\.dll)$',
        '^bin\\x64\\(Debug|Release)\\DashboardAssets\\RaceBear\\[^\\]+\.(png|ttf|rbdash)$',
        '^bin\\x64\\(Debug|Release)\\DashboardAssets\\RaceBear\\[^\\]+\\[^\\]+\.(png|ttf|rbdash)$',
        '^bin\\x64\\(Debug|Release)\\DashboardAssets\\RaceBear\\[^\\]+\\[^\\]+\\[^\\]+\.(png|ttf|rbdash)$',
        '^include\\RaceBearSDK\.h$',
        '^docs\\(RaceBearSDK_API|RaceBearSDK_FRONTEND_GUIDE|RaceBearSDK_RB_CAN_GUIDE|RELEASE_PROCESS)\.md$',
        '^examples\\README\.md$',
        '^examples\\(cpp|python|qt)\\[^\\]+\.(cpp|h|py|md|txt)$',
        '^examples\\RaceBearMotionStudio\\(RaceBearMotionStudio\.(sln|vcxproj|vcxproj\.filters)|README\.md)$',
        '^examples\\RaceBearMotionStudio\\src\\[^\\]+\.(cpp|h)$',
        '^examples\\RaceBearMotionStudio\\src\\pages\\[^\\]+\.(cpp|h)$',
        '^tools\\Publish-Release\.ps1$',
        '^tools\\DashboardDesigner\\(app\.js|index\.html|Start-DashboardDesigner\.cmd|styles\.css)$',
        '^tools\\skills\\racebear-frontend-panel\\SKILL\.md$',
        '^tools\\skills\\racebear-frontend-panel\\references\\[^\\]+\.md$'
    )
    try {
        $packagePaths = @('bin', 'include', 'docs', 'examples', 'tools', 'README.md', 'LICENSE')
        $stagedFiles = [System.Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
        foreach ($path in $packagePaths) {
            $source = Join-Path $publishRoot $path
            $files = if (Test-Path -LiteralPath $source -PathType Leaf) {
                @(Get-Item -LiteralPath $source)
            }
            else {
                @(Get-ChildItem -LiteralPath $source -Recurse -File)
            }
            foreach ($file in $files) {
                $relative = $file.FullName.Substring($publishRoot.Length + 1)
                if (-not @($allowed | Where-Object { $relative -match $_ }).Count) {
                    throw "Unexpected file in release package paths: $relative"
                }
                $target = Join-Path $stage $relative
                New-Item -ItemType Directory -Path (Split-Path -Parent $target) -Force | Out-Null
                Copy-Item -LiteralPath $file.FullName -Destination $target -Force
                if ((Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash -ne
                    (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash) {
                    throw "Staging hash mismatch: $relative"
                }
                [void]$stagedFiles.Add($relative)
            }
        }
        foreach ($required in @('README.md', 'LICENSE', 'include\RaceBearSDK.h',
            'bin\x64\Debug\RaceBearSDK.dll', 'bin\x64\Debug\RaceBearSDK.lib',
            'bin\x64\Release\RaceBearSDK.dll', 'bin\x64\Release\RaceBearSDK.lib')) {
            if (-not $stagedFiles.Contains($required)) {
                throw "Release staging misses required file: $required"
            }
        }
        return $stage
    }
    catch {
        Remove-ReleaseStage $stage
        throw
    }
}

function Invoke-PublicMirrorAbiGate {
    $gate = Join-Path $SdkSourceRoot 'tests\Test-PublicMirrorAbi.ps1'
    if (-not (Test-Path -LiteralPath $gate)) {
        throw "Public/mirror ABI gate not found: $gate"
    }
    $powershell = Get-Command powershell.exe -ErrorAction SilentlyContinue
    if (-not $powershell) {
        throw 'Windows PowerShell is required to run the public/mirror ABI gate.'
    }
    Invoke-Checked $powershell.Source @(
        '-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', $gate,
        '-SdkRoot', $SdkSourceRoot,
        '-MirrorHeader', (Join-Path (Split-Path -Parent $SdkSourceRoot) 'SimRacebear\module\RB_SDK\RBSDK\RaceBearSDK.h')
    )
}

function Invoke-ReleasePreflight {
    $gitCommand = Get-Command git -ErrorAction SilentlyContinue
    if (-not $gitCommand) {
        throw 'Git is required for a release.'
    }

    $actualRoot = (& git -C $publishRoot rev-parse --show-toplevel 2>$null).Trim()
    if (-not $actualRoot) {
        throw "The publish directory is not a Git worktree: $publishRoot"
    }
    if ([System.IO.Path]::GetFullPath($actualRoot).TrimEnd('\') -ne
        [System.IO.Path]::GetFullPath($publishRoot).TrimEnd('\')) {
        throw "Git root does not match the publish repository: $actualRoot"
    }

    $branch = (& git -C $publishRoot symbolic-ref --quiet --short HEAD 2>$null).Trim()
    if ($branch -ne 'main') {
        throw "Release must run from the main branch; current branch is '$branch'."
    }

    $origin = (& git -C $publishRoot remote get-url origin 2>$null).Trim()
    if (-not $origin) {
        throw 'Git remote origin is not configured.'
    }
    $normalizedOrigin = $origin.TrimEnd('/').TrimEnd('.git')
    if ($normalizedOrigin -notmatch "/$([regex]::Escape($Repository))$") {
        throw "origin '$origin' does not match the requested GitHub repository '$Repository'."
    }

    $status = @(& git -C $publishRoot status --porcelain=v1 --untracked-files=all)
    if ($status.Count -gt 0) {
        $details = $status -join [Environment]::NewLine
        throw "Release requires a clean worktree. Existing changes were not modified:`n$details"
    }

    if (& git -C $publishRoot show-ref --verify --quiet "refs/tags/$tag") {
        throw "Version tag already exists locally: $tag. Choose a new version."
    }
    $remoteTag = (& git -C $publishRoot ls-remote --tags origin "refs/tags/$tag" 2>$null).Trim()
    if ($remoteTag) {
        throw "Version tag already exists on origin: $tag. Choose a new version."
    }

    Write-Host "Release preflight passed: clean main worktree, matching origin, unused tag $tag."
}

if (-not (Test-Path -LiteralPath $project)) {
    throw "SDK project not found: $project"
}
if (-not (Test-Path -LiteralPath $msbuild)) {
    throw "Visual Studio 2019 MSBuild not found: $msbuild"
}
$ghCommand = Get-Command gh -ErrorAction SilentlyContinue
$gh = if ($ghCommand) { $ghCommand.Source } else { 'C:\Program Files\GitHub CLI\gh.exe' }
if (-not (Test-Path -LiteralPath $gh)) {
    throw 'GitHub CLI is required. Install it with: winget install --id GitHub.cli'
}

Push-Location $publishRoot
try {
    Invoke-ReleasePreflight
    if ($PreflightOnly) {
        return
    }

    # Publishing is explicit: normal IDE builds never invoke this script.
    if (-not $SkipBuild) {
        Invoke-Checked $msbuild @($project, '/t:Rebuild', '/p:Configuration=Debug', '/p:Platform=x64', '/m')
        Invoke-Checked $msbuild @($project, '/t:Rebuild', '/p:Configuration=Release', '/p:Platform=x64', '/m')
    }

    Copy-SdkPackage
    Assert-NoPrivateSdkSource
    Invoke-PublicMirrorAbiGate

    if (Test-Path -LiteralPath $archivePath) {
        Remove-Item -LiteralPath $archivePath -Force
    }
    if (Test-Path -LiteralPath $hashPath) {
        Remove-Item -LiteralPath $hashPath -Force
    }

    $stage = New-ReleaseStage
    try {
        Compress-Archive -Path (Join-Path $stage '*') -DestinationPath $archivePath -CompressionLevel Optimal
    }
    finally {
        Remove-ReleaseStage $stage
    }

    $hash = (Get-FileHash -LiteralPath $archivePath -Algorithm SHA256).Hash
    "$hash  $archiveName" | Set-Content -LiteralPath $hashPath -Encoding ASCII

    # This machine has a stale global proxy; per-command empty proxy values keep publishing deterministic.
    Invoke-Checked git @('-c', 'http.proxy=', '-c', 'https.proxy=', 'add', '--all')
    $changes = & git status --porcelain
    if ($changes) {
        Invoke-Checked git @('commit', '-m', "Release RaceBear SDK $Version")
    }

    Invoke-Checked git @('-c', 'http.proxy=', '-c', 'https.proxy=', '-c', 'http.version=HTTP/1.1', 'push', 'origin', 'main')

    $existingTag = & git tag --list $tag
    if (-not $existingTag) {
        Invoke-Checked git @('tag', '-a', $tag, '-m', "RaceBear SDK $Version")
        Invoke-Checked git @('-c', 'http.proxy=', '-c', 'https.proxy=', '-c', 'http.version=HTTP/1.1', 'push', 'origin', $tag)
    }

    $releaseExists = $false
    & $gh release view $tag --repo $Repository *> $null
    if ($LASTEXITCODE -eq 0) {
        $releaseExists = $true
    }

    if ($releaseExists) {
        Invoke-Checked $gh @(
            'release', 'upload', $tag, $archivePath, $hashPath,
            '--repo', $Repository, '--clobber'
        )
    }
    else {
        Invoke-Checked $gh @(
            'release', 'create', $tag, $archivePath, $hashPath,
            '--repo', $Repository,
            '--title', "RaceBear SDK $Version",
            '--generate-notes'
        )
    }

    Write-Host "Published $tag to https://github.com/$Repository/releases/tag/$tag"
}
finally {
    Pop-Location
}

param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^\d+\.\d+\.\d+$')]
    [string]$Version,

    [string]$SdkSourceRoot = 'D:\AI_Workspace\RaceBear_SDK',
    [string]$Repository = 'ming982566/RB_SDK',
    [switch]$SkipBuild
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$publishRoot = Split-Path -Parent $PSScriptRoot
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
        (Join-Path $publishRoot 'examples')
    )
    foreach ($directory in $directories) {
        New-Item -ItemType Directory -Path $directory -Force | Out-Null
    }

    Copy-Item (Join-Path $debugSource 'RaceBearSDK.dll') (Join-Path $publishRoot 'bin\x64\Debug') -Force
    Copy-Item (Join-Path $debugSource 'RaceBearSDK.lib') (Join-Path $publishRoot 'bin\x64\Debug') -Force
    Copy-Item (Join-Path $releaseSource 'RaceBearSDK.dll') (Join-Path $publishRoot 'bin\x64\Release') -Force
    Copy-Item (Join-Path $releaseSource 'RaceBearSDK.lib') (Join-Path $publishRoot 'bin\x64\Release') -Force
    Copy-Item (Join-Path $SdkSourceRoot 'include\RaceBearSDK.h') (Join-Path $publishRoot 'include\RaceBearSDK.h') -Force
    Copy-Item (Join-Path $SdkSourceRoot 'docs\RaceBearSDK_API.md') (Join-Path $publishRoot 'RaceBearSDK_API.md') -Force
    Copy-Item (Join-Path $SdkSourceRoot 'docs\RaceBearSDK_FRONTEND_GUIDE.md') (Join-Path $publishRoot 'RaceBearSDK_FRONTEND_GUIDE.md') -Force
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
    # Publishing is explicit: normal IDE builds never invoke this script.
    if (-not $SkipBuild) {
        Invoke-Checked $msbuild @($project, '/t:Rebuild', '/p:Configuration=Debug', '/p:Platform=x64', '/m')
        Invoke-Checked $msbuild @($project, '/t:Rebuild', '/p:Configuration=Release', '/p:Platform=x64', '/m')
    }

    Copy-SdkPackage
    Assert-NoPrivateSdkSource

    if (Test-Path -LiteralPath $archivePath) {
        Remove-Item -LiteralPath $archivePath -Force
    }
    if (Test-Path -LiteralPath $hashPath) {
        Remove-Item -LiteralPath $hashPath -Force
    }

    $packageItems = @(
        (Join-Path $publishRoot 'bin'),
        (Join-Path $publishRoot 'include'),
        (Join-Path $publishRoot 'examples'),
        (Join-Path $publishRoot 'RaceBearSDK_API.md'),
        (Join-Path $publishRoot 'RaceBearSDK_FRONTEND_GUIDE.md'),
        (Join-Path $publishRoot 'README.md'),
        (Join-Path $publishRoot 'LICENSE')
    )
    Compress-Archive -Path $packageItems -DestinationPath $archivePath -CompressionLevel Optimal

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

param([string]$Root = (Split-Path -Parent $PSScriptRoot))
$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$Root = [IO.Path]::GetFullPath($Root).TrimEnd('\')
$seen = @{}
# 清单路径只允许当前包内文件，避免被篡改清单读取任意外部路径。
foreach ($line in Get-Content -LiteralPath (Join-Path $Root 'SHA256SUMS.txt')) {
    if ($line -notmatch '^([0-9A-Fa-f]{64})  (.+)$') { throw "Invalid manifest line: $line" }
    $expected = $Matches[1]
    $relative = $Matches[2].Replace('/', '\')
    $path = [IO.Path]::GetFullPath((Join-Path $Root $relative))
    if (-not $path.StartsWith($Root + '\', [StringComparison]::OrdinalIgnoreCase) -or $seen.ContainsKey($relative)) {
        throw "Invalid or duplicate delivery path: $relative"
    }
    if (-not (Test-Path -LiteralPath $path -PathType Leaf) -or
        (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $expected) {
        throw "Missing or modified delivery file: $relative"
    }
    $seen[$relative] = $true
}
foreach ($file in Get-ChildItem -LiteralPath $Root -Recurse -File) {
    $relative = $file.FullName.Substring($Root.Length + 1)
    if ($relative -match '^(src|tests|InternalReferences)\\|(^|\\)SimHubInternal(\\|$)|INTERNAL_ONLY') {
        throw "Private content in delivery: $relative"
    }
    if ($relative -match '^bin\\x64\\(Debug|Release)\\DashboardAssets\\' -and
        $relative -notmatch '^bin\\x64\\(Debug|Release)\\DashboardAssets\\RaceBear\\') {
        throw "Nonpublic dashboard resource in delivery: $relative"
    }
    if ($relative -ne 'SHA256SUMS.txt' -and -not $seen.ContainsKey($relative)) {
        throw "Unlisted delivery file: $relative"
    }
}
$manifest = Get-Content -LiteralPath (Join-Path $Root 'SDK_DEPENDENCIES.json') -Raw -Encoding UTF8 | ConvertFrom-Json
foreach ($item in $manifest.files) {
    $path = Join-Path $Root $item.path
    if ((Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash -ne $item.sha256) {
        throw "Dependency manifest mismatch: $($item.path)"
    }
    if ($item.path -match 'RaceBearSDK\.dll$') {
        $info = (Get-Item -LiteralPath $path).VersionInfo
        if ($info.ProductVersion -ne $manifest.version -or $info.FileVersion -ne $manifest.version) {
            throw "SDK file version differs from package: $($item.path)"
        }
        $data = [IO.File]::ReadAllBytes($path)
        $pe = [BitConverter]::ToInt32($data, 0x3c)
        if ([BitConverter]::ToUInt16($data, $pe + 4) -ne 0x8664) { throw 'SDK DLL is not x64.' }
    }
}
$services = Get-Content -LiteralPath (Join-Path $Root 'docs\service-api.json') -Raw -Encoding UTF8 | ConvertFrom-Json
if ($services.protocolVersion -ne 1 -or $services.operations.Count -ne $manifest.publicOperations) {
    throw 'Offline service catalog differs from delivery identity.'
}
Write-Output "Verified $($manifest.version) x64 delivery: $($seen.Count) files, $($services.operations.Count) public operations."

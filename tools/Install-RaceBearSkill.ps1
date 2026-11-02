[CmdletBinding()]
param(
    [ValidateSet('auto', 'claude', 'codex', 'dsh', 'qoder', 'all')]
    [string[]] $Target = @('auto'),
    [string] $SdkRoot,
    [string] $HomeDirectory,
    [switch] $Uninstall
)

$ErrorActionPreference = 'Stop'
try { [Console]::OutputEncoding = [System.Text.Encoding]::UTF8 } catch { }

$SkillName = 'racebear-frontend-panel'
$MarkerBegin = '<!-- BEGIN RaceBear SDK install block -->'
$MarkerEnd = '<!-- END RaceBear SDK install block -->'

# 本脚本位于 <包根>\tools\ 下，包根即其上级目录
$root = if ($SdkRoot) { (Resolve-Path -LiteralPath $SdkRoot).ProviderPath } else { Split-Path -Parent $PSScriptRoot }
$skillSource = Join-Path $root ("tools\skills\" + $SkillName)

function Test-SdkRoot([string] $path) {
    return (Test-Path -LiteralPath (Join-Path $path 'include\RaceBearSDK.h') -PathType Leaf) -and
           (Test-Path -LiteralPath (Join-Path $path 'docs\RaceBearSDK_FRONTEND_GUIDE.md') -PathType Leaf)
}

Write-Host ''
Write-Host 'RaceBear SDK - AI Skill 安装'
Write-Host ('包根: ' + $root)

# 卸载只需要知道装过什么，不依赖包内技能文件是否还在
if (-not $Uninstall) {
    if (-not (Test-Path -LiteralPath $skillSource -PathType Container)) {
        Write-Host ('找不到技能目录: ' + $skillSource) -ForegroundColor Red
        Write-Host '请把本脚本保留在 SDK 包里（tools\Install-RaceBearSkill.ps1），不要单独复制出来。'
        exit 1
    }
    if (-not (Test-SdkRoot $root)) {
        Write-Host '包根缺少 include\RaceBearSDK.h 或 docs\RaceBearSDK_FRONTEND_GUIDE.md，包不完整。' -ForegroundColor Red
        exit 1
    }
}

$homeDir = if ($HomeDirectory) { $HomeDirectory } else { $env:USERPROFILE }
$targets = [ordered]@{
    'claude' = [pscustomobject]@{ Label = 'Claude Code'; Root = (Join-Path $homeDir '.claude'); Skills = (Join-Path $homeDir '.claude\skills') }
    'codex'  = [pscustomobject]@{ Label = 'Codex CLI';   Root = (Join-Path $homeDir '.codex');  Skills = (Join-Path $homeDir '.codex\skills') }
    'dsh'    = [pscustomobject]@{ Label = 'DSH';         Root = (Join-Path $homeDir '.dsh');    Skills = (Join-Path $homeDir '.dsh\skills') }
    'qoder'  = [pscustomobject]@{ Label = 'Qoder';       Root = (Join-Path $homeDir '.qoder');  Skills = (Join-Path $homeDir '.qoder\skills') }
}

$selected = if ($Target -contains 'all') { @($targets.Keys) }
            elseif ($Target -contains 'auto') { @($targets.Keys | Where-Object { Test-Path -LiteralPath $targets[$_].Root -PathType Container }) }
            else { @($Target) }

if ($selected.Count -eq 0) {
    Write-Host ''
    Write-Host ('未检测到已安装的 AI 工具（在 ' + $homeDir + ' 下找 .claude / .codex / .dsh / .qoder）') -ForegroundColor Yellow
    Write-Host '可手动指定：Install-RaceBearSkill.cmd -Target claude'
    Write-Host '或把 tools\skills\racebear-frontend-panel 整个文件夹复制到你所用工具的 skills 目录。'
    exit 2
}

function New-InstallBlock([string] $sdkRoot) {
    $indentedRoot = '    ' + $sdkRoot
    $lines = @(
        $MarkerBegin,
        '## Installed SDK Location',
        '',
        'This skill was installed outside the SDK tree. The SDK root is:',
        '',
        $indentedRoot,
        '',
        'Resolve every path in Start With The Contract against that root:',
        '  include\RaceBearSDK.h  (signature and ABI authority)',
        '  docs\RaceBearSDK_FRONTEND_GUIDE.md, docs\RaceBearSDK_API.md, docs\RaceBearSDK_RB_CAN_GUIDE.md',
        '  bin\x64\Release\  (DLL, import library, DashboardAssets)',
        '',
        'Do not search the current project for the SDK; it lives outside the project tree.',
        $MarkerEnd
    )
    return ($lines -join [Environment]::NewLine)
}

function Remove-InstallBlock([string] $text) {
    $i = $text.IndexOf($MarkerBegin)
    if ($i -lt 0) { return $text }
    $j = $text.IndexOf($MarkerEnd, $i)
    if ($j -lt 0) { return $text }
    return ($text.Substring(0, $i).TrimEnd() + [Environment]::NewLine + $text.Substring($j + $MarkerEnd.Length))
}

$sourceFiles = if ($Uninstall) { @() } else { @(Get-ChildItem -LiteralPath $skillSource -Recurse -File) }
if (-not $Uninstall) { Write-Host ('技能文件: ' + $sourceFiles.Count + ' 个') }
Write-Host ''

$failed = 0
foreach ($key in $selected) {
    $t = $targets[$key]
    $dest = Join-Path $t.Skills $SkillName
    $skillFile = Join-Path $dest 'SKILL.md'
    $existing = if (Test-Path -LiteralPath $skillFile -PathType Leaf) { Get-Content -LiteralPath $skillFile -Raw -Encoding UTF8 } else { '' }
    if ($Uninstall) {
        if (-not (Test-Path -LiteralPath $dest -PathType Container)) { Write-Host ('[跳过] ' + $t.Label + ' 未安装'); continue }
        if ($existing.Contains($MarkerBegin)) {
            Remove-Item -LiteralPath $dest -Recurse -Force
            Write-Host ('[卸载] ' + $t.Label + ' -> 已删除 ' + $dest) -ForegroundColor Green
        } else {
            Write-Host ('[跳过] ' + $t.Label + ' 处的同名技能不是本脚本安装的，未删除: ' + $dest) -ForegroundColor Yellow
        }
        continue
    }
    try {
        if (Test-Path -LiteralPath $dest -PathType Container) {
            if ($existing -and -not $existing.Contains($MarkerBegin)) {
                $bak = $dest + '.bak-' + (Get-Date -Format 'yyyyMMdd-HHmmss')
                Move-Item -LiteralPath $dest -Destination $bak
                Write-Host ('       原有目录非本脚本安装，已备份 -> ' + $bak) -ForegroundColor DarkGray
            } else {
                Remove-Item -LiteralPath $dest -Recurse -Force
            }
        }
        $parent = Split-Path -Parent $dest
        if (-not (Test-Path -LiteralPath $parent -PathType Container)) { New-Item -ItemType Directory -Path $parent -Force | Out-Null }
        New-Item -ItemType Directory -Path $dest -Force | Out-Null
        foreach ($item in Get-ChildItem -LiteralPath $skillSource -Force) {
            Copy-Item -LiteralPath $item.FullName -Destination $dest -Recurse -Force
        }
        $body = Remove-InstallBlock (Get-Content -LiteralPath $skillFile -Raw -Encoding UTF8)
        $body = $body.TrimEnd() + [Environment]::NewLine + [Environment]::NewLine + (New-InstallBlock $root) + [Environment]::NewLine
        [System.IO.File]::WriteAllText($skillFile, $body, (New-Object System.Text.UTF8Encoding($false)))
        $count = @(Get-ChildItem -LiteralPath $dest -Recurse -File).Count
        $ok = (Get-Content -LiteralPath $skillFile -Raw -Encoding UTF8).Contains($root)
        if ($count -eq $sourceFiles.Count -and $ok) {
            Write-Host ('[完成] ' + $t.Label + ' -> ' + $dest) -ForegroundColor Green
        } else {
            Write-Host ('[失败] ' + $t.Label + ' 文件数 ' + $count + '/' + $sourceFiles.Count + '，包根写入=' + $ok) -ForegroundColor Red
            $failed++
        }
    } catch {
        Write-Host ('[失败] ' + $t.Label + ' : ' + $_.Exception.Message) -ForegroundColor Red
        $failed++
    }
}

if (-not $Uninstall) {
    Write-Host ''
    Write-Host 'Cursor / Windsurf / Gemini CLI 不使用这种 skills 目录格式：把包根的 docs\RaceBearSDK_AI_DEVELOPMENT.md 指给它们即可。'
}
Write-Host ''
exit $(if ($failed -gt 0) { 1 } else { 0 })

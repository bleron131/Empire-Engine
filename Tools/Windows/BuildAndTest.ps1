# Empire Engine - builds the editor and runs the Empire automation tests on Windows.
#
# Usage: double-click BuildAndTest.bat (or run this script with PowerShell).
# Optional: -EnginePath "C:\Program Files\Epic Games\UE_5.8" to choose an engine explicitly.
#
# Writes a summary to Saved\EmpireReport\Report.txt that can be shared for troubleshooting.

param(
	[string]$EnginePath = ""
)

$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$ProjectFile = Join-Path $ProjectRoot "EmpireEngine.uproject"
$ReportDir = Join-Path $ProjectRoot "Saved\EmpireReport"
$ReportFile = Join-Path $ReportDir "Report.txt"
$BuildLog = Join-Path $ReportDir "Build.log"
$AutomationDir = Join-Path $ReportDir "Automation"
$TestLogName = "EmpireAutomation.log"
$TestLog = Join-Path $ProjectRoot "Saved\Logs\$TestLogName"

New-Item -ItemType Directory -Force -Path $ReportDir | Out-Null
$Report = New-Object System.Collections.Generic.List[string]

function Write-Report([string]$Text)
{
	Write-Host $Text
	$Report.Add($Text)
}

function Save-Report
{
	$Report | Set-Content -Path $ReportFile -Encoding UTF8
	Write-Host ""
	Write-Host "Bericht gespeichert: $ReportFile"
}

function Find-Engine
{
	if ($EnginePath)
	{
		$Name = Split-Path $EnginePath -Leaf
		$Major = 0; $Minor = 0
		if ($Name -match '^UE_(\d+)\.(\d+)$') { $Major = [int]$Matches[1]; $Minor = [int]$Matches[2] }
		return [pscustomobject]@{ Path = $EnginePath; Major = $Major; Minor = $Minor }
	}

	$Candidates = @()

	# Engines installed through the Epic Games Launcher are listed in this file.
	$Manifest = Join-Path $env:ProgramData "Epic\UnrealEngineLauncher\LauncherInstalled.dat"
	if (Test-Path $Manifest)
	{
		$Installed = Get-Content $Manifest -Raw | ConvertFrom-Json
		foreach ($Item in $Installed.InstallationList)
		{
			if ($Item.AppName -match '^UE_(\d+)\.(\d+)$')
			{
				$Candidates += [pscustomobject]@{ Path = $Item.InstallLocation; Major = [int]$Matches[1]; Minor = [int]$Matches[2] }
			}
		}
	}

	# Fallback: default install folder.
	if ($Candidates.Count -eq 0)
	{
		foreach ($Dir in Get-ChildItem "C:\Program Files\Epic Games" -Directory -Filter "UE_*" -ErrorAction SilentlyContinue)
		{
			if ($Dir.Name -match '^UE_(\d+)\.(\d+)$')
			{
				$Candidates += [pscustomobject]@{ Path = $Dir.FullName; Major = [int]$Matches[1]; Minor = [int]$Matches[2] }
			}
		}
	}

	return $Candidates |
		Where-Object { $_.Major -eq 5 -and (Test-Path (Join-Path $_.Path "Engine\Build\BatchFiles\Build.bat")) } |
		Sort-Object -Property Major, Minor -Descending |
		Select-Object -First 1
}

Write-Report "Empire Engine - Build- und Testbericht"
Write-Report "Datum:   $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
Write-Report "Projekt: $ProjectFile"
Write-Report "Windows: $([System.Environment]::OSVersion.VersionString)"
try { Write-Report "Git:     $(git -C $ProjectRoot rev-parse --short HEAD 2>$null)" } catch { }

# --- 1. Find the engine -------------------------------------------------------------------
$Engine = Find-Engine
if (-not $Engine)
{
	Write-Report ""
	Write-Report "FEHLER: Keine Unreal Engine 5 gefunden."
	Write-Report "Bitte Unreal Engine ueber den Epic Games Launcher installieren (siehe docs\11_Einrichtung_Windows.md)."
	Save-Report
	exit 1
}
$EngineVersion = "$($Engine.Major).$($Engine.Minor)"
Write-Report "Engine:  $($Engine.Path) (Version $EngineVersion)"

# Point the project at the installed engine version so double-clicking the .uproject works.
$ProjectText = Get-Content $ProjectFile -Raw
$UpdatedText = $ProjectText -replace '"EngineAssociation":\s*"[^"]*"', "`"EngineAssociation`": `"$EngineVersion`""
if ($Engine.Major -eq 5 -and $Engine.Minor -gt 0 -and $UpdatedText -ne $ProjectText)
{
	[System.IO.File]::WriteAllText($ProjectFile, $UpdatedText)
	Write-Report "Hinweis: EngineAssociation im Projekt auf $EngineVersion gesetzt."
}

# --- 2. Build the editor target -----------------------------------------------------------
Write-Report ""
Write-Report "Schritt 1/2: Kompilieren (beim ersten Mal 5-30 Minuten)..."
$BatchDir = Join-Path $Engine.Path "Engine\Build\BatchFiles"
$BuildStart = Get-Date

# Run Build.bat from its own folder: calling a .bat by a quoted path with spaces while also
# passing quoted arguments is unreliable in Windows PowerShell.
Push-Location $BatchDir
$PreviousPreference = $ErrorActionPreference
$ErrorActionPreference = "Continue"
& .\Build.bat EmpireEngineEditor Win64 Development "-Project=$ProjectFile" -WaitMutex 2>&1 |
	ForEach-Object { "$_" } | Tee-Object -FilePath $BuildLog
$BuildExit = $LASTEXITCODE
$ErrorActionPreference = $PreviousPreference
Pop-Location

$BuildMinutes = [math]::Round(((Get-Date) - $BuildStart).TotalMinutes, 1)
if ($BuildExit -ne 0)
{
	Write-Report "Kompilieren: FEHLGESCHLAGEN (Exit-Code $BuildExit, $BuildMinutes min)"
	Write-Report ""
	Write-Report "--- Fehlerzeilen ---"
	Get-Content $BuildLog | Select-String -Pattern 'error|Error|FEHLER' | Select-Object -First 40 | ForEach-Object { Write-Report $_.Line }
	Write-Report ""
	Write-Report "--- Letzte 40 Zeilen ---"
	Get-Content $BuildLog -Tail 40 | ForEach-Object { Write-Report $_ }
	Save-Report
	exit 1
}
Write-Report "Kompilieren: OK ($BuildMinutes min)"

# --- 3. Run automation tests --------------------------------------------------------------
Write-Report ""
Write-Report "Schritt 2/2: Automatisierte Tests (beim ersten Mal einige Minuten)..."
$EditorCmd = Join-Path $Engine.Path "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
if (Test-Path $AutomationDir) { Remove-Item $AutomationDir -Recurse -Force }

# A single argument string is passed verbatim, which keeps the inner quotes Unreal expects.
$TestArgs = "`"$ProjectFile`" -ExecCmds=`"Automation RunTests Empire`" -TestExit=`"Automation Test Queue Empty`" " +
	"-ReportExportPath=`"$AutomationDir`" -unattended -nopause -nosplash -NullRHI -log=$TestLogName"
$TestStart = Get-Date
$Process = Start-Process -FilePath $EditorCmd -ArgumentList $TestArgs -Wait -PassThru -NoNewWindow
$TestMinutes = [math]::Round(((Get-Date) - $TestStart).TotalMinutes, 1)

$Succeeded = 0; $Failed = 0; $Total = 0
$IndexFile = Join-Path $AutomationDir "index.json"
if (Test-Path $IndexFile)
{
	$Index = Get-Content $IndexFile -Raw | ConvertFrom-Json
	$Succeeded = [int]$Index.succeeded + [int]$Index.succeededWithWarnings
	$Failed = [int]$Index.failed
	$Total = $Succeeded + $Failed + [int]$Index.notRun
	foreach ($Test in $Index.tests)
	{
		Write-Report ("  [{0}] {1}" -f $Test.state, $Test.fullTestPath)
		foreach ($Entry in $Test.entries)
		{
			if ($Entry.event.type -eq "Error") { Write-Report "      $($Entry.event.message)" }
		}
	}
}
else
{
	Write-Report "  Kein Testbericht gefunden (index.json fehlt)."
}

if (Test-Path $TestLog)
{
	$Problems = Get-Content $TestLog | Select-String -Pattern 'Error:|Fatal|Result=\{Fail' | Select-Object -First 20
	if ($Problems)
	{
		Write-Report ""
		Write-Report "--- Auffaellige Logzeilen ($TestLog) ---"
		$Problems | ForEach-Object { Write-Report $_.Line }
	}
}

Write-Report ""
if ($Total -gt 0 -and $Failed -eq 0)
{
	Write-Report "Tests: OK ($Succeeded von $Total bestanden, $TestMinutes min, Editor-Exit-Code $($Process.ExitCode))"
	Write-Report ""
	Write-Report "ERGEBNIS: ERFOLGREICH"
	Save-Report
	exit 0
}

Write-Report "Tests: FEHLGESCHLAGEN ($Succeeded bestanden, $Failed fehlgeschlagen, $Total gesamt, Editor-Exit-Code $($Process.ExitCode))"
Write-Report ""
Write-Report "ERGEBNIS: FEHLGESCHLAGEN"
Save-Report
exit 1

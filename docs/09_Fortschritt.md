# Fortschrittsbericht

## Status je Funktion

Zustände: Geplant · In Arbeit · Implementiert · Kompiliert · Getestet · Im Spiel überprüft · Blockiert

| Funktion | Status | Nachweis |
|---|---|---|
| Phase-0-Bestandsaufnahme und Dokumentation | abgeschlossen | `docs/00`–`10` |
| `FMoney` (überlaufsicher, ganzzahlig) | **Getestet** (Standalone: GCC, Clang, ASan/UBSan) | `Tests/SimCore/MoneyTests.cpp` |
| `FLedger` (atomar, Idempotenz, Integritätsprüfung) | **Getestet** (Standalone: GCC, Clang, ASan/UBSan) | `Tests/SimCore/LedgerTests.cpp` |
| Kern unter MSVC | **Getestet** (CI, `windows-latest`) | `.github/workflows/simcore.yml` |
| Kern als DLL (wie Unreal-Editor-Build) | **Getestet** (GCC/Clang shared lokal; MSVC-DLL in CI-Lauf 3, `/W4 /WX`, keine C4251) | `EMPIRE_SHARED=ON` |
| UE-Modul `EmpireSimCore` (`Build.cs`, Modulregistrierung) | **Implementiert**, nicht kompiliert | wartet auf deinen PC |
| Unreal-Projekt (`.uproject`, Targets, Modul `EmpireEngine`, Config) | **Implementiert**, nicht kompiliert | AP-1.5, wartet auf deinen PC |
| Unreal-Smoke-Test `Empire.SimCore.LedgerSmoke` | **Implementiert**, nicht ausgeführt | AP-1.5 |
| `Tools/Windows/BuildAndTest` (Build + Tests + Bericht) | **Implementiert**, nie ausgeführt | AP-1.5 |
| Spieluhr, Speicherformat, Inventar, Welt-Transaktion | Geplant | AP-1.2 – AP-1.4 |
| Alle Gameplay-Systeme | Geplant | Phase 2+ |

Nichts ist bisher *im Spiel überprüft*, weil noch kein Spiel existiert.

## Bericht 1 – 2026-10-27: Phase 0 + AP-1.1

**A. Erledigt:** Umgebungsanalyse; Projektstruktur, Versionskontrollstrategie (Git + LFS), Architektur,
Entwicklungsplan, Abnahmekriterien Phase 1; Simulationskern mit `FMoney` und `FLedger`; Standalone-Build
und Tests; CI für Linux und Windows.

**B. Dateien:** `README.md`, `.gitignore`, `.gitattributes`, `.github/workflows/simcore.yml`, `docs/*`,
`Source/EmpireSimCore/**`, `Tests/SimCore/**` (alle neu).

**C. Tests:** 19 Tests, alle bestanden mit GCC 13.3 und Clang 18.1 (Debug, `-Werror`), GCC Release und GCC
mit ASan/UBSan. Die Gegenprobe (absichtlicher Fehler) wurde erkannt. In der CI (GitHub Actions) sind alle 4 Jobs grün:
Linux GCC, Linux Clang, Linux GCC + Sanitizer, Windows MSVC. Clang-Sanitizer wurden lokal nicht ausgeführt.

**D. Offen:** L-001 bis L-007 in `07_Bekannte_Fehler.md`; ADR-001 wartet auf deine Bestätigung.

**E. Auswirkungen:** Keine bestehenden Systeme (Repository war leer).

**F. Nächster Schritt:** AP-1.5 (Unreal-Projekt), sobald die UE-Version bekannt ist, parallel dazu
AP-1.3 (versioniertes Speicherformat inklusive Tests für beschädigte Daten), weil Speichern zu den kritischsten Grundlagen gehört
und vollständig ohne UE testbar ist.

## Bericht 2 – 2026-10-27: AP-1.5 vorbereitet (Unreal-Projekt)

**A. Erledigt:** Engine-Wahl UE 5.8 + Visual Studio Community (ADR-008); Hardware dokumentiert, Entwicklungsrechner
als Mindest-Referenzsystem (ADR-009); ADR-001 angenommen; Unreal-Projekt angelegt (`.uproject`, Targets,
Gameplay-Modul `EmpireEngine`, Config, Enhanced Input); Unreal-Smoke-Test für den Kern; Ein-Klick-Skript
`BuildAndTest.bat` (Engine finden, Version eintragen, kompilieren, Unreal-Tests, Bericht); Exporte des Kerns
DLL-tauglich gemacht und per Shared-Build in CI abgesichert; Einsteiger-Anleitung `docs/11_Einrichtung_Windows.md`.

**B. Dateien:** neu: `EmpireEngine.uproject`, `Config/Default{Engine,Game,Input}.ini`, `Source/EmpireEngine*.Target.cs`,
`Source/EmpireEngine/**`, `Tools/Windows/BuildAndTest.{bat,ps1}`, `docs/11_Einrichtung_Windows.md`;
geändert: `Ledger.h` (Exporte pro Funktion), `Money.h` (Schutz vor `min`/`max`-Makros), `Tests/SimCore/CMakeLists.txt`
(`EMPIRE_SHARED`), CI-Workflow, `.gitattributes` (CRLF für Windows-Skripte), `README.md`, `docs/00`, `02`, `03`, `06`–`10`.

**C. Tests:** lokal 19/19 in sechs Konfigurationen (GCC/Clang statisch und shared, GCC + Sanitizer, Release); die Gegenprobe
für einen fehlenden Export wurde erkannt. **Nicht** getestet: alles, was Unreal betrifft, sowie das PowerShell-Skript.

**D. Offen:** L-001, L-008, L-009; der erste Build auf deinem PC kann Anpassungen erfordern (Engine- oder VS-Version).

**E. Auswirkungen:** Kern-API unverändert, nur die Export-Markierung ist verschoben. Keine Verhaltensänderung.

**F. Nächster Schritt:** Du führst `docs/11_Einrichtung_Windows.md` aus und schickst `Report.txt`. Parallel setze ich
AP-1.3 um (versioniertes Speicherformat), weil es ohne Unreal testbar ist.

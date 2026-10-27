# Fortschrittsbericht

## Status je Funktion

Zustände: Geplant · In Arbeit · Implementiert · Kompiliert · Getestet · Im Spiel überprüft · Blockiert

| Funktion | Status | Nachweis |
|---|---|---|
| Phase-0-Bestandsaufnahme und Dokumentation | abgeschlossen | `docs/00`–`10` |
| `FMoney` (überlaufsicher, ganzzahlig) | **Getestet** (Standalone: GCC, Clang, ASan/UBSan) | `Tests/SimCore/MoneyTests.cpp` |
| `FLedger` (atomar, Idempotenz, Integritätsprüfung) | **Getestet** (Standalone: GCC, Clang, ASan/UBSan) | `Tests/SimCore/LedgerTests.cpp` |
| Kern unter MSVC | siehe CI-Lauf | `.github/workflows/simcore.yml` |
| UE-Modul `EmpireSimCore` (`Build.cs`, Modulregistrierung) | **Implementiert**, nicht kompiliert | benötigt UE |
| Unreal-Projekt | **Blockiert** – UE-Version unbekannt | AP-1.5 |
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
mit ASan/UBSan. Die Gegenprobe (absichtlicher Fehler) wurde erkannt. Clang-Sanitizer und MSVC wurden lokal nicht ausgeführt.

**D. Offen:** L-001 bis L-007 in `07_Bekannte_Fehler.md`; ADR-001 wartet auf deine Bestätigung.

**E. Auswirkungen:** Keine bestehenden Systeme (Repository war leer).

**F. Nächster Schritt:** AP-1.5 (Unreal-Projekt), sobald die UE-Version bekannt ist, parallel dazu
AP-1.3 (versioniertes Speicherformat inklusive Tests für beschädigte Daten), weil Speichern zu den kritischsten Grundlagen gehört
und vollständig ohne UE testbar ist.

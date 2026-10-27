# Empire Engine

Realistische Open-World-Lebens-, Fahrzeug-, Wirtschafts- und Unternehmenssimulation
für PC (zunächst Windows) auf Basis von Unreal Engine 5. Einzelspieler und privater Koop (2–8 Spieler).

> **Aktueller Stand: Phase 1 läuft.** Unreal-Projekt (UE 5.8) angelegt, aber **noch nicht auf Windows kompiliert**.
> Der engine-unabhängige Simulationskern `EmpireSimCore` mit Geld-Ledger ist in CI getestet.
> Details: [docs/09_Fortschritt.md](docs/09_Fortschritt.md).
>
> **Erste Schritte auf Windows:** [docs/11_Einrichtung_Windows.md](docs/11_Einrichtung_Windows.md)

## Repository-Struktur

```
EmpireEngine.uproject        Unreal-Projektdatei
Config/                      Unreal-Konfiguration
Source/EmpireEngine/         Unreal-Gameplay-Modul (Smoke-Test unter Private/Tests)
Source/EmpireSimCore/        Engine-unabhängiger Simulationskern (zugleich Unreal-Modul)
  Public/Empire/...          Öffentliche Header
  Private/...                Implementierung
  EmpireSimCore.Build.cs     UE-Moduldefinition (noch nicht mit UE kompiliert)
Tests/SimCore/               Standalone-Build (CMake) und Tests des Simulationskerns
Tools/Windows/               BuildAndTest.bat: kompilieren + Unreal-Tests + Bericht
docs/                        Projektdokumentation (siehe unten)
.github/workflows/           CI: Build + Tests auf Linux (GCC, Clang, Sanitizer, Shared) und Windows (MSVC, DLL)
```

## Schnellstart (Simulationskern)

```bash
cmake -S Tests/SimCore -B build/simcore
cmake --build build/simcore
ctest --test-dir build/simcore --output-on-failure
```

Vollständige Anleitung inkl. Unreal-Schritten: [docs/10_Build_Anleitung.md](docs/10_Build_Anleitung.md).

## Dokumentation

| Dokument | Inhalt |
|---|---|
| [00_Ausgangsbericht](docs/00_Ausgangsbericht.md) | Phase-0-Bestandsaufnahme der Entwicklungsumgebung |
| [01_Projektuebersicht](docs/01_Projektuebersicht.md) | Vision, Ziele, Prioritäten |
| [02_Architektur](docs/02_Architektur.md) | Architektur, Modulschnitt, C++/Blueprint-Grenzen |
| [03_Entwicklungsplan](docs/03_Entwicklungsplan.md) | Phasen, Arbeitspakete, Abnahmekriterien |
| [04_Datenmodell](docs/04_Datenmodell.md) | Datenmodelle (Geld, Konten, Transaktionen) |
| [05_Schnittstellen](docs/05_Schnittstellen.md) | Öffentliche Schnittstellen der Systeme |
| [06_Testplan](docs/06_Testplan.md) | Teststrategie und tatsächlich ausgeführte Tests |
| [07_Bekannte_Fehler](docs/07_Bekannte_Fehler.md) | Bekannte Fehler und Einschränkungen |
| [08_Entscheidungsprotokoll](docs/08_Entscheidungsprotokoll.md) | Architekturentscheidungen (ADR) |
| [09_Fortschritt](docs/09_Fortschritt.md) | Fortschrittsberichte und Status je Funktion |
| [10_Build_Anleitung](docs/10_Build_Anleitung.md) | Öffnen, Erstellen und Testen |
| [11_Einrichtung_Windows](docs/11_Einrichtung_Windows.md) | Ersteinrichtung auf Windows ohne Vorkenntnisse |

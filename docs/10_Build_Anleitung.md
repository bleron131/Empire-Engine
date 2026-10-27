# Anleitung: Öffnen, Erstellen und Testen

## A. Unreal-Projekt auf Windows

Für die Ersteinrichtung ohne Vorkenntnisse: **[11_Einrichtung_Windows.md](11_Einrichtung_Windows.md)**.

Kurzfassung, wenn alles installiert ist:

| Aufgabe | Vorgehen |
|---|---|
| Kompilieren + Unreal-Tests + Bericht | `Tools\Windows\BuildAndTest.bat` doppelklicken → `Saved\EmpireReport\Report.txt` |
| Bestimmte Engine verwenden | `powershell -ExecutionPolicy Bypass -File Tools\Windows\BuildAndTest.ps1 -EnginePath "D:\UE_5.8"` |
| Editor öffnen | `EmpireEngine.uproject` doppelklicken (nach erfolgreichem Build) |
| Unreal-Tests im Editor | *Tools → Test Automation* → Filter „Empire“ → *Start Tests* |

Projektstruktur (Unreal):

```
EmpireEngine.uproject             Projektdatei (Module: EmpireEngine, EmpireSimCore; Plugin: EnhancedInput)
Config/                           Default*.ini (Engine, Spiel, Eingabe)
Source/EmpireEngine.Target.cs     Spiel-Target
Source/EmpireEngineEditor.Target.cs  Editor-Target
Source/EmpireEngine/              Unreal-Gameplay-Modul (Tests unter Private/Tests)
Source/EmpireSimCore/             Engine-unabhängiger Simulationskern
```

## B. Simulationskern allein (Linux, Windows, macOS)

Voraussetzungen: CMake ≥ 3.20 und ein C++20-Compiler (Visual Studio, GCC ≥ 11 oder Clang ≥ 14).

```bash
cmake -S Tests/SimCore -B build/simcore
cmake --build build/simcore --config Debug
ctest --test-dir build/simcore -C Debug --output-on-failure
```

Optionen:

| Option | Wirkung |
|---|---|
| `-DEMPIRE_SANITIZE=ON` | GCC/Clang: AddressSanitizer + UndefinedBehaviorSanitizer |
| `-DEMPIRE_SHARED=ON` | Kern als DLL/Shared Library mit versteckten Symbolen (wie Unreals Editor-Build) |
| `-DEMPIRE_WARNINGS_AS_ERRORS=OFF` | Warnungen nicht als Fehler behandeln |

## C. Git LFS

GitHub Desktop bringt Git LFS mit. Bei der Kommandozeile einmalig: `git lfs install`.

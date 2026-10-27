# Anleitung: Öffnen, Erstellen und Testen

## A. Simulationskern (funktioniert heute)

Voraussetzungen: CMake ≥ 3.20 und ein C++20-Compiler (Visual Studio 2022, GCC ≥ 11 oder Clang ≥ 14).

```bash
git clone https://github.com/bleron131/Empire-Engine.git
cd Empire-Engine
cmake -S Tests/SimCore -B build/simcore
cmake --build build/simcore --config Debug
ctest --test-dir build/simcore -C Debug --output-on-failure
```

Optionen: `-DEMPIRE_SANITIZE=ON` (GCC/Clang: ASan + UBSan), `-DEMPIRE_WARNINGS_AS_ERRORS=OFF`.

Unter Windows funktioniert das in der „Developer PowerShell for VS 2022“ oder direkt in Visual Studio
über *Datei → Öffnen → CMake…* mit `Tests/SimCore/CMakeLists.txt`.

## B. Git LFS (einmalig vor dem ersten Asset)

```bash
git lfs install
```

## C. Unreal-Projekt (noch nicht vorhanden – AP-1.5)

Diese Schritte erstelle ich gemeinsam mit dir, sobald die UE-Version feststeht. Vorgesehener Ablauf:

1. Du nennst mir die installierte UE-Version (Epic Games Launcher → Bibliothek) und die Visual-Studio-Version.
2. Ich erzeuge `EmpireEngine.uproject`, `Source/EmpireEngine.Target.cs`, `Source/EmpireEngineEditor.Target.cs`
   und das Spielmodul `Source/EmpireEngine/`, passend zur Version, und binde `EmpireSimCore` ein.
3. Du führst aus: Rechtsklick auf `EmpireEngine.uproject` → *Generate Visual Studio project files*,
   dann in Visual Studio *Development Editor | Win64* bauen und den Editor starten.
4. Du schickst mir das Build-Ergebnis (bei Fehlern den Ausgabetext). Erst dann gilt AP-1.5 als *kompiliert*.

# Entscheidungsprotokoll (ADR)

Status: *Vorgeschlagen* (Bestätigung durch dich nötig) · *Angenommen* · *Ersetzt*

## ADR-001
**Engine-unabhängiger Simulationskern `EmpireSimCore`** · Status: **Angenommen** (2026-10-27, Entscheidung an die KI delegiert)

- **Kontext:** Die KI kann Unreal nicht kompilieren oder ausführen. Die wichtigsten Anforderungen
  (Datenintegrität, Besitzrechte, wirtschaftliche Konsistenz, Exploit-Schutz) müssen trotzdem früh und
  automatisiert getestet werden.
- **Entscheidung:** Wirtschafts-, Auftrags-, Inventar-, Produktions-, Berechtigungs- und Serialisierungslogik
  entsteht als reines C++20 ohne Unreal-Abhängigkeiten in `Source/EmpireSimCore`. Unreal kompiliert
  dieselben Dateien als Modul; ein CMake-Build (`Tests/SimCore`) kompiliert und testet sie eigenständig.
  Unreal-Gameplay-Code ruft den Kern über Subsysteme auf.
- **Alternativen:** (a) Alles als UObjects/UE-Automation-Tests – nur auf deinem PC testbar, langsamere
  Rückkopplung. (b) Kern als externe Bibliothek/DLL – zusätzlicher Build-Aufwand ohne Nutzen.
- **Konsequenzen:** + schnell, deterministisch, in CI testbar; + saubere Trennung Regeln/Darstellung.
  − Brückenschicht nötig (Stringtypen, Containertypen, Replikation); − Blueprints sehen Kerntypen nur über
  UE-Wrapper (`USTRUCT`s). Kern-Containertypen sind `std::`-Typen; das ist in UE-Modulen zulässig.
- **Umkehrbarkeit:** mittel. Die Logik ließe sich später in UE-Typen überführen.
- **Ergänzung:** Öffentliche Funktionen werden einzeln mit `EMPIRESIMCORE_API` exportiert, nicht die ganze Klasse
  (vermeidet MSVC-Warnung C4251 bei `std::`-Membern in DLLs). Der CI-Build `EMPIRE_SHARED=ON` prüft das.

## ADR-002
**Geld als `int64` in kleinster Einheit, vorerst eine Währung** · Status: Angenommen (Währungsfrage offen)

Gleitkomma würde zwischen Host und Clients sowie über Speicherstände hinweg zu Abweichungen führen.
Offen: Haben Regionen eigene Währungen mit Wechselkursen, oder gibt es eine Spielwährung? Erst relevant,
wenn eine zweite Region entsteht; das Datenmodell lässt eine Währungskennung als Erweiterung zu.

## ADR-003
**Doppelte Buchführung mit Systemkonten und Idempotenz-Schlüsseln** · Status: Angenommen

Jede Geldbewegung ist eine ausgeglichene Transaktion. Geld entsteht für Spieler nur über Systemkonten
(NPC-Kunden, Staat, Lieferanten). Dadurch sind Geldschöpfung und -vernichtung jederzeit messbar und
Exploits in Audits erkennbar. Idempotenz-Schlüssel verhindern Doppelzahlungen bei Wiederholungen,
doppelten Netzwerknachrichten oder einer Wiederaufnahme nach einem Absturz.

## ADR-004
**Konventionen** · Status: Angenommen

Code, Bezeichner und Code-Kommentare auf Englisch im Unreal-Namensstil; Projektdokumentation auf Deutsch.
Keine Exceptions/RTTI im Kern; Fehler als Rückgabewerte.

## ADR-005
**Eigenes minimales Testframework für den Kern** · Status: Angenommen

Unterstützt keine Exceptions (wie der Kern), hat keine externen Abhängigkeiten und keine Netzwerkdownloads.
Engine-seitige Tests verwenden später das Unreal-Automation-Framework. Ein Wechsel auf z. B. doctest oder
GoogleTest ist möglich, falls das Framework nicht mehr ausreicht.

## ADR-006
**Repository-Layout = Unreal-Projektwurzel, Git LFS für Binärassets** · Status: Angenommen

Das Repository-Wurzelverzeichnis wird später das `.uproject` enthalten. `Binaries/`, `Intermediate/`,
`Saved/`, `DerivedDataCache/` werden ignoriert. `.uasset`, `.umap` und Quellmedien laufen über Git LFS.
Offen: Reicht das GitHub-LFS-Kontingent für fotorealistische Assets, oder wird später z. B. Perforce benötigt?

## ADR-007
**Host-autoritatives, single-threaded Kernmodell; Berechtigungen als eigene Schicht** · Status: Angenommen

Der Host besitzt den einzigen veränderbaren Kernzustand. Besitz (`FOwnerRef`) und operative Berechtigung
sind getrennt. Berechtigungsprüfungen erfolgen vor jedem Commit in einer eigenen Schicht (geplant).

## ADR-008
**Unreal Engine 5.8 (Launcher-Version) + Visual Studio Community** · Status: Angenommen (2026-10-27)

- **Kontext:** Du hast keine Unreal-Erfahrung und noch keine Engine installiert. Vorgabe: kostenlos.
- **Entscheidung:** Neueste stabile Version aus dem Epic Games Launcher, derzeit **5.8** (Juni 2026,
  Hotfixes bis 5.8.4). Keine Engine aus dem Quellcode. Visual Studio Community, laut 5.8-Release-Notes
  ist **2026 empfohlen** und 2022 v17.14 das Minimum. Die Targets verwenden `BuildSettingsVersion.Latest`
  und `EngineIncludeOrderVersion.Latest`, damit sie ohne Änderung mit der installierten 5.x-Version bauen.
  `BuildAndTest.ps1` trägt die tatsächlich installierte Version automatisch als `EngineAssociation` ein.
- **Begründung:** kostenlos; die neueste Version hat die längste Lebensdauer bis UE 6 (Early Access frühestens Ende 2027).
  Der Build läuft direkt über `Build.bat` (UnrealBuildTool), ohne Visual-Studio-Projektdateien. Dadurch umgehen wir
  gemeldete Probleme der Projektgenerierung mit VS 2026 und brauchen keine IDE-Kenntnisse.
- **Risiko:** Die Epic-Dokuseiten waren aus der KI-Umgebung nicht abrufbar. Die Angaben stammen aus den
  5.8-Release-Notes (Suchergebnis) und Forenbeiträgen. Bei Compiler-Problemen ist die VS-2022-Toolchain (MSVC v143) der Rückfallweg.
- **Vor Release:** Engine-Version fest pinnen (`BuildSettingsVersion.Vx`) und die dann gültige Epic-Lizenz prüfen.

## ADR-009
**Entwicklungsrechner = Mindest-Referenzsystem** · Status: Angenommen (2026-10-27)

- **Kontext:** Dein Rechner (HP Envy x360 16, Ryzen 7 8840HS, integrierte Radeon 780M, 16 GB RAM) liegt unter
  den üblichen Empfehlungen für Unreal-Entwicklung (dedizierte GPU, 32 GB RAM).
- **Entscheidung:** Entwickelt wird trotzdem auf diesem Gerät, mit Editor-Skalierbarkeit „Mittel“ oder „Niedrig“.
  Das Gerät dient zugleich als **unteres Referenzsystem**: Das Spiel muss auf niedrigen Einstellungen hier spielbar sein.
  Für das fotorealistische Grafikziel auf Gaming-PCs braucht es später zusätzliche Messungen auf stärkerer Hardware
  (offen, z. B. durch Spieltester).
- **Konsequenzen:** Lange Kompilier- und Shaderzeiten; große Inhaltsmengen (Weltgröße, Nanite-Assets) lassen sich hier
  nur eingeschränkt beurteilen. Rendering-Features (Lumen, Nanite, Virtual Shadow Maps) werden nach dem ersten
  Benchmark festgelegt, nicht vorher.

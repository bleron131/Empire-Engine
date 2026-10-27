# Phase 0 – Technischer Ausgangsbericht

Stand: 2026-10-27. Alle Angaben wurden in der Entwicklungsumgebung durch Befehle tatsächlich geprüft.

## 1. Repository

| Prüfung | Ergebnis |
|---|---|
| Git-Repository | vorhanden (`github.com/bleron131/Empire-Engine`) |
| Commits | **keine** – leeres Repository |
| Dateien | **keine** |
| Unreal-Projekt (`.uproject`) | **nicht vorhanden** |
| Arbeitsbranch | `claude/confident-ramanujan-0w8vsi` |

## 2. Entwicklungsumgebung der KI (Cloud-Container)

| Komponente | Ergebnis |
|---|---|
| Betriebssystem | Ubuntu 24.04.5 LTS (Linux x86_64), **kein Windows** |
| CPU / RAM | 4 Kerne / 15 GB |
| GPU | **keine** (kein `/dev/dri`, kein `nvidia-smi`) |
| Unreal Engine | **nicht installiert**; Epic-Server über das Netzwerk **nicht erreichbar** (Proxy 403) |
| Unreal Editor / UBT | **nicht verfügbar** |
| C++-Compiler | GCC 13.3.0, Clang 18.1.3 |
| MSVC | **nicht verfügbar** lokal (wird über GitHub Actions `windows-latest` geprüft) |
| Build-Werkzeuge | CMake 3.28.3, Ninja, Make |
| Git LFS | 3.4.1 |
| Python | 3.13 |
| Clang-Sanitizer-Runtime | **fehlt** (ASan/UBSan nur mit GCC lokal nutzbar) |

## 3. Daraus folgende technische Einschränkungen

1. **Unreal-C++ und Blueprints können von der KI weder kompiliert noch ausgeführt werden.**
   Jeder UE-spezifische Code (UObjects, Actors, Blueprints, Replikation) ist bis zur Prüfung auf
   deinem Rechner höchstens *implementiert*, nie *kompiliert* oder *getestet*.
2. **Keine Laufzeittests im Spiel**, keine Performance-Messungen, keine Multiplayer-Tests,
   keine Grafikprüfung durch die KI.
3. **Blueprints und `.uasset`/`.umap`-Dateien** sind Binärdateien, die nur der Unreal Editor erzeugen kann.
   Die KI kann sie nicht erstellen.
4. Die genaue **Unreal-Engine-Version** lässt sich aus dieser Umgebung nicht bestimmen. Sie hängt von
   der Installation auf deinem PC ab und muss von dir genannt werden.

## 4. Was tatsächlich möglich ist

- Engine-unabhängigen C++20-Code (Wirtschaft, Ledger, Inventar, Aufträge, Produktion, Spieluhr,
  Speicherformat, Berechtigungen) **schreiben, kompilieren und automatisiert testen** – mit GCC, Clang
  und (über CI) MSVC, mit denselben Einschränkungen wie Unreal (keine Exceptions, kein RTTI).
- UE-Projektdateien (`.uproject`, `*.Build.cs`, `*.Target.cs`, C++-Klassen) als Text erstellen,
  ausdrücklich als *nicht kompiliert* gekennzeichnet, zur Prüfung auf deinem Rechner.
- Dokumentation, Planung, Code-Reviews, Auswertung von Build-Logs, die du aus dem Editor zurückmeldest.

## 5. Gewählter Ansatz

Siehe [ADR-001](08_Entscheidungsprotokoll.md#adr-001): Die kritische Simulationslogik liegt in einem
engine-unabhängigen Kernmodul `EmpireSimCore`, das hier vollständig testbar ist und von Unreal als
normales Modul kompiliert wird. Unreal übernimmt Darstellung, Eingabe, Physik, Replikation und Inhalte.

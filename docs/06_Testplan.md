# Testplan

## 1. Testebenen

| Ebene | Werkzeug | Ausführbar durch KI? |
|---|---|---|
| Unit-/Eigenschaftstests des Simulationskerns | `Tests/SimCore` (CMake, eigenes Mini-Framework) | **ja** |
| CI (Linux GCC/Clang/Sanitizer, Windows MSVC) | GitHub Actions `.github/workflows/simcore.yml` | ja (Ergebnis abrufbar) |
| Unreal-Automation-Tests (Gameplay, Integration) | UE Automation (`Empire.*`), ausgeführt durch `Tools\Windows\BuildAndTest.bat` | **nein** – auf deinem PC |
| Multiplayer-Tests | PIE mit mehreren Clients, später echte Sitzungen | **nein** – auf deinem PC |
| Performance-Messungen | Unreal Insights, `stat unit` auf realer Hardware | **nein** – auf deinem PC |
| Manuelle Funktionstests | Checklisten je Abnahmekriterium | **nein** – auf deinem PC |

## 2. Tatsächlich ausgeführte Tests (2026-10-27)

Umgebung: Ubuntu 24.04, Testlauf `EmpireSimCoreTests`.

| Konfiguration | Ergebnis |
|---|---|
| GCC 13.3, Debug, `-Werror` | 19/19 bestanden |
| Clang 18.1, Debug, `-Werror` | 19/19 bestanden |
| GCC 13.3, Debug, ASan + UBSan | 19/19 bestanden, keine Befunde |
| GCC 13.3, Release (`-O2`) | 19/19 bestanden |
| Clang + Sanitizer | **nicht ausgeführt** – Sanitizer-Runtime fehlt in der Umgebung |
| MSVC (GitHub Actions `windows-latest`, Debug, `/W4 /WX`) | bestanden (CI-Lauf 1) |
| CI Linux GCC, Clang, GCC + Sanitizer | bestanden (CI-Lauf 1) |

### Nachtrag 2026-10-27 (Arbeitspaket AP-1.5)

| Konfiguration | Ergebnis |
|---|---|
| GCC 13.3 und Clang 18.1, Shared Library mit versteckten Symbolen (`EMPIRE_SHARED=ON`) | 19/19 bestanden |
| Gegenprobe: Export von `VerifyIntegrity` entfernt | Linkfehler erkannt (`undefined reference`), danach wiederhergestellt |
| Wiederholung GCC, Clang, GCC + Sanitizer, Release (statisch) | je 19/19 bestanden |
| Unreal-Smoke-Test `Empire.SimCore.LedgerSmoke` | **nicht ausgeführt** – benötigt Unreal auf deinem PC |
| `BuildAndTest.ps1` | **nicht ausgeführt** – keine Windows-/PowerShell-Umgebung bei der KI |

**Gegenprobe:** Die Summenprüfung im Ledger wurde testweise deaktiviert. Der Test
`Ledger_RejectsMalformedRequests` schlug daraufhin wie erwartet fehl; danach wurde der Code wiederhergestellt.
Erkenntnis: Der Zufallstest allein hätte diesen Fehler nicht gefunden, weil `Transfer` immer
ausgeglichen bucht. Gezielte Negativtests bleiben deshalb nötig.

## 3. Abgedeckte Anforderungen (Wirtschaftstests §19.2)

| Anforderung | Test(s) |
|---|---|
| Keine unberechtigte Geldgenerierung | `InternalTransfersDoNotCreateMoney`, `RandomOperationsPreserveInvariants`, `RejectsMalformedRequests` |
| Keine Doppelauszahlung | `DuplicateKeyNeverPaysTwice`, `ReusedKeyWithDifferentContentIsConflict` |
| Atomarität | `MultiPostingIsAtomicWhenOneLegFails`, `InsufficientFundsChangesNothing` |
| Keine negativen Salden ohne Kreditrahmen | `InsufficientFundsChangesNothing`, `CreditLineAllowsNegativeUpToLimit`, Zufallstest |
| Überlauf | `OverflowIsRejected`, `Money_*` |
| Konsistenzprüfung | `VerifyIntegrityPassesOnConsistentState`, Zufallstest |

**Noch nicht abgedeckt:** Erkennung manipulierter/beschädigter Zustände durch `VerifyIntegrity`
(sinnvoll testbar erst mit dem Laden von Speicherständen, AP-1.3), Waren, Aufträge, Schulden/Insolvenz.

## 4. Ausführen

```bash
cmake -S Tests/SimCore -B build/simcore
cmake --build build/simcore
ctest --test-dir build/simcore --output-on-failure
# einzelne Tests: build/simcore/EmpireSimCoreTests Ledger_
```

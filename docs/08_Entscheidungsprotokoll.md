# Entscheidungsprotokoll (ADR)

Status: *Vorgeschlagen* (Bestätigung durch dich nötig) · *Angenommen* · *Ersetzt*

## ADR-001
**Engine-unabhängiger Simulationskern `EmpireSimCore`** · Status: **Vorgeschlagen – Bestätigung erbeten**

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

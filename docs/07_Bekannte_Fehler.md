# Bekannte Fehler und Einschränkungen

## Fehler

Derzeit keine bekannten Fehler im getesteten Code.

## Einschränkungen

| Id | Bereich | Beschreibung | Geplante Lösung |
|---|---|---|---|
| L-001 | Gesamtprojekt | Unreal-Projektdateien (`.uproject`, `*.Target.cs`, `*.Build.cs`, Modul-Code, Smoke-Test) sind **nicht kompiliert** | `BuildAndTest.bat` auf deinem PC (AP-1.5) |
| L-002 | Ledger | Journal und Idempotenz-Index wachsen unbegrenzt (Speicher und Speicherstandgröße) | Snapshots/Archivierung abgeschlossener Perioden, zusammen mit AP-1.3 |
| L-003 | Ledger | Duplikaterkennung vergleicht nur Postings und Grund, nicht Memo/Spielzeit (gewollt: Wiederholungen dürfen später erfolgen) | – (dokumentiertes Verhalten) |
| L-004 | Ledger | Kontodaten (Name, Eigentümer, Kreditrahmen) sind nach dem Anlegen nicht änderbar | Bei Bedarf mit Eigentumswechsel/Krediten |
| L-005 | Ledger | Nur eine Währung | ADR-002 offen |
| L-006 | Tests | Clang-Sanitizer lokal nicht verfügbar (GCC-Sanitizer laufen lokal und in CI); MSVC nur über CI geprüft | – |
| L-008 | Werkzeuge | `Tools/Windows/BuildAndTest.ps1` wurde nie ausgeführt (keine Windows-Umgebung bei der KI); Auswertung von `index.json` basiert auf dem bekannten Format der Unreal-Automation-Berichte | erster Lauf auf deinem PC |
| L-009 | Grafik | Kein Level und keine Rendering-Einstellungen; der Editor öffnet ein leeres Level | Phase 2 und erster Benchmark |
| L-007 | Ledger | Nicht thread-sicher (gewollt, Host-autoritativ, Game-Thread) | Bei Bedarf asynchrone Hintergrundsimulation über Befehlswarteschlange |

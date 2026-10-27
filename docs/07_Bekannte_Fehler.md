# Bekannte Fehler und Einschränkungen

## Fehler

Derzeit keine bekannten Fehler im getesteten Code.

## Einschränkungen

| Id | Bereich | Beschreibung | Geplante Lösung |
|---|---|---|---|
| L-001 | Gesamtprojekt | Kein Unreal-Projekt vorhanden; UE-Dateien (`EmpireSimCore.Build.cs`, `EmpireSimCoreModule.cpp`) sind **nicht kompiliert** | AP-1.5 auf deinem PC |
| L-002 | Ledger | Journal und Idempotenz-Index wachsen unbegrenzt (Speicher und Speicherstandgröße) | Snapshots/Archivierung abgeschlossener Perioden, zusammen mit AP-1.3 |
| L-003 | Ledger | Duplikaterkennung vergleicht nur Postings und Grund, nicht Memo/Spielzeit (gewollt: Wiederholungen dürfen später erfolgen) | – (dokumentiertes Verhalten) |
| L-004 | Ledger | Kontodaten (Name, Eigentümer, Kreditrahmen) sind nach dem Anlegen nicht änderbar | Bei Bedarf mit Eigentumswechsel/Krediten |
| L-005 | Ledger | Nur eine Währung | ADR-002 offen |
| L-006 | Tests | Clang-Sanitizer lokal nicht verfügbar (GCC-Sanitizer laufen lokal und in CI); MSVC nur über CI geprüft | – |
| L-007 | Ledger | Nicht thread-sicher (gewollt, Host-autoritativ, Game-Thread) | Bei Bedarf asynchrone Hintergrundsimulation über Befehlswarteschlange |

# Entwicklungsplan

Planungsgrundlage: ca. 5–10 Stunden pro Woche. Arbeitspakete (AP) sind so geschnitten, dass jedes
einzeln abgeschlossen, getestet und überprüft werden kann. Es werden bewusst **keine Kalenderdaten**
versprochen; die Reihenfolge ist verbindlich, die Dauer eine Schätzung.

Legende Abhängigkeit **[UE]**: benötigt Unreal Engine auf deinem PC (Kompilieren/Testen durch dich).

## Phase 0 – Umgebungsanalyse und Planung ✔ abgeschlossen

Ergebnisse: Ausgangsbericht, Architektur, Projektstruktur, dieser Plan, Werkzeugliste, Risiken.

## Phase 1 – Technisches Grundgerüst (laufend)

| AP | Inhalt | Abhängigkeit | Status |
|---|---|---|---|
| AP-1.1 | Geldtyp `FMoney` und atomares Ledger mit Idempotenz und Integritätsprüfung | – | getestet (Standalone) |
| AP-1.2 | Gemeinsame Spieluhr (konfigurierbare Tageslänge, Standard 60–90 reale Minuten, Datum/Jahreszeit) | – | geplant |
| AP-1.3 | Versioniertes Speicherformat für den Kernzustand: Serialisierung, Prüfsumme, Versionsnummer, Migrationsgerüst, Integritätsprüfung beim Laden, Tests für beschädigte/abgeschnittene Daten | AP-1.1 | geplant |
| AP-1.4 | Warenmodell und Inventar (Waren-Definitionen datengetrieben) + übergreifende Welt-Transaktion (Geld + Waren atomar) | AP-1.1 | geplant |
| AP-1.5 | Unreal-Projekt anlegen, `EmpireSimCore` einbinden, Editor-Build erfolgreich, Unreal-Smoke-Test grün | **[UE]** | implementiert, wartet auf Build auf deinem PC |
| AP-1.6 | UE-Subsystem als Brücke zum Kern, Logging-Kategorien, Speichern/Laden auf Datenträger (atomar: temporäre Datei + Umbenennen, Backups) | AP-1.3, AP-1.5 **[UE]** | geplant |
| AP-1.7 | Eingabe- (Enhanced Input; Maus/Tastatur/Controller) und Interaktionsarchitektur (gemeinsame Schnittstelle) | AP-1.5 **[UE]** | geplant |

**Abnahmekriterien Phase 1**

1. Das UE-Projekt lässt sich auf deinem PC kompilieren und öffnen (Build-Log als Nachweis).
2. `EmpireSimCore` baut in CI auf Linux (GCC, Clang, Sanitizer) und Windows (MSVC) und alle Tests sind grün.
3. Ein Testzustand (Konten + Buchungen + Spielzeit) wird gespeichert, geladen und ist danach identisch;
   `VerifyIntegrity()` meldet keine Probleme.
4. Beschädigte, abgeschnittene und versionsfremde Speicherdaten werden erkannt und abgelehnt (automatisierte Tests).
5. Struktur, Build und Tests sind dokumentiert.

## Phase 2 – Spielbarer Kern

Charakter, First-/Third-Person, Bewegung, Interaktion, ein fahrbares Fahrzeug (Chaos Vehicles),
ein Lieferauftrag, Geldbelohnung über das Ledger, einfache Warenbewegung, Speichern/Laden.

**Abnahme:** Welt betreten → Fahrzeug nutzen → Auftrag erledigen → Geld erhalten → speichern → laden.
Dieser Ablauf muss tatsächlich funktionieren, bevor weitere Inhalte entstehen. **[UE]**

Kernseitig vorbereitbar (ohne UE): Auftragsmodell mit Zuständen und Abschlusskriterien, Lieferauftrag
als erste Auftragsart, Fahrzeugzustand (Eigentümer, Treibstoff, Verschleiß, Ladung) als Datenmodell.

## Phase 3 – Früher Multiplayer-Test

Zwei Spieler, synchronisierte Bewegung und Interaktionen, gemeinsame Uhr, gemeinsame Aufträge,
konsistente Geld-/Warenänderungen, Eigentumsrechte, Speichern/Wiederherstellen.
Kernseitig: Berechtigungsmodell (Besitz ≠ operative Berechtigung) mit Tests. **[UE]** für Netzwerk.

## Phasen 4–8

Wie im Master-Prompt beschrieben (breiter Prototyp, Vertiefung der Simulation, Welt und Inhalte,
persönliche Systeme, Stabilisierung). Detailplanung erfolgt erst, wenn Phase 2 abgenommen ist,
weil sich Annahmen bis dahin noch ändern werden.

## Risiken

| Risiko | Auswirkung | Gegenmaßnahme |
|---|---|---|
| KI kann Unreal nicht kompilieren/ausführen | UE-Code bleibt ungeprüft, Integrationsfehler spät | Logik in testbaren Kern verlagern; kleine UE-Schritte; Build-Logs zurückmelden; optional Self-hosted Runner |
| Umfang vs. 5–10 h/Woche | Sehr lange Gesamtdauer | Strikte Phasen, vertikale Durchstiche, keine Inhaltsproduktion vor Abnahme Phase 2 |
| Fotorealistische Assets | Hoher Aufwand/Kosten, Lizenzfragen | Früh Asset-Quellen und Lizenzen klären (offene Entscheidung) |
| Host-Wechsel im Koop | Technisch schwierig in UE-Listen-Server-Modell | Zuerst sicherer Ablauf: Speichern → neuer Host lädt; automatischer Wechsel später prüfen |
| Git-LFS-Kontingent auf GitHub | Push großer Assets kann scheitern/kosten | Kontingent vor Asset-Import prüfen; Alternative (z. B. Perforce) bewerten |
| MSVC-spezifische Fehler | Kern baut auf Windows nicht | CI-Job `windows-msvc` |

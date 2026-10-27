# Architekturübersicht

## 1. Schichten

```
┌──────────────────────────────────────────────────────────────────────┐
│  Inhalte & Blueprints                                                 │
│  Level, Gebäude-/Maschinen-Konfiguration, Interaktionsabläufe, UI-Bindung │
├──────────────────────────────────────────────────────────────────────┤
│  Unreal-Gameplay-Modul EmpireEngine (C++, UObject/Actor)  [Gerüst angelegt] │
│  EmpireEngine: Charakter, Kameras, Interaktion, Fahrzeuge, Replikation, │
│  Speichern auf Datenträger, Subsysteme als Brücke zum Simulationskern │
├──────────────────────────────────────────────────────────────────────┤
│  EmpireSimCore (C++20, engine-unabhängig)                [vorhanden]  │
│  Geld/Ledger ✔ · Spieluhr · Waren/Inventar · Aufträge · Unternehmen · │
│  Produktion · Berechtigungen · Serialisierung/Migration               │
└──────────────────────────────────────────────────────────────────────┘
```

**Abhängigkeitsrichtung:** nur von oben nach unten. `EmpireSimCore` kennt Unreal nicht.

## 2. Warum ein engine-unabhängiger Kern (Kurzfassung von ADR-001)

- Die kritischsten Systeme (Prioritäten 1, 2 und 5: Datenintegrität, Besitzrechte, wirtschaftliche
  Konsistenz) werden deterministisch und automatisiert testbar, ohne Editor und ohne Spielstart.
- Die Simulation läuft nur auf dem Host (autoritativ). Clients erhalten replizierte Ergebnisse.
- Zusammengefasste Hintergrundsimulation entfernter Regionen nutzt dieselben Regeln wie die Nahsimulation.
- Kosten: eine Brückenschicht (Typumwandlung `FString`↔`std::string`, Replikation von Kernzuständen).

## 3. Regeln für `EmpireSimCore`

| Regel | Grund |
|---|---|
| Keine Unreal-Header (außer `EmpireSimCoreModule.cpp`) | Standalone-Build und Tests |
| Keine Exceptions, kein RTTI | Unreal-Standardeinstellungen; Fehler als Rückgabewerte |
| Ganzzahlige Geldbeträge (`FMoney`, Minor Units) | Determinismus Host/Client, keine Rundungsdrift |
| Öffentliche Klassen mit `EMPIRESIMCORE_API` | DLL-Export in modularen Editor-Builds |
| Unreal-Namenskonvention (`F`, `E`, `T`-Präfixe, PascalCase) | Einheitlicher Stil neben UE-Code |
| Single-threaded, Eigentum beim Host | Einfache, nachweisbare Atomarität |
| Mutationen nur über validierende Commit-Funktionen | Atomarität, keine Teilzustände |

## 4. Geplante logische Module und Zuordnung

| Bereich | Ort | Status |
|---|---|---|
| Core (Typen, Ids, Geld) | EmpireSimCore | teilweise implementiert |
| Economy (Ledger, Preise, Märkte) | EmpireSimCore | Ledger implementiert + getestet |
| World-Zeit (Spieluhr) | EmpireSimCore | geplant (AP-1.2) |
| Inventory, Jobs, Businesses, Production, Logistics, Personnel | EmpireSimCore | geplant |
| Persistence (Format, Versionierung, Migration) | EmpireSimCore; Dateizugriff in UE | geplant (AP-1.3) |
| Characters, Vehicles, Interaction, UI, Audio, Weltstreaming | Unreal-Module | geplant, abhängig vom UE-Projekt |
| Multiplayer (Replikation, RPC-Validierung) | Unreal-Module, Regeln im Kern | geplant |
| AI (NPC-Verhalten in der Welt) | Unreal; wirtschaftliche Entscheidungen der Konkurrenz im Kern | geplant |

Ein eigenes Unreal-Modul wird erst angelegt, wenn tatsächliche Abhängigkeiten es rechtfertigen.

## 5. Grenzen C++ ↔ Blueprints

- **C++:** alle Regeln, die Geld, Eigentum, Waren, Verträge oder Speicherstände verändern;
  Berechtigungsprüfung; Replikation; Performance-kritisches.
- **Blueprints:** Interaktionsabläufe, Animation/Effekte, UI-Bindung, Konfiguration konkreter
  Gebäude/Maschinen, Prototypen.
- Blueprints **rufen** Kernfunktionen über `BlueprintCallable`-Funktionen von Subsystemen auf;
  sie **implementieren** keine wirtschaftlichen Regeln. Damit gibt es keine doppelte Logik.

## 6. Atomarität über Systemgrenzen (Ausblick)

Das Ledger ist bereits atomar. Ein Kauf verändert aber Geld **und** Waren. Geplant ist eine
übergeordnete Welt-Transaktion (Validieren aller beteiligten Systeme → Anwenden), siehe AP-1.4.
Bis dahin darf kein Gameplay-Code Geld- und Warenänderungen getrennt ausführen.

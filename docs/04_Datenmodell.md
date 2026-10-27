# Datenmodell

Nur tatsächlich implementierte Modelle sind als *implementiert* gekennzeichnet.

## FMoney – implementiert, getestet

`Source/EmpireSimCore/Public/Empire/Core/Money.h`

| Feld | Typ | Bedeutung |
|---|---|---|
| Value | `int64` | Betrag in kleinster Währungseinheit (z. B. Rappen/Cent), vorzeichenbehaftet |

- Keine Gleitkommazahlen. Rechnen nur über `CheckedAdd`/`CheckedSub`/`CheckedNegate`
  (liefern `std::nullopt` bei Überlauf).
- Vorerst **eine** Währung pro Welt. Mehrere Währungen (Regionen) sind eine offene Entscheidung (ADR-002).

## TStrongId – implementiert

`Source/EmpireSimCore/Public/Empire/Core/Ids.h` – typisierte 64-Bit-Ids, `0` = ungültig.
Aliase: `FAccountId`, `FTransactionId`.

## FAccount – implementiert, getestet

| Feld | Typ | Bedeutung |
|---|---|---|
| Id | `FAccountId` | eindeutig innerhalb der Welt |
| DisplayName | `string` | Anzeigename |
| Kind | `EAccountKind` | `Private`, `Business`, `System` |
| Owner | `FOwnerRef` | `{World/Player/Business, Id}` – Eigentum, **nicht** Berechtigung |
| MinimumBalance | `FMoney` | Untergrenze (0 = kein Dispo, negativ = Kreditrahmen); für System-Konten ignoriert |
| Balance | `FMoney` | aktueller Saldo (nur über Commits veränderbar) |

**Systemkonten** repräsentieren die Welt außerhalb der simulierten Wirtschaft (NPC-Kunden, Lieferanten,
Staat). Sie dürfen negativ werden. Ihr Saldo zeigt genau, wie viel Geld in die Spielerwirtschaft
hinein- oder herausgeflossen ist.

## FTransactionRecord – implementiert, getestet

| Feld | Typ | Bedeutung |
|---|---|---|
| Id | `FTransactionId` | fortlaufend |
| IdempotencyKey | `string` | eindeutig je logischem Vorgang, z. B. `job:4711:payout` |
| Reason | `ETransactionReason` | JobReward, Purchase, Sale, Salary, Investment, Loan, LoanRepayment, Dividend, InternalTransfer, Fee, Adjustment |
| GameTime | `int64` | Zeitstempel der gemeinsamen Spieluhr (Spielsekunden seit Weltstart) |
| Postings | `FPosting[]` | `{Konto, Delta}`; mind. 2, jedes Konto höchstens einmal, Summe = 0 |
| Memo | `string` | Freitext für Berichte |

### Invarianten (durch Tests abgesichert)

1. Summe aller Salden = 0 (doppelte Buchführung).
2. Nicht-Systemkonten unterschreiten nie ihr `MinimumBalance` durch eine Belastung.
3. Ein Idempotenz-Schlüssel wird höchstens einmal gebucht.
4. Die Salden lassen sich jederzeit aus dem Journal rekonstruieren (`VerifyIntegrity`).
5. Interne Überweisungen ändern die umlaufende Geldmenge nicht.

## Geplante Modelle (noch nicht implementiert)

- **Ware (FGoodDefinition / FGoodStack):** Waren-Id, Kategorie, Gewicht, Volumen, optionale typisierte
  Eigenschaften (Qualität, Zustand, Haltbarkeit, Lageranforderungen), Herkunft, Besitzer, Ort. (AP-1.4)
- **Spielzeit (FGameTime):** Spielsekunden seit Weltstart, Kalender, Jahreszeit. (AP-1.2)
- **Speicherstand:** Header (Magic, Formatversion, Prüfsumme), Abschnitte je System. (AP-1.3)

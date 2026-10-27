# Schnittstellenübersicht

## FLedger (`Empire/Economy/Ledger.h`) – implementiert, getestet

| Funktion | Beschreibung |
|---|---|
| `FAccountId OpenAccount(const FAccountSpec&)` | Legt ein Konto mit Saldo 0 an |
| `const FAccount* FindAccount(FAccountId) const` | `nullptr`, wenn unbekannt |
| `FTransactionResult Commit(const FTransactionRequest&)` | Validiert vollständig, wendet dann atomar an |
| `FTransactionResult Transfer(Key, From, To, Amount, Reason, GameTime, Memo)` | Zwei-Parteien-Überweisung, `Amount > 0` |
| `const std::vector<FTransactionRecord>& GetJournal() const` | Alle gebuchten Transaktionen |
| `std::optional<FMoney> GetCirculatingMoney() const` | Summe aller Nicht-Systemkonten |
| `FLedgerIntegrityReport VerifyIntegrity() const` | Journal-Replay und Abgleich mit gespeicherten Salden |

### Ergebnis von `Commit`

| Fall | `Error` | `bDuplicate` | Zustand verändert? |
|---|---|---|---|
| Neu gebucht | `None` | false | ja |
| Gleicher Schlüssel, gleicher Inhalt (Postings + Reason) | `None` | true | nein |
| Gleicher Schlüssel, anderer Inhalt | `IdempotencyConflict` | false | nein |
| Fehlender Schlüssel | `MissingIdempotencyKey` | false | nein |
| < 2 Postings / Nullbetrag / Konto doppelt / unbekannt | `TooFewPostings` / `ZeroPosting` / `DuplicateAccount` / `UnknownAccount` | false | nein |
| Summe ≠ 0 | `Unbalanced` | false | nein |
| Untergrenze unterschritten | `InsufficientFunds` (+ `OffendingAccount`) | false | nein |
| Überlauf | `Overflow` | false | nein |
| `Transfer` mit Betrag ≤ 0 | `InvalidAmount` | false | nein |

Abgelehnte Transaktionen verbrauchen ihren Schlüssel **nicht**; ein späterer Versuch ist möglich.

### Verantwortung des Aufrufers

- **Berechtigung prüfen, bevor** `Commit` aufgerufen wird. Das Ledger prüft nur wirtschaftliche Gültigkeit.
- **Stabile Schlüssel** aus dem fachlichen Vorgang ableiten (z. B. Auftrags-Id + Schritt), nicht zufällig
  pro Aufruf erzeugen, sonst wirkt der Doppelzahlungsschutz nicht.
- Nur auf dem **Host** aufrufen. Clients senden Anfragen, der Host entscheidet.

## Geplante Schnittstellen

- Spieluhr (AP-1.2), Serialisierung (AP-1.3), Inventar und Welt-Transaktion (AP-1.4),
  UE-Subsystem `UEmpireEconomySubsystem` als Blueprint-Zugang (AP-1.6).

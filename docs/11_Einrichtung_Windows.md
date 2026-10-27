# Einrichtung auf deinem Windows-PC (Schritt für Schritt, ohne Vorkenntnisse)

Diese Anleitung ist für jemanden ohne Unreal-Erfahrung geschrieben. Du musst **keinen Code schreiben**
und **Visual Studio nie öffnen**. Am Ende doppelklickst du eine Datei und schickst mir den Bericht.

Alle verwendeten Programme sind kostenlos:

| Programm | Wofür | Kosten |
|---|---|---|
| Epic Games Launcher + Unreal Engine 5.8 | Die Spiel-Engine | kostenlos. Laut Epic-Standardlizenz fallen erst Lizenzgebühren an, wenn ein Spiel mehr als 1 Mio. USD Umsatz macht. Vor einer Veröffentlichung prüfen wir die dann gültige Lizenz. |
| Visual Studio Community | Der C++-Compiler, den Unreal braucht | kostenlos für Einzelpersonen |
| GitHub Desktop | Holt das Projekt von GitHub auf deinen PC | kostenlos |

Zeitbedarf: etwa 1–2 Stunden, überwiegend Downloads. Speicherplatz: grob 60–120 GB (du hast ca. 400 GB frei).

---

## Schritt 0 – Vorbereitung

1. Laptop **ans Netzteil** anschließen.
2. Windows-Einstellungen → System → Strom und Akku → Energiemodus: **Beste Leistung**.

## Schritt 1 – Unreal Engine installieren

1. Den **Epic Games Launcher** von der Epic-Games-Website herunterladen und installieren.
2. Mit einem (kostenlosen) Epic-Konto anmelden.
3. Links auf **Unreal Engine** klicken, oben auf **Bibliothek**.
4. Neben „Engine-Versionen“ auf das **+** klicken. Es erscheint eine Kachel. Dort die neueste **5.8.x**-Version wählen.
5. Bevor du installierst, auf **Optionen** klicken und Speicher sparen:
   - Haken **entfernen** bei „Editor-Symbole für das Debugging“ (sehr groß, nicht nötig).
   - Bei den Zielplattformen **Android, iOS und Linux abwählen**. Wir brauchen nur Windows.
6. **Installieren** klicken und warten (je nach Internet 30–90 Minuten).

## Schritt 2 – Visual Studio Community installieren

1. **Visual Studio Community** von der Microsoft-Website herunterladen. Für UE 5.8 empfiehlt Epic Version **2026**.
2. Im Installer unter „Workloads“ diese beiden Haken setzen:
   - **Spieleentwicklung mit C++** (Game development with C++)
   - **Desktopentwicklung mit C++** (Desktop development with C++)
3. **Installieren** klicken. Danach kannst du Visual Studio schließen; du musst es nie selbst öffnen.

> Falls mein Skript später einen Compiler-Fehler meldet, schicke mir den Bericht. Es kann sein, dass
> zusätzlich die Komponente „MSVC v143 – VS 2022 C++ x64/x86-Buildtools“ nötig ist. Ich sage dir dann genau, was zu tun ist.

## Schritt 3 – Projekt herunterladen

1. **GitHub Desktop** installieren und mit deinem GitHub-Konto (bleron131) anmelden.
2. *File → Clone repository…* → `bleron131/Empire-Engine` auswählen.
3. Als Speicherort einen **kurzen Pfad ohne Leerzeichen und außerhalb von OneDrive** wählen, z. B. `C:\Dev\Empire-Engine`.
   - Wichtig: **nicht** in „Dokumente“ oder „Desktop“, wenn diese mit OneDrive synchronisiert werden.
     Unreal erzeugt sehr viele temporäre Dateien, die OneDrive sonst ständig hochladen würde.
4. Oben bei **Current Branch** den Branch `claude/confident-ramanujan-0w8vsi` wählen (falls nicht schon aktiv).

## Schritt 4 – Kompilieren und testen (ein Doppelklick)

1. Im Projektordner `Tools\Windows\` öffnen.
2. **`BuildAndTest.bat` doppelklicken.**
   - Falls Windows „Der Computer wurde durch Windows geschützt“ anzeigt: *Weitere Informationen → Trotzdem ausführen*.
3. Warten. Beim ersten Mal kann das **10–40 Minuten** dauern. Ein schwarzes Fenster mit viel Text ist normal.
4. Am Ende steht `ERGEBNIS: ERFOLGREICH` oder `ERGEBNIS: FEHLGESCHLAGEN`.
5. **Schicke mir den Inhalt der Datei `Saved\EmpireReport\Report.txt`** (öffnen, alles markieren, kopieren,
   hier einfügen) – in beiden Fällen. Ein Screenshot geht auch, Text ist mir aber lieber.

Das Skript macht automatisch:
- die installierte Unreal-Version finden und im Projekt eintragen,
- das Projekt kompilieren,
- die automatisierten Tests in Unreal ausführen,
- einen Bericht schreiben.

## Schritt 5 – Editor öffnen (erst nach erfolgreichem Schritt 4)

1. **`EmpireEngine.uproject` doppelklicken.** Der Unreal Editor startet (beim ersten Mal dauert das einige Minuten).
2. Du siehst ein leeres Level. Das ist im Moment richtig – es gibt noch keine Spielwelt.
3. Für flüssigeres Arbeiten auf deinem Laptop: oben rechts **Einstellungen → Engine-Skalierbarkeit → Mittel**.

Wenn sich nach einem Doppelklick ein Fenster „Missing Modules … Would you like to rebuild them now?“ öffnet:
auf **Nein** klicken und stattdessen erneut `BuildAndTest.bat` ausführen. So bekomme ich einen Bericht,
falls etwas schiefgeht.

---

## Häufige Probleme

| Problem | Lösung |
|---|---|
| „Keine Unreal Engine 5 gefunden“ | Schritt 1 abschließen. Liegt die Engine an einem anderen Ort, schicke mir den Pfad. |
| Fehler mit „Visual Studio“, „MSVC“ oder „toolchain“ im Bericht | Bericht schicken. Wahrscheinlich fehlt eine Visual-Studio-Komponente. |
| Sehr langsam, Lüfter laut | Normal beim ersten Kompilieren und beim Shader-Kompilieren. Andere Programme schließen (16 GB RAM sind knapp). |
| Das Fenster schließt sich sofort | `Saved\EmpireReport\Report.txt` trotzdem öffnen und schicken. Falls nicht vorhanden: Screenshot des Fensters. |

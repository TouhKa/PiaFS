# FS
*Verwendete IDE: Visual Studio Code*

### Vergleiche:  https://github.com/DerNiccoo/MyFS 
**Beachte: Wir verwenden keine DMAP, da die freien Blöcke aus der FAT ausgelesen werden!**

## Bash scripte
Es gibt drei bash scripte im master tree: run, runMount und runUnmount:
**Da der darin enthaltene Pfad absolut ist, bitte ändert diesen auf euren Nutzer ab.**
 1. run -> befüllt die container.bin 
 2. runMount -> Mountet das FS. 
  **Beachte der Ordner in dem nachher das FS liegt muss schon existent sein!!!**
 3. runUnmount -> Entfernt das FS
 
## fuse tests
Die im Ordner "Fusetests" vorfindbaren JS Datei testet das Mounten, hinzufügen, ändern und löschen einer Datei in unserem FS 
und vergleicht selbeige Änderungen an einer Datei im "normalen FS". 
**Es wird nodejs und npm benötigt, sowie außerdem die in der Datei genutzten Module**, zum Zugriff auf das FS. 

Um die Module zu installieren reicht ``npm install modulename -g ``

Die Fusetests können mit dem Befehlt ``mocha fusetests.js`` gestartet werden.

## unit tests 
Testen jeweils die Logik der Strukturen des FS. 
Die Dokumentation des Catch2 Test Frameworks findet sich [hier](https://github.com/catchorg/Catch2). 

## Issues
Änderungen von Files über den Text Editor gehen bei mir nicht, da ein Fehler von Ubuntu **.goutputstream-xxxx Dateien** erzeugt,
anstelle die Änderungen in die bestehende Datei zu übernehmen. 
Wenn dieser Fehler bei jemandem auftritt -> **File über command line mit nano ändern.** Das funktioniert!

# TODO

- [x] Fehler Codes bei den FUSE Operationen zurückgeben

- [ ] In diesem FS fehlen drei von Waldhorst geforderte FUSE-Implementierungen: fuseOpenDir, fuseReleaseDir und fuseRelease.
- [x] Die ersten beiden werden nicht wirlich benötigt, daher sollte es passen wenn wir da nur ein Log-Eintrag machen, dass die Methode aufgerufen wird
  Zur dritten Methode **fuseRelease**: es funktioniert auch ohne, also ...wenn wir erklären können wo wir quasi eine Datei anderswo schließen, sollte das passen.  
  
 - [nicht mehr gefixt - nur auskommentiert]die fuse test (fusetests.js) haben einen auskommentierten Bereich. Ich weiß nicht warum es mir in zwei identischen Dateien unterschiedliche Dinge schreibt, wenn ich ein Offset definiere. Einmal im FS steht es Richtig an Stelle X in der Datei, die Originaldatei die zum Abgleich ebenfalls geändert wird, wird komplett überschrieben...vllt hat da noch jmd eine Idee!^^"
 
 Bsp: Am Offset 512 soll "test" geschrieben werden. Datei 1 und 2 haben den selben Inhalt "abcd". 
  Datei 1: abcd\u0000....\u0000test
  Datei 2: test
  
- [ ] Doku
 

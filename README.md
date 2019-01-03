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

- Fehler Codes bei den FUSE Operationen zurückgeben- **nicht immer gemacht bisher**.
  Einfach drüber schauen, ob es Sinn macht welche zurückzugeben. 
  Die entsprechenden Fehlercodes pro Operation finden sich in den Folien von Herrn Waldhorst.
- In diesem FS fehlen drei von Waldhorst geforderte FUSE-Implementierungen: fuseOpenDir, fuseReleaseDir und fuseRelease.
  Die ersten beiden werden nicht wirlich benötigt, daher sollte es passen wenn wir da nur ein Log-Eintrag machen, dass die Methode aufgerufen wird.
  Zur dritten Methode **fuseRelease**: es funktioniert auch ohne, also ...wenn wir erklären können wo wir quasi eine Datei anderswo schließen, sollte das passen.  
- Doku
 

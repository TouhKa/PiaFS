# FS
*Verwendete IDE: Visual Studio Code*

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



 

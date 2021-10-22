# cppfinalproject
Bei dem im folgenden beschriebenen Programm handelt es sich um das Abschlussprojekt
zum Master-Praktikum „Systems Programming in C++“, das ich im letzten Sommer absolviert
habe. Das Praktikum war keine Teamarbeit, sämtliche Aufgaben wurden individuell
bearbeitet. Der Code ist komplett von mir geschrieben.
Das Programm ist ein Compiler und eine Laufzeitumgebung einer stark simplifizierten
Programmiersprache. Mit dieser Programmiersprache kann man einzelne Funktionen
definieren, die simple Berechnungen auf Integer Werten durchführt und am Ende einen
Integer Wert zurückgibt. Die Syntax der Programmiersprache ist auf den Seiten 5 bis 7 in
final.pdf dargestellt. Innerhalb der main Methode in Pljit/main.cpp kann man Funktionen in
Form von Strings in dieser Sprache formulieren. Diese kann man dann über das pljit Objekt
registrieren. Wenn alles korrekt ist (keine Syntaxfehler, ...) kann man die Funktion
anschließend laufen lassen.
<h3>Beispiel (aus main.cpp)</h3>
In Zeile 1 werden zwei Parameter definiert, diese müssen bei jedem späteren
Lauf der Funktion übergeben werden. In Zeile 2 werden zwei Variablen
definiert, die im weiteren Verlauf verwendet werden, ebenso eine Konstante in
Zeile 3. Die Definitionen von Parametern, Variablen und Konstanten sind
jeweils optional und können auch weggelassen werden, falls man z.B. keine
Parameter braucht. Anschließend folgt die Berechnung, bestehend aus
Zuweisungen und arithmetischen Ausdrücken. In Zeile 7 wird das Ergebnis der
Berechnung zurückgegeben (dies ist der Wert, der weiter unten beim Aufruf der
Funktion in der Variablen res gespeichert wird.  
<pre><code>
string program = "PARAM a, b;\n"
"VAR c, d;\n"
"CONST e = 220;\n"
"BEGIN\n"
"c := 2 * a + e;\n"
"d := c + b;\n"
"RETURN c * d + a\n"
"END.";

Pljit jit{};

// program registrieren, Rückgabewert ist ein handle über welches das Programm
// gestartet werden kann
auto h = jit.registerFunction(program);

// program mit den Parametern 5 für a und 7 für b ausgeführt. Das Integer 
// Ergebnis wird in einer optional\<int\> Variable gekapselt zurückgegeben.
auto res = h({5,7});
</code></pre>

Der Compiler erkennt sowohl Syntaxfehler (z.B. ein fehlendes ;) als auch
semantische Fehler (z.B. die Verwendung einer nicht initialisierten Variablen
oder Zuweisung eines Wertes an eine Konstante) und zeigt die Stelle im Code, an
der der Fehler auftritt und um was für einen Fehler es sich handelt (durch
Einfügen eines Fehlers im Code der program Variablen kann man dies
ausprobieren). Außerdem werden Optimierungen im Code vorgenommen.
Zusätzlich kann man sich zu jedem registrierten Programm den dazugehörigen
Parse-Tree und den Abstract-Syntax-Tree graphisch darstellen lassen.
```cpp
jit.printParseTree(h, "PATH_TO_FIILE");
```
bzw.
```cpp
jit.printAST(h, "PATH_TO_FIILE");
```

PATH_TO_FILE ersetzt man durch den gewünschen Namen der Datei. Die Bäume werden
im Dot-Format gespeichert und können unter Linux mit dem Programm xdot geöffnet
werden. Die beiden Bäume des Beispielprogramms sind im Wurzelverzeichnis
exemplarisch beigefügt (ast.dot und parsetree.dot).
Der dargestellte Code befindet sich in der Datei Pljit/main.cpp. Eine
ausführliche Beschreibung befindet sich in der offiziellen Projektbeschreibung
final.pdf des Praktikums, die sich im Wurzelverzeichnis befindet.

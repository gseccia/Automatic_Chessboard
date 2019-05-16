# Automatic Chessboard
Complete description of hardware and software implementation of an automatic chess-board made during the "Embedded Systems" university course.

All the project will be translated at the end of the development... STAY TUNED!

## Overall description
Il progetto proposto consiste in una scacchiera intelligente in grado di giocare in modo autonomo contro l’utente, fornendo un’esperienza di gioco realistica. 
I problemi principali da affrontare sono:
•	Rilevamento posizione dei pezzi sulla scacchiera
•	Installazione del software per la meccanica di gioco
•	Spostamento automatico dei pezzi
Altre caratteristiche sono:
•	Gestione situazioni di errore
•	Interfaccia utente
Rilevamento posizione dei pezzi sulla scacchiera
La soluzione proposta consiste nella costruzione di una griglia costituita da switch magnetici posti in corrispondenza di ogni casella. 
I pezzi degli scacchi saranno dotati di una calamita sul fondo. 
Spostamento automatico dei pezzi
La scacchiera è dotata di 2 guide a scorrimento, ortogonali tra loro, per lo spostamento del gancio magnetico.
Lo spostamento lungo le guide sarà attuato da 2 motori passo passo, mentre l’attivazione e la disattivazione del gangio sarà gestita da un servomotore.
Installazione del software per la meccanica di gioco
Si utilizzerà o una rete pre-addestrata che consenta il chess-engine o, alternativamente, si utilizzerà un approccio classico per definire la dinamica di gioco.
Gestione situazioni di errore
Le situazioni di errore che verranno gestite sono:
•	Mossa non consentita da parte dell’utente.
o	Ripristino della situazione precedente.
•	Spostamento contemporaneo di più pezzi, ad eccezione della mossa ‘arrocco’.
o	Annullamento del gioco
•	Pezzi mancanti non previsti sulla griglia.
o	Attesa reinserimento contando sull’onestà dell’utente.
Interfaccia utente
L’utente può giocare muovendo direttamente i pezzi.
La scacchiera sarà dotata di un display per comunicare informazioni all’utente e di un joystick analogico, dotato anche di bottone digitale, per la selezione delle configurazioni di gioco e per il passaggio di turno.


## Materials (WAIT UPDATE)
https://www.amazon.it/Yizhet-Magneti-neodimio-Potente-calamita/dp/B078W1VY1T/ref=asc_df_B07PHWC6MV/?tag=googshopit-21&linkCode=df0&hvadid=330921120173&hvpos=1o5&hvnetw=g&hvrand=13864160234445311620&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=20599&hvtargid=pla-667939196811&th=1

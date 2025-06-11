# LM35DZ-ET-GENERATEUR-DE-COURANT-DFTA217
Déportation filaire capteur de température LM35DZ

Ici vous avez un petit comparatif sur l'affichage de la température fournie par 2 LM35DZ affichée sur des écrans oleds différents.

Autre différence le premier capteur est câblé tout à fait normalement en tension tandis que le 2ème est monté en générateur de courant et ce courant qui peut transiter dans un câble de longueur certaine est converti en tension
par une résistance en entrée du microcontroleur qui est dans notre exemple un Seeeduino XIAO.
Le code prévoit pour chacun des capteurs et de manière différenciée une correction de zéro et une correction d'échelle.

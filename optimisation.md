This file is use to track the performance impact of code change:
1. Only do "glGetUniformLocation" at initialisation and not for every frame.
2. Passage de la taille du volume, l'inverse de la resolution, diminuer les utilisation de length dans le shader
Gain d'environs 5 frames
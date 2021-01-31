# Projet OpenGL
![](./Ressources/OpenGLGIF.gif)

Travail réalisé pour le cours de Temps Réel

# Sources

http://www.opengl-tutorial.org/



# Elements

## Le triangle

- Vertices définis à la maindans un tableau de GLFloat
- Couleurs RGB aleatoires pour chaque vertex et Alpha initialisé à 0.3f
- Tentative de calcul de normales (pas exploité)
- Translation constante en fonction du temps

## Le cube

- Vertices définis à la main dans un tableau de GLFloat
- Couleurs aléatoire pour chaque triangle
- Uvs définis à la main dans un tableau de GLFloat
- Tentative de calcul de normales (pas exploité)
- Application d'une texture

## Le singe

- Utilisation loader de .obj créer au préalable avec Blender
- Le loader retourne deux vector de glm::vec3 (vertices, normales) et un vector de glm::vec2(uvs)
- Les couleurs sont également stockées dans un vector de glm::vec3 mais sont générés aléatoirement pour le mesh.
- Les tangentes et le bitangentes sont calculées avec les vector créées avec le loader.
- Pour un soucis de performances un indexer VBO est utilisé, cela permet de réduires le nombre d' element en vérifiant les doublons (2904 -> 590)
- Deux texture sont appliquées :
  - Une texture 2D (la meme pour le cube)
  - Une normal map qui permet d'interférer lors du calcul de la lumiere et donner une impression de relief au mesh.


# La lumiere

*light.h*

- 3 données membre :
 - Position
 - Couleur
 - Puissance

## Calcul de la couleur

Couleur = Composante Ambiante + Composante Diffuse + Composante Spéculaire

> Voir détails dans le vertex shader et le fragment shader

> La source de lumiere tourne autour du centre de la scene 

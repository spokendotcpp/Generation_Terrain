# Génération de maillages
## __Sujet 5__ - Génération de terrain en 3D
  
Thibault HECKEL
Valentin MONTINI
Julien MARCELLESI

### Compilation :
```
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Attention !
Si vous compilez avec QtCreator, veuillez faire attention à bien
mettre le dossier build/ à la racine du projet (à côté de src/).

L'exécutable ira chercher les shaders dans le dossier "../shaders/".

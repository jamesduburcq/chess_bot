# Chess Bot

Un jeu d'échecs complet développé en C avec interface graphique (SDL2), moteur de jeu (IA) et persistance des données (SQLite).

---

## 📋 Fonctionnalités

- **Moteur d'échecs & IA** : Algorithmes d'évaluation et de calcul des déplacements (`engine.c`, `deplacement.c`, `board.c`).
- **Interface Graphique (GUI)** : Rendu visuel de l'échiquier et interaction à la souris via **SDL2** et **SDL2_image** (`gui.c`).
- **Base de données** : Sauvegarde et gestion des données via **SQLite** (`database.c`, `setup_db.c`, `sqlite3.c`).

---

## 🛠️ Prérequis

Le projet est configuré pour être compilé sous Windows avec **MinGW-w64** (via [MSYS2](https://www.msys2.org/)).

Assurez-vous d'avoir installé les paquets nécessaires (GCC, SDL2, SDL2_image) :

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image
```

---

## 🚀 Compilation et Exécution

### 1. Initialiser la base de données

Avant de lancer le jeu, compilez et exécutez le script d'initialisation de la base SQLite :

```bash
gcc setup_db.c sqlite3.c -o setup_db.exe
./setup_db.exe
```

### 2. Compiler le jeu graphique

Compilez l'ensemble des modules du jeu avec les optimisations et les bibliothèques requises :

```bash
gcc board.c deplacement.c engine.c database.c gui.c sqlite3.c \
    -o jeu_graphique \
    -O3 \
    -w \
    -I/mingw64/include/SDL2 \
    -L/mingw64/lib \
    -D_REENTRANT \
    -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread -lm
```

> **Détail des options :**
> - `-O3` : Optimisation maximale du code (essentiel pour la vitesse de calcul du bot).
> - `-w` : Désactive les avertissements du compilateur.
> - `-I/mingw64/include/SDL2` et `-L/mingw64/lib` : Chemins vers les en-têtes et bibliothèques SDL2.
> - `-lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lpthread -lm` : Dépendances graphiques, multithreading et mathématiques.

### 3. Lancer le jeu

Une fois la compilation terminée avec succès, exécutez le programme :

```bash
./jeu_graphique.exe
```

---

## 📁 Structure du projet

```text
├── board.c          # Représentation et gestion du plateau d'échecs
├── deplacement.c    # Règles de déplacement des pièces et validation des coups
├── engine.c         # Moteur de réflexion / IA du bot
├── gui.c            # Rendu visuel et gestion des événements (SDL2)
├── database.c       # Requêtes et interactions avec la base SQLite
├── setup_db.c       # Script d'initialisation de la base de données
├── sqlite3.c        # Moteur SQLite (amalgamation)
└── sqlite3.h        # En-tête SQLite
```

---

## 👤 Auteur

- GitHub : [@jamesduburcq](https://github.com/jamesduburcq)

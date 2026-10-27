# Chess Bot

Moteur de jeu d'échecs codé en C avec interface en ligne de commande interactive et persistance des données via SQLite.

## Description

Ce projet implémente un jeu d'échecs complet exécutable directement dans le terminal. Il prend en charge l'initialisation de l'échiquier, la vérification des règles et la validation des déplacements de chaque pièce. Il intègre également un shell interactif pour piloter la partie et une base SQLite embarquée permettant d'enregistrer l'historique des coups joués.

## Fonctionnalités

* **Plateau de jeu interactif** : modélisation de la grille 8x8 et affichage matriciel en console avec actualisation après chaque action.
* **Moteur de règles** :
  * Validation des trajectoires propres à chaque pièce (pions, cavaliers, fous, tours, dame, roi).
  * Détection des obstacles, des prises et des coups illégaux.
  * Gestion du tour par tour entre les Blancs et les Noirs.
* **Interface console (CLI)** :
  * Boucle de jeu interactive avec invites de commande claires.
  * Analyse syntaxique des coordonnées saisies par les joueurs.
* **Persistance des données avec SQLite** :
  * Initialisation automatique des tables via `setup_db1.c`.
  * Enregistrement systématique des coups et des données de session en base locale.

## Architecture du code

* `board.c` / `board.h` : structures de données représentant l'échiquier, placement initial des pièces et fonctions d'affichage.
* `deplacement.c` / `deplacement.h` : algorithmes de calcul de légalité des déplacements et contraintes géométriques du jeu.
* `shell.c` : point d'entrée du programme (`main`), gestion des entrées utilisateur et boucle principale de la partie.
* `setup_db1.c` : fonctions d'interfaçage avec SQLite (création de la base, requêtes SQL d'insertion et de consultation).
* `sqlite3.c` / `sqlite3.h` / `sqlite3ext.h` : code source complet du moteur SQLite3 inclus directement au projet pour éviter toute dépendance externe.

## Prérequis

* Un compilateur C compatible C99 ou supérieur (`gcc` ou `clang`).
* L'outil Git pour cloner le dépôt.

## Compilation et exécution

### 1. Récupération du dépôt

```bash
git clone [https://github.com/jamesduburcq/chess_bot.git](https://github.com/jamesduburcq/chess_bot.git)
cd chess_bot
```

### 2. Compilation

Sous **Windows** (PowerShell ou Invite de commandes avec GCC / MinGW) :

```bash
gcc -Wall -Wextra shell.c board.c deplacement.c setup_db1.c sqlite3.c -o chess_bot.exe
```

Sous **Linux** ou **macOS** :

```bash
gcc -Wall -Wextra shell.c board.c deplacement.c setup_db1.c sqlite3.c -o chess_bot -lpthread -ldl
```

### 3. Lancement du jeu

Sous Windows :

```powershell
.\chess_bot.exe
```

Sous Linux ou macOS :

```bash
./chess_bot
```

## Déroulement d'une partie

1. Le programme démarre en générant la base de données locale si elle n'existe pas encore.
2. L'échiquier s'affiche avec le repère de colonnes (lettres `a` à `h`) et de rangées (chiffres `1` à `8`).
3. Les joueurs cliquent sur les pièces pour les déplacer.
4. Le moteur valide la conformité du coup, met à jour le plateau et stocke le mouvement dans la base de données.

## Équipe

Projet développé en collaboration par :

* **James Duburcq** ([@jamesduburcq](https://github.com/jamesduburcq))
* Collaborateurs du projet

## Licence

Ce projet est distribué sous licence open source libre. Le moteur embarqué SQLite relève quant à lui du domaine public.

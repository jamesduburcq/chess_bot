# Chess Bot ♟️

> Un moteur de jeu d'échecs développé en langage C, doté d'une interface en ligne de commande interactive et d'un système de persistance des données propulsé par SQLite.

<!-- Si tu as une capture d'écran de ton terminal en jeu, tu peux l'ajouter ici -->
<!-- ![Aperçu du jeu d'échecs](chemin/vers/image.png) -->

## 🚀 Fonctionnalités
- **Gestion complète de l'échiquier** : Représentation et affichage de l'état des pièces (`board.c`/`board.h`).
- **Moteur de règles et déplacements** : Validation des coups légaux et gestion de la logique du jeu d'échecs (`deplacement.c`/`deplacement.h`).
- **Interface en ligne de commande (CLI)** : Un shell interactif fluide pour saisir les commandes et jouer la partie (`shell.c`).
- **Base de données intégrée** : Utilisation de SQLite pour sauvegarder et gérer les données du jeu (historique, configuration ou parties) (`setup_db1.c` et `sqlite3`).

## 🛠️ Technologies
- **Langage** : C (C99 ou supérieur)
- **Base de données** : SQLite3 (embarquée)
- **Outils** : GCC / Make

## ⚙️ Compilation et Lancement

### Prérequis
Assure-toi d'avoir un compilateur C installé sur ta machine (comme `gcc` via MinGW sur Windows, ou par défaut sur Linux/macOS).

### Étapes d'installation et de compilation

1. **Cloner le dépôt :**
   ```bash
   git clone [https://github.com/jamesduburcq/chess_bot.git](https://github.com/jamesduburcq/chess_bot.git)
   cd chess_bot

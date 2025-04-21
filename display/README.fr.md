# Git Graph Visualizer

Une application en C pour visualiser l'historique Git sous forme de graphique.

## Prérequis

- GCC (compilateur C)
- libgit2 (bibliothèque pour manipuler les dépôts Git)
- ncurses (bibliothèque pour créer des interfaces en mode texte)
- make

## À propos de ncurses

ncurses (new curses) est une bibliothèque qui permet de créer des interfaces utilisateur en mode texte. Elle offre :
- La gestion du terminal et de son affichage
- La capture des entrées clavier (touches spéciales comme les flèches)
- La possibilité de créer des menus interactifs
- Le support des couleurs et des attributs de texte (gras, souligné, etc.)

Dans notre application, ncurses est utilisée pour créer une interface interactive permettant de naviguer facilement entre les différentes fonctionnalités.

## Installation

1. Installer les dépendances :
   ```bash
   # Sur macOS avec Homebrew
   brew install libgit2 ncurses
   
   # Sur Ubuntu/Debian
   sudo apt-get install libgit2-dev libncurses-dev
   ```

2. Compiler le projet :
   ```bash
   make
   ```

## Utilisation

```bash
./git-graph /chemin/vers/votre/repo
```

## Navigation

- Flèches haut/bas : Naviguer dans le menu
- Entrée : Sélectionner une option
- 'q' ou Échap : Quitter l'application

## Fonctionnalités

- Lecture de l'historique Git
- Génération d'une visualisation graphique des commits
- Affichage des branches et des merges 
# Git Graph Visualizer

Une application en C pour visualiser l'historique Git sous forme de graphique.

## Prérequis

- GCC (compilateur C)
- libgit2 (bibliothèque pour manipuler les dépôts Git)
- make

## Installation

1. Installer les dépendances :
   ```bash
   # Sur macOS avec Homebrew
   brew install libgit2
   
   # Sur Ubuntu/Debian
   sudo apt-get install libgit2-dev
   ```

2. Compiler le projet :
   ```bash
   make
   ```

## Utilisation

```bash
./git-graph /chemin/vers/votre/repo
```

## Fonctionnalités

- Lecture de l'historique Git
- Génération d'une visualisation graphique des commits
- Affichage des branches et des merges 
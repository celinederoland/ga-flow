# Git Graph Visualizer

Une application en C pour visualiser l'historique Git sous forme de graphique.

## Prérequis

- GCC (compilateur C)
- libgit2 (bibliothèque pour manipuler les dépôts Git)
- make
- CMake 3.31 ou supérieur

## Installation

1. Installer les dépendances :
   ```bash
   # Sur macOS avec Homebrew
   brew install libgit2
   brew install cmake
   brew upgrade cmake
   # dans .zshrc : alias cmake="/usr/local/bin/cmake"

   # Sur Ubuntu/Debian
   sudo apt-get install libgit2-dev
   sudo apt-get update cmake
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
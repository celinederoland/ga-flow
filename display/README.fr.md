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

# Organisation du code

# GitSprintFlow

GitSprintFlow est une application en C qui permet de visualiser l'historique Git d'un projet sous forme de graphe, avec un accent particulier sur la gestion des sprints.

## Organisation du code

Le projet est organisé en plusieurs modules :

### Modèle de données (`model.h`, `model.c`)
- Définit les structures de données principales :
  - `Graph` : structure principale contenant les branches et commits
  - `Branch` : représente une branche Git avec son type (main, story, hotfix, sprint)
  - `Commit` : représente un commit avec ses relations aux branches
- Gère la création et la libération du modèle

### Traversée Git (`git_traverser.h`, `git_traverser.c`)
- Interface avec la bibliothèque libgit2
- Fonctions pour :
  - Analyser les branches (`analyze_branches`)
  - Récupérer les commits (`get_branch_commits`, `get_branches_commits`)
  - Journalisation des détails (`log_branches_details`, `log_commits_details`)

### Affichage et débogage (`model_debug.h`, `model_debug.c`)
- Fonctions d'affichage du graphe
- Outils de débogage pour visualiser l'état du modèle

### Gestion des branches (`list_branches.h`, `list_branches.c`)
- Fonctions spécifiques pour lister et manipuler les branches

### Programme principal (`main.c`)
- Gestion des arguments en ligne de commande
- Initialisation de libgit2
- Orchestration des différentes fonctionnalités

### Types et configurations (`types.h`, `CMakeLists.txt`)
- Définition des types et enums
- Configuration de la compilation avec CMake

Le projet utilise une architecture modulaire où chaque composant a une responsabilité claire, avec une séparation nette entre la manipulation des données Git, la modélisation et l'affichage.
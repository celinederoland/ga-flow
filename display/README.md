# Git Graph Visualizer

A C application to visualize Git history as a graph.

## Prerequisites

- GCC (C compiler)
- libgit2 (library for Git repository manipulation)
- make
- CMake 3.31 or higher

## Installation

1. Install dependencies:
   ```bash
   # On macOS with Homebrew
   brew install libgit2
   brew install cmake
   brew upgrade cmake
   # in .zshrc: alias cmake="/usr/local/bin/cmake"

   # On Ubuntu/Debian
   sudo apt-get install libgit2-dev
   sudo apt-get update cmake
   ```

2. Compile the project:
   ```bash
   make
   ```

## Usage

```bash
./git-graph /path/to/your/repo
```

## Features

- Git history reading
- Graphical visualization of commits
- Display of branches and merges

## Code Organization

The project is organized into several modules:

### Data Model (`model.h`, `model.c`)
- Defines main data structures:
  - `Graph`: main structure containing branches and commits
  - `Branch`: represents a Git branch with its type (main, story, hotfix, sprint)
  - `Commit`: represents a commit with its branch relationships
- Manages model creation and cleanup

### Git Traversal (`git_traverser.h`, `git_traverser.c`)
- Interface with libgit2 library
- Functions for:
  - Branch analysis (`analyze_branches`)
  - Commit retrieval (`get_branch_commits`, `get_branches_commits`)
  - Detail logging (`log_branches_details`, `log_commits_details`)

### Display and Debugging (`model_debug.h`, `model_debug.c`)
- Graph display functions
- Debugging tools to visualize model state

### Branch Management (`list_branches.h`, `list_branches.c`)
- Specific functions for listing and manipulating branches
- Special support for sprint branches

### Main Program (`main.c`)
- Command line argument handling
- libgit2 initialization
- Orchestration of different functionalities

### Types and Configuration (`types.h`, `CMakeLists.txt`)
- Type and enum definitions
- CMake compilation configuration

The project uses a modular architecture where each component has a clear responsibility, with a clear separation between Git data manipulation, modeling, and display.

---

*This document has been automatically translated from French by a generative AI. The original version can be found in README.fr.md* 
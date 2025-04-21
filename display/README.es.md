# Visualizador de Gráficos Git

Una aplicación en C para visualizar el historial de Git en forma de gráfico.

## Requisitos Previos

- GCC (compilador C)
- libgit2 (biblioteca para manipular repositorios Git)
- make
- CMake 3.31 o superior

## Instalación

1. Instalar dependencias:
   ```bash
   # En macOS con Homebrew
   brew install libgit2
   brew install cmake
   brew upgrade cmake
   # en .zshrc: alias cmake="/usr/local/bin/cmake"

   # En Ubuntu/Debian
   sudo apt-get install libgit2-dev
   sudo apt-get update cmake
   ```

2. Compilar el proyecto:
   ```bash
   make
   ```

## Uso

```bash
./git-graph /ruta/a/tu/repositorio
```

## Características

- Lectura del historial de Git
- Visualización gráfica de commits
- Visualización de ramas y fusiones

## Organización del Código

El proyecto está organizado en varios módulos:

### Modelo de Datos (`model.h`, `model.c`)
- Define las estructuras de datos principales:
  - `Graph`: estructura principal que contiene ramas y commits
  - `Branch`: representa una rama Git con su tipo (main, story, hotfix, sprint)
  - `Commit`: representa un commit con sus relaciones con las ramas
- Gestiona la creación y liberación del modelo

### Recorrido Git (`git_traverser.h`, `git_traverser.c`)
- Interfaz con la biblioteca libgit2
- Funciones para:
  - Análisis de ramas (`analyze_branches`)
  - Recuperación de commits (`get_branch_commits`, `get_branches_commits`)
  - Registro de detalles (`log_branches_details`, `log_commits_details`)

### Visualización y Depuración (`model_debug.h`, `model_debug.c`)
- Funciones de visualización del gráfico
- Herramientas de depuración para visualizar el estado del modelo

### Gestión de Ramas (`list_branches.h`, `list_branches.c`)
- Funciones específicas para listar y manipular ramas
- Soporte especial para ramas de sprint

### Programa Principal (`main.c`)
- Gestión de argumentos de línea de comandos
- Inicialización de libgit2
- Orquestación de las diferentes funcionalidades

### Tipos y Configuración (`types.h`, `CMakeLists.txt`)
- Definición de tipos y enums
- Configuración de compilación con CMake

El proyecto utiliza una arquitectura modular donde cada componente tiene una responsabilidad clara, con una separación nítida entre la manipulación de datos Git, el modelado y la visualización.

---

*Este documento ha sido traducido automáticamente del francés por una IA generativa. La versión original se encuentra en README.fr.md* 
cmake_minimum_required(VERSION 3.31)
project(display C)

set(CMAKE_C_STANDARD 17)

# Configurer libgit2
find_library(GIT2_LIBRARY NAMES git2)
find_path(GIT2_INCLUDE_DIR NAMES git2.h)

if(NOT GIT2_LIBRARY OR NOT GIT2_INCLUDE_DIR)
    message(FATAL_ERROR "libgit2 not found. Please install it using 'brew install libgit2'")
endif()

# Ajouter les chemins d'inclusion
include_directories(${GIT2_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Créer l'exécutable
add_executable(display main.c)

# Lier les bibliothèques
target_link_libraries(display ${GIT2_LIBRARY})

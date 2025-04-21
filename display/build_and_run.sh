#!/bin/bash

# Couleurs pour les messages
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Fonction pour afficher les messages
print_message() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Vérifier si le répertoire build existe, sinon le créer
if [ ! -d "build" ]; then
    print_message "Création du répertoire build..."
    mkdir build
fi

# Se déplacer dans le répertoire build
cd build

# Configurer CMake
print_message "Configuration de CMake..."
cmake .. || { print_error "Erreur lors de la configuration CMake"; exit 1; }

# Compiler le projet
print_message "Compilation du projet..."
make || { print_error "Erreur lors de la compilation"; exit 1; }

# Exécuter le programme
print_message "Exécution du programme..."
./display ../..

# Revenir au répertoire parent
cd .. 
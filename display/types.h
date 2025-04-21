#ifndef TYPES_H
#define TYPES_H

// Définition des modes d'opération
typedef enum {
    MODE_HELP,      // Afficher l'aide
    MODE_HISTORY,   // Afficher l'historique
    MODE_BRANCHES,  // Afficher les branches
    MODE_GRAPH,     // Générer le graph
    MODE_LIST_SPRINTS
} RunMode;

// Structure pour stocker les options
typedef struct {
    const char *repo_path;  // Chemin du repository
    int verbosity;          // Niveau de verbosité (0-3)
    RunMode mode;         // Mode d'opération
} RunArguments;

#endif // TYPES_H 
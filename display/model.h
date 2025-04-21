#ifndef MODEL_H
#define MODEL_H

#include <git2.h>

// Déclarations anticipées
typedef struct Branch Branch;
typedef struct Commit Commit;
typedef struct Graph Graph;

typedef enum {
    BRANCH_MAIN,
    BRANCH_STORY,
    BRANCH_HOTFIX,
    BRANCH_SPRINT,
    BRANCH_UNKNOWN
} BranchType;

// Structure pour un commit dans notre modèle
struct Commit {
    git_commit *git_commit;  // Pointeur vers le commit Git
    Branch **branches;       // Liste des branches contenant ce commit
    size_t branch_count;     // Nombre de branches contenant ce commit
};

// Structure pour une branche dans notre modèle
struct Branch {
    git_reference *git_ref;  // Pointeur vers la référence Git
    Commit **commits;        // Liste des commits de cette branche
    size_t commit_count;     // Nombre de commits dans cette branche
    BranchType type;
};

// Structure principale du graph
struct Graph {
    Branch **branches;       // Liste des branches
    size_t branch_count;     // Nombre de branches
    Commit **commits;        // Liste des commits
    size_t commit_count;     // Nombre de commits
};

int fillModel(git_repository * repo, Graph * graph);

void freeGraph(const Graph *graph);

#endif // MODEL_H

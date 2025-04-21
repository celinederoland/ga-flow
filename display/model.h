#ifndef MODEL_H
#define MODEL_H

#include <git2.h>

// Types de branches
typedef enum {
    BRANCH_MAIN,    // Branche principale
    BRANCH_SPRINT,  // Branche de sprint
    BRANCH_STORY,   // Branche de story
    BRANCH_HOTFIX   // Branche de hotfix
} BranchType;

// Types de commits
typedef enum {
    COMMIT_WORK,    // Commit de travail normal
    COMMIT_MERGE    // Commit de merge
} CommitType;

// Déclaration anticipée des structures
typedef struct Commit Commit;
typedef struct Branch Branch;
typedef struct Merge Merge;

// Structure pour un commit
struct Commit {
    git_oid oid;            // Identifiant du commit
    char *message;          // Message du commit
    char *author;           // Auteur du commit
    time_t time;            // Date du commit
    CommitType type;        // Type de commit
    Commit *parent;         // Commit parent (pour les commits de travail)
    Commit *merged;         // Commit fusionné (pour les commits de merge)
};

// Structure pour une branche
struct Branch {
    char *name;             // Nom de la branche
    BranchType type;        // Type de branche
    Commit *head;           // Commit le plus récent
    Branch *next;           // Branche suivante dans la liste
};

// Structure pour une flèche de merge
struct Merge {
    Commit *from;           // Commit source (commit de travail)
    Commit *to;             // Commit destination (commit de merge)
    Branch *target_branch;  // Branche cible du merge
    Merge *next;            // Merge suivant dans la liste
};

// Structure principale du modèle
typedef struct {
    Branch *branches;       // Liste des branches
    Merge *merges;          // Liste des merges
    git_repository *repo;   // Repository Git
} GraphModel;

// Fonctions pour créer et manipuler le modèle
GraphModel* create_model(git_repository *repo);
void free_model(GraphModel *model);

// Fonctions pour ajouter des éléments au modèle
Branch* add_branch(GraphModel *model, const char *name, BranchType type);
Commit* add_commit(GraphModel *model, Branch *branch, const git_oid *oid, CommitType type);
Merge* add_merge(GraphModel *model, Commit *from, Commit *to, Branch *target_branch);

// Fonctions pour analyser le repository
int analyze_repository(GraphModel *model);

#endif // MODEL_H 
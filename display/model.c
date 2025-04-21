#include "model.h"
#include <stdlib.h>
#include <string.h>
#include <git2.h>

GraphModel* create_model(git_repository *repo) {
    GraphModel *model = malloc(sizeof(GraphModel));
    if (!model) return NULL;

    model->branches = NULL;
    model->merges = NULL;
    model->repo = repo;
    return model;
}

void free_model(GraphModel *model) {
    if (!model) return;

    // Libérer les branches
    Branch *branch = model->branches;
    while (branch) {
        Branch *next = branch->next;
        
        // Libérer les commits de la branche
        Commit *commit = branch->head;
        while (commit) {
            Commit *next_commit = commit->parent;
            free(commit->message);
            free(commit->author);
            free(commit);
            commit = next_commit;
        }
        
        free(branch->name);
        free(branch);
        branch = next;
    }

    // Libérer les merges
    Merge *merge = model->merges;
    while (merge) {
        Merge *next = merge->next;
        free(merge);
        merge = next;
    }

    free(model);
}

Branch* add_branch(GraphModel *model, const char *name, BranchType type) {
    Branch *branch = malloc(sizeof(Branch));
    if (!branch) return NULL;

    branch->name = strdup(name);
    branch->type = type;
    branch->head = NULL;
    branch->next = model->branches;
    model->branches = branch;

    return branch;
}

Commit* add_commit(GraphModel *model, Branch *branch, const git_oid *oid, CommitType type) {
    Commit *commit = malloc(sizeof(Commit));
    if (!commit) return NULL;

    // Récupérer les informations du commit
    git_commit *git_commit;
    if (git_commit_lookup(&git_commit, model->repo, oid) < 0) {
        free(commit);
        return NULL;
    }

    // Copier l'OID
    git_oid_cpy(&commit->oid, oid);

    // Copier le message
    const char *message = git_commit_message(git_commit);
    commit->message = strdup(message);

    // Copier l'auteur
    const git_signature *author = git_commit_author(git_commit);
    commit->author = strdup(author->name);

    // Copier la date
    commit->time = git_commit_time(git_commit);

    // Définir le type
    commit->type = type;

    // Lier au commit précédent
    commit->parent = branch->head;
    commit->merged = NULL;

    // Mettre à jour la tête de la branche
    branch->head = commit;

    git_commit_free(git_commit);
    return commit;
}

Merge* add_merge(GraphModel *model, Commit *from, Commit *to, Branch *target_branch) {
    Merge *merge = malloc(sizeof(Merge));
    if (!merge) return NULL;

    merge->from = from;
    merge->to = to;
    merge->target_branch = target_branch;
    merge->next = model->merges;
    model->merges = merge;

    return merge;
}
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

// Fonction utilitaire pour déterminer le type de branche à partir de son nom
static BranchType determine_branch_type(const char *name) {
    if (strcmp(name, "main") == 0 || strcmp(name, "master") == 0) {
        return BRANCH_MAIN;
    } else if (strstr(name, "sprint") != NULL) {
        return BRANCH_SPRINT;
    } else if (strstr(name, "story") != NULL) {
        return BRANCH_STORY;
    } else if (strstr(name, "hotfix") != NULL) {
        return BRANCH_HOTFIX;
    }
    return BRANCH_STORY; // Par défaut, considérer comme une story
}

int analyze_repository(GraphModel *model) {
    // Récupérer toutes les références (branches)
    git_reference_iterator *iter;
    if (git_reference_iterator_new(&iter, model->repo) < 0) {
        return -1;
    }

    git_reference *ref;
    while (git_reference_next(&ref, iter) == 0) {
        // Ne traiter que les branches
        if (git_reference_is_branch(ref)) {
            const char *name = git_reference_name(ref);
            BranchType type = determine_branch_type(name);
            Branch *branch = add_branch(model, name, type);

            if (!branch) {
                git_reference_free(ref);
                git_reference_iterator_free(iter);
                return -1;
            }

            // Récupérer le commit HEAD de la branche
            git_commit *commit;
            if (git_commit_lookup(&commit, model->repo, git_reference_target(ref)) == 0) {
                add_commit(model, branch, git_reference_target(ref), COMMIT_WORK);
                git_commit_free(commit);
            }
        }
        git_reference_free(ref);
    }
    git_reference_iterator_free(iter);

    return 0;
} 
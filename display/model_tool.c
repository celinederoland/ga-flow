#include <stdio.h>
#include <string.h>
#include "model_tool.h"

const char* get_branch_type_name(BranchType type) {
    switch (type) {
        case BRANCH_MAIN: return "MAIN";
        case BRANCH_SPRINT: return "SPRINT";
        case BRANCH_STORY: return "STORY";
        case BRANCH_HOTFIX: return "HOTFIX";
        default: return "UNKNOWN";
    }
}

const char* get_commit_type_name(CommitType type) {
    switch (type) {
        case COMMIT_WORK: return "WORK";
        case COMMIT_MERGE: return "MERGE";
        default: return "UNKNOWN";
    }
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

void print_model(const GraphModel *model) {
    printf("\n=== Modèle du Repository ===\n\n");

    // Afficher les branches
    printf("Branches:\n");
    Branch *branch = model->branches;
    while (branch) {
        printf("  %s (%s)\n", branch->name, get_branch_type_name(branch->type));
        
        // Afficher les commits de la branche
        Commit *commit = branch->head;
        while (commit) {
            char oid_str[GIT_OID_HEXSZ + 1];
            git_oid_tostr(oid_str, sizeof(oid_str), &commit->oid);
            
            printf("    - %s [%s] %s\n", 
                   oid_str,
                   get_commit_type_name(commit->type),
                   commit->message);
            
            commit = commit->parent;
        }
        printf("\n");
        branch = branch->next;
    }

    // Afficher les merges
    printf("Merges:\n");
    Merge *merge = model->merges;
    while (merge) {
        char from_oid[GIT_OID_HEXSZ + 1];
        char to_oid[GIT_OID_HEXSZ + 1];
        
        git_oid_tostr(from_oid, sizeof(from_oid), &merge->from->oid);
        git_oid_tostr(to_oid, sizeof(to_oid), &merge->to->oid);
        
        printf("  %s -> %s (sur %s)\n", 
               from_oid,
               to_oid,
               merge->target_branch->name);
               
        merge = merge->next;
    }
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
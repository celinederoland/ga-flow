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
    if (!model || !model->repo) {
        return -1;
    }

    // Récupérer toutes les références (branches, tags, etc.)
    git_strarray ref_list;
    if (git_reference_list(&ref_list, model->repo) != 0) {
        return -1;
    }

    // Parcourir toutes les références
    for (size_t i = 0; i < ref_list.count; i++) {
        git_reference *ref;
        if (git_reference_lookup(&ref, model->repo, ref_list.strings[i]) != 0) {
            continue;
        }

        // Vérifier si c'est une branche locale
        if (git_reference_is_branch(ref)) {
            const char *name = git_reference_name(ref);
            BranchType type = determine_branch_type(name);
            const char *branch_name = git_reference_shorthand(ref);
            Branch *branch = add_branch(model, branch_name, type);

            // Récupérer le commit HEAD de la branche
            git_commit *head_commit;
            if (git_commit_lookup(&head_commit, model->repo, git_reference_target(ref)) == 0) {
                // Ajouter le commit HEAD
                add_commit(model, branch, git_reference_target(ref), COMMIT_WORK);

                // Récupérer l'historique des commits
                git_revwalk *walker;
                if (git_revwalk_new(&walker, model->repo) == 0) {
                    // Configurer le walker pour parcourir l'historique
                    git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);
                    git_revwalk_push_ref(walker, ref_list.strings[i]);

                    // Parcourir tous les commits
                    git_oid oid;
                    while (git_revwalk_next(&oid, walker) == 0) {
                        // Ne pas ajouter le commit HEAD une deuxième fois
                        if (git_oid_cmp(&oid, git_reference_target(ref)) != 0) {
                            add_commit(model, branch, &oid, COMMIT_WORK);
                        }
                    }

                    git_revwalk_free(walker);
                }

                git_commit_free(head_commit);
            }
        }

        git_reference_free(ref);
    }

    git_strarray_dispose(&ref_list);
    return 0;
} 
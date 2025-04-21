#include "model.h"
#include "git_traverser.h"
#include "model_debug.h"
#include <string.h>

static BranchType determine_branch_type(const char *branch_name) {
    if (strcmp(branch_name, "main") == 0 || strcmp(branch_name, "master") == 0) {
        return BRANCH_MAIN;
    } else if (strncmp(branch_name, "story/", 6) == 0) {
        return BRANCH_STORY;
    } else if (strncmp(branch_name, "hotfix/", 7) == 0) {
        return BRANCH_HOTFIX;
    } else if (strncmp(branch_name, "sprint/", 7) == 0) {
        return BRANCH_SPRINT;
    }
    return BRANCH_UNKNOWN;
}

static const char *branch_type_to_string(BranchType type) {
    switch (type) {
        case BRANCH_MAIN: return "main";
        case BRANCH_STORY: return "story";
        case BRANCH_HOTFIX: return "hotfix";
        case BRANCH_SPRINT: return "sprint";
        default: return "unknown";
    }
}

int fillModel(git_repository * repo, Graph* graph) {
    git_reference **branch_refs;
    size_t branch_count;
    if (analyze_branches(repo, &branch_count, &branch_refs) < 0) {
        return 1;
    }

    // Initialiser le tableau des commits
    size_t commit_capacity = 10;
    graph->commits = malloc(commit_capacity * sizeof(Commit *));
    if (!graph->commits) {
        return 1;
    }
    graph->commit_count = 0;

    // Initialiser le tableau des branches
    graph->branches = malloc(sizeof(Branch *) * branch_count);
    graph->branch_count = branch_count;

    for (size_t i = 0; i < branch_count; i++) {
        graph->branches[i] = malloc(sizeof(Branch));
        graph->branches[i]->git_ref = branch_refs[i];
        graph->branches[i]->commits = NULL;
        graph->branches[i]->commit_count = 0;
        graph->branches[i]->type = determine_branch_type(git_reference_shorthand(branch_refs[i]));

        // Récupérer les commits de la branche
        git_commit **branch_commits = NULL;
        size_t branch_commit_count = 0;
        size_t branch_commit_capacity = 10;
        branch_commits = malloc(branch_commit_capacity * sizeof(git_commit *));

        if (get_branch_commits(branch_refs[i], &branch_commits, &branch_commit_count, &branch_commit_capacity) != 0) {
            free(branch_commits);
            continue;
        }

        // Pour chaque commit de la branche
        for (size_t j = 0; j < branch_commit_count; j++) {
            git_commit *current_commit = branch_commits[j];
            const git_oid *commit_id = git_commit_id(current_commit);

            // Vérifier si le commit existe déjà dans le graph
            Commit *existing_commit = NULL;
            for (size_t k = 0; k < graph->commit_count; k++) {
                const git_oid *existing_id = git_commit_id(graph->commits[k]->git_commit);
                if (git_oid_cmp(commit_id, existing_id) == 0) {
                    existing_commit = graph->commits[k];
                    break;
                }
            }

            if (existing_commit) {
                // Le commit existe déjà, on l'ajoute à la branche
                graph->branches[i]->commits = realloc(graph->branches[i]->commits, 
                    (graph->branches[i]->commit_count + 1) * sizeof(Commit *));
                graph->branches[i]->commits[graph->branches[i]->commit_count++] = existing_commit;

                // On ajoute la branche à la liste des branches du commit
                existing_commit->branches = realloc(existing_commit->branches,
                    (existing_commit->branch_count + 1) * sizeof(Branch *));
                existing_commit->branches[existing_commit->branch_count++] = graph->branches[i];
            } else {
                // Le commit n'existe pas, on le crée
                if (graph->commit_count >= commit_capacity) {
                    commit_capacity *= 2;
                    Commit **temp = realloc(graph->commits, commit_capacity * sizeof(Commit *));
                    if (!temp) {
                        free(branch_commits);
                        continue;
                    }
                    graph->commits = temp;
                }

                Commit *new_commit = malloc(sizeof(Commit));
                if (!new_commit) {
                    free(branch_commits);
                    continue;
                }

                new_commit->git_commit = current_commit;
                new_commit->branches = malloc(sizeof(Branch *));
                new_commit->branches[0] = graph->branches[i];
                new_commit->branch_count = 1;

                graph->commits[graph->commit_count++] = new_commit;

                // Ajouter le commit à la branche
                graph->branches[i]->commits = realloc(graph->branches[i]->commits,
                    (graph->branches[i]->commit_count + 1) * sizeof(Commit *));
                graph->branches[i]->commits[graph->branches[i]->commit_count++] = new_commit;
            }
        }

        free(branch_commits);
    }

    return 0;
}

void freeGraph(const Graph *graph) {

    // Free each branch and its reference
    for (size_t i = 0; i < graph->branch_count; i++) {
        git_reference_free(graph->branches[i]->git_ref);
        free(graph->branches[i]); // Free branch first
    }
    free(graph->branches); // Then free array of branches

    // Free each commit
    for (size_t i = 0; i < graph->commit_count; i++) {
        git_commit_free(graph->commits[i]->git_commit);
        free(graph->commits[i]);
    }
    free(graph->commits);
}

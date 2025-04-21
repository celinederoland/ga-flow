#include "list_branches.h"
#include "model.h"
#include <stdio.h>

void list_sprints_branches(const Graph *graph) {
    if (!graph) {
        printf("Graph is NULL\n");
        return;
    }

    printf("\n=== Sprint Branches ===\n");
    for (size_t i = 0; i < graph->branch_count; i++) {
        Branch *branch = graph->branches[i];
        if (branch->type == BRANCH_SPRINT) {
            const char *branch_name = git_reference_shorthand(branch->git_ref);
            printf("Sprint: %s\n", branch_name);
            printf("  Number of commits: %zu\n", branch->commit_count);
            
            // Afficher les commits de la branche
            for (size_t j = 0; j < branch->commit_count; j++) {
                Commit *commit = branch->commits[j];
                const git_oid *oid = git_commit_id(commit->git_commit);
                char oid_str[GIT_OID_HEXSZ + 1];
                git_oid_tostr(oid_str, sizeof(oid_str), oid);
                printf("  - Commit: %s\n", oid_str);
            }
            printf("\n");
        }
    }
}

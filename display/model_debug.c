#include <stdio.h>
#include "model.h"

void print_graph(const Graph *graph) {
    if (!graph) {
        printf("Graph est NULL\n");
        return;
    }

    printf("\n=== Contenu du Graph ===\n");
    printf("Nombre total de branches: %zu\n", graph->branch_count);
    printf("Nombre total de commits: %zu\n\n", graph->commit_count);

    // Afficher les branches et leurs commits
    for (size_t i = 0; i < graph->branch_count; i++) {
        Branch *branch = graph->branches[i];
        const char *branch_name = git_reference_shorthand(branch->git_ref);
        
        printf("Branche: %s\n", branch_name);
        printf("Nombre de commits: %zu\n", branch->commit_count);
        
        // Afficher les commits de la branche
        for (size_t j = 0; j < branch->commit_count; j++) {
            Commit *commit = branch->commits[j];
            const git_oid *oid = git_commit_id(commit->git_commit);
            char oid_str[GIT_OID_HEXSZ + 1];
            git_oid_tostr(oid_str, sizeof(oid_str), oid);
            
            printf("  Commit: %s - %s \n", oid_str, git_commit_message(commit->git_commit));
            printf("    Présent sur %zu branches\n", commit->branch_count);
        }
        printf("\n");
    }

    // Afficher les commits et leurs branches
    printf("=== Liste des commits ===\n");
    for (size_t i = 0; i < graph->commit_count; i++) {
        Commit *commit = graph->commits[i];
        const git_oid *oid = git_commit_id(commit->git_commit);
        char oid_str[GIT_OID_HEXSZ + 1];
        git_oid_tostr(oid_str, sizeof(oid_str), oid);
        
        printf("Commit: %s - %s \n", oid_str, git_commit_message(commit->git_commit));
        printf("  Présent sur les branches: ");
        for (size_t j = 0; j < commit->branch_count; j++) {
            const char *branch_name = git_reference_shorthand(commit->branches[j]->git_ref);
            printf("%s ", branch_name);
        }
        printf("\n\n");
    }
}

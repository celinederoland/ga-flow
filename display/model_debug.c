#include <stdio.h>
#include "model.h"
#include "model_debug.h"

static const char *branch_type_to_string(BranchType type) {
    switch (type) {
        case BRANCH_MAIN: return "main";
        case BRANCH_STORY: return "story";
        case BRANCH_HOTFIX: return "hotfix";
        case BRANCH_SPRINT: return "sprint";
        default: return "unknown";
    }
}

void print_graph(const Graph *graph) {
    if (!graph) {
        printf("Graph is NULL\n");
        return;
    }

    printf("Graph contains %zu branches and %zu commits\n", graph->branch_count, graph->commit_count);
    printf("\nBranches:\n");
    for (size_t i = 0; i < graph->branch_count; i++) {
        const Branch *branch = graph->branches[i];
        const char *branch_name = git_reference_shorthand(branch->git_ref);
        printf("  Branch %zu: %s (type: %s)\n", i, branch_name, branch_type_to_string(branch->type));
        printf("    Contains %zu commits:\n", branch->commit_count);
        for (size_t j = 0; j < branch->commit_count; j++) {
            const git_oid *commit_id = git_commit_id(branch->commits[j]->git_commit);
            char commit_hash[GIT_OID_HEXSZ + 1];
            git_oid_fmt(commit_hash, commit_id);
            commit_hash[GIT_OID_HEXSZ] = '\0';
            printf("      Commit %zu: %s\n", j, commit_hash);
        }
    }

    printf("\nCommits:\n");
    for (size_t i = 0; i < graph->commit_count; i++) {
        const Commit *commit = graph->commits[i];
        const git_oid *commit_id = git_commit_id(commit->git_commit);
        char commit_hash[GIT_OID_HEXSZ + 1];
        git_oid_fmt(commit_hash, commit_id);
        commit_hash[GIT_OID_HEXSZ] = '\0';
        printf("  Commit %zu: %s\n", i, commit_hash);
        printf("    Present in %zu branches:\n", commit->branch_count);
        for (size_t j = 0; j < commit->branch_count; j++) {
            const char *branch_name = git_reference_shorthand(commit->branches[j]->git_ref);
            printf("      Branch %zu: %s (type: %s)\n", j, branch_name, 
                   branch_type_to_string(commit->branches[j]->type));
        }
    }
}

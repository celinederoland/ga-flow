#include "model.h"
#include "git_traverser.h"
#include "model_debug.h"

int fillModel(git_repository * repo) {
    git_reference **branch_refs;
    size_t branch_count;
    if (analyze_branches(repo, &branch_count, &branch_refs) < 0) {
        return 1;
    }

    Graph graph = {
        .branches = NULL,
        .branch_count = 0,
        .commits = NULL,
        .commit_count = 0
    };

    graph.branches = malloc(sizeof(Branch *) * branch_count);
    graph.branch_count = branch_count;

    for (size_t i = 0; i < branch_count; i++) {
        graph.branches[i] = malloc(sizeof(Branch));
        graph.branches[i]->git_ref = branch_refs[i];
        graph.branches[i]->commits = NULL;
        graph.branches[i]->commit_count = 0;
    }

    print_graph(&graph);

    git_commit **commit_refs;
    size_t commit_count;
    get_branches_commits(branch_refs, branch_count, &commit_count, &commit_refs);


    // Free each branch and its reference
    for (size_t i = 0; i < graph.branch_count; i++) {
        free(graph.branches[i]); // Free branch first
    }
    free(graph.branches); // Then free array of branches

    // Free each branch reference
    for (size_t i = 0; i < branch_count; i++) {
        git_reference_free(branch_refs[i]);
    }
    free(branch_refs);

    // Free each commit
    for (size_t i = 0; i < commit_count; i++) {
        git_commit_free(commit_refs[i]);
    }
    free(commit_refs);
    return 0;
}



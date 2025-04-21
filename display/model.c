#include "model.h"

#include <stdio.h>
#include <git2.h>

int analyze_branches(git_repository *repo, size_t *branch_count, git_reference ***branch_refs) {
    *branch_count = 0;
    git_strarray ref_list;

    if (git_reference_list(&ref_list, repo) != 0) {
        git_strarray_dispose(&ref_list);
        *branch_refs = NULL;
        return -1;
    }

    // Tableau pour stocker les références des branches
    *branch_refs = malloc(ref_list.count * sizeof(git_reference *));

    for (size_t i = 0; i < ref_list.count; i++) {
        git_reference *ref;
        if (git_reference_lookup(&ref, repo, ref_list.strings[i]) != 0) {
            continue;
        }
        if (git_reference_is_branch(ref)) {
            (*branch_refs)[(*branch_count)++] = ref;
            fprintf(stderr, "Retrieve Branch %zu : %s\n", *branch_count, git_reference_shorthand(ref));
        } else {
            git_reference_free(ref);
        }
    }

    git_strarray_dispose(&ref_list);
    fprintf(stderr, "\n\n\n");
    return 0;
}

void log_branches_details(git_reference **branch_refs, size_t branch_count) {
    for (size_t i = 0; i < branch_count; i++) {
        git_reference *ref = branch_refs[i];
        const char *branch_name = git_reference_shorthand(ref);
        git_oid *target = (git_oid *) git_reference_target(ref);
        char oid_str[GIT_OID_HEXSZ + 1];
        git_oid_fmt(oid_str, target);
        oid_str[GIT_OID_HEXSZ] = '\0';
        // NOTE : important - ici le oid que j'obtiens est celui du commit head
        // ==> c'est à partir de là que je vais pouvoir remonter dans le graphe
        printf("Retrieved Branch: %zu - %s -> %s\n", i, branch_name, oid_str);
        git_reference_free(ref);
    }
}

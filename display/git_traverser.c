#include "git_traverser.h"

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
        fprintf(stderr,"Retrieved Branch: %zu - %s -> %s\n", i, branch_name, oid_str);
        git_reference_free(ref);
    }
}

int get_branch_commits(git_reference *branch_ref, git_commit ***commit_refs, size_t *commit_count, size_t *capacity) {
    const char *branch_name = git_reference_shorthand(branch_ref);
    git_oid *target = (git_oid *) git_reference_target(branch_ref);
    char oid_str[GIT_OID_HEXSZ + 1];
    git_oid_fmt(oid_str, target);
    oid_str[GIT_OID_HEXSZ] = '\0';
    fprintf(stderr,"Retrieved Branch: %s -> %s\n", branch_name, oid_str);

    // Récupérer le commit head de la branche (le plus récent)
    git_commit *commit = NULL;
    if (git_commit_lookup(&commit, git_reference_owner(branch_ref), target) != 0) {
        const git_error *e = git_error_last();
        fprintf(stderr,"Error looking up commit: %s\n", e->message);
        return -1;
    }

    // Stocker le commit dans le tableau
    if (*commit_count >= *capacity) {
        *capacity *= 2;
        git_commit **temp = realloc(*commit_refs, *capacity * sizeof(git_commit *));
        if (!temp) {
            fprintf(stderr, "Erreur de réallocation mémoire\n");
            git_commit_free(commit);
            return -1;
        }
        *commit_refs = temp;
    }
    (*commit_refs)[(*commit_count)++] = commit;

    const git_oid *commit_id = git_commit_id(commit);
    char commit_hash[GIT_OID_HEXSZ + 1];
    git_oid_fmt(commit_hash, commit_id);
    commit_hash[GIT_OID_HEXSZ] = '\0';
    fprintf(stderr,"Head Commit Hash: %s\n", commit_hash);

    // Remonter dans l'arbre pour récupérer tous les commits de la branche
    while (commit != NULL) {
        // Afficher les informations du commit
        const char *message = git_commit_message(commit);
        const git_signature *author = git_commit_author(commit);
        time_t commit_time = git_commit_time(commit);

        fprintf(stderr,"Commit %zu:\n", *commit_count);
        fprintf(stderr,"  Hash: %s\n", commit_hash);
        fprintf(stderr,"  Author: %s <%s>\n", author->name, author->email);
        fprintf(stderr,"  Date: %s", ctime(&commit_time));
        fprintf(stderr,"  Message: %s\n", message);

        // Passer au parent
        git_commit *parent = NULL;
        size_t parent_count = git_commit_parentcount(commit);
        if (parent_count > 1) {
            fprintf(stderr,"Warning: Commit %s has multiple parents (%zu parents)\n", commit_hash, parent_count);
        }
        if (git_commit_parent(&parent, commit, 0) != 0) {
            break;
        }

        // Stocker le parent dans le tableau
        if (*commit_count >= *capacity) {
            *capacity *= 2;
            git_commit **temp = realloc(*commit_refs, *capacity * sizeof(git_commit *));
            if (!temp) {
                fprintf(stderr, "Erreur de réallocation mémoire\n");
                git_commit_free(commit);
                git_commit_free(parent);
                return -1;
            }
            *commit_refs = temp;
        }
        (*commit_refs)[(*commit_count)++] = parent;

        git_commit_free(commit);
        commit = parent;

        // Mettre à jour le hash pour la prochaine itération
        const git_oid *parent_id = git_commit_id(commit);
        git_oid_fmt(commit_hash, parent_id);
        commit_hash[GIT_OID_HEXSZ] = '\0';
    }

    git_commit_free(commit);
    return 0;
}

int get_branches_commits(git_reference **branch_refs, size_t branch_count, size_t *commit_count, git_commit ***commit_refs) {
    size_t count = 0;
    size_t capacity = 10; // Capacité initiale du tableau

    // Allouer le tableau initial
    *commit_refs = malloc(capacity * sizeof(git_commit *));
    if (!*commit_refs) {
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        return -1;
    }

    for (size_t i = 0; i < branch_count; i++) {
        if (get_branch_commits(branch_refs[i], commit_refs, &count, &capacity) != 0) {
            // En cas d'erreur, on continue avec la branche suivante
            continue;
        }
    }

    // Mettre à jour le nombre de commits
    *commit_count = count;
    return 0;
}

void log_commits_details(git_commit **commit_refs, size_t commit_count) {
    for (size_t i = 0; i < commit_count; i++) {
        git_commit *ref = commit_refs[i];
        const git_oid *commit_id = git_commit_id(ref);
        char oid_str[GIT_OID_HEXSZ + 1];
        git_oid_fmt(oid_str, commit_id);
        oid_str[GIT_OID_HEXSZ] = '\0';

        fprintf(stderr, "Retrieved Commit: %zu (%s) -> %s\n", i, git_commit_message(ref), oid_str);
    }
}

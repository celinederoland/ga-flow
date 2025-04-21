#ifndef MODEL_H
#define MODEL_H
#include <stdio.h>
#include <git2.h>

int analyze_branches(git_repository *repo, size_t *branch_count, git_reference ***branch_refs);
void log_branches_details(git_reference **branch_refs, size_t branch_count);

#endif //MODEL_H

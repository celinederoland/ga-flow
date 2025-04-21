#ifndef GIT_TRAVERSER_H
#define GIT_TRAVERSER_H
#include <stdio.h>
#include <git2.h>

int analyze_branches(git_repository *repo, size_t *branch_count, git_reference ***branch_refs);
void log_branches_details(git_reference **branch_refs, size_t branch_count);
int get_branches_commits(git_reference **branch_refs, size_t branch_count, size_t *commit_count, git_commit ***commit_refs);
void log_commits_details(git_commit **commit_refs, size_t commit_count);
#endif //GIT_TRAVERSER_H

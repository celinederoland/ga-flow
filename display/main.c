#include <stdio.h>
#include <string.h>
#include <git2.h>
#include <getopt.h>

#include "model.h"
#include "model_debug.h"
#include "types.h"
#include "git_traverser.h"
#include "list_branches.h"

void print_usage(const char *program_name) {
    printf("Usage: %s [options] [repo_path]\n\n", program_name);
    printf("Options:\n");
    printf("  --help     Display this help\n");
    printf("  -v             Display warnings\n");
    printf("  -vv            Display information\n");
    printf("  -vvv           Display debug messages\n");
    printf("  -h             Display complete history\n");
    printf("  -b             Display branches\n");
    printf("  -g             Generate graph\n");
    printf("  --list-sprints Display sprint branches\n");
    printf("\n");
    printf("If no mode is specified (-b or -g), history mode is used by default.\n");
    printf("If no path is specified, the current directory (.) is used.\n");
}

RunArguments parse_arguments(int argc, char **argv) {
    RunArguments opts = {
        .repo_path = ".",
        .verbosity = 0,
        .mode = MODE_HISTORY  // Mode par défaut
    };

    for (int i = 1; i < argc; i++) {
        // Options avec un seul tiret
        if (argv[i][0] == '-' && argv[i][1] != '-') {
            if (strcmp(argv[i], "-h") == 0) {
                opts.mode = MODE_HISTORY;
                return opts;
            } else if (strcmp(argv[i], "-b") == 0) {
                opts.mode = MODE_BRANCHES;
            } else if (strcmp(argv[i], "-g") == 0) {
                opts.mode = MODE_GRAPH;
            } else if (strncmp(argv[i], "-v", 2) == 0) {
                // Compter le nombre de 'v'
                opts.verbosity = strlen(argv[i]) - 1;
            }
        }
        // Options avec deux tirets
        else if (argv[i][0] == '-' && argv[i][1] == '-') {
            if (strcmp(argv[i], "--help") == 0) {
                opts.mode = MODE_HELP;
                return opts;
            } else if (strcmp(argv[i], "--list-sprints") == 0) {
                opts.mode = MODE_LIST_SPRINTS;
            }
        }
        // Argument qui n'est pas une option (doit être le chemin du repo)
        else {
            opts.repo_path = argv[i];
        }
    }

    return opts;
}

int doAction(const Graph *graph, RunArguments opts) {
    if (opts.mode == MODE_LIST_SPRINTS) {
        list_sprints_branches(graph);
        return 0;
    }
    
    printf("do an action\n");
    print_graph(graph);
    return 0;
}

int main(int argc, char **argv) {
    // Initialiser libgit2
    git_libgit2_init();

    // Parser les arguments
    RunArguments opts = parse_arguments(argc, argv);

    // Si l'utilisateur a demandé l'aide, l'afficher et quitter
    if (opts.mode == MODE_HELP) {
        print_usage(argv[0]);
        git_libgit2_shutdown();
        return 0;
    }

    git_repository *repo = NULL;
    int error = git_repository_open(&repo, opts.repo_path);
    if (error < 0) {
        const git_error *e = git_error_last();
        printf("Error: Unable to open Git repository at '%s': %s\n", opts.repo_path, e->message);
        git_libgit2_shutdown();
        return 1;
    }

    Graph graph = {
        .branches = NULL,
        .branch_count = 0,
        .commits = NULL,
        .commit_count = 0
    };
    const int r = fillModel(repo, &graph);
    if (r != 0) {
        return r;
    }
    const int r2 = doAction(&graph, opts);

    freeGraph(&graph);
    git_repository_free(repo);
    git_libgit2_shutdown();
    return r2;
}

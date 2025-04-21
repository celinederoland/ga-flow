#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <git2.h>
#include "types.h"

void print_usage(const char *program_name) {
    printf("Usage: %s [options] [chemin_du_repo]\n\n", program_name);
    printf("Options:\n");
    printf("  -h, --help     Afficher cette aide\n");
    printf("  -v             Afficher les avertissements\n");
    printf("  -vv            Afficher les informations\n");
    printf("  -vvv           Afficher les messages de debug\n");
    printf("  -b             Afficher les branches\n");
    printf("  -g             Générer le graph\n");
    printf("\n");
    printf("Si aucun mode n'est spécifié (-b ou -g), le mode historique est utilisé par défaut.\n");
    printf("Si aucun chemin n'est spécifié, le répertoire courant (.) est utilisé.\n");
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
                opts.mode = MODE_HELP;
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
            }
        }
        // Argument qui n'est pas une option (doit être le chemin du repo)
        else {
            opts.repo_path = argv[i];
        }
    }

    return opts;
}

int validate_repository(const char *repo_path) {
    git_repository *repo = NULL;
    int error = git_repository_open(&repo, repo_path);
    
    if (error < 0) {
        const git_error *e = git_error_last();
        printf("Erreur: Impossible d'ouvrir le dépôt Git à '%s': %s\n", repo_path, e->message);
        return 0;
    }

    // Vérifier si c'est un dépôt bare
    if (git_repository_is_bare(repo)) {
        printf("Erreur: Le dépôt à '%s' est un dépôt bare. Veuillez utiliser un dépôt de travail.\n", repo_path);
        git_repository_free(repo);
        return 0;
    }

    // Vérifier si c'est un dépôt shallow
    if (git_repository_is_shallow(repo)) {
        printf("Erreur: Le dépôt à '%s' est un dépôt shallow. Veuillez utiliser un dépôt complet.\n", repo_path);
        git_repository_free(repo);
        return 0;
    }

    git_repository_free(repo);
    return 1;
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

    // Valider le dépôt Git
    if (!validate_repository(opts.repo_path)) {
        git_libgit2_shutdown();
        return 1;
    }

    // Afficher les options choisies (pour debug)
    printf("Options choisies:\n");
    printf("  Repository: %s\n", opts.repo_path);
    printf("  Verbosité: %d\n", opts.verbosity);
    printf("  Mode: ");
    switch (opts.mode) {
        case MODE_HISTORY:
            printf("Historique\n");
            break;
        case MODE_BRANCHES:
            printf("Branches\n");
            break;
        case MODE_GRAPH:
            printf("Graph\n");
            break;
        default:
            printf("Inconnu\n");
    }

    // TODO: Implémenter les différentes fonctionnalités selon le mode

    // Nettoyer libgit2
    git_libgit2_shutdown();
    return 0;
}
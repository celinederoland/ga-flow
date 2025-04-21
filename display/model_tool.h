#ifndef MODEL_TOOL_H
#define MODEL_TOOL_H

#include "model.h"

/**
 * Affiche le modèle du repository dans la console
 * @param model Le modèle à afficher
 */
void print_model(const GraphModel *model);

/**
 * Analyse le repository Git et remplit le modèle
 * @param model Le modèle à remplir
 * @return 0 en cas de succès, -1 en cas d'erreur
 */
int analyze_repository(GraphModel *model);

#endif //MODEL_TOOL_H 
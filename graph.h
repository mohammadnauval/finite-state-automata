#ifndef __GRAPH_H
#define __GRAPH_H

#include <stdio.h>
#include <stdbool.h>

#include "fa.h"

struct graph {
	size_t state_count;
	struct state_set *adjacency; /* see: fa.h */
};

/* EXERCICE 4 */
void graph_depth_first_search(const struct graph *self, size_t state, bool *visited);
bool graph_has_path(const struct graph *self, size_t from, size_t to);
void graph_add_transition(struct graph *self, const size_t from, const size_t to); /* additional function */
void graph_create_from_fa(struct graph *self, const struct fa *fa, bool inverted);
void graph_destroy(struct graph *self);

#endif 
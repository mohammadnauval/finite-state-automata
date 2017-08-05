#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"
#include "fa.h"

void graph_depth_first_search(const struct graph *self, size_t state, bool *visited) {
	if (NULL == self) {
		fprintf(stderr, "Invalid automate.\n");
		exit(EXIT_FAILURE);
	}
	if (state >= self->state_count) {
		fprintf(stderr, "Error: state %d doesn't exist.\n", (int)state);
		exit(EXIT_FAILURE);
	}

	visited[state] = true;
	for (size_t i=0; i<self->adjacency[state].size; i++) {
		if (visited[self->adjacency[state].states[i]] == false) {
			graph_depth_first_search(self, self->adjacency[state].states[i], visited);
		}
	}
}

bool graph_has_path(const struct graph *self, size_t from, size_t to) {
	if (NULL == self) {
		fprintf(stderr, "Invalid automate.\n");
		exit(EXIT_FAILURE);
	}
	if (from >= self->state_count) {
		fprintf(stderr, "Error: state %d doesn't exist.\n", (int)from);
		exit(EXIT_FAILURE);
	}
	if (to >= self->state_count) {
		fprintf(stderr, "Error: state %d doesn't exist.\n", (int)to);
		exit(EXIT_FAILURE);
	}

	bool *visited = calloc(self->state_count, sizeof(bool));
	for (size_t i=0; i<self->state_count; i++) {
		visited[i] = false;
	}
	graph_depth_first_search(self, from, visited);
	bool has_path = visited[to];
	free(visited);
	return has_path;
}

/* additional function to insert a transition into the graph */
void graph_add_transition(struct graph *self, const size_t from, const size_t to) {
	if (self == NULL) {
		fprintf(stderr, "Invalid automate.\n");
		exit(EXIT_FAILURE);
	}
	if (from >= self->state_count) {
		fprintf(stderr, "Error: state %d doesn't exist.\n", (int)from);
		exit(EXIT_FAILURE);
	}
	if (to >= self->state_count) {
		fprintf(stderr, "Error: state %d doesn't exist.\n", (int)to);
		exit(EXIT_FAILURE);
	}

	/* verification if the transition already exists */
	for (size_t i=0; i<self->adjacency[from].size; i++) {
		if (self->adjacency[from].states[i] == to) {
			return;
		}
	}

	/* transitions addition */
	/* if the array of adjacent list for a state is already full */
	if (self->adjacency[from].size == self->adjacency[from].capacity) {
		size_t old_capacity = self->adjacency[from].capacity;
		size_t new_capacity = old_capacity + 2;
		self->adjacency[from].capacity = new_capacity;
		size_t *tmp = calloc(new_capacity, sizeof(size_t));
		memcpy(tmp, self->adjacency[from].states, old_capacity*sizeof(size_t));
		free(self->adjacency[from].states);
		self->adjacency[from].states = tmp;
	}
	/* insert in the array */
	self->adjacency[from].states[self->adjacency[from].size] = to;
	self->adjacency[from].size++;

	/* sort the array */
	for (size_t i=0; i<self->adjacency[from].size-1; i++) {
		for (size_t j=self->adjacency[from].size-1; j>0; j--) {
			if (self->adjacency[from].states[j] < self->adjacency[from].states[j-1]) {
				size_t tmp = self->adjacency[from].states[j];
				self->adjacency[from].states[j] = self->adjacency[from].states[j-1];
				self->adjacency[from].states[j-1] = tmp;
			}
		}
	}
}

void graph_create_from_fa(struct graph *self, const struct fa *fa, bool inverted) {
	self->state_count = fa->state_count;
	self->adjacency = calloc(self->state_count, sizeof(struct state_set));

	for (size_t i=0; i<self->state_count; i++) {
		size_t adjacency_initial_capacity = 2;
		self->adjacency[i].states = calloc(adjacency_initial_capacity, sizeof(size_t));
		self->adjacency[i].capacity = adjacency_initial_capacity;
		self->adjacency[i].size = 0;
	}

	for (size_t i=0; i<fa->state_count; i++) {
		for (size_t j=0; j<fa->alpha_count; j++) {
			for (size_t k=0; k<fa->transitions[i][j].size; k++) {
				if (inverted == true) {
					graph_add_transition(self, fa->transitions[i][j].states[k], i);
				}else {
					graph_add_transition(self, i, fa->transitions[i][j].states[k]);
				} 
			}
		}
	}
}

void graph_destroy(struct graph *self) {
	for (size_t i=0; i<self->state_count; i++) {
		free(self->adjacency[i].states);
	}
	free(self->adjacency);
	free(self);
}



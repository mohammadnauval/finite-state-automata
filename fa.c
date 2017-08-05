#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "fa.h"
#include "graph.h"

/********************************************* EXERCICE 2 *********************************************** */

void fa_create(struct fa *self, size_t alpha_count, size_t state_count) {
	self->alpha_count = alpha_count;
	self->state_count = state_count;

	// initiate the state array
	self->states = calloc(state_count, sizeof(struct state));
	for (size_t i=0; i<state_count; i++) {
		self->states[i].is_initial = false;
		self->states[i].is_final = false;
	}

	// initiate the array of transitions
	self->transitions = calloc(state_count, sizeof(struct state_set));
	for (size_t i=0; i<state_count; i++) {
		self->transitions[i] = calloc(alpha_count, sizeof(struct state_set));
		for (size_t j=0; j<alpha_count; j++) {
			self->transitions[i][j].size = 0;
			self->transitions[i][j].capacity = 2;
			size_t capacity = self->transitions[i][j].capacity;
			self->transitions[i][j].states = calloc(capacity, sizeof(size_t));
		}
	}
}

void fa_destroy(struct fa *self) {
	free(self->states);
	for (size_t i=0; i<self->state_count; i++) {
		for (size_t j=0; j<self->alpha_count; j++) {
			free(self->transitions[i][j].states);
		}
		free(self->transitions[i]);
	}
	free(self->transitions);
	free(self);
}

void fa_set_state_initial(struct fa *self, size_t state) {
	self->states[state].is_initial = true;
}

void fa_set_state_final(struct fa *self, size_t state) {
	self->states[state].is_final = true;
}

void fa_add_transition(struct fa *self, size_t from, char alpha, size_t to) {
	if (from >= self->state_count) {
		fa_destroy(self);
		fprintf(stderr, "Error: invalid initial state\n");
		exit(EXIT_FAILURE);
	}
	if (to >= self->state_count) {
		fa_destroy(self);
		fprintf(stderr, "Error: invalid destination state\n");
		exit(EXIT_FAILURE);
	}
	if ((alpha-'a') >= (int)self->alpha_count) {
		fa_destroy(self);
		fprintf(stderr, "Error: invalid alphabet\n");
		exit(EXIT_FAILURE);
	}

	for (size_t i=0; i<self->transitions[from][alpha-'a'].size; i++) {
		if (to == self->transitions[from][alpha-'a'].states[i]) {
			return;
		}
	}

	size_t oldCapacity = self->transitions[from][alpha-'a'].capacity;
	size_t size = self->transitions[from][alpha-'a'].size; 
	
	// if the array is full
	if (size == oldCapacity) {
		size_t newCapacity = oldCapacity + 2;
		self->transitions[from][alpha-'a'].capacity = newCapacity;
		size_t *newStates = calloc(newCapacity, sizeof(size_t));
		memcpy(newStates, self->transitions[from][alpha-'a'].states, oldCapacity*sizeof(size_t));
		free(self->transitions[from][alpha-'a'].states);
		self->transitions[from][alpha-'a'].states = newStates;
	}

	// insertion in the array
	self->transitions[from][alpha-'a'].states[size] = to;
	self->transitions[from][alpha-'a'].size = size + 1;

	// sort the transitions array
	for (size_t i=0; i<self->transitions[from][alpha-'a'].size-1; i++) {
		for (size_t j=self->transitions[from][alpha-'a'].size-1; j>0; j--) {
			if (self->transitions[from][alpha-'a'].states[j] < self->transitions[from][alpha-'a'].states[j-1]) {
				size_t tmp = self->transitions[from][alpha-'a'].states[j];
				self->transitions[from][alpha-'a'].states[j] = self->transitions[from][alpha-'a'].states[j-1];
				self->transitions[from][alpha-'a'].states[j-1] = tmp;
			}
		}
	}
}

void fa_pretty_print(const struct fa *self, FILE *out) {
	fprintf(out, "Initial states: \n\t");
	for (size_t i=0; i<self->state_count; i++) {
		if (self->states[i].is_initial==true) {
			fprintf(out, "%d ", (int)i);
		}
	}
	fprintf(out, "\n");
	fprintf(out, "Final states: \n\t");
	for (size_t i=0; i<self->state_count; i++) {
		if (self->states[i].is_final==true) {
			fprintf(out, "%d ",(int)i);
		}
	}
	fprintf(out ,"\n");

	fprintf(out, "transitions:\n");
	for (size_t i=0; i<self->state_count; i++) {
		fprintf(out, "\tFor state %d\n", (int)i);
		for (size_t j=0; j<self->alpha_count; j++) {
			fprintf(out, "\t\tFor letter %c : ", (unsigned char)j+'a');
			for (size_t k=0; k<self->transitions[i][j].size; k++) {
				fprintf(out, "%zu ", self->transitions[i][j].states[k]);
			}
			fprintf(out, "\n");
		}
		fprintf(out, "\n");
	}

	fprintf(out ,"Nombre de transitions : %d\n", (int)fa_count_transitions(self));
	if (fa_is_deterministic(self)) {
		fprintf(out, "l'automate est determinist. \n");
	}else {
		fprintf(out, "l'automate n'est pas determinist. \n");
	}
	if (fa_is_complete(self)) {
		fprintf(out, "l'automate est complet. \n");
	}else {
		fprintf(out, "l'automate n'est pas complet. \n");
	}
}

/********************************************* EXERCICE 3 *********************************************** */

void fa_remove_transition(struct fa *self, size_t from, char alpha, size_t to) {
	if (NULL == self) {
		fprintf(stderr, "Error: invalid automate. \n");
		exit(EXIT_FAILURE);
	}
	if (from >= self->state_count) {
		fa_destroy(self);
		fprintf(stderr, "Error: invalid initial state\n");
		exit(EXIT_FAILURE);
	}
	if (to >= self->state_count) {
		fa_destroy(self);
		fprintf(stderr, "Error: invalid destination state\n");
		exit(EXIT_FAILURE);
	}
	if ((alpha-'a') >= (int)self->alpha_count) {
		fa_destroy(self);
		fprintf(stderr, "Error: invalid alphabet\n");
		exit(EXIT_FAILURE);
	}

	size_t *states = self->transitions[from][alpha-'a'].states;
	size_t size = self->transitions[from][alpha-'a'].size;

	bool transitionFound = false;
	for (size_t i=0; i<size; i++) {
		// if we find the transition
		if (states[i] == to) {
			transitionFound = true;
			for (size_t j=i; j<size-1; j++) {
				states[j] = states[j+1];
			}
			self->transitions[from][alpha-'a'].size = size - 1;
			break;
		}
	}
	if (transitionFound == false) {
		fa_destroy(self);
		fprintf(stderr, "Error: transition from %d to %d with alphabet %c doesn't exist.\n", (int)from, (int)to, alpha);
		exit(EXIT_FAILURE);
	}
}

void fa_remove_state(struct fa *self, size_t state) {
	if (state > self->state_count) {
		fa_destroy(self);
		fprintf(stderr, "Error: invalid state to remove. \n");
		exit(EXIT_FAILURE);
	}
	if (self->states[state].is_initial == true) {
		char delete_initial = ' ';
		printf("Initial state is going to be removed. proceed? (y/n) ");
		scanf("%c", &delete_initial);
		if (delete_initial == 'n') {
			fa_destroy(self);
			printf("Exit program. The state isn't removed. \n");
			exit(EXIT_FAILURE);
		}
	}

	// create a new state array
	struct state *states = calloc(self->state_count-1, sizeof(struct state));
	for (size_t i=0; i<self->state_count-1; i++) {
		if (i>=state) {
			states[i] = self->states[i+1];
		}else {
			states[i] = self->states[i];
		}
	}
	/*if (state == self->state_count-1) {
		states[self->state_count-1] = self->states[self->state_count-1];
	}*/
	free(self->states);
	self->states = states;

	// create a new array of transitions
	struct state_set **transitions = calloc(self->state_count-1, sizeof(struct state_set));
	for (size_t i=0; i<self->state_count-1; i++) {
		transitions[i] = calloc(self->alpha_count, sizeof(struct state_set));
		for (size_t j=0; j<self->alpha_count; j++) {
			// for all the states before the state that we will remove
			if (i < state) {
				for (size_t k=0; k<self->transitions[i][j].size; k++) {
					if (self->transitions[i][j].states[k] == state) {
						fa_remove_transition(self, i, j+'a', state);
					}
					if (self->transitions[i][j].states[k] > state) {
						self->transitions[i][j].states[k] = self->transitions[i][j].states[k] - 1;
					}
				}
				size_t capacity = self->transitions[i][j].capacity;
				size_t size = self->transitions[i][j].size;
				transitions[i][j].size = size;
				transitions[i][j].capacity = capacity;
				transitions[i][j].states = calloc(capacity, sizeof(size_t));
				memcpy(transitions[i][j].states, self->transitions[i][j].states, capacity*sizeof(size_t));
			// for the states that are found after the state that we remove
			}else if (i >= state) {
				for (size_t k=0; k<self->transitions[i+1][j].size; k++) {
					// remove all the transitions to the state that will be removed
					if (self->transitions[i+1][j].states[k] == state) {
						fa_remove_transition(self, i, j+'a', state);
					}
					// decrement all the trasitions to the states that are after the state removed
					if (self->transitions[i+1][j].states[k] > state) {
						self->transitions[i+1][j].states[k] = self->transitions[i+1][j].states[k] - 1;
					}
				}
				size_t capacity = self->transitions[i+1][j].capacity;
				size_t size = self->transitions[i+1][j].size;
				transitions[i][j].size = size;
				transitions[i][j].capacity = capacity;
				transitions[i][j].states = calloc(capacity, sizeof(size_t));
				memcpy(transitions[i][j].states, self->transitions[i+1][j].states, capacity*sizeof(size_t));
			}
			free(self->transitions[i][j].states);
			if (i == self->state_count-2) {
				free(self->transitions[i+1][j].states);
			}
		}
		free(self->transitions[i]);
		if (i == self->state_count-2) {
			free(self->transitions[i+1]);
		}
	}
	free(self->transitions);
	self->state_count = self->state_count - 1;
	self->transitions = transitions;
}

size_t fa_count_transitions(const struct fa *self) {
	size_t nbTransition = 0;
	for (size_t i=0; i<self->state_count; i++) {
		for (size_t j=0; j<self->alpha_count; j++) {
			for (size_t k=0; k<self->transitions[i][j].size; k++) {
				nbTransition = nbTransition + 1;
			}
		}
	}
	return nbTransition;
}

bool fa_is_deterministic(const struct fa *self) {
	for (size_t i=0; i<self->state_count; i++) {
		for (size_t j=0; j<self->alpha_count; j++) {
			if (self->transitions[i][j].size>1) {
				return false;
			}
		}
	}
	return true;
}

bool fa_is_complete(const struct fa *self) {
	for (size_t i=0; i<self->state_count; i++) {
		for (size_t j=0; j<self->alpha_count; j++) {
			if (self->transitions[i][j].size == 0) {
				return false;
			}
		}
	}
	return true;
}

void fa_make_complete(struct fa *self) {
	struct state *new_states = calloc(self->state_count+1, sizeof(struct state));
	memcpy(new_states, self->states, self->state_count*sizeof(struct state));
	free(self->states);
	self->states = new_states;
	self->state_count = self->state_count + 1;

	struct state_set **transitions = calloc(self->state_count, sizeof(struct state_set));
	for (size_t i=0; i<self->state_count; i++) {
		transitions[i] = calloc(self->alpha_count, sizeof(struct state_set));
		for (size_t j=0; j<self->alpha_count; j++) {
			// le cas de l'état poubelle
			if (i == self->state_count-1) {
				transitions[i][j].capacity = 1;
				transitions[i][j].size = 0;
				transitions[i][j].states = calloc(transitions[i][j].capacity, sizeof(size_t));
				transitions[i][j].states[transitions[i][j].size] = i;
				transitions[i][j].size++;
			// default states
			}else {
				// states that don't have any transitions
				if (self->transitions[i][j].size == 0) {
					fa_add_transition(self, i, j+'a', self->state_count-1);
				}
				size_t capacity = self->transitions[i][j].capacity;
				size_t size = self->transitions[i][j].size;
				transitions[i][j].states = calloc(capacity, sizeof(size_t));
				transitions[i][j].size = size;
				transitions[i][j].capacity = capacity;
				memcpy(transitions[i][j].states, self->transitions[i][j].states, size*sizeof(size_t));
				free(self->transitions[i][j].states);
			}
		}
		free(self->transitions[i]);
	}
	free(self->transitions);
	self->transitions = transitions;
}

void fa_dot_print(const struct fa *self, FILE *out) {
	fprintf(out, "%s\n", "digraph automate_fini");
  	fprintf(out, "%s\n", "{");

  	fprintf(out, "%s\n", "rankdir = LR;");
  	fprintf(out, "%s\n", "size = \"9\";");

  	fprintf(out, "%s\n", "node [shape = point, color = white, fontcolor = white];start;");
	
	for (size_t i=0; i<self->state_count; i++) {
		if (self->states[i].is_final == true) {
			fprintf(out, "%s%zu%s\n", "node [shape = doublecircle, color = black, fontcolor = black];",i,";");
		}
	}

	for (size_t i=0; i<self->state_count; i++) {
		if (self->states[i].is_final==false) {
      		fprintf(out, "%s%zu%s\n", "node [shape = circle, color = black, fontcolor = black];",i,";");
    	}
    	if	(self->states[i].is_initial==true) {
      		fprintf(out, "%s%zu%s\n", "start -> ",i,";");
    	}
	}

	for (size_t i=0; i<self->state_count; i++) {
		for (size_t j=0; j<self->alpha_count; j++) {
			for (size_t k=0; k<self->transitions[i][j].size; k++) {
				fprintf(out, "%zu%s%zu%s%c%s\n", i , " -> " , self->transitions[i][j].states[k] , " [label = " , (unsigned char)j+'a' , "];");
			}
		}
	}
	fprintf(out,"}\n");
}

/********************************************* EXERCICE 4 *********************************************** */

bool fa_is_language_empty(const struct fa *self) {
	for (size_t i=0; i<self->state_count; i++) {
		if (self->states[i].is_final && self->states[i].is_initial) {
			return true;
		}
	}
	return false;
}

/********************************************* EXERCICE 5 *********************************************** */

void fa_remove_non_accessible_states(struct fa *self) {
	size_t state_count = self->state_count;
	struct graph *graph = malloc(sizeof(struct graph));
	graph->state_count = state_count;
	graph_create_from_fa(graph, self, false);
	bool *accessible = calloc(state_count, sizeof(bool));
	for (size_t i=0; i<state_count; i++) {
		accessible[i] = false;
	}

	/* search for all the non accessible states */
	for (size_t i=0; i<state_count; i++) {
		for (size_t j=0; j<state_count; j++) {
			if (self->states[j].is_initial == true) {
				if (graph_has_path(graph, j, i) == true) {
					accessible[i] = true;
				}
			}
		}
		if (self->states[i].is_initial == true) {
			accessible[i] = true;
		}
	}

	/* count the number of non accessible states */
	size_t non_accessible_count = 0;
	for (size_t i=0; i<state_count; i++) {
		if (accessible[i] == false) {
			non_accessible_count++;
		}
		printf("state %d is %s\n", (int)i, accessible[i] ? "accessible" : "not accessible");
	}

	/* remove all the non accessible states */
	size_t index = 0;
	while (non_accessible_count > 0) {
		if (accessible[index] == false) {
			fa_remove_state(self, index);
			non_accessible_count--;
		}else {
			index++;
		}
	}

	graph_destroy(graph);
	free(accessible);
}

void fa_remove_non_co_accessible_states(struct fa *self) {
	size_t state_count = self->state_count;
	struct graph *graph = malloc(sizeof(struct graph));
	graph->state_count = state_count;
	graph_create_from_fa(graph, self, false);

	/*  */
	bool *co_accessible = calloc(state_count, sizeof(bool));
	for (size_t i=0; i<state_count; i++) {
		co_accessible[i] = false;
	}

	/* search for all the non co-accessible states */
	for (size_t i=0; i<state_count; i++) {
		for (size_t j=0; j<state_count; j++) {
			if (self->states[j].is_final == true) {
				if (graph_has_path(graph, i, j) == true) {
					co_accessible[i] = true;
				}
			}
		}
		if (self->states[i].is_final == true) {
			co_accessible[i] = true;
		}
	}

	/* count the number of non co-accessible states */
	size_t non_co_accessible_count = 0;
	for (size_t i=0; i<state_count; i++) {
		if (co_accessible[i] == false) {
			non_co_accessible_count++;
		}
		printf("state %d is %s\n", (int)i, co_accessible[i] ? "co-accessible" : "non co-accessible");
	}

	/* remove all the non co-accessible states */
	size_t index = 0;
	while (non_co_accessible_count > 0) {
		if (co_accessible[index] == false) {
			fa_remove_state(self, index);
			non_co_accessible_count--;
		}else {
			index++;
		}
	}

	free(co_accessible);
	graph_destroy(graph);
}

/********************************************* EXERCICE 6 *********************************************** */

void fa_create_product(struct fa *self, const struct fa *lhs, const struct fa *rhs) {
	if ((fa_is_deterministic(lhs) == false) || (fa_is_deterministic(rhs) == false)) {
		fprintf(stderr, "Error: one or two of the two states isn`t deterministic.\n");
		return;
	}
	if ((fa_is_complete(lhs) == false) || (fa_is_complete(rhs) == false)) {
		fprintf(stderr, "Error: one or two of the two states isn`t complete.\n");
		return;
	}

	size_t lhs_state_count = lhs->state_count;
	size_t rhs_state_count = rhs->state_count;
	size_t product_state_count = lhs_state_count * rhs_state_count;
	size_t product_alpha_count;
	if (lhs->alpha_count <= rhs->alpha_count) {
		product_alpha_count = lhs->alpha_count;
	}else {
		product_alpha_count = rhs->alpha_count;
	}

	size_t biggest_state_count;	/* the biggest number of states of the two automatas */
	bool lhs_is_bigger = false;	/* lhs has bigger number of states than rhs */
	if (lhs_state_count > rhs_state_count) {
		biggest_state_count = lhs_state_count;
		lhs_is_bigger = true;
	}else {
		biggest_state_count = rhs_state_count; 
	}

	fa_create(self, product_alpha_count, product_state_count);

	for (size_t i=0; i<product_state_count; i++) {
		/* set the initial and final states */
		if (lhs_is_bigger == true) {	/* if lhs has bigger number of states than rhs */
			if ((rhs->states[i/biggest_state_count].is_initial == true) && (lhs->states[i%biggest_state_count].is_initial == true)) {
				fa_set_state_initial(self, i);
			}
			if ((rhs->states[i/biggest_state_count].is_final == true) || (lhs->states[i%biggest_state_count].is_final == true)) {
				fa_set_state_final(self, i);
			}
		}else {		/* if rhs has bigger number of states than lhs */
			if ((lhs->states[i/biggest_state_count].is_initial == true) && (rhs->states[i%biggest_state_count].is_initial == true)) {
				fa_set_state_initial(self, i);
			}
			if ((lhs->states[i/biggest_state_count].is_final == true) || (rhs->states[i%biggest_state_count].is_final == true)) {
				fa_set_state_final(self, i);
			}
		}
		/* transitions */
		for (size_t j=0; j<product_alpha_count; j++) {
			size_t destination;
			if (lhs_is_bigger == true) { 	/* if lhs has bigger number of states than rhs */
				destination = rhs->transitions[i/biggest_state_count][j].states[0] * biggest_state_count + lhs->transitions[i%biggest_state_count][j].states[0];
			}else { 	/* if rhs has bigger number of states than lhs */
				destination = lhs->transitions[i/biggest_state_count][j].states[0] * biggest_state_count + rhs->transitions[i%biggest_state_count][j].states[0];
			}
			
			fa_add_transition(self, i, j+'a', destination);
		}
	}
}

bool fa_has_empty_intersection(const struct fa *lhs, const struct fa *rhs) {
	struct fa *product = malloc(sizeof(struct fa));
	fa_create_product(product, lhs, rhs);
	bool is_empty = fa_is_language_empty(product);
	fa_destroy(product);
	return is_empty;
}

void fa_merge_states(struct fa *self, size_t s1, size_t s2) {
	if (NULL == self) {
		fprintf(stderr, "Automate doesn't exist. \n");
		exit(EXIT_FAILURE);
	}
	if (s1 >= self->state_count) {
		fa_destroy(self);
		fprintf(stderr, "Merge isn't possible because state %d doesn't exist. \n", (int)s1);
		exit(EXIT_FAILURE);
	}
	if (s2 >= self->state_count) {
		fa_destroy(self);
		fprintf(stderr, "Merge isn't possible because state %d doesn't exist. \n", (int)s2);
		exit(EXIT_FAILURE);
	}
	if (self->states[s2].is_initial == true) {
		self->states[s1].is_initial = true;
	}
	if (self->states[s2].is_final == true) {
		self->states[s1].is_final = true;
	}

	for (size_t i=0; i<self->state_count; i++) {
		for (size_t j=0; j<self->alpha_count; j++) {
			for (size_t k=0; k<self->transitions[i][j].size; k++) {
				if (self->transitions[i][j].states[k] == s2) {
					fa_remove_transition(self, i, j+'a', s2);
					fa_add_transition(self, i, j+'a', s1);
				}
				if (i == s2) {
					fa_add_transition(self, s1, j+'a', self->transitions[i][j].states[k]);
				}
			}
		}
	}
	fa_remove_state(self, s2);
}

/********************************************* EXERCICE 7 *********************************************** */
void print_table_correspondance(struct table_correspondance *tc) {
	printf("DETERMINATION: \n");
	for (size_t i=0; i<tc->state_count; i++) {
		printf("%zu : [", i);
		for (size_t j=0; j<tc->composing_states[i].size; j++) {
			printf("%zu ", tc->composing_states[i].states[j]);
		}
		printf("]");

		for (size_t j=0; j<tc->alpha_count; j++) {
			printf("\t\t{");
			for (size_t k=0; k<tc->transitions[i][j].size; k++) {
				printf("%zu ", tc->transitions[i][j].states[k]);
			}
			printf("}");
		}
		printf("\n");
	}
}

void create_table_correspondance(struct table_correspondance *tc, const struct fa *fa) {
	tc->alpha_count = fa->alpha_count;
	tc->state_count = 0;
	tc->capacity = 4;
	tc->trans_count = 0;
	tc->composing_states = calloc(tc->capacity, sizeof(struct state_set));
	for (size_t i=0; i<tc->capacity; i++) {
		tc->composing_states[i].size = 0;
		tc->composing_states[i].capacity = 2;
		tc->composing_states[i].states = calloc(tc->composing_states[i].capacity, sizeof(size_t));
	}
	tc->transitions = calloc(tc->capacity, sizeof(struct state_set));
	for (size_t i=0; i<tc->capacity; i++) {
		tc->transitions[i] = calloc(tc->alpha_count, sizeof(struct state_set));
		for (size_t j=0; j<tc->alpha_count; j++) {
			tc->transitions[i][j].size = 0;
			tc->transitions[i][j].capacity = 2;
			tc->transitions[i][j].states = calloc(tc->transitions[i][j].capacity, sizeof(size_t));
		}
	}
}

void tc_destroy(struct table_correspondance *tc) {
	for (size_t i=0; i<tc->capacity; i++) {
		free(tc->composing_states[i].states);
	}
	free(tc->composing_states);

	for (size_t i=0; i<tc->capacity; i++) {
		for (size_t j=0; j<tc->alpha_count; j++) {
			free(tc->transitions[i][j].states);
		}
		free(tc->transitions[i]);
	}
	free(tc->transitions);
} 

void tc_add_capacity(struct table_correspondance *tc) {
	size_t old_capacity = tc->capacity;
	tc->capacity = tc->capacity + 3;
	struct state_set *composing_states_tmp = calloc(tc->capacity, sizeof(struct state_set));
	for (size_t i=0; i<tc->capacity; i++) {
		if (i < old_capacity) {
			composing_states_tmp[i].size = tc->composing_states[i].size;
			composing_states_tmp[i].capacity = tc->composing_states[i].capacity;
			composing_states_tmp[i].states = calloc(composing_states_tmp[i].capacity, sizeof(size_t));
			memcpy(composing_states_tmp[i].states, tc->composing_states[i].states, composing_states_tmp[i].size*sizeof(size_t));
			free(tc->composing_states[i].states);
		} else {
			composing_states_tmp[i].size = 0;
			composing_states_tmp[i].capacity = 3;
			composing_states_tmp[i].states = calloc(composing_states_tmp[i].capacity, sizeof(size_t));
		}
	}
	free(tc->composing_states);
	tc->composing_states = composing_states_tmp;

	struct state_set **transitions_temp = calloc(tc->capacity, sizeof(struct state_set));
	for (size_t i=0; i<tc->capacity; i++) {
		transitions_temp[i] = calloc(tc->alpha_count, sizeof(struct state_set));
		for (size_t j=0; j<tc->alpha_count; j++) {
			if (i < old_capacity) {
				transitions_temp[i][j].size = tc->transitions[i][j].size;
				transitions_temp[i][j].capacity = tc->transitions[i][j].capacity;
				transitions_temp[i][j].states = calloc(transitions_temp[i][j].capacity, sizeof(size_t));
				memcpy(transitions_temp[i][j].states, tc->transitions[i][j].states, transitions_temp[i][j].size*sizeof(size_t));
				free(tc->transitions[i][j].states);
			} else {
				transitions_temp[i][j].size = 0;
				transitions_temp[i][j].capacity = 3;
				transitions_temp[i][j].states = calloc(transitions_temp[i][j].capacity, sizeof(size_t));
			}
		}
		free(tc->transitions[i]);
	}
	free(tc->transitions);
	tc->transitions = transitions_temp;
}

bool composing_states_exist(struct table_correspondance *tc, struct state_set *new_states) {
	bool exist = false;
	for (size_t i=0; i<tc->state_count; i++) {
		if (tc->composing_states[i].size == new_states->size) {
			size_t n = 0;
			for (size_t j=0; j<tc->composing_states[i].size; j++) {
				if (tc->composing_states[i].states[j] == new_states->states[j]) {
					n = n + 1;
				}
			}
			if (n == tc->composing_states[i].size) {
				exist = true;
				break;
			}
		}
	}
	return exist;
}

void add_state_to_state_set(struct state_set *state_set, size_t state) {
	for (size_t i = 0; i < state_set->size; i++) {
		if (state_set->states[i] == state) {
			return;
		}
	}

	size_t current_size = state_set->size;
	size_t current_capacity = state_set->capacity;

	if (current_size == current_capacity) {
		size_t new_capacity = current_capacity + 2;
		state_set->capacity = new_capacity;
		size_t *temp = calloc(new_capacity, sizeof(size_t));
		memcpy(temp, state_set->states, current_capacity*sizeof(size_t));
		free(state_set->states);
		state_set->states = temp;
	}
	state_set->states[current_size] = state;
	state_set->size = state_set->size + 1;

	for (size_t i=0; i<state_set->size; i++) {
		for (size_t j=state_set->size-1; j>0; j--) {
			if (state_set->states[j] < state_set->states[j-1]) {
				size_t tmp = state_set->states[j];
				state_set->states[j] = state_set->states[j-1];
				state_set->states[j-1] = tmp;
			}
		}
	}
}

size_t search_state_index(struct table_correspondance *tc, struct state_set *state_set) {
	for (size_t i=0; i<tc->state_count; i++) {
		if (tc->composing_states[i].size == state_set->size) {
			size_t n = 0;
			for (size_t j=0; j<tc->composing_states[i].size; j++) {
				if (tc->composing_states[i].states[j] == state_set->states[j]) {
					n = n + 1;
				}
			}
			if (n == tc->composing_states[i].size) {
				return i;
			}
		}
	}
	fprintf(stderr, "Error : the determination is either incomplete or incorrect.\n");
	tc_destroy(tc);
	exit(EXIT_FAILURE);
}

void fa_create_deterministic(struct fa *self, const struct fa *nfa) {
	struct table_correspondance *tc = malloc(sizeof(struct table_correspondance));
	create_table_correspondance(tc, nfa);
	//tc_add_capacity(tc);
	// create the first composing states (inital state)
	for (size_t i=0; i<nfa->state_count; i++) {
		if (nfa->states[i].is_initial == true) {
			add_state_to_state_set(&tc->composing_states[0], i);
		}	
	}
	tc->state_count = tc->state_count + 1;

	// create the table correspondence
	size_t index = 0;
	bool cont = true;
	while (cont) {
		for (size_t i=0; i<tc->composing_states[index].size; i++) {
			size_t from_state = tc->composing_states[index].states[i];
			for (size_t j=0; j<tc->alpha_count; j++) {
				for (size_t k=0; k<nfa->transitions[from_state][j].size; k++) {
					size_t state = nfa->transitions[from_state][j].states[k];
					add_state_to_state_set(&tc->transitions[index][j], state);
				}
			}
		}
		tc->trans_count = tc->trans_count + 1;
		
		for (size_t l=0; l<tc->alpha_count; l++) {
			if (composing_states_exist(tc, &tc->transitions[index][l]) == false) {
				for (size_t m=0; m<tc->transitions[index][l].size; m++) {
					size_t state = tc->transitions[index][l].states[m];
					if (tc->state_count == tc->capacity) {
						tc_add_capacity(tc);
					}
					add_state_to_state_set(&tc->composing_states[tc->state_count], state);
				}
				tc->state_count = tc->state_count + 1;
			}
		}

		if (tc->state_count == tc->trans_count) {
			cont = false;
		}
		index++;
		
	}
	
	// create the automata
	fa_create(self, tc->alpha_count, tc->state_count);
	fa_set_state_initial(self, 0);
	for (size_t i=0; i<tc->state_count; i++) {
		for (size_t j=0; j<tc->alpha_count; j++) {
			size_t dest = search_state_index(tc, &tc->transitions[i][j]);
			fa_add_transition(self, i, j+'a', dest);
			for (size_t k=0; k<tc->transitions[i][j].size; k++) {
				if (nfa->states[tc->transitions[i][j].states[k]].is_final == true) {
					fa_set_state_final(self, dest);
				}
			}
		}
	}

	print_table_correspondance(tc);
	tc_destroy(tc);
}

/********************************************* EXERCICE 8 *********************************************** */

void minimisation_print_table(struct dfa_minimisation *minimisation) {
	printf("\t");
	for (size_t i=0; i<minimisation->state_count; i++) {
		printf("%zu\t", i);
	}
	printf("\n");
	for (size_t i=0; i<minimisation->congruence_count; i++) {
		printf("c:%zu\t", i);
		for (size_t j=0; j<minimisation->state_count; j++) {
			printf("%zu\t", minimisation->congruence_table[i].congruence[j]);
		}
		printf("\n");
		for (size_t j=0; j<minimisation->alpha_count; j++) {
			printf("%c\t", (unsigned char)j+'a');
			for (size_t k=0; k<minimisation->state_count; k++) {
				printf("%zu\t", minimisation->transitions[i].destination[j][k]);
			}
			printf("\n");
		}
	}
	printf("\n");
}

void minimisation_table_create(struct dfa_minimisation *minimisation, const struct fa *fa) {
	minimisation->state_count = fa->state_count;
	minimisation->alpha_count = fa->alpha_count;
	minimisation->congruence_count = 0;
	minimisation->trans_count = 0;
	minimisation->trans_capacity = 8;
	minimisation->congruence_capacity = 8;
	minimisation->congruence_table = calloc(minimisation->congruence_capacity, sizeof(struct congruence_table));
	for (size_t i=0; i<minimisation->congruence_capacity; i++) {
		minimisation->congruence_table[i].congruence = calloc(minimisation->state_count, sizeof(size_t));
	}
	minimisation->transitions = calloc(minimisation->trans_capacity, sizeof(struct transitions));
	for (size_t i=0; i<minimisation->trans_capacity; i++) {
		minimisation->transitions[i].destination = calloc(minimisation->alpha_count, sizeof(size_t));
		for (size_t j=0; j<minimisation->alpha_count; j++) {
			minimisation->transitions[i].destination[j] = calloc(minimisation->state_count, sizeof(size_t));
		}
	}
} 

void fa_create_minimal_moore(struct fa *self, const struct fa *other) {
	struct dfa_minimisation *minimisation = malloc(sizeof (struct dfa_minimisation));
	minimisation_table_create(minimisation, other);

	// 1st congruence
	for (size_t i=0; i<minimisation->state_count; i++) {
		if (other->states[i].is_final) {
			minimisation->congruence_table[0].congruence[i] = 2;
		} else {
			minimisation->congruence_table[0].congruence[i] = 1;
		}
	}
	minimisation->congruence_count = minimisation->congruence_count + 1;

	size_t index = 0;
	while (index < 1) {
		for (size_t i=0; i<minimisation->alpha_count; i++) {
			for (size_t j=0; j<minimisation->state_count; j++) {
				size_t dest_state = other->transitions[j][i].states[0];
				minimisation->transitions[minimisation->trans_count].destination[i][j] = minimisation->congruence_table[index].congruence[dest_state];
			}
		}
		
		// new congruence table
		size_t congruence_index = 1;
		size_t size = 1;
		minimisation->congruence_table[minimisation->congruence_count].congruence[0] = 1;
		for (size_t i=1; i<minimisation->state_count; i++) {
			bool same = true;
			for (size_t j=0; j<minimisation->alpha_count; j++) {
				for (size_t k=0; k<size; k++) {
					if (minimisation->congruence_table[index].congruence[k] != minimisation->congruence_table[index].congruence[i]) {
						same = false;
					}
					if (minimisation->transitions[index].destination[j][k] != minimisation->transitions[index].destination[j][i]) {
						same = false;
					}
				}
				
			}
		}
		index++;
	}
	minimisation_print_table(minimisation);
}

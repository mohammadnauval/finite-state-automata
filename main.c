#include "graph.h"
#include "fa.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

int main() {
	/* ------------------------------ TEST FOR THE DETERMINATION ---------------------------------- */
	// create the non-deterministic automata
	/*struct fa *nfa = malloc(sizeof(struct fa));
	// automata 1
	fa_create(nfa, 2, 5);
	fa_set_state_initial(nfa, 0);
	fa_set_state_final(nfa, 4);
	fa_add_transition(nfa, 0, 'b', 1);
	fa_add_transition(nfa, 0, 'a', 2);
	fa_add_transition(nfa, 0, 'b', 2);
	fa_add_transition(nfa, 0, 'a', 3);
	fa_add_transition(nfa, 1, 'b', 4);
	fa_add_transition(nfa, 2, 'a', 2);
	fa_add_transition(nfa, 2, 'b', 2);
	fa_add_transition(nfa, 2, 'b', 1);
	fa_add_transition(nfa, 2, 'a', 3);
	fa_add_transition(nfa, 3, 'a', 4);
	fa_add_transition(nfa, 4, 'a', 4);
	fa_add_transition(nfa, 4, 'b', 4);*/

	// automata 2
	/*fa_create(nfa, 2, 4);
	fa_set_state_initial(nfa, 0);
	fa_set_state_final(nfa, 3);
	fa_add_transition(nfa, 0, 'a', 0);
	fa_add_transition(nfa, 0, 'b', 0);
	fa_add_transition(nfa, 0, 'a', 1);
	fa_add_transition(nfa, 1, 'a', 2);
	fa_add_transition(nfa, 1, 'b', 2);
	fa_add_transition(nfa, 2, 'a', 3);
	fa_add_transition(nfa, 2, 'b', 3);

	FILE *f1 = fopen("nfa.dot", "w");
	fa_dot_print(nfa, f1);

	struct fa *fa = malloc(sizeof(struct fa));
	fa_create_deterministic(fa, nfa);

	FILE *f2 = fopen("fa.dot", "w");
	fa_dot_print(fa, f2);

	fa_destroy(nfa);
	fa_destroy(fa);
	fclose(f1);
	fclose(f2);*/

	/* ---------------------------------- TEST FOR MINIMISATION --------------------------------------- */

	struct fa *non_minimised_fa = malloc(sizeof(struct fa));
	fa_create(non_minimised_fa, 2, 6);
	fa_set_state_initial(non_minimised_fa, 0);
	fa_set_state_final(non_minimised_fa, 3);
	fa_set_state_final(non_minimised_fa, 4);
	fa_add_transition(non_minimised_fa, 0, 'a', 1);
	fa_add_transition(non_minimised_fa, 0, 'b', 2);
	fa_add_transition(non_minimised_fa, 1, 'a', 2);
	fa_add_transition(non_minimised_fa, 1, 'b', 3);
	fa_add_transition(non_minimised_fa, 2, 'a', 1);
	fa_add_transition(non_minimised_fa, 2, 'b', 4);
	fa_add_transition(non_minimised_fa, 3, 'a', 4);
	fa_add_transition(non_minimised_fa, 3, 'b', 5);
	fa_add_transition(non_minimised_fa, 4, 'a', 3);
	fa_add_transition(non_minimised_fa, 4, 'b', 5);
	fa_add_transition(non_minimised_fa, 5, 'a', 5);
	fa_add_transition(non_minimised_fa, 5, 'b', 5);

	FILE *f3 = fopen("non_minimised_fa.dot", "w");
	fa_dot_print(non_minimised_fa, f3); 

	struct fa *minimised_fa = malloc(sizeof(struct fa));

	fa_create_minimal_moore(minimised_fa, non_minimised_fa);
}
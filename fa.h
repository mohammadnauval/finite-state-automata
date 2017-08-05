#ifndef __FA_H
#define __FA_H

struct fa {
	size_t alpha_count;
	size_t state_count;
	struct state *states;
	struct state_set **transitions;
};

struct state {
	bool is_initial;
	bool is_final;
};

struct state_set {
	size_t size;
	size_t capacity;
	size_t *states;
};

struct table_correspondance {
	size_t alpha_count;
	size_t state_count;
	size_t trans_count;
	size_t capacity;
	struct state_set *composing_states;
	struct state_set **transitions; 
};

struct dfa_minimisation {
	size_t state_count;
	size_t alpha_count;
	size_t congruence_count;
	size_t trans_count;
	size_t trans_capacity;
	size_t congruence_capacity;
	struct congruence_table *congruence_table;
	struct transitions *transitions;
};

struct congruence_table {
	size_t *congruence;
};

struct transitions {
	size_t **destination;
};

/* EXERCICE 2 */
void fa_create(struct fa *self, size_t alpha_count, size_t state_count);
void fa_destroy(struct fa *self);
void fa_set_state_initial(struct fa *self, size_t state);
void fa_set_state_final(struct fa *self, size_t state);
void fa_add_transition(struct fa *self, size_t from, char alpha, size_t to);
void fa_pretty_print(const struct fa *self, FILE *out);

/* EXERCICE 3 */
void fa_remove_transition(struct fa *self, size_t from, char alpha, size_t to);
void fa_remove_state(struct fa *self, size_t state);
size_t fa_count_transitions(const struct fa *self);
bool fa_is_deterministic(const struct fa *self);
bool fa_is_complete(const struct fa *self);
void fa_make_complete(struct fa *self);
void fa_dot_print(const struct fa *self, FILE *out);

/* EXERCICE 4 */
bool fa_is_language_empty(const struct fa *self);

/* EXERCICE 5 */
void fa_remove_non_accessible_states(struct fa *self);
void fa_remove_non_co_accessible_states(struct fa *self);

/* EXERCICE 6 */
void fa_create_product(struct fa *self, const struct fa *lhs, const struct fa *rhs);
bool fa_has_empty_intersection(const struct fa *lhs, const struct fa *rhs);
void fa_merge_states(struct fa *self, size_t s1, size_t s2);

/* EXERCICE 7 */
void print_table_correspondance(struct table_correspondance *tc) ;
void create_table_correspondance(struct table_correspondance *tc, const struct fa *fa);
void tc_destroy(struct table_correspondance *tc);
void tc_add_capacity(struct table_correspondance *tc);
bool composing_states_exist(struct table_correspondance *tc, struct state_set *new_states);
void add_state_to_state_set(struct state_set *state_set, size_t state);
size_t search_state_index(struct table_correspondance *tc, struct state_set *state_set);
void fa_create_deterministic(struct fa *self, const struct fa *nfa);

/* EXERCICE 8 */
void minimisation_print_table(struct dfa_minimisation *minimisation);
void minimisation_table_create(struct dfa_minimisation *minimisation, const struct fa *fa);
void fa_create_minimal_moore(struct fa *self, const struct fa *other);

#endif
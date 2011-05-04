
/*
 * Automatically generated by lexi version 2.0
 */

#ifndef LEXI_GENERATED_HEADER_lexi__INCLUDED
#define LEXI_GENERATED_HEADER_lexi__INCLUDED



	#include <stdio.h>

	typedef struct lex_state * lex_statep;

	struct lex_state *lex_init(FILE *f);
	int lex_nexttoken(struct lex_state *state);
	void lex_free(struct lex_state *state);
	const char *lex_tokbuf(struct lex_state *state);

/*
 * This struct holds state for the lexer; its representation is
 * private, but present here for ease of allocation.
 */
struct lexi_state {
	int (*zone)(struct lexi_state *);

	/*
	 * Lexi's buffer is a simple stack.
	 */
	int buffer[1];
	int buffer_index;
	lex_statep lex_state;
};

/* Read a character */
int lexi_readchar(struct lexi_state *state);

/* Push a character to lexi's buffer */
void lexi_push(struct lexi_state *state, const int c);

/* Pop a character from lexi's buffer */
int lexi_pop(struct lexi_state *state);

/* Flush lexi's buffer */
void lexi_flush(struct lexi_state *state);

#ifndef LEXI_EOF
#define LEXI_EOF -1
#endif

enum lexi_groups {
	lexi_group_alnum = 0x1,
	lexi_group_alpha = 0x2,
	lexi_group_white = 0x4,
	lexi_pattern_regex_white = 0,
	lexi_pattern_string_white = 0,
	lexi_pattern_literal_white = 0,
	lexi_comment_white = 0,
};

/* true if the given character is present in the given group */
int lexi_group(enum lexi_groups group, int c);

/* Identify a token */
int lexi_read_token(struct lexi_state *state);

/* Initialise a lexi_state structure */
void lexi_init(struct lexi_state *state, lex_statep lex_state);



#endif


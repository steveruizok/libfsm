
/*
 * Automatically generated by lexi version 2.0
 */

#include "lexer.h"



	#include <assert.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>

	#include "parser.h"

	#define lexi_unknown_token lex_unknown

	struct lex_state {
		struct lexi_state lexi_state;
		char tokbuf[20];
		FILE *file;
	};

	static void push(struct lex_state *lex_state, int c) {
		size_t l;

		assert(lex_state != NULL);

		l = strlen(lex_state->tokbuf);
		if (l == sizeof lex_state->tokbuf) {
			fprintf(stderr, "token buffer full\n");
			exit(1);
		}

		lex_state->tokbuf[l] = c;
		lex_state->tokbuf[l + 1] = '\0';
	}

	static int lexi_getchar(struct lexi_state *state) {
		int c;

		assert(state != NULL);
		assert(state->lex_state != NULL);
		assert(state->lex_state->file != NULL);

		c = fgetc(state->lex_state->file);
		if (c == EOF) {
			return LEXI_EOF;
		}

		return c;
	}

int lexi_readchar(struct lexi_state *state) {
	if (state->buffer_index) {
		return lexi_pop(state);
	}

	return lexi_getchar(state);
}
void lexi_push(struct lexi_state *state, const int c) {
	state->buffer[state->buffer_index++] = c;
}

int lexi_pop(struct lexi_state *state) {
	return state->buffer[--state->buffer_index];
}

void lexi_flush(struct lexi_state *state) {
	state->buffer_index = 0;
}


/* LOOKUP TABLE */

typedef unsigned char lookup_type;
static lookup_type lookup_tab[] = {
	   0,    0,    0,    0,    0,    0,    0,    0,    0,  0x1,  0x1,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,  0x1,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	 0xa,  0xa,  0xa,  0xa,  0xa,  0xa,  0xa,  0xa,  0xa,  0xa,    0,    0, 
	   0,    0,    0,    0,    0,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6, 
	 0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6, 
	 0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,    0,    0,    0,    0,  0x6, 
	   0,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6, 
	 0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6,  0x6, 
	 0x6,  0x6,  0x6,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, 
	   0,    0,    0,    0
};

int lexi_group(enum lexi_groups group, int c) {
	return lookup_tab[c] & group;
}


/* PRE-PASS ANALYSERS */

void lexi_init(struct lexi_state *state, lex_statep lex_state) {
	state->zone = lexi_read_token;
	state->buffer_index = 0;
	state->lex_state = lex_state;
}
/* ZONES PASS ANALYSER PROTOTYPES*/

static int lexi_read_token_label1(struct lexi_state *state);
static int lexi_read_token_label0(struct lexi_state *state);
static void lexi_read_token_comment(struct lexi_state *state);
/* MAIN PASS ANALYSERS */


/* MAIN PASS ANALYSER for label1 */
static int
lexi_read_token_label1(struct lexi_state *state)
{
	start: {
		int c0 = lexi_readchar(state);
		if (c0 == '\'') {
			return lex_label;
		}

		/* DEFAULT */
		/* ACTION <push> */
		{

	push(state->lex_state, c0);	/* XXX c0 hack */
		}
		/* END ACTION <push> */
		goto start; /* DEFAULT */
	}
}

/* MAIN PASS ANALYSER for label0 */
static int
lexi_read_token_label0(struct lexi_state *state)
{
	start: {
		int c0 = lexi_readchar(state);
		if (c0 == '"') {
			return lex_label;
		}

		/* DEFAULT */
		/* ACTION <push> */
		{

	push(state->lex_state, c0);	/* XXX c0 hack */
		}
		/* END ACTION <push> */
		goto start; /* DEFAULT */
	}
}

/* MAIN PASS ANALYSER for comment */
static void
lexi_read_token_comment(struct lexi_state *state)
{
	start: {
		int c0 = lexi_readchar(state);
		if (c0 == '\n') {
			return;
		}

		/* DEFAULT */
		goto start; /* DEFAULT */
	}
}

/* MAIN PASS ANALYSER for the global zone */
int
lexi_read_token(struct lexi_state *state)
{
	if (state->zone != lexi_read_token)
		return state->zone(state);
	start: {
		int c0 = lexi_readchar(state);
		if (lexi_group(lexi_group_white, c0)) goto start;
		switch (c0) {
		case LEXI_EOF: {
				return lex_eof;
			}

		case '=': {
				return lex_equals;
			}

		case ',': {
				return lex_comma;
			}

		case ';': {
				return lex_sep;
			}

		case '-': {
				int c1 = lexi_readchar(state);
				if (c1 == '>') {
					return lex_arrow;
				}
				lexi_push(state, c1);
			}
			break;

		case 'e': {
				int c1 = lexi_readchar(state);
				if (c1 == 'n') {
					int c2 = lexi_readchar(state);
					if (c2 == 'd') {
						int c3 = lexi_readchar(state);
						if (c3 == ':') {
							return lex_end;
						}
						lexi_push(state, c3);
					}
					lexi_push(state, c2);
				}
				lexi_push(state, c1);
			}
			break;

		case 's': {
				int c1 = lexi_readchar(state);
				if (c1 == 't') {
					int c2 = lexi_readchar(state);
					if (c2 == 'a') {
						int c3 = lexi_readchar(state);
						if (c3 == 'r') {
							int c4 = lexi_readchar(state);
							if (c4 == 't') {
								int c5 = lexi_readchar(state);
								if (c5 == ':') {
									return lex_start;
								}
								lexi_push(state, c5);
							}
							lexi_push(state, c4);
						}
						lexi_push(state, c3);
					}
					lexi_push(state, c2);
				}
				lexi_push(state, c1);
			}
			break;

		case '?': {
				return lex_any;
			}

		case '\'': {
				/* ACTION <flush> */
				{

	state->lex_state->tokbuf[0] = '\0';
				}
				/* END ACTION <flush> */
				return lexi_read_token_label1(state);
			}

		case '"': {
				/* ACTION <flush> */
				{

	state->lex_state->tokbuf[0] = '\0';
				}
				/* END ACTION <flush> */
				return lexi_read_token_label0(state);
			}

		case '#': {
				lexi_read_token_comment(state);
				goto start;	/* pure function */
			}

		}
		if (lexi_group(lexi_group_alnum, c0)) {
			/* ACTION <flush> */
			{

	state->lex_state->tokbuf[0] = '\0';
			}
			/* END ACTION <flush> */
			/* ACTION <read_id> */
			{

	int c;

	/* XXX: c0 hack */
	for (c = c0; lexi_group(lexi_group_alnum, c); c = lexi_readchar(state)) {
		push(state->lex_state, c);
	}

	if (c == LEXI_EOF) {
		return lex_eof;
	}

	lexi_push(state, c);
			}
			/* END ACTION <read_id> */
			return lex_ident;
		}

		/* DEFAULT */
		return lexi_unknown_token;
	}
}


	struct lex_state *lex_init(FILE *f) {
		struct lex_state *new;

		assert(f != NULL);

		new = malloc(sizeof *new);
		if (new == NULL) {
			return NULL;
		}

		new->file = f;

		lexi_init(&new->lexi_state, new);

		return new;
	}

	int lex_nexttoken(struct lex_state *state) {
		assert(state != NULL);

		return lexi_read_token(&state->lexi_state);
	}

	void lex_free(struct lex_state *state) {
		assert(state != NULL);

		free(state);
	}

	const char *lex_tokbuf(struct lex_state *state) {
		assert(state != NULL);

		return state->tokbuf;
	}



/* Generated by lx */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>

#include LX_HEADER

static enum lx_token z1(struct lx *lx);
static enum lx_token z2(struct lx *lx);
static enum lx_token z3(struct lx *lx);
static enum lx_token z4(struct lx *lx);
static enum lx_token z5(struct lx *lx);

static int
lx_getc(struct lx *lx)
{
	int c;

	assert(lx != NULL);
	assert(lx->lgetc != NULL);

	if (lx->c != EOF) {
		c = lx->c, lx->c = EOF;
	} else {
		c = lx->lgetc(lx);
		if (c == EOF) {
			return EOF;
		}
	}

	lx->end.byte++;
	lx->end.col++;

	if (c == '\n') {
		lx->end.line++;
		lx->end.col = 0;
	}

	return c;
}

static void
lx_ungetc(struct lx *lx, int c)
{
	assert(lx != NULL);
	assert(lx->c == EOF);

	lx->c = c;

	if (lx->pop != NULL) {
		lx->pop(lx);
	}

	lx->end.byte--;
	lx->end.col--;

	if (c == '\n') {
		lx->end.line--;
		lx->end.col = 0; /* XXX: lost information */
	}
}

int
lx_fgetc(struct lx *lx)
{
	assert(lx != NULL);
	assert(lx->opaque != NULL);

	return fgetc(lx->opaque);
}

int
lx_sgetc(struct lx *lx)
{
	char *s;

	assert(lx != NULL);
	assert(lx->opaque != NULL);

	s = lx->opaque;
	if (*s == '\0') {
		return EOF;
	}

	return lx->opaque = s + 1, *s;
}

int
lx_agetc(struct lx *lx)
{
	struct lx_arr *a;

	assert(lx != NULL);
	assert(lx->opaque != NULL);

	a = lx->opaque;

	assert(a != NULL);
	assert(a->p != NULL);

	if (a->len == 0) {
		return EOF;
	}

	return a->len--, *a->p++;
}

int
lx_dgetc(struct lx *lx)
{
	struct lx_fd *d;

	assert(lx != NULL);
	assert(lx->opaque != NULL);

	d = lx->opaque;
	assert(d->fd != -1);
	assert(d->p != NULL);

	if (d->len == 0) {
		ssize_t r;

		assert((fcntl(d->fd, F_GETFL) & O_NONBLOCK) == 0);

		d->p = (char *) d + sizeof *d;

		r = read(d->fd, d->p, d->bufsz);
		if (r == -1) {
			assert(errno != EAGAIN);
			return EOF;
		}

		if (r == 0) {
			return EOF;
		}

		d->len = r;
	}

	return d->len--, *d->p++;
}

int
lx_dynpush(struct lx *lx, char c)
{
	struct lx_dynbuf *t;

	assert(lx != NULL);
	assert(c != EOF);

	t = lx->buf;

	assert(t != NULL);

	if (t->p == t->a + t->len) {
		size_t len;
		char *tmp;

		if (t->len == 0) {
			assert(LX_DYN_LOW > 0);
			len = LX_DYN_LOW;
		} else {
			len = t->len * LX_DYN_FACTOR;
			if (len < t->len) {
				errno = ERANGE;
				return -1;
			}
		}

		tmp = realloc(t->a, len);
		if (tmp == NULL) {
			return -1;
		}

		t->p   = tmp + (t->p - t->a);
		t->a   = tmp;
		t->len = len;
	}

	assert(t->p != NULL);
	assert(t->a != NULL);

	*t->p++ = c;

	return 0;
}

void
lx_dynpop(struct lx *lx)
{
	struct lx_dynbuf *t;

	assert(lx != NULL);

	t = lx->buf;

	assert(t != NULL);
	assert(t->a != NULL);
	assert(t->p >= t->a);

	if (t->p == t->a) {
		return;
	}

	t->p--;
}

int
lx_dynclear(struct lx *lx)
{
	struct lx_dynbuf *t;

	assert(lx != NULL);

	t = lx->buf;

	assert(t != NULL);

	if (t->len > LX_DYN_HIGH) {
		size_t len;
		char *tmp;

		len = t->len / LX_DYN_FACTOR;

		tmp = realloc(t->a, len);
		if (tmp == NULL) {
			return -1;
		}

		t->a   = tmp;
		t->len = len;
	}

	t->p = t->a;

	return 0;
}

void
lx_dynfree(struct lx *lx)
{
	struct lx_dynbuf *t;

	assert(lx != NULL);

	t = lx->buf;

	assert(t != NULL);

	free(t->a);
}
int
lx_fixedpush(struct lx *lx, char c)
{
	struct lx_fixedbuf *t;

	assert(lx != NULL);
	assert(c != EOF);

	t = lx->buf;

	assert(t != NULL);
	assert(t->p != NULL);
	assert(t->a != NULL);

	if (t->p == t->a + t->len) {
		errno = ENOMEM;
		return -1;
	}

	*t->p++ = c;

	return 0;
}

void
lx_fixedpop(struct lx *lx)
{
	struct lx_fixedbuf *t;

	assert(lx != NULL);

	t = lx->buf;

	assert(t != NULL);
	assert(t->a != NULL);
	assert(t->p >= t->a);

	if (t->p == t->a) {
		return;
	}

	t->p--;
}

int
lx_fixedclear(struct lx *lx)
{
	struct lx_fixedbuf *t;

	assert(lx != NULL);

	t = lx->buf;

	assert(t != NULL);
	assert(t->p != NULL);
	assert(t->a != NULL);

	t->p = t->a;

	return 0;
}

static enum lx_token
z1(struct lx *lx)
{
	int c;

	enum {
		S1, S2, S3, S4
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx);
	}

	state = S4;

lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		switch (state) {
			break;

		default:
			if (lx->push != NULL) {
				if (-1 == lx->push(lx, c)) {
					return TOK_ERROR;
				}
			}
			break;

		}

		switch (state) {
		case S1:
			switch (c) {
			default:  lx_ungetc(lx, c); return lx->z = z5, TOK_RE;
			}

		case S2:
			switch (c) {
			case 'i': state = S1;      continue;
			case 'r': state = S1;      continue;
			default:  lx_ungetc(lx, c); return lx->z = z5, TOK_RE;
			}

		case S3:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_CHAR;
			}

		case S4:
			switch (c) {
			case '/': state = S2;      continue;
			default:  state = S3;     continue;
			}
		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case S1: return TOK_RE;
	case S2: return TOK_RE;
	case S3: return TOK_CHAR;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

static enum lx_token
z2(struct lx *lx)
{
	int c;

	enum {
		S1, S2, S3, S4, S5
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx);
	}

	state = S5;

lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		switch (state) {
			break;

		default:
			if (lx->push != NULL) {
				if (-1 == lx->push(lx, c)) {
					return TOK_ERROR;
				}
			}
			break;

		}

		switch (state) {
		case S1:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_ESC;
			}

		case S2:
			switch (c) {
			default:  lx_ungetc(lx, c); return lx->z = z5, TOK_STR;
			}

		case S3:
			switch (c) {
			case '\\': state = S1;      continue;
			case 'n': state = S1;      continue;
			case 'r': state = S1;      continue;
			case 't': state = S1;      continue;
			default:  lx->lgetc = NULL; return TOK_UNKNOWN;
			}

		case S4:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_CHAR;
			}

		case S5:
			switch (c) {
			case '\"': state = S2;      continue;
			case '\\': state = S3;      continue;
			default:  state = S4;     continue;
			}
		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case S1: return TOK_ESC;
	case S2: return TOK_STR;
	case S4: return TOK_CHAR;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

static enum lx_token
z3(struct lx *lx)
{
	int c;

	enum {
		S1, S2, S3
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx);
	}

	state = S3;

lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		switch (state) {
			break;

		default:
			if (lx->push != NULL) {
				if (-1 == lx->push(lx, c)) {
					return TOK_ERROR;
				}
			}
			break;

		}

		switch (state) {
		case S1:
			switch (c) {
			default:  lx_ungetc(lx, c); return lx->z = z5, TOK_STR;
			}

		case S2:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_CHAR;
			}

		case S3:
			switch (c) {
			case '\'': state = S1;      continue;
			default:  state = S2;     continue;
			}
		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case S1: return TOK_STR;
	case S2: return TOK_CHAR;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

static enum lx_token
z4(struct lx *lx)
{
	int c;

	enum {
		S1, S2, S3
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx);
	}

	state = S3;

lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		switch (state) {
		case S1:
		case S2:
		case S3:
			break;

		default:
			if (lx->push != NULL) {
				if (-1 == lx->push(lx, c)) {
					return TOK_ERROR;
				}
			}
			break;

		}

		switch (state) {
		case S1:
			switch (c) {
			default:  lx_ungetc(lx, c); return lx->z = z5, lx->z(lx);
			}

		case S2:
			switch (c) {
			default:  lx_ungetc(lx, c); return lx->z(lx);
			}

		case S3:
			switch (c) {
			case '\n': state = S1;      continue;
			default:  state = S2;     continue;
			}
		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case S1: return TOK_EOF;
	case S2: return TOK_EOF;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

static enum lx_token
z5(struct lx *lx)
{
	int c;

	enum {
		S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, 
		S11, S12, S13, S14, S15, S16, S17, S18, S19, S20, 
		S21, S22, S23, S24, S25
	} state;

	assert(lx != NULL);

	if (lx->clear != NULL) {
		lx->clear(lx);
	}

	state = S25;

lx->start = lx->end;

	while (c = lx_getc(lx), c != EOF) {
		switch (state) {
		case S4:
		case S6:
		case S7:
		case S9:
		case S16:
			break;

		default:
			if (lx->push != NULL) {
				if (-1 == lx->push(lx, c)) {
					return TOK_ERROR;
				}
			}
			break;

		}

		switch (state) {
		case S1:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_TO;
			}

		case S2:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_MAP;
			}

		case S3:
			switch (c) {
			case '0':	          continue;
			case '1':	          continue;
			case '2':	          continue;
			case '3':	          continue;
			case '4':	          continue;
			case '5':	          continue;
			case '6':	          continue;
			case '7':	          continue;
			case '8':	          continue;
			case '9':	          continue;
			case 'A':	          continue;
			case 'B':	          continue;
			case 'C':	          continue;
			case 'D':	          continue;
			case 'E':	          continue;
			case 'F':	          continue;
			case 'G':	          continue;
			case 'H':	          continue;
			case 'I':	          continue;
			case 'J':	          continue;
			case 'K':	          continue;
			case 'L':	          continue;
			case 'M':	          continue;
			case 'N':	          continue;
			case 'O':	          continue;
			case 'P':	          continue;
			case 'Q':	          continue;
			case 'R':	          continue;
			case 'S':	          continue;
			case 'T':	          continue;
			case 'U':	          continue;
			case 'V':	          continue;
			case 'W':	          continue;
			case 'X':	          continue;
			case 'Y':	          continue;
			case 'Z':	          continue;
			case '_':	          continue;
			case 'a':	          continue;
			case 'b':	          continue;
			case 'c':	          continue;
			case 'd':	          continue;
			case 'e':	          continue;
			case 'f':	          continue;
			case 'g':	          continue;
			case 'h':	          continue;
			case 'i':	          continue;
			case 'j':	          continue;
			case 'k':	          continue;
			case 'l':	          continue;
			case 'm':	          continue;
			case 'n':	          continue;
			case 'o':	          continue;
			case 'p':	          continue;
			case 'q':	          continue;
			case 'r':	          continue;
			case 's':	          continue;
			case 't':	          continue;
			case 'u':	          continue;
			case 'v':	          continue;
			case 'w':	          continue;
			case 'x':	          continue;
			case 'y':	          continue;
			case 'z':	          continue;
			default:  lx_ungetc(lx, c); return TOK_TOKEN;
			}

		case S4:
			switch (c) {
			case '\t':	          continue;
			case '\n':	          continue;
			case '\r':	          continue;
			case ' ':	          continue;
			default:  lx_ungetc(lx, c); return lx->z(lx);
			}

		case S5:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_BANG;
			}

		case S6:
			switch (c) {
			default:  lx_ungetc(lx, c); return lx->z = z2, lx->z(lx);
			}

		case S7:
			switch (c) {
			default:  lx_ungetc(lx, c); return lx->z = z4, lx->z(lx);
			}

		case S8:
			switch (c) {
			case 'A': state = S3;      continue;
			case 'B': state = S3;      continue;
			case 'C': state = S3;      continue;
			case 'D': state = S3;      continue;
			case 'E': state = S3;      continue;
			case 'F': state = S3;      continue;
			case 'G': state = S3;      continue;
			case 'H': state = S3;      continue;
			case 'I': state = S3;      continue;
			case 'J': state = S3;      continue;
			case 'K': state = S3;      continue;
			case 'L': state = S3;      continue;
			case 'M': state = S3;      continue;
			case 'N': state = S3;      continue;
			case 'O': state = S3;      continue;
			case 'P': state = S3;      continue;
			case 'Q': state = S3;      continue;
			case 'R': state = S3;      continue;
			case 'S': state = S3;      continue;
			case 'T': state = S3;      continue;
			case 'U': state = S3;      continue;
			case 'V': state = S3;      continue;
			case 'W': state = S3;      continue;
			case 'X': state = S3;      continue;
			case 'Y': state = S3;      continue;
			case 'Z': state = S3;      continue;
			case '_': state = S3;      continue;
			case 'a': state = S3;      continue;
			case 'b': state = S3;      continue;
			case 'c': state = S3;      continue;
			case 'd': state = S3;      continue;
			case 'e': state = S3;      continue;
			case 'f': state = S3;      continue;
			case 'g': state = S3;      continue;
			case 'h': state = S3;      continue;
			case 'i': state = S3;      continue;
			case 'j': state = S3;      continue;
			case 'k': state = S3;      continue;
			case 'l': state = S3;      continue;
			case 'm': state = S3;      continue;
			case 'n': state = S3;      continue;
			case 'o': state = S3;      continue;
			case 'p': state = S3;      continue;
			case 'q': state = S3;      continue;
			case 'r': state = S3;      continue;
			case 's': state = S3;      continue;
			case 't': state = S3;      continue;
			case 'u': state = S3;      continue;
			case 'v': state = S3;      continue;
			case 'w': state = S3;      continue;
			case 'x': state = S3;      continue;
			case 'y': state = S3;      continue;
			case 'z': state = S3;      continue;
			default:  lx->lgetc = NULL; return TOK_UNKNOWN;
			}

		case S9:
			switch (c) {
			default:  lx_ungetc(lx, c); return lx->z = z3, lx->z(lx);
			}

		case S10:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_LPAREN;
			}

		case S11:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_RPAREN;
			}

		case S12:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_STAR;
			}

		case S13:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_CROSS;
			}

		case S14:
			switch (c) {
			case '>': state = S2;      continue;
			default:  lx_ungetc(lx, c); return TOK_DASH;
			}

		case S15:
			switch (c) {
			case '.': state = S1;      continue;
			default:  lx_ungetc(lx, c); return TOK_DOT;
			}

		case S16:
			switch (c) {
			default:  lx_ungetc(lx, c); return lx->z = z1, lx->z(lx);
			}

		case S17:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_SEMI;
			}

		case S18:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_BIND;
			}

		case S19:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_QMARK;
			}

		case S20:
			switch (c) {
			case '0':	          continue;
			case '1':	          continue;
			case '2':	          continue;
			case '3':	          continue;
			case '4':	          continue;
			case '5':	          continue;
			case '6':	          continue;
			case '7':	          continue;
			case '8':	          continue;
			case '9':	          continue;
			case 'A':	          continue;
			case 'B':	          continue;
			case 'C':	          continue;
			case 'D':	          continue;
			case 'E':	          continue;
			case 'F':	          continue;
			case 'G':	          continue;
			case 'H':	          continue;
			case 'I':	          continue;
			case 'J':	          continue;
			case 'K':	          continue;
			case 'L':	          continue;
			case 'M':	          continue;
			case 'N':	          continue;
			case 'O':	          continue;
			case 'P':	          continue;
			case 'Q':	          continue;
			case 'R':	          continue;
			case 'S':	          continue;
			case 'T':	          continue;
			case 'U':	          continue;
			case 'V':	          continue;
			case 'W':	          continue;
			case 'X':	          continue;
			case 'Y':	          continue;
			case 'Z':	          continue;
			case '_':	          continue;
			case 'a':	          continue;
			case 'b':	          continue;
			case 'c':	          continue;
			case 'd':	          continue;
			case 'e':	          continue;
			case 'f':	          continue;
			case 'g':	          continue;
			case 'h':	          continue;
			case 'i':	          continue;
			case 'j':	          continue;
			case 'k':	          continue;
			case 'l':	          continue;
			case 'm':	          continue;
			case 'n':	          continue;
			case 'o':	          continue;
			case 'p':	          continue;
			case 'q':	          continue;
			case 'r':	          continue;
			case 's':	          continue;
			case 't':	          continue;
			case 'u':	          continue;
			case 'v':	          continue;
			case 'w':	          continue;
			case 'x':	          continue;
			case 'y':	          continue;
			case 'z':	          continue;
			default:  lx_ungetc(lx, c); return TOK_IDENT;
			}

		case S21:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_OPEN;
			}

		case S22:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_PIPE;
			}

		case S23:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_CLOSE;
			}

		case S24:
			switch (c) {
			default:  lx_ungetc(lx, c); return TOK_TILDE;
			}

		case S25:
			switch (c) {
			case '\t': state = S4;      continue;
			case '\n': state = S4;      continue;
			case '\r': state = S4;      continue;
			case ' ': state = S4;      continue;
			case '!': state = S5;      continue;
			case '\"': state = S6;      continue;
			case '#': state = S7;      continue;
			case '$': state = S8;      continue;
			case '\'': state = S9;      continue;
			case '(': state = S10;      continue;
			case ')': state = S11;      continue;
			case '*': state = S12;      continue;
			case '+': state = S13;      continue;
			case '-': state = S14;      continue;
			case '.': state = S15;      continue;
			case '/': state = S16;      continue;
			case ';': state = S17;      continue;
			case '=': state = S18;      continue;
			case '?': state = S19;      continue;
			case 'A': state = S20;      continue;
			case 'B': state = S20;      continue;
			case 'C': state = S20;      continue;
			case 'D': state = S20;      continue;
			case 'E': state = S20;      continue;
			case 'F': state = S20;      continue;
			case 'G': state = S20;      continue;
			case 'H': state = S20;      continue;
			case 'I': state = S20;      continue;
			case 'J': state = S20;      continue;
			case 'K': state = S20;      continue;
			case 'L': state = S20;      continue;
			case 'M': state = S20;      continue;
			case 'N': state = S20;      continue;
			case 'O': state = S20;      continue;
			case 'P': state = S20;      continue;
			case 'Q': state = S20;      continue;
			case 'R': state = S20;      continue;
			case 'S': state = S20;      continue;
			case 'T': state = S20;      continue;
			case 'U': state = S20;      continue;
			case 'V': state = S20;      continue;
			case 'W': state = S20;      continue;
			case 'X': state = S20;      continue;
			case 'Y': state = S20;      continue;
			case 'Z': state = S20;      continue;
			case '_': state = S20;      continue;
			case 'a': state = S20;      continue;
			case 'b': state = S20;      continue;
			case 'c': state = S20;      continue;
			case 'd': state = S20;      continue;
			case 'e': state = S20;      continue;
			case 'f': state = S20;      continue;
			case 'g': state = S20;      continue;
			case 'h': state = S20;      continue;
			case 'i': state = S20;      continue;
			case 'j': state = S20;      continue;
			case 'k': state = S20;      continue;
			case 'l': state = S20;      continue;
			case 'm': state = S20;      continue;
			case 'n': state = S20;      continue;
			case 'o': state = S20;      continue;
			case 'p': state = S20;      continue;
			case 'q': state = S20;      continue;
			case 'r': state = S20;      continue;
			case 's': state = S20;      continue;
			case 't': state = S20;      continue;
			case 'u': state = S20;      continue;
			case 'v': state = S20;      continue;
			case 'w': state = S20;      continue;
			case 'x': state = S20;      continue;
			case 'y': state = S20;      continue;
			case 'z': state = S20;      continue;
			case '{': state = S21;      continue;
			case '|': state = S22;      continue;
			case '}': state = S23;      continue;
			case '~': state = S24;      continue;
			default:  lx->lgetc = NULL; return TOK_UNKNOWN;
			}
		}
	}

	lx->lgetc = NULL;

	switch (state) {
	case S1: return TOK_TO;
	case S2: return TOK_MAP;
	case S3: return TOK_TOKEN;
	case S4: return TOK_EOF;
	case S5: return TOK_BANG;
	case S6: return TOK_EOF;
	case S7: return TOK_EOF;
	case S9: return TOK_EOF;
	case S10: return TOK_LPAREN;
	case S11: return TOK_RPAREN;
	case S12: return TOK_STAR;
	case S13: return TOK_CROSS;
	case S14: return TOK_DASH;
	case S15: return TOK_DOT;
	case S16: return TOK_EOF;
	case S17: return TOK_SEMI;
	case S18: return TOK_BIND;
	case S19: return TOK_QMARK;
	case S20: return TOK_IDENT;
	case S21: return TOK_OPEN;
	case S22: return TOK_PIPE;
	case S23: return TOK_CLOSE;
	case S24: return TOK_TILDE;
	default: errno = EINVAL; return TOK_ERROR;
	}
}

const char *
lx_name(enum lx_token t)
{
	switch (t) {
	case TOK_IDENT: return "IDENT";
	case TOK_TOKEN: return "TOKEN";
	case TOK_PIPE: return "PIPE";
	case TOK_DOT: return "DOT";
	case TOK_BANG: return "BANG";
	case TOK_DASH: return "DASH";
	case TOK_TILDE: return "TILDE";
	case TOK_QMARK: return "QMARK";
	case TOK_CROSS: return "CROSS";
	case TOK_STAR: return "STAR";
	case TOK_RPAREN: return "RPAREN";
	case TOK_LPAREN: return "LPAREN";
	case TOK_CLOSE: return "CLOSE";
	case TOK_OPEN: return "OPEN";
	case TOK_MAP: return "MAP";
	case TOK_TO: return "TO";
	case TOK_SEMI: return "SEMI";
	case TOK_BIND: return "BIND";
	case TOK_RE: return "RE";
	case TOK_ESC: return "ESC";
	case TOK_STR: return "STR";
	case TOK_CHAR: return "CHAR";
	case TOK_EOF:     return "EOF";
	case TOK_ERROR:   return "ERROR";
	case TOK_UNKNOWN: return "UNKNOWN";
	default: return "?";
	}
}

void
lx_init(struct lx *lx)
{
	static const struct lx lx_default;

	assert(lx != NULL);

	*lx = lx_default;

	lx->c = EOF;
	lx->z = NULL;

	lx->end.byte = 0;
	lx->end.line = 1;
	lx->end.col  = 1;
}

enum lx_token
lx_next(struct lx *lx)
{
	enum lx_token t;

	assert(lx != NULL);

	if (lx->lgetc == NULL) {
		return TOK_EOF;
	}

	if (lx->z == NULL) {
		lx->z = z5;
	}

	t = lx->z(lx);

	if (lx->push != NULL) {
		if (-1 == lx->push(lx, '\0')) {
			return TOK_ERROR;
		}
	}

	if (lx->lgetc == NULL && lx->free != NULL) {
		lx->free(lx);
	}

	return t;
}


#include "re_char_class.h"
#include "re_ast.h"

#include <ctype.h>

static void
cc_free(struct re_char_class *cc);

static void
cc_add_byte(struct re_char_class *cc, unsigned char byte);

static void
cc_add_range(struct re_char_class *cc, unsigned char from, unsigned char to);

static void
cc_invert(struct re_char_class *cc);

static void
cc_mask(struct re_char_class *cc, const struct re_char_class *mask);

void
cc_dump(FILE *f, struct re_char_class *cc);

struct re_char_class_ast *
re_char_class_ast_concat(struct re_char_class_ast *l,
    struct re_char_class_ast *r)
{
	struct re_char_class_ast *res = calloc(1, sizeof(*res));
	if (res == NULL) { return NULL; }
	res->t = RE_CHAR_CLASS_AST_CONCAT;
	res->u.concat.l = l;
	res->u.concat.r = r;
	return res;
}

struct re_char_class_ast *
re_char_class_ast_literal(unsigned char c)
{
	struct re_char_class_ast *res = calloc(1, sizeof(*res));
	if (res == NULL) { return NULL; }
	res->t = RE_CHAR_CLASS_AST_LITERAL;
	res->u.literal.c = c;
	return res;
}


struct re_char_class_ast *
re_char_class_ast_range(unsigned char from, unsigned char to)
{
	struct re_char_class_ast *res = calloc(1, sizeof(*res));
	if (res == NULL) { return NULL; }
	res->t = RE_CHAR_CLASS_AST_RANGE;
	res->u.range.from = from;
	res->u.range.to = to;
	return res;
}

struct re_char_class_ast *
re_char_class_ast_flags(enum re_char_class_flags flags)
{
	struct re_char_class_ast *res = calloc(1, sizeof(*res));
	if (res == NULL) { return NULL; }
	res->t = RE_CHAR_CLASS_AST_FLAGS;
	res->u.flags.f = flags;
	return res;
}

struct re_char_class_ast *
re_char_class_ast_subtract(struct re_char_class_ast *ast,
    struct re_char_class_ast *mask)
{
	struct re_char_class_ast *res = calloc(1, sizeof(*res));
	if (res == NULL) { return NULL; }
	res->t = RE_CHAR_CLASS_AST_SUBTRACT;
	res->u.subtract.ast = ast;
	res->u.subtract.mask = mask;
	return res;
}

static void
print_char_or_esc(FILE *f, unsigned char c)
{
	if (isprint(c)) {
		fprintf(f, "%c", c);
	} else {
		fprintf(f, "\\x%02x", c);
	}
}

static void
pp_iter(FILE *f, struct re_char_class_ast *n, size_t indent)
{
	size_t i;
	assert(n != NULL);
	for (i = 0; i < indent; i++) { fprintf(f, " "); }

	switch (n->t) {
	case RE_CHAR_CLASS_AST_CONCAT:
		fprintf(f, "CLASS-CONCAT %p: \n", (void *)n);
		pp_iter(f, n->u.concat.l, indent + 4);
		pp_iter(f, n->u.concat.r, indent + 4);
		break;
	case RE_CHAR_CLASS_AST_LITERAL:
		fprintf(f, "CLASS-LITERAL %p: '", (void *)n);
		print_char_or_esc(f, n->u.literal.c);
		fprintf(f, "'\n");
		break;
	case RE_CHAR_CLASS_AST_RANGE:
		fprintf(f, "CLASS-RANGE %p: '", (void *)n);
		print_char_or_esc(f, n->u.range.from);
		fprintf(f, "'-'");
		print_char_or_esc(f, n->u.range.to);
		fprintf(f, "'\n");
		break;
	case RE_CHAR_CLASS_AST_NAMED:
		fprintf(f, "CLASS-NAMED %p: '%s'\n",
		    (void *)n, n->u.named.name);
		break;
	case RE_CHAR_CLASS_AST_FLAGS:
		fprintf(f, "CLASS-FLAGS %p: [", (void *)n);
		if (n->u.flags.f & RE_CHAR_CLASS_FLAG_INVERTED) { 
			fprintf(f, "^");
		}
		if (n->u.flags.f & RE_CHAR_CLASS_FLAG_MINUS) {
			fprintf(f, "-");
		}
		fprintf(f, "]\n");
		break;
	case RE_CHAR_CLASS_AST_SUBTRACT:
		fprintf(f, "CLASS-SUBTRACT %p: '", (void *)n);
		pp_iter(f, n->u.subtract.ast, indent + 4);
		pp_iter(f, n->u.subtract.mask, indent + 4);
		break;
	default:
		fprintf(stderr, "(MATCH FAIL)\n");
		assert(0);
	}
}

void
re_char_class_ast_prettyprint(FILE *f,
    struct re_char_class_ast *ast, size_t indent)
{
	pp_iter(f, ast, indent);
}

static void
free_iter(struct re_char_class_ast *n)
{
	assert(n != NULL);
	switch (n->t) {
	case RE_CHAR_CLASS_AST_CONCAT:
		free_iter(n->u.concat.l);
		free_iter(n->u.concat.r);
		break;
	case RE_CHAR_CLASS_AST_SUBTRACT:
		free_iter(n->u.subtract.ast);
		free_iter(n->u.subtract.mask);
		break;
	case RE_CHAR_CLASS_AST_LITERAL:
	case RE_CHAR_CLASS_AST_RANGE:
	case RE_CHAR_CLASS_AST_NAMED:
	case RE_CHAR_CLASS_AST_FLAGS:
		break;

	default:
		fprintf(stderr, "(MATCH FAIL)\n");
		assert(0);
	}
	free(n);	
}

void
re_char_class_ast_free(struct re_char_class_ast *ast)
{
	free_iter(ast);
}

static void
re_char_class_free(struct re_char_class *cc)
{
	free(cc);
}

static int
comp_iter(struct re_char_class *cc, struct re_char_class_ast *n)
{
	assert(cc != NULL);
	assert(n != NULL);
	switch (n->t) {
	case RE_CHAR_CLASS_AST_CONCAT:
		if (!comp_iter(cc, n->u.concat.l)) { return 0; }
		if (!comp_iter(cc, n->u.concat.r)) { return 0; }
		break;
	case RE_CHAR_CLASS_AST_LITERAL:
		cc_add_byte(cc, n->u.literal.c);
		break;
	case RE_CHAR_CLASS_AST_RANGE:
		cc_add_range(cc, n->u.range.from, n->u.range.to);
		break;
	case RE_CHAR_CLASS_AST_NAMED:
		abort();	/* TODO */
		break;
	case RE_CHAR_CLASS_AST_FLAGS:
		cc->flags |= n->u.flags.f;
		break;
	case RE_CHAR_CLASS_AST_SUBTRACT:
	{
		struct re_char_class *mask = re_char_class_ast_compile(n->u.subtract.mask);
		if (mask == NULL) { return 0; }
		cc_mask(cc, mask);
		re_char_class_free(mask);
		break;
	}
	default:
		fprintf(stderr, "(MATCH FAIL)\n");
		assert(0);
	}

	return 1;
}

struct re_char_class *
re_char_class_ast_compile(struct re_char_class_ast *cca)
{
	struct re_char_class *res = calloc(1, sizeof(*res));
	if (res == NULL) { return NULL; }

	if (!comp_iter(res, cca)) {
		cc_free(res);
		return NULL;
	}

	if (res->flags & RE_CHAR_CLASS_FLAG_MINUS) {
		cc_add_byte(res, '-');
		res->flags &=~ RE_CHAR_CLASS_FLAG_MINUS;
	}

	if (res->flags & RE_CHAR_CLASS_FLAG_INVERTED) {
		cc_invert(res);
		res->flags &=~ RE_CHAR_CLASS_FLAG_INVERTED;
	}

	return res;
}

static void
bitset_pos(unsigned char byte, unsigned *pos, unsigned char *bit)
{
	*pos = byte / 8;
	*bit = 1U << (byte & 0x07);
}

static void
cc_free(struct re_char_class *c)
{
	free(c);
}

void
cc_add_byte(struct re_char_class *cc, unsigned char byte)
{
	unsigned pos;
	unsigned char bit;
	assert(cc != NULL);
	fprintf(stderr, "ADDING 0x%02x\n", byte);
	bitset_pos(byte, &pos, &bit);
	cc->chars[pos] |= bit;

	cc_dump(stderr, cc);
	fprintf(stderr, "\n");
}

void
cc_add_range(struct re_char_class *cc, unsigned char from, unsigned char to)
{
	unsigned char i;
	assert(cc != NULL);
	assert(from <= to);
	for (i = from; i <= to; i++) {
		cc_add_byte(cc, i);		
	}
	
}

void
cc_invert(struct re_char_class *cc)
{
	unsigned i;
	for (i = 0; i < sizeof(cc->chars)/sizeof(cc->chars[0]); i++) {
		cc->chars[i] = ~cc->chars[i];
	}
}

void
cc_mask(struct re_char_class *cc, const struct re_char_class *mask)
{
	unsigned i;
	for (i = 0; i < sizeof(cc->chars)/sizeof(cc->chars[0]); i++) {
		cc->chars[i] &= mask->chars[i];
	}
}

void
cc_dump(FILE *f, struct re_char_class *cc)
{
	unsigned i;
	for (i = 0; i < 256; i++) {
		unsigned pos;
		unsigned char bit;
		bitset_pos((unsigned char)i, &pos, &bit);
		if (cc->chars[pos] & bit) {
			if (isprint(i)) {
				fprintf(f, "%c", i);
			} else {
				fprintf(f, "\\x%02x", i);
			}
		}
	}
}

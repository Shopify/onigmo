#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "onigmo.h"
#include "regparse.h"

static void
Indent(FILE* f, int indent)
{
  int i;
  for (i = 0; i < indent; i++) putc(' ', f);
}

static void
p_string(FILE* f, ptrdiff_t len, UChar* s)
{
  fputs(":", f);
  while (len-- > 0) { fputc(*s++, f); }
}

static void
print_indent_tree(FILE* f, Node* node, int indent)
{
  int i, type, container_p = 0;
  int add = 3;
  UChar* p;

  Indent(f, indent);
  if (IS_NULL(node)) {
    fprintf(f, "ERROR: null node!!!\n");
    exit (0);
  }

  type = NTYPE(node);
  switch (type) {
  case NT_LIST:
  case NT_ALT:
    if (NTYPE(node) == NT_LIST)
      fprintf(f, "<list:%"PRIxPTR">\n", (intptr_t )node);
    else
      fprintf(f, "<alt:%"PRIxPTR">\n", (intptr_t )node);

    print_indent_tree(f, NCAR(node), indent + add);
    while (IS_NOT_NULL(node = NCDR(node))) {
      if (NTYPE(node) != type) {
	fprintf(f, "ERROR: list/alt right is not a cons. %d\n", NTYPE(node));
	exit(0);
      }
      print_indent_tree(f, NCAR(node), indent + add);
    }
    break;

  case NT_STR:
    fprintf(f, "<string%s:%"PRIxPTR">",
	    (NSTRING_IS_RAW(node) ? "-raw" : ""), (intptr_t )node);
    for (p = NSTR(node)->s; p < NSTR(node)->end; p++) {
      if (*p >= 0x20 && *p < 0x7f)
	fputc(*p, f);
      else {
	fprintf(f, " 0x%02x", *p);
      }
    }
    break;

  case NT_CCLASS:
    fprintf(f, "<cclass:%"PRIxPTR">", (intptr_t )node);
    if (IS_NCCLASS_NOT(NCCLASS(node))) fputs("not ", f);
    if (NCCLASS(node)->mbuf) {
      BBuf* bbuf = NCCLASS(node)->mbuf;
      OnigCodePoint* data = (OnigCodePoint* )bbuf->p;
      OnigCodePoint* end = (OnigCodePoint* )(bbuf->p + bbuf->used);
      fprintf(f, "%d", *data++);
      for (; data < end; data+=2) {
	fprintf(f, ",");
	fprintf(f, "%04x-%04x", data[0], data[1]);
      }
    }
    break;

  case NT_CTYPE:
    fprintf(f, "<ctype:%"PRIxPTR"> ", (intptr_t )node);
    switch (NCTYPE(node)->ctype) {
    case ONIGENC_CTYPE_WORD:
      if (NCTYPE(node)->not != 0)
	fputs("not word",       f);
      else
	fputs("word",           f);
      break;

    default:
      fprintf(f, "ERROR: undefined ctype.\n");
      exit(0);
    }
    break;

  case NT_CANY:
    fprintf(f, "<anychar:%"PRIxPTR">", (intptr_t )node);
    break;

  case NT_ANCHOR:
    fprintf(f, "<anchor:%"PRIxPTR"> ", (intptr_t )node);
    switch (NANCHOR(node)->type) {
    case ANCHOR_BEGIN_BUF:      fputs("begin buf",      f); break;
    case ANCHOR_END_BUF:        fputs("end buf",        f); break;
    case ANCHOR_BEGIN_LINE:     fputs("begin line",     f); break;
    case ANCHOR_END_LINE:       fputs("end line",       f); break;
    case ANCHOR_SEMI_END_BUF:   fputs("semi end buf",   f); break;
    case ANCHOR_BEGIN_POSITION: fputs("begin position", f); break;

    case ANCHOR_WORD_BOUND:      fputs("word bound",     f); break;
    case ANCHOR_NOT_WORD_BOUND:  fputs("not word bound", f); break;
#  ifdef USE_WORD_BEGIN_END
    case ANCHOR_WORD_BEGIN:      fputs("word begin", f);     break;
    case ANCHOR_WORD_END:        fputs("word end", f);       break;
#  endif
    case ANCHOR_PREC_READ:       fputs("prec read",      f); container_p = TRUE; break;
    case ANCHOR_PREC_READ_NOT:   fputs("prec read not",  f); container_p = TRUE; break;
    case ANCHOR_LOOK_BEHIND:     fputs("look_behind",    f); container_p = TRUE; break;
    case ANCHOR_LOOK_BEHIND_NOT: fputs("look_behind_not",f); container_p = TRUE; break;
    case ANCHOR_KEEP:            fputs("keep",f);            break;

    default:
      fprintf(f, "ERROR: undefined anchor type.\n");
      break;
    }
    break;

  case NT_BREF:
    {
      int* p;
      BRefNode* br = NBREF(node);
      p = BACKREFS_P(br);
      fprintf(f, "<backref:%"PRIxPTR">", (intptr_t )node);
      for (i = 0; i < br->back_num; i++) {
	if (i > 0) fputs(", ", f);
	fprintf(f, "%d", p[i]);
      }
    }
    break;

#  ifdef USE_SUBEXP_CALL
  case NT_CALL:
    {
      CallNode* cn = NCALL(node);
      fprintf(f, "<call:%"PRIxPTR">", (intptr_t )node);
      p_string(f, cn->name_end - cn->name, cn->name);
    }
    break;
#  endif

  case NT_QTFR:
    fprintf(f, "<quantifier:%"PRIxPTR">{%d,%d}%s\n", (intptr_t )node,
	    NQTFR(node)->lower, NQTFR(node)->upper,
	    (NQTFR(node)->greedy ? "" : "?"));
    print_indent_tree(f, NQTFR(node)->target, indent + add);
    break;

  case NT_ENCLOSE:
    fprintf(f, "<enclose:%"PRIxPTR"> ", (intptr_t )node);
    switch (NENCLOSE(node)->type) {
    case ENCLOSE_OPTION:
      fprintf(f, "option:%d", NENCLOSE(node)->option);
      break;
    case ENCLOSE_MEMORY:
      fprintf(f, "memory:%d", NENCLOSE(node)->regnum);
      break;
    case ENCLOSE_STOP_BACKTRACK:
      fprintf(f, "stop-bt");
      break;
    case ENCLOSE_CONDITION:
      fprintf(f, "condition:%d", NENCLOSE(node)->regnum);
      break;
    case ENCLOSE_ABSENT:
      fprintf(f, "absent");
      break;

    default:
      break;
    }
    fprintf(f, "\n");
    print_indent_tree(f, NENCLOSE(node)->target, indent + add);
    break;

  default:
    fprintf(f, "print_indent_tree: undefined node type %d\n", NTYPE(node));
    break;
  }

  if (type != NT_LIST && type != NT_ALT && type != NT_QTFR &&
      type != NT_ENCLOSE)
    fprintf(f, "\n");

  if (container_p) print_indent_tree(f, NANCHOR(node)->target, indent + add);

  fflush(f);
}

static void
fail(int r, OnigErrorInfo *einfo) {
  OnigUChar s[ONIG_MAX_ERROR_MESSAGE_LEN];
  onig_error_code_to_str(s, r, &einfo);
  fprintf(stderr, "ERROR: %s\n", s);
}

static VALUE
parse(VALUE self, VALUE string) {
  int r;
  regex_t *reg;
  OnigErrorInfo einfo;
  OnigRegion *region;

  UChar *pattern = (UChar* ) StringValueCStr(string);
  UChar *pattern_end = pattern + strlen((char *) pattern);

  reg = (regex_t *) malloc(sizeof(regex_t));
  if (reg == NULL) {
    fail(ONIGERR_MEMORY, &einfo);
    return Qfalse;
  }

  r = onig_reg_init(reg, ONIG_OPTION_DEFAULT, ONIGENC_CASE_FOLD_DEFAULT, ONIG_ENCODING_ASCII, ONIG_SYNTAX_DEFAULT);
  if (r) {
    free(reg);
    fail(r, &einfo);
    return Qfalse;
  }

  OnigDistance init_size;
  Node *root;
  ScanEnv scan_env = {0};

  if (reg->alloc == 0) {
    init_size = (pattern_end - pattern) * 2;
    if (init_size <= 0) init_size = 20;
    r = BBUF_INIT(reg, init_size);

    if (r) {
      free(reg);
      fail(r, &einfo);
      return Qfalse;
    }
  } else {
    reg->used = 0;
  }

  reg->num_mem = 0;
  reg->num_repeat = 0;
  reg->num_null_check = 0;
  reg->repeat_range_alloc = 0;
  reg->repeat_range = (OnigRepeatRange* ) NULL;

  r = onig_parse_make_tree(&root, pattern, pattern_end, reg, &scan_env);
  if (r) {
    free(reg);
    fail(r, &einfo);
    return Qfalse;
  }

  print_indent_tree(stderr, root, 0);

  onig_node_free(root);
  free(reg);
  return Qtrue;
}

void Init_onigmo(void) {
  VALUE rb_cOnigmo = rb_define_module("Onigmo");
  rb_define_singleton_method(rb_cOnigmo, "parse", parse, 1);
}

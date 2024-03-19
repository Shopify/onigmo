#include <ruby.h>
#include <ruby/onigmo.h>
#include <ruby/encoding.h>

#include "regint.h"
#include "regparse.h"

VALUE rb_cOnigmoNode;
VALUE rb_cOnigmoAlternationNode;
VALUE rb_cOnigmoAnchorBufferBeginNode;
VALUE rb_cOnigmoAnchorBufferEndNode;
VALUE rb_cOnigmoAnchorKeepNode;
VALUE rb_cOnigmoAnchorLineBeginNode;
VALUE rb_cOnigmoAnchorLineEndNode;
VALUE rb_cOnigmoAnchorPositionBeginNode;
VALUE rb_cOnigmoAnchorSemiEndNode;
VALUE rb_cOnigmoAnchorWordBoundaryNode;
VALUE rb_cOnigmoAnchorWordBoundaryInvertNode;
VALUE rb_cOnigmoAnyNode;
VALUE rb_cOnigmoBackrefNode;
VALUE rb_cOnigmoCallNode;
VALUE rb_cOnigmoCClassNode;
VALUE rb_cOnigmoCClassInvertNode;
VALUE rb_cOnigmoEncloseAbsentNode;
VALUE rb_cOnigmoEncloseConditionNode;
VALUE rb_cOnigmoEncloseMemoryNode;
VALUE rb_cOnigmoEncloseOptionsNode;
VALUE rb_cOnigmoEncloseStopBacktrackNode;
VALUE rb_cOnigmoListNode;
VALUE rb_cOnigmoLookAheadNode;
VALUE rb_cOnigmoLookAheadInvertNode;
VALUE rb_cOnigmoLookBehindNode;
VALUE rb_cOnigmoLookBehindInvertNode;
VALUE rb_cOnigmoQuantifierNode;
VALUE rb_cOnigmoStringNode;
VALUE rb_cOnigmoWordNode;
VALUE rb_cOnigmoWordInvertNode;

static VALUE
build_options(OnigOptionType option) {
    VALUE options = rb_ary_new();

    if (option & ONIG_OPTION_NONE) rb_ary_push(options, ID2SYM(rb_intern("none")));
    if (option & ONIG_OPTION_IGNORECASE) rb_ary_push(options, ID2SYM(rb_intern("ignorecase")));
    if (option & ONIG_OPTION_EXTEND) rb_ary_push(options, ID2SYM(rb_intern("extend")));
    if (option & ONIG_OPTION_MULTILINE) rb_ary_push(options, ID2SYM(rb_intern("multiline")));
    if (option & ONIG_OPTION_DOTALL) rb_ary_push(options, ID2SYM(rb_intern("dotall")));
    if (option & ONIG_OPTION_SINGLELINE) rb_ary_push(options, ID2SYM(rb_intern("singleline")));
    if (option & ONIG_OPTION_FIND_LONGEST) rb_ary_push(options, ID2SYM(rb_intern("find_longest")));
    if (option & ONIG_OPTION_FIND_NOT_EMPTY) rb_ary_push(options, ID2SYM(rb_intern("find_not_empty")));
    if (option & ONIG_OPTION_NEGATE_SINGLELINE) rb_ary_push(options, ID2SYM(rb_intern("negate_singleline")));
    if (option & ONIG_OPTION_DONT_CAPTURE_GROUP) rb_ary_push(options, ID2SYM(rb_intern("dont_capture_group")));
    if (option & ONIG_OPTION_CAPTURE_GROUP) rb_ary_push(options, ID2SYM(rb_intern("capture_group")));
    if (option & ONIG_OPTION_NOTBOL) rb_ary_push(options, ID2SYM(rb_intern("not_bol")));
    if (option & ONIG_OPTION_NOTEOL) rb_ary_push(options, ID2SYM(rb_intern("not_eol")));
    if (option & ONIG_OPTION_NOTBOS) rb_ary_push(options, ID2SYM(rb_intern("not_bos")));
    if (option & ONIG_OPTION_NOTEOS) rb_ary_push(options, ID2SYM(rb_intern("not_eos")));
    if (option & ONIG_OPTION_ASCII_RANGE) rb_ary_push(options, ID2SYM(rb_intern("ascii_range")));
    if (option & ONIG_OPTION_POSIX_BRACKET_ALL_RANGE) rb_ary_push(options, ID2SYM(rb_intern("posix_bracket_all_range")));
    if (option & ONIG_OPTION_WORD_BOUND_ALL_RANGE) rb_ary_push(options, ID2SYM(rb_intern("word_bound_all_range")));
    if (option & ONIG_OPTION_NEWLINE_CRLF) rb_ary_push(options, ID2SYM(rb_intern("newline_crlf")));

    return options;
}

static VALUE
build_bitset(BitSetRef ref, OnigEncoding encoding) {
    VALUE values = rb_ary_new();

    for (int index = 0; index < SINGLE_BYTE_SIZE; index++) {
        if (BITSET_AT(ref, index) != 0) {
            const char character = (const char) index;
            rb_ary_push(values, rb_enc_str_new(&character, 1, encoding));
        }
    }

    return values;
}

static VALUE
build_node(Node *node, OnigEncoding encoding) {
    int type = NTYPE(node);

    switch (type) {
        case NT_STR: {
            VALUE value = rb_enc_str_new((const char *) NSTR(node)->s, NSTR(node)->end - NSTR(node)->s, encoding);
            VALUE argv[] = { value };
            return rb_class_new_instance(1, argv, rb_cOnigmoStringNode);
        }
        case NT_CCLASS: {
            CClassNode* cclass_node = NCCLASS(node);
            VALUE values = build_bitset(cclass_node->bs, encoding);

            if (cclass_node->mbuf != NULL) {
                BBuf *bbuf = cclass_node->mbuf;
                OnigCodePoint *data = (OnigCodePoint *) bbuf->p;
                OnigCodePoint *end = (OnigCodePoint *) (bbuf->p + bbuf->used);

                for (++data; data < end; data += 2) {
                    for (OnigCodePoint code = data[0]; code < data[1]; code++) {
                        rb_ary_push(values, INT2NUM(code));
                    }
                }
            }

            VALUE argv[] = { values };
            if (IS_NCCLASS_NOT(cclass_node)) {
                return rb_class_new_instance(1, argv, rb_cOnigmoCClassInvertNode);
            } else {
                return rb_class_new_instance(1, argv, rb_cOnigmoCClassNode);
            }
        }
        case NT_CTYPE: {
            if (NCTYPE(node)->ctype == ONIGENC_CTYPE_WORD) {
                if (NCTYPE(node)->not == 0) {
                    return rb_class_new_instance(0, NULL, rb_cOnigmoWordNode);
                } else {
                    return rb_class_new_instance(0, NULL, rb_cOnigmoWordInvertNode);
                }
            } else {
                RUBY_ASSERT("unknown ctype");
                return Qnil;
            }
        }
        case NT_CANY: {
            return rb_class_new_instance(0, NULL, rb_cOnigmoAnyNode);
        }
        case NT_BREF: {
            BRefNode *backref_node = NBREF(node);   
            int *backrefs = BACKREFS_P(backref_node);

            VALUE values = rb_ary_new();
            for (int index = 0; index < backref_node->back_num; index++) {
                rb_ary_push(values, INT2NUM(backrefs[index]));
            }

            VALUE argv[] = { values };
            return rb_class_new_instance(1, argv, rb_cOnigmoBackrefNode);
        }
        case NT_QTFR: {
            int lower = NQTFR(node)->lower;
            int upper = NQTFR(node)->upper;

            VALUE argv[] = {
                lower == -1 ? Qnil : INT2NUM(lower),
                upper = -1 ? Qnil : INT2NUM(upper),
                (NQTFR(node)->greedy ? Qtrue : Qfalse),
                build_node(NQTFR(node)->target, encoding)
            };

            return rb_class_new_instance(4, argv, rb_cOnigmoQuantifierNode);
        }
        case NT_ENCLOSE: {
            VALUE target = build_node(NENCLOSE(node)->target, encoding);

            switch (NENCLOSE(node)->type) {
                case ENCLOSE_OPTION: {
                    VALUE argv[] = { build_options(NENCLOSE(node)->option), target };
                    return rb_class_new_instance(2, argv, rb_cOnigmoEncloseOptionsNode);
                }
                case ENCLOSE_MEMORY: {
                    VALUE argv[] = { INT2NUM(NENCLOSE(node)->regnum), target };
                    return rb_class_new_instance(2, argv, rb_cOnigmoEncloseMemoryNode);
                }
                case ENCLOSE_STOP_BACKTRACK: {
                    VALUE argv[] = { target };
                    return rb_class_new_instance(1, argv, rb_cOnigmoEncloseStopBacktrackNode);
                }
                case ENCLOSE_CONDITION: {
                    VALUE argv[] = { INT2NUM(NENCLOSE(node)->regnum), target };
                    return rb_class_new_instance(2, argv, rb_cOnigmoEncloseConditionNode);
                }
                case ENCLOSE_ABSENT: {
                    VALUE argv[] = { target };
                    return rb_class_new_instance(1, argv, rb_cOnigmoEncloseAbsentNode);
                }
                default:
                    RUBY_ASSERT("unknown enclose type");
                    return Qnil;
            }
        }
        case NT_ANCHOR: {
            switch (NANCHOR(node)->type) {
                case ANCHOR_BEGIN_BUF:
                    return rb_class_new_instance(0, NULL, rb_cOnigmoAnchorBufferBeginNode);
                case ANCHOR_END_BUF:
                    return rb_class_new_instance(0, NULL, rb_cOnigmoAnchorBufferEndNode);
                case ANCHOR_BEGIN_LINE:
                    return rb_class_new_instance(0, NULL, rb_cOnigmoAnchorLineBeginNode);
                case ANCHOR_END_LINE:
                    return rb_class_new_instance(0, NULL, rb_cOnigmoAnchorLineEndNode);
                case ANCHOR_SEMI_END_BUF:
                    return rb_class_new_instance(0, NULL, rb_cOnigmoAnchorSemiEndNode);
                case ANCHOR_BEGIN_POSITION:
                    return rb_class_new_instance(0, NULL, rb_cOnigmoAnchorPositionBeginNode);
                case ANCHOR_WORD_BOUND:
                    return rb_class_new_instance(0, NULL, rb_cOnigmoAnchorWordBoundaryNode);
                case ANCHOR_NOT_WORD_BOUND:
                    return rb_class_new_instance(0, NULL, rb_cOnigmoAnchorWordBoundaryInvertNode);
                case ANCHOR_PREC_READ: {
                    VALUE target = build_node(NANCHOR(node)->target, encoding);
                    VALUE argv[] = { target };
                    return rb_class_new_instance(1, argv, rb_cOnigmoLookAheadNode);
                }
                case ANCHOR_PREC_READ_NOT: {
                    VALUE target = build_node(NANCHOR(node)->target, encoding);
                    VALUE argv[] = { target };
                    return rb_class_new_instance(1, argv, rb_cOnigmoLookAheadInvertNode);
                }
                case ANCHOR_LOOK_BEHIND: {
                    VALUE target = build_node(NANCHOR(node)->target, encoding);
                    VALUE argv[] = { target };
                    return rb_class_new_instance(1, argv, rb_cOnigmoLookBehindNode);
                }
                case ANCHOR_LOOK_BEHIND_NOT: {
                    VALUE target = build_node(NANCHOR(node)->target, encoding);
                    VALUE argv[] = { target };
                    return rb_class_new_instance(1, argv, rb_cOnigmoLookBehindInvertNode);
                }
                case ANCHOR_KEEP:
                    return rb_class_new_instance(0, NULL, rb_cOnigmoAnchorKeepNode);
                default:
                    RUBY_ASSERT("unknown anchor type");
                    return Qnil;
            }
        }
        case NT_LIST: {
            VALUE nodes = rb_ary_new();
            rb_ary_push(nodes, build_node(NCAR(node), encoding));

            while (IS_NOT_NULL(node = NCDR(node))) {
                RUBY_ASSERT(NTYPE(node) == type);
                rb_ary_push(nodes, build_node(NCAR(node), encoding));
            }

            VALUE argv[] = { nodes };
            return rb_class_new_instance(1, argv, rb_cOnigmoListNode);
        }
        case NT_ALT: {
            VALUE nodes = rb_ary_new();
            rb_ary_push(nodes, build_node(NCAR(node), encoding));

            while (IS_NOT_NULL(node = NCDR(node))) {
                RUBY_ASSERT(NTYPE(node) == type);
                rb_ary_push(nodes, build_node(NCAR(node), encoding));
            }

            VALUE argv[] = { nodes };
            return rb_class_new_instance(1, argv, rb_cOnigmoAlternationNode);
        }
        case NT_CALL: {
            CallNode *call_node = NCALL(node);

            VALUE name;
            ptrdiff_t length = call_node->name_end - call_node->name;

            if (length > 0) {
                name = rb_enc_str_new((const char *) call_node->name, length, encoding);
            } else {
                name = Qnil;
            }

            VALUE argv[] = { INT2NUM(call_node->group_num), name };
            return rb_class_new_instance(2, argv, rb_cOnigmoCallNode);
        }
        default: {
            RUBY_ASSERT("unknown node type");
            return Qnil;
        }
    }
}

static void
fail(int result, regex_t *regex, OnigErrorInfo *einfo) {
    OnigUChar message[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(message, result, einfo);

    onig_free(regex);
    onig_end();

    rb_raise(rb_eArgError, "%s", message);
}

static VALUE
parse(VALUE self, VALUE string) {
    const OnigUChar *pattern = (const OnigUChar *) StringValueCStr(string); 
    const OnigUChar *pattern_end = pattern + strlen((const char *) pattern);

    regex_t *regex = calloc(1, sizeof(regex_t));
    if (regex == NULL) {
        rb_raise(rb_eNoMemError, "failed to allocate memory");
        return Qnil;
    }

    int result;
    OnigEncoding encoding = rb_enc_get(string);

    if ((result = onig_reg_init(regex, ONIG_OPTION_DEFAULT, ONIGENC_CASE_FOLD_DEFAULT, encoding, ONIG_SYNTAX_DEFAULT)) != ONIG_NORMAL) {
        fail(result, regex, NULL);
        return Qnil;
    }

    OnigDistance init_size = (pattern_end - pattern) * 2;
    result = BBUF_INIT(regex, init_size);

    if (result != ONIG_NORMAL) {
        fail(result, regex, NULL);
        return Qnil;
    }

    Node *root;
    ScanEnv scan_env = { 0 };

    result = onig_parse_make_tree(&root, pattern, pattern_end, regex, &scan_env);
    if (result != ONIG_NORMAL) {
        fail(result, regex, NULL);
        return Qnil;
    }

    VALUE node = build_node(root, encoding);

    onig_node_free(root);
    onig_free(regex);
    onig_end();

    return node;
}

static VALUE
read_memnum(const unsigned char **cursor) {
    MemNumType memnum = *((MemNumType *) *cursor);
    *cursor += SIZE_MEMNUM;
    return INT2NUM(memnum);
}

static VALUE
read_reladdr(const unsigned char **cursor) {
    RelAddrType address;
    GET_RELADDR_INC(address, *cursor);
    return INT2NUM(address);
}

static VALUE
read_absaddr(const unsigned char **cursor) {
    AbsAddrType address;
    GET_ABSADDR_INC(address, *cursor);
    return INT2NUM(address);
}

static VALUE
read_exact(const unsigned char **cursor, int length, OnigEncoding encoding) {
    VALUE exact = rb_enc_str_new((const char *) *cursor, length, encoding);
    *cursor += length;
    return exact;
}

static VALUE
read_length(const unsigned char **cursor) {
    LengthType length;
    GET_LENGTH_INC(length, *cursor);
    return INT2NUM(length);
}

static VALUE
read_bitset(const unsigned char **cursor, OnigEncoding encoding) {
    VALUE bitset = build_bitset((BitSetRef) (*cursor), encoding);
    *cursor += SIZE_BITSET;
    return bitset;
}

static VALUE
read_option(const unsigned char **cursor) {
    OnigOptionType option = *((OnigOptionType *) cursor);
	*cursor += SIZE_OPTION;
    return build_options(option);
}

static VALUE
read_state_check(const unsigned char **cursor) {
    StateCheckNumType state_check = *((StateCheckNumType *) cursor);
    *cursor += SIZE_STATE_CHECK_NUM;
    return INT2NUM(state_check);
}

static VALUE
read_codepoint(const unsigned char **cursor, LengthType length) {
    const unsigned char *buffer = *cursor;

#ifndef PLATFORM_UNALIGNED_WORD_ACCESS
    ALIGNMENT_RIGHT(buffer);
#endif

    OnigCodePoint code = *((OnigCodePoint *) buffer);
    *cursor += length;

    return INT2NUM(code);
}

static VALUE
compile(VALUE self, VALUE string) {
    const OnigUChar *pattern = (const OnigUChar *) StringValueCStr(string);

    regex_t *regex;
    OnigErrorInfo einfo;

    OnigEncoding encoding = rb_enc_get(string);
    int result = onig_new(&regex, pattern, pattern + strlen((const char *) pattern), ONIG_OPTION_DEFAULT, encoding, ONIG_SYNTAX_DEFAULT, &einfo);

    if (result != ONIG_NORMAL) {
        fail(result, regex, &einfo);
        return Qnil;
    }

    VALUE insns = rb_ary_new();
    const unsigned char *cursor = regex->p;
    const unsigned char *end = cursor + regex->used;
    LengthType length;

    while (cursor < end) {
        VALUE insn = rb_ary_new();

        switch (*cursor++) {
            case OP_FINISH: {
                rb_ary_push(insn, ID2SYM(rb_intern("finish")));
                break;
            }
            case OP_END: {
                rb_ary_push(insn, ID2SYM(rb_intern("end")));
                break;
            }
            case OP_EXACT1: {
                rb_ary_push(insn, ID2SYM(rb_intern("exact1")));
                rb_ary_push(insn, read_exact(&cursor, 1, encoding));
                break;
            }
            case OP_EXACT2: {
                rb_ary_push(insn, ID2SYM(rb_intern("exact2")));
                rb_ary_push(insn, read_exact(&cursor, 2, encoding));
                break;
            }
            case OP_EXACT3: {
                rb_ary_push(insn, ID2SYM(rb_intern("exact3")));
                rb_ary_push(insn, read_exact(&cursor, 3, encoding));
                break;
            }
            case OP_EXACT4: {
                rb_ary_push(insn, ID2SYM(rb_intern("exact4")));
                rb_ary_push(insn, read_exact(&cursor, 4, encoding));
                break;
            }
            case OP_EXACT5: {
                rb_ary_push(insn, ID2SYM(rb_intern("exact5")));
                rb_ary_push(insn, read_exact(&cursor, 5, encoding));
                break;
            }
            case OP_EXACTN: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactn")));
                rb_ary_push(insn, read_length(&cursor));
                break;
            }
            case OP_EXACTMB2N1: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactmb2n1")));
                rb_ary_push(insn, read_exact(&cursor, 2, encoding));
                break;
            }
            case OP_EXACTMB2N2: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactmb2n2")));
                rb_ary_push(insn, read_exact(&cursor, 4, encoding));
                break;
            }
            case OP_EXACTMB2N3: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactmb2n3")));
                rb_ary_push(insn, read_exact(&cursor, 6, encoding));
                break;
            }
            case OP_EXACTMB2N: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactmb2n")));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);

                rb_ary_push(insn, read_exact(&cursor, NUM2INT(length) * 2, encoding));
                break;
            }
            case OP_EXACTMB3N: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactmb3n")));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);

                rb_ary_push(insn, read_exact(&cursor, NUM2INT(length) * 3, encoding));
                break;
            }
            case OP_EXACTMBN: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactmbn")));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);

                rb_ary_push(insn, read_exact(&cursor, NUM2INT(length) * 2, encoding));
                break;
            }
            case OP_EXACT1_IC: {
                rb_ary_push(insn, ID2SYM(rb_intern("exact1_ic")));
                length = enclen(encoding, cursor, end);
                rb_ary_push(insn, read_exact(&cursor, length, encoding));
                break;
            }
            case OP_EXACTN_IC: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactn_ic")));
                length = enclen(encoding, cursor, end);
                rb_ary_push(insn, read_exact(&cursor, length, encoding));
                break;
            }
            case OP_CCLASS: {
                rb_ary_push(insn, ID2SYM(rb_intern("cclass")));
                rb_ary_push(insn, read_bitset(&cursor, encoding));
                break;
            }
            case OP_CCLASS_MB: {
                rb_ary_push(insn, ID2SYM(rb_intern("cclass_mb")));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);
                rb_ary_push(insn, read_codepoint(&cursor, NUM2INT(length)));

                break;
            }
            case OP_CCLASS_MB_NOT: {
                rb_ary_push(insn, ID2SYM(rb_intern("cclass_mb_not")));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);
                rb_ary_push(insn, read_codepoint(&cursor, NUM2INT(length)));

                break;
            }
            case OP_CCLASS_NOT: {
                rb_ary_push(insn, ID2SYM(rb_intern("cclass_not")));
                rb_ary_push(insn, read_bitset(&cursor, encoding));
                break;
            }
            case OP_CCLASS_MIX: {
                rb_ary_push(insn, ID2SYM(rb_intern("cclass_mix")));
                rb_ary_push(insn, read_bitset(&cursor, encoding));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);
                rb_ary_push(insn, read_codepoint(&cursor, NUM2INT(length)));

                break;
            }
            case OP_CCLASS_MIX_NOT: {
                rb_ary_push(insn, ID2SYM(rb_intern("cclass_mix_not")));
                rb_ary_push(insn, read_bitset(&cursor, encoding));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);
                rb_ary_push(insn, read_codepoint(&cursor, NUM2INT(length)));

                break;
            }
            case OP_ANYCHAR: {
                rb_ary_push(insn, ID2SYM(rb_intern("anychar")));
                break;
            }
            case OP_ANYCHAR_ML: {
                rb_ary_push(insn, ID2SYM(rb_intern("anychar_ml")));
                break;
            }
            case OP_ANYCHAR_STAR: {
                rb_ary_push(insn, ID2SYM(rb_intern("anychar_star")));
                break;
            }
            case OP_ANYCHAR_ML_STAR: {
                rb_ary_push(insn, ID2SYM(rb_intern("anychar_ml_star")));
                break;
            }
            case OP_ANYCHAR_STAR_PEEK_NEXT: {
                rb_ary_push(insn, ID2SYM(rb_intern("anychar_star_peek_next")));
                rb_ary_push(insn, read_exact(&cursor, 1, encoding));
                break;
            }
            case OP_ANYCHAR_ML_STAR_PEEK_NEXT: {
                rb_ary_push(insn, ID2SYM(rb_intern("anychar_ml_star_peek_next")));
                rb_ary_push(insn, read_exact(&cursor, 1, encoding));
                break;
            }
            case OP_WORD: {
                rb_ary_push(insn, ID2SYM(rb_intern("word")));
                break;
            }
            case OP_NOT_WORD: {
                rb_ary_push(insn, ID2SYM(rb_intern("not_word")));
                break;
            }
            case OP_WORD_BOUND: {
                rb_ary_push(insn, ID2SYM(rb_intern("word_bound")));
                break;
            }
            case OP_NOT_WORD_BOUND: {
                rb_ary_push(insn, ID2SYM(rb_intern("not_word_bound")));
                break;
            }
            case OP_WORD_BEGIN: {
                rb_ary_push(insn, ID2SYM(rb_intern("word_begin")));
                break;
            }
            case OP_WORD_END: {
                rb_ary_push(insn, ID2SYM(rb_intern("word_end")));
                break;
            }
            case OP_ASCII_WORD: {
                rb_ary_push(insn, ID2SYM(rb_intern("ascii_word")));
                break;
            }
            case OP_NOT_ASCII_WORD: {
                rb_ary_push(insn, ID2SYM(rb_intern("not_ascii_word")));
                break;
            }
            case OP_ASCII_WORD_BOUND: {
                rb_ary_push(insn, ID2SYM(rb_intern("ascii_word_bound")));
                break;
            }
            case OP_NOT_ASCII_WORD_BOUND: {
                rb_ary_push(insn, ID2SYM(rb_intern("not_ascii_word_bound")));
                break;
            }
            case OP_ASCII_WORD_BEGIN: {
                rb_ary_push(insn, ID2SYM(rb_intern("ascii_word_begin")));
                break;
            }
            case OP_ASCII_WORD_END: {
                rb_ary_push(insn, ID2SYM(rb_intern("ascii_word_end")));
                break;
            }
            case OP_BEGIN_BUF: {
                rb_ary_push(insn, ID2SYM(rb_intern("begin_buf")));
                break;
            }
            case OP_END_BUF: {
                rb_ary_push(insn, ID2SYM(rb_intern("end_buf")));
                break;
            }
            case OP_BEGIN_LINE: {
                rb_ary_push(insn, ID2SYM(rb_intern("begin_line")));
                break;
            }
            case OP_END_LINE: {
                rb_ary_push(insn, ID2SYM(rb_intern("end_line")));
                break;
            }
            case OP_SEMI_END_BUF: {
                rb_ary_push(insn, ID2SYM(rb_intern("semi_end_buf")));
                break;
            }
            case OP_BEGIN_POSITION: {
                rb_ary_push(insn, ID2SYM(rb_intern("begin_position")));
                break;
            }
            case OP_BACKREF1: {
                rb_ary_push(insn, ID2SYM(rb_intern("backref1")));
                break;
            }
            case OP_BACKREF2: {
                rb_ary_push(insn, ID2SYM(rb_intern("backref2")));
                break;
            }
            case OP_BACKREFN: {
                rb_ary_push(insn, ID2SYM(rb_intern("backrefn")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;
            }
            case OP_BACKREFN_IC: {
                rb_ary_push(insn, ID2SYM(rb_intern("backrefn_ic")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;
            }
            case OP_BACKREF_MULTI: {
                rb_ary_push(insn, ID2SYM(rb_intern("backref_multi")));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);

                for (int i = 0; i < NUM2INT(length); i++) {
                    rb_ary_push(insn, read_memnum(&cursor));
                }

                break;
            }
            case OP_BACKREF_MULTI_IC: {
                rb_ary_push(insn, ID2SYM(rb_intern("backref_multi_ic")));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);

                for (int i = 0; i < NUM2INT(length); i++) {
                    rb_ary_push(insn, read_memnum(&cursor));
                }

                break;
            }
            case OP_BACKREF_WITH_LEVEL: {
                rb_ary_push(insn, ID2SYM(rb_intern("backref_with_level")));

                rb_ary_push(insn, read_option(&cursor));
                rb_ary_push(insn, read_length(&cursor));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);

                for (int i = 0; i < NUM2INT(length); i++) {
                    rb_ary_push(insn, read_memnum(&cursor));
                }

                break;
            }
            case OP_MEMORY_START:
                rb_ary_push(insn, ID2SYM(rb_intern("memory_start")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;

            case OP_MEMORY_START_PUSH:
                rb_ary_push(insn, ID2SYM(rb_intern("memory_start_push")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;

            case OP_MEMORY_END_PUSH:
                rb_ary_push(insn, ID2SYM(rb_intern("memory_end_push")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;

            case OP_MEMORY_END_PUSH_REC:
                rb_ary_push(insn, ID2SYM(rb_intern("memory_end_push_rec")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;

            case OP_MEMORY_END:
                rb_ary_push(insn, ID2SYM(rb_intern("memory_end")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;

            case OP_MEMORY_END_REC:
                rb_ary_push(insn, ID2SYM(rb_intern("memory_end_rec")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;

            case OP_KEEP: {
                rb_ary_push(insn, ID2SYM(rb_intern("keep")));
                break;
            }
            case OP_FAIL: {
                rb_ary_push(insn, ID2SYM(rb_intern("fail")));
                break;
            }
            case OP_JUMP: {
                rb_ary_push(insn, ID2SYM(rb_intern("jump")));
                rb_ary_push(insn, read_reladdr(&cursor));
                break;
            }
            case OP_PUSH: {
                rb_ary_push(insn, ID2SYM(rb_intern("push")));
                rb_ary_push(insn, read_reladdr(&cursor));
                break;
            }
            case OP_POP: {
                rb_ary_push(insn, ID2SYM(rb_intern("pop")));
                break;
            }
            case OP_PUSH_OR_JUMP_EXACT1: {
                rb_ary_push(insn, ID2SYM(rb_intern("push_or_jump_exact1")));
                rb_ary_push(insn, read_reladdr(&cursor));
                rb_ary_push(insn, read_exact(&cursor, 1, encoding));
                break;
            }
            case OP_PUSH_IF_PEEK_NEXT: {
                rb_ary_push(insn, ID2SYM(rb_intern("push_if_peek_next")));
                rb_ary_push(insn, read_reladdr(&cursor));
                rb_ary_push(insn, read_exact(&cursor, 1, encoding));
                break;
            }
            case OP_REPEAT: {
                rb_ary_push(insn, ID2SYM(rb_intern("repeat")));
                rb_ary_push(insn, read_memnum(&cursor));
                rb_ary_push(insn, read_reladdr(&cursor));
                break;
            }
            case OP_REPEAT_NG: {
                rb_ary_push(insn, ID2SYM(rb_intern("repeat_ng")));
                rb_ary_push(insn, read_memnum(&cursor));
                rb_ary_push(insn, read_reladdr(&cursor));
                break;
            }
            case OP_REPEAT_INC: {
                rb_ary_push(insn, ID2SYM(rb_intern("repeat_inc")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;
            }
            case OP_REPEAT_INC_NG: {
                rb_ary_push(insn, ID2SYM(rb_intern("repeat_inc_ng")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;
            }
            case OP_REPEAT_INC_SG: {
                rb_ary_push(insn, ID2SYM(rb_intern("repeat_inc_sg")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;
            }
            case OP_REPEAT_INC_NG_SG: {
                rb_ary_push(insn, ID2SYM(rb_intern("repeat_inc_ng_sg")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;
            }
            case OP_NULL_CHECK_START: {
                rb_ary_push(insn, ID2SYM(rb_intern("null_check_start")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;
            }
            case OP_NULL_CHECK_END: {
                rb_ary_push(insn, ID2SYM(rb_intern("null_check_end")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;
            }
            case OP_NULL_CHECK_END_MEMST: {
                rb_ary_push(insn, ID2SYM(rb_intern("null_check_end_memst")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;
            }
            case OP_NULL_CHECK_END_MEMST_PUSH: {
                rb_ary_push(insn, ID2SYM(rb_intern("null_check_end_memst_push")));
                rb_ary_push(insn, read_memnum(&cursor));
                break;
            }
            case OP_PUSH_POS: {
                rb_ary_push(insn, ID2SYM(rb_intern("push_pos")));
                break;
            }
            case OP_POP_POS: {
                rb_ary_push(insn, ID2SYM(rb_intern("pop_pos")));
                break;
            }
            case OP_PUSH_POS_NOT: {
                rb_ary_push(insn, ID2SYM(rb_intern("push_pos_not")));
                rb_ary_push(insn, read_reladdr(&cursor));
                break;
            }
            case OP_FAIL_POS: {
                rb_ary_push(insn, ID2SYM(rb_intern("fail_pos")));
                break;
            }
            case OP_PUSH_STOP_BT: {
                rb_ary_push(insn, ID2SYM(rb_intern("push_stop_bt")));
                break;
            }
            case OP_POP_STOP_BT: {
                rb_ary_push(insn, ID2SYM(rb_intern("pop_stop_bt")));
                break;
            }
            case OP_LOOK_BEHIND: {
                rb_ary_push(insn, ID2SYM(rb_intern("look_behind")));
                rb_ary_push(insn, read_length(&cursor));
                break;
            }
            case OP_PUSH_LOOK_BEHIND_NOT: {
                rb_ary_push(insn, ID2SYM(rb_intern("push_look_behind_not")));
                rb_ary_push(insn, read_reladdr(&cursor));
                rb_ary_push(insn, read_length(&cursor));
                break;
            }
            case OP_FAIL_LOOK_BEHIND_NOT: {
                rb_ary_push(insn, ID2SYM(rb_intern("fail_look_behind_not")));
                break;
            }
            case OP_PUSH_ABSENT_POS: {
                rb_ary_push(insn, ID2SYM(rb_intern("push_absent_pos")));
                break;
            }
            case OP_ABSENT: {
                rb_ary_push(insn, ID2SYM(rb_intern("absent")));
                rb_ary_push(insn, read_reladdr(&cursor));
                break;
            }
            case OP_ABSENT_END: {
                rb_ary_push(insn, ID2SYM(rb_intern("absent_end")));
                break;
            }
            case OP_CALL: {
                rb_ary_push(insn, ID2SYM(rb_intern("call")));
                rb_ary_push(insn, read_absaddr(&cursor));
                break;
            }
            case OP_RETURN: {
                rb_ary_push(insn, ID2SYM(rb_intern("return")));
                break;
            }
            case OP_CONDITION: {
                rb_ary_push(insn, ID2SYM(rb_intern("condition")));
                rb_ary_push(insn, read_memnum(&cursor));
                rb_ary_push(insn, read_reladdr(&cursor));
                break;
            }
            case OP_STATE_CHECK_PUSH: {
                rb_ary_push(insn, ID2SYM(rb_intern("state_check_push")));
                rb_ary_push(insn, read_state_check(&cursor));
                rb_ary_push(insn, read_reladdr(&cursor));
                break;
            }
            case OP_STATE_CHECK_PUSH_OR_JUMP: {
                rb_ary_push(insn, ID2SYM(rb_intern("state_check_push_or_jump")));
                rb_ary_push(insn, read_state_check(&cursor));
                rb_ary_push(insn, read_reladdr(&cursor));
                break;
            }
            case OP_STATE_CHECK: {
                rb_ary_push(insn, ID2SYM(rb_intern("state_check")));
                rb_ary_push(insn, read_state_check(&cursor));
                break;
            }
            case OP_STATE_CHECK_ANYCHAR_STAR: {
                rb_ary_push(insn, ID2SYM(rb_intern("state_check_anychar_star")));
                rb_ary_push(insn, read_state_check(&cursor));
                break;
            }
            case OP_STATE_CHECK_ANYCHAR_ML_STAR: {
                rb_ary_push(insn, ID2SYM(rb_intern("state_check_anychar_ml_star")));
                rb_ary_push(insn, read_state_check(&cursor));
                break;
            }
            case OP_SET_OPTION_PUSH: {
                rb_ary_push(insn, ID2SYM(rb_intern("set_option_push")));
                rb_ary_push(insn, read_option(&cursor));
                break;
            }
            case OP_SET_OPTION: {
                rb_ary_push(insn, ID2SYM(rb_intern("set_option")));
                rb_ary_push(insn, read_option(&cursor));
                break;
            }
        }

        rb_ary_push(insns, insn);
    }

    onig_free(regex);
    onig_end();
    return insns;
}

void
Init_onigmo(void) {
    VALUE rb_cOnigmo = rb_define_module("Onigmo");
    rb_define_singleton_method(rb_cOnigmo, "parse", parse, 1);
    rb_define_singleton_method(rb_cOnigmo, "compile", compile, 1);

    rb_cOnigmoNode = rb_define_class_under(rb_cOnigmo, "Node", rb_cObject);
    rb_cOnigmoAlternationNode = rb_define_class_under(rb_cOnigmo, "AlternationNode", rb_cOnigmoNode);
    rb_cOnigmoAnchorBufferBeginNode = rb_define_class_under(rb_cOnigmo, "AnchorBufferBeginNode", rb_cOnigmoNode);
    rb_cOnigmoAnchorBufferEndNode = rb_define_class_under(rb_cOnigmo, "AnchorBufferEndNode", rb_cOnigmoNode);
    rb_cOnigmoAnchorKeepNode = rb_define_class_under(rb_cOnigmo, "AnchorKeepNode", rb_cOnigmoNode);
    rb_cOnigmoAnchorLineBeginNode = rb_define_class_under(rb_cOnigmo, "AnchorLineBeginNode", rb_cOnigmoNode);
    rb_cOnigmoAnchorLineEndNode = rb_define_class_under(rb_cOnigmo, "AnchorLineEndNode", rb_cOnigmoNode);
    rb_cOnigmoAnchorPositionBeginNode = rb_define_class_under(rb_cOnigmo, "AnchorPositionBeginNode", rb_cOnigmoNode);
    rb_cOnigmoAnchorSemiEndNode = rb_define_class_under(rb_cOnigmo, "AnchorSemiEndNode", rb_cOnigmoNode);
    rb_cOnigmoAnchorWordBoundaryNode = rb_define_class_under(rb_cOnigmo, "AnchorWordBoundaryNode", rb_cOnigmoNode);
    rb_cOnigmoAnchorWordBoundaryInvertNode = rb_define_class_under(rb_cOnigmo, "AnchorWordBoundaryInvertNode", rb_cOnigmoNode);
    rb_cOnigmoAnyNode = rb_define_class_under(rb_cOnigmo, "AnyNode", rb_cOnigmoNode);
    rb_cOnigmoBackrefNode = rb_define_class_under(rb_cOnigmo, "BackrefNode", rb_cOnigmoNode);
    rb_cOnigmoCallNode = rb_define_class_under(rb_cOnigmo, "CallNode", rb_cOnigmoNode);
    rb_cOnigmoCClassNode = rb_define_class_under(rb_cOnigmo, "CClassNode", rb_cOnigmoNode);
    rb_cOnigmoCClassInvertNode = rb_define_class_under(rb_cOnigmo, "CClassInvertNode", rb_cOnigmoNode);
    rb_cOnigmoEncloseAbsentNode = rb_define_class_under(rb_cOnigmo, "EncloseAbsentNode", rb_cOnigmoNode);
    rb_cOnigmoEncloseConditionNode = rb_define_class_under(rb_cOnigmo, "EncloseConditionNode", rb_cOnigmoNode);
    rb_cOnigmoEncloseMemoryNode = rb_define_class_under(rb_cOnigmo, "EncloseMemoryNode", rb_cOnigmoNode);
    rb_cOnigmoEncloseOptionsNode = rb_define_class_under(rb_cOnigmo, "EncloseOptionsNode", rb_cOnigmoNode);
    rb_cOnigmoEncloseStopBacktrackNode = rb_define_class_under(rb_cOnigmo, "EncloseStopBacktrackNode", rb_cOnigmoNode);
    rb_cOnigmoListNode = rb_define_class_under(rb_cOnigmo, "ListNode", rb_cOnigmoNode);
    rb_cOnigmoLookAheadNode = rb_define_class_under(rb_cOnigmo, "LookAheadNode", rb_cOnigmoNode);
    rb_cOnigmoLookAheadInvertNode = rb_define_class_under(rb_cOnigmo, "LookAheadInvertNode", rb_cOnigmoNode);
    rb_cOnigmoLookBehindNode = rb_define_class_under(rb_cOnigmo, "LookBehindNode", rb_cOnigmoNode);
    rb_cOnigmoLookBehindInvertNode = rb_define_class_under(rb_cOnigmo, "LookBehindInvertNode", rb_cOnigmoNode);
    rb_cOnigmoQuantifierNode = rb_define_class_under(rb_cOnigmo, "QuantifierNode", rb_cOnigmoNode);
    rb_cOnigmoStringNode = rb_define_class_under(rb_cOnigmo, "StringNode", rb_cOnigmoNode);
    rb_cOnigmoWordNode = rb_define_class_under(rb_cOnigmo, "WordNode", rb_cOnigmoNode);
    rb_cOnigmoWordInvertNode = rb_define_class_under(rb_cOnigmo, "WordInvertNode", rb_cOnigmoNode);
}

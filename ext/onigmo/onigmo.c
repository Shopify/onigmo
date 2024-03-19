#include <ruby.h>
#include <ruby/onigmo.h>

#include "regint.h"
#include "regparse.h"

VALUE rb_cOnigmoAlternationNode;
VALUE rb_cOnigmoAnchorNode;
VALUE rb_cOnigmoAnyNode;
VALUE rb_cOnigmoBackrefNode;
VALUE rb_cOnigmoCallNode;
VALUE rb_cOnigmoCClassNode;
VALUE rb_cOnigmoCTypeNode;
VALUE rb_cOnigmoEncloseNode;
VALUE rb_cOnigmoListNode;
VALUE rb_cOnigmoQuantifierNode;
VALUE rb_cOnigmoStringNode;

static VALUE
build_node(Node *node) {
    int type = NTYPE(node);

    switch (type) {
        case NT_STR: {
            VALUE value = rb_str_new((const char *) NSTR(node)->s, NSTR(node)->end - NSTR(node)->s);
            VALUE argv[] = { value };
            return rb_class_new_instance(1, argv, rb_cOnigmoStringNode);
        }
        case NT_CCLASS: {
            VALUE inverted = IS_NCCLASS_NOT(NCCLASS(node)) ? Qtrue : Qfalse;
            VALUE ranges = rb_ary_new();

            if (NCCLASS(node)->mbuf) {
                BBuf *bbuf = NCCLASS(node)->mbuf;
                OnigCodePoint *data = (OnigCodePoint *) bbuf->p;
                OnigCodePoint *end = (OnigCodePoint *) (bbuf->p + bbuf->used);

                for (++data; data < end; data += 2) {
                    VALUE range = rb_ary_new();
                    rb_ary_push(range, INT2NUM(data[0]));
                    rb_ary_push(range, INT2NUM(data[1]));
                    rb_ary_push(ranges, range);
                }
            }

            VALUE argv[] = { inverted, ranges };
            return rb_class_new_instance(2, argv, rb_cOnigmoCClassNode);
        }
        case NT_CTYPE: {
            VALUE type = Qnil;

            switch (NCTYPE(node)->ctype) {
                case ONIGENC_CTYPE_WORD:
                    if (NCTYPE(node)->not != 0) {
                        type = ID2SYM(rb_intern("not_word"));
                    } else {
                        type = ID2SYM(rb_intern("word"));
                    }
                    break;
                default:
                    RUBY_ASSERT("unknown ctype");
                    break;
            }

            VALUE argv[] = { type };
            return rb_class_new_instance(1, argv, rb_cOnigmoCTypeNode);
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
                build_node(NQTFR(node)->target)
            };

            return rb_class_new_instance(4, argv, rb_cOnigmoQuantifierNode);
        }
        case NT_ENCLOSE: {
            VALUE type = Qnil;

            switch (NENCLOSE(node)->type) {
                case ENCLOSE_OPTION: {
                    type = rb_ary_new_capa(2);
                    rb_ary_push(type, ID2SYM(rb_intern("option")));
                    rb_ary_push(type, INT2NUM(NENCLOSE(node)->option));
                    break;
                }
                case ENCLOSE_MEMORY: {
                    type = rb_ary_new_capa(2);
                    rb_ary_push(type, ID2SYM(rb_intern("memory")));
                    rb_ary_push(type, INT2NUM(NENCLOSE(node)->regnum));
                    break;
                }
                case ENCLOSE_STOP_BACKTRACK: {
                    type = rb_ary_new_capa(1);
                    rb_ary_push(type, ID2SYM(rb_intern("stop_backtrack")));
                    break;
                }
                case ENCLOSE_CONDITION: {
                    type = rb_ary_new_capa(2);
                    rb_ary_push(type, ID2SYM(rb_intern("condition")));
                    rb_ary_push(type, INT2NUM(NENCLOSE(node)->regnum));
                    break;
                }
                case ENCLOSE_ABSENT: {
                    type = rb_ary_new_capa(1);
                    rb_ary_push(type, ID2SYM(rb_intern("absent")));
                    break;
                }
                default:
                    RUBY_ASSERT("unknown enclose type");
                    break;
            }

            VALUE argv[] = { type, build_node(NENCLOSE(node)->target) };
            return rb_class_new_instance(2, argv, rb_cOnigmoEncloseNode);
        }
        case NT_ANCHOR: {
            VALUE type = Qnil;

            switch (NANCHOR(node)->type) {
                case ANCHOR_BEGIN_BUF: type = ID2SYM(rb_intern("begin_buf")); break;
                case ANCHOR_END_BUF: type = ID2SYM(rb_intern("end_buf")); break;
                case ANCHOR_BEGIN_LINE: type = ID2SYM(rb_intern("begin_line")); break;
                case ANCHOR_END_LINE: type = ID2SYM(rb_intern("end_line")); break;
                case ANCHOR_SEMI_END_BUF: type = ID2SYM(rb_intern("semi_end_buf")); break;
                case ANCHOR_BEGIN_POSITION: type = ID2SYM(rb_intern("begin_position")); break;
                case ANCHOR_WORD_BOUND: type = ID2SYM(rb_intern("word_bound")); break;
                case ANCHOR_NOT_WORD_BOUND: type = ID2SYM(rb_intern("not_word_bound")); break;
                case ANCHOR_WORD_BEGIN: type = ID2SYM(rb_intern("word_begin")); break;
                case ANCHOR_WORD_END: type = ID2SYM(rb_intern("word_end")); break;
                case ANCHOR_PREC_READ: type = ID2SYM(rb_intern("prec_read")); break;
                case ANCHOR_PREC_READ_NOT: type = ID2SYM(rb_intern("prec_read_not")); break;
                case ANCHOR_LOOK_BEHIND: type = ID2SYM(rb_intern("look_behind")); break;
                case ANCHOR_LOOK_BEHIND_NOT: type = ID2SYM(rb_intern("look_behind_not")); break;
                case ANCHOR_KEEP: type = ID2SYM(rb_intern("keep")); break;
                default: RUBY_ASSERT("unknown anchor type"); break;
            }

            VALUE argv[] = { type };
            return rb_class_new_instance(1, argv, rb_cOnigmoAnchorNode);
        }
        case NT_LIST: {
            VALUE nodes = rb_ary_new();
            rb_ary_push(nodes, build_node(NCAR(node)));

            while (IS_NOT_NULL(node = NCDR(node))) {
                RUBY_ASSERT(NTYPE(node) == type);
                rb_ary_push(nodes, build_node(NCAR(node)));
            }

            VALUE argv[] = { nodes };
            return rb_class_new_instance(1, argv, rb_cOnigmoListNode);
        }
        case NT_ALT: {
            VALUE nodes = rb_ary_new();
            rb_ary_push(nodes, build_node(NCAR(node)));

            while (IS_NOT_NULL(node = NCDR(node))) {
                RUBY_ASSERT(NTYPE(node) == type);
                rb_ary_push(nodes, build_node(NCAR(node)));
            }

            VALUE argv[] = { nodes };
            return rb_class_new_instance(1, argv, rb_cOnigmoAlternationNode);
        }
        case NT_CALL: {
            CallNode *call_node = NCALL(node);
            VALUE name = rb_str_new((const char *) call_node->name, call_node->name_end - call_node->name);

            VALUE argv[] = { name };
            return rb_class_new_instance(1, argv, rb_cOnigmoCallNode);
        }
        default: {
            RUBY_ASSERT("unknown node type");
            return Qnil;
        }
    }
}

static VALUE
parse(VALUE self, VALUE string) {
    const OnigUChar *pattern = (const OnigUChar *) StringValueCStr(string); 
    const OnigUChar *pattern_end = pattern + strlen((const char *) pattern);

    regex_t *regex = calloc(1, sizeof(regex_t));
    int result;

    OnigEncoding encoding = ONIG_ENCODING_ASCII;
    OnigErrorInfo einfo = { 0 };

    if ((result = onig_reg_init(regex, ONIG_OPTION_DEFAULT, ONIGENC_CASE_FOLD_DEFAULT, encoding, ONIG_SYNTAX_DEFAULT)) != 0) {
        OnigUChar message[ONIG_MAX_ERROR_MESSAGE_LEN];
        onig_error_code_to_str(message, result, &einfo);

        onig_free(regex);
        onig_end();

        rb_raise(rb_eArgError, "%s", message);
        return Qnil;
    }

    OnigDistance init_size = (pattern_end - pattern) * 2;
    result = BBUF_INIT(regex, init_size);

    if (result != 0) {
        OnigUChar message[ONIG_MAX_ERROR_MESSAGE_LEN];
        onig_error_code_to_str(message, result, &einfo);

        onig_free(regex);
        onig_end();

        rb_raise(rb_eArgError, "%s", message);
        return Qnil;
    }

    Node *root;
    ScanEnv scan_env = { 0 };

    result = onig_parse_make_tree(&root, pattern, pattern_end, regex, &scan_env);
    if (result != 0) {
        OnigUChar message[ONIG_MAX_ERROR_MESSAGE_LEN];
        onig_error_code_to_str(message, result, &einfo);

        onig_node_free(root);
        onig_free(regex);
        onig_end();

        rb_raise(rb_eArgError, "%s", message);
        return Qnil;
    }

    VALUE node = build_node(root);

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
read_exact(const unsigned char **cursor, int length) {
    VALUE exact = rb_str_new((const char *) *cursor, length);
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
read_bitset(const unsigned char **cursor) {
    BitSetRef ref = (BitSetRef) (*cursor);
    int bitset = 0;

    for (int index = 0; index < SINGLE_BYTE_SIZE; index++) {
        if (BITSET_AT(ref, index)) bitset++;
    }

    *cursor += SINGLE_BYTE_SIZE;
    return INT2NUM(bitset);
}

static VALUE
read_option(const unsigned char **cursor) {
    OnigOptionType option = *((OnigOptionType *) cursor);
	*cursor += SIZE_OPTION;

    VALUE result = Qnil;
    if (option == ONIG_OPTION_NONE) {
        result = ID2SYM(rb_intern("none"));
    } else if (option == ONIG_OPTION_IGNORECASE) {
        result = ID2SYM(rb_intern("ignorecase"));
    } else if (option == ONIG_OPTION_EXTEND) {
        result = ID2SYM(rb_intern("extend"));
    } else if (option == ONIG_OPTION_MULTILINE) {
        result = ID2SYM(rb_intern("multiline"));
    } else if (option == ONIG_OPTION_DOTALL) {
        result = ID2SYM(rb_intern("dotall"));
    } else if (option == ONIG_OPTION_SINGLELINE) {
        result = ID2SYM(rb_intern("singleline"));
    } else if (option == ONIG_OPTION_FIND_LONGEST) {
        result = ID2SYM(rb_intern("find_longest"));
    } else if (option == ONIG_OPTION_FIND_NOT_EMPTY) {
        result = ID2SYM(rb_intern("find_not_empty"));
    } else if (option == ONIG_OPTION_NEGATE_SINGLELINE) {
        result = ID2SYM(rb_intern("negate_singleline"));
    } else if (option == ONIG_OPTION_DONT_CAPTURE_GROUP) {
        result = ID2SYM(rb_intern("dont_capture_group"));
    } else if (option == ONIG_OPTION_CAPTURE_GROUP) {
        result = ID2SYM(rb_intern("capture_group"));
    } else if (option == ONIG_OPTION_NOTBOL) {
        result = ID2SYM(rb_intern("notbol"));
    } else if (option == ONIG_OPTION_NOTEOL) {
        result = ID2SYM(rb_intern("noteol"));
    } else if (option == ONIG_OPTION_NOTBOS) {
        result = ID2SYM(rb_intern("notbos"));
    } else if (option == ONIG_OPTION_NOTEOS) {
        result = ID2SYM(rb_intern("noteos"));
    } else if (option == ONIG_OPTION_ASCII_RANGE) {
        result = ID2SYM(rb_intern("ascii_range"));
    } else if (option == ONIG_OPTION_POSIX_BRACKET_ALL_RANGE) {
        result = ID2SYM(rb_intern("posix_bracket_all_range"));
    } else if (option == ONIG_OPTION_WORD_BOUND_ALL_RANGE) {
        result = ID2SYM(rb_intern("word_bound_all_range"));
    } else if (option == ONIG_OPTION_NEWLINE_CRLF) {
        result = ID2SYM(rb_intern("newline_crlf"));
    }

    return result;
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

    OnigEncoding encoding = ONIG_ENCODING_ASCII;
    int result = onig_new(&regex, pattern, pattern + strlen((const char *) pattern), ONIG_OPTION_DEFAULT, encoding, ONIG_SYNTAX_DEFAULT, &einfo);

    if (result != ONIG_NORMAL) {
        OnigUChar message[ONIG_MAX_ERROR_MESSAGE_LEN];
        onig_error_code_to_str(message, result, &einfo);

        onig_free(regex);
        onig_end();

        rb_raise(rb_eArgError, "%s", message);
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
                rb_ary_push(insn, read_exact(&cursor, 1));
                break;
            }
            case OP_EXACT2: {
                rb_ary_push(insn, ID2SYM(rb_intern("exact2")));
                rb_ary_push(insn, read_exact(&cursor, 2));
                break;
            }
            case OP_EXACT3: {
                rb_ary_push(insn, ID2SYM(rb_intern("exact3")));
                rb_ary_push(insn, read_exact(&cursor, 3));
                break;
            }
            case OP_EXACT4: {
                rb_ary_push(insn, ID2SYM(rb_intern("exact4")));
                rb_ary_push(insn, read_exact(&cursor, 4));
                break;
            }
            case OP_EXACT5: {
                rb_ary_push(insn, ID2SYM(rb_intern("exact5")));
                rb_ary_push(insn, read_exact(&cursor, 5));
                break;
            }
            case OP_EXACTN: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactn")));
                rb_ary_push(insn, read_length(&cursor));
                break;
            }
            case OP_EXACTMB2N1: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactmb2n1")));
                rb_ary_push(insn, read_exact(&cursor, 2));
                break;
            }
            case OP_EXACTMB2N2: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactmb2n2")));
                rb_ary_push(insn, read_exact(&cursor, 4));
                break;
            }
            case OP_EXACTMB2N3: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactmb2n3")));
                rb_ary_push(insn, read_exact(&cursor, 6));
                break;
            }
            case OP_EXACTMB2N: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactmb2n")));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);

                rb_ary_push(insn, read_exact(&cursor, NUM2INT(length) * 2));
                break;
            }
            case OP_EXACTMB3N: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactmb3n")));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);

                rb_ary_push(insn, read_exact(&cursor, NUM2INT(length) * 3));
                break;
            }
            case OP_EXACTMBN: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactmbn")));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);

                rb_ary_push(insn, read_exact(&cursor, NUM2INT(length) * 2));
                break;
            }
            case OP_EXACT1_IC: {
                rb_ary_push(insn, ID2SYM(rb_intern("exact1_ic")));
                length = enclen(encoding, cursor, end);
                rb_ary_push(insn, read_exact(&cursor, length));
                break;
            }
            case OP_EXACTN_IC: {
                rb_ary_push(insn, ID2SYM(rb_intern("exactn_ic")));
                length = enclen(encoding, cursor, end);
                rb_ary_push(insn, read_exact(&cursor, length));
                break;
            }
            case OP_CCLASS: {
                rb_ary_push(insn, ID2SYM(rb_intern("cclass")));
                rb_ary_push(insn, read_bitset(&cursor));
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
                rb_ary_push(insn, read_bitset(&cursor));
                break;
            }
            case OP_CCLASS_MIX: {
                rb_ary_push(insn, ID2SYM(rb_intern("cclass_mix")));
                rb_ary_push(insn, read_bitset(&cursor));

                VALUE length = read_length(&cursor);
                rb_ary_push(insn, length);
                rb_ary_push(insn, read_codepoint(&cursor, NUM2INT(length)));

                break;
            }
            case OP_CCLASS_MIX_NOT: {
                rb_ary_push(insn, ID2SYM(rb_intern("cclass_mix_not")));
                rb_ary_push(insn, read_bitset(&cursor));

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
                rb_ary_push(insn, read_exact(&cursor, 1));
                break;
            }
            case OP_ANYCHAR_ML_STAR_PEEK_NEXT: {
                rb_ary_push(insn, ID2SYM(rb_intern("anychar_ml_star_peek_next")));
                rb_ary_push(insn, read_exact(&cursor, 1));
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
                rb_ary_push(insn, read_exact(&cursor, 1));
                break;
            }
            case OP_PUSH_IF_PEEK_NEXT: {
                rb_ary_push(insn, ID2SYM(rb_intern("push_if_peek_next")));
                rb_ary_push(insn, read_reladdr(&cursor));
                rb_ary_push(insn, read_exact(&cursor, 1));
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

    rb_cOnigmoAlternationNode = rb_define_class_under(rb_cOnigmo, "AlternationNode", rb_cObject);
    rb_cOnigmoAnchorNode = rb_define_class_under(rb_cOnigmo, "AnchorNode", rb_cObject);
    rb_cOnigmoAnyNode = rb_define_class_under(rb_cOnigmo, "AnyNode", rb_cObject);
    rb_cOnigmoBackrefNode = rb_define_class_under(rb_cOnigmo, "BackrefNode", rb_cObject);
    rb_cOnigmoCallNode = rb_define_class_under(rb_cOnigmo, "CallNode", rb_cObject);
    rb_cOnigmoCClassNode = rb_define_class_under(rb_cOnigmo, "CClassNode", rb_cObject);
    rb_cOnigmoCTypeNode = rb_define_class_under(rb_cOnigmo, "CTypeNode", rb_cObject);
    rb_cOnigmoEncloseNode = rb_define_class_under(rb_cOnigmo, "EncloseNode", rb_cObject);
    rb_cOnigmoListNode = rb_define_class_under(rb_cOnigmo, "ListNode", rb_cObject);
    rb_cOnigmoQuantifierNode = rb_define_class_under(rb_cOnigmo, "QuantifierNode", rb_cObject);
    rb_cOnigmoStringNode = rb_define_class_under(rb_cOnigmo, "StringNode", rb_cObject);
}

#ifdef DECLARING
#   define DEFSYM(__name, __s) rbb::object SY_##__name = rbb::symbol(__s);
#else
#   define DEFSYM(__name, __s) extern rbb::object SY_##__name;
#endif

DEFSYM(ASTER, "*")
DEFSYM(CIRC, "^")
DEFSYM(DASH, "-")
DEFSYM(DEQ, "==")
DEFSYM(DLT, "<<")
DEFSYM(DLTQM, "<<?")
DEFSYM(EQ, "=")
DEFSYM(GE, ">=")
DEFSYM(GT, ">")
DEFSYM(I_ARR, "[|]")
DEFSYM(I_BL, "[{}]")
DEFSYM(I_BOOL, "[?]")
DEFSYM(I_SYM, "[a]")
DEFSYM(I_TABLE, "[:]")
DEFSYM(L_AND, "/\\")
DEFSYM(L_NOT, "><")
DEFSYM(L_OR, "\\/")
DEFSYM(LDF_BACK, "back")
DEFSYM(LDF_CHVAL, "char_val")
DEFSYM(LDF_EVAL, "eval")
DEFSYM(LDF_ERES, "eval_res")
DEFSYM(LDF_EXPVAL, "expr_val")
DEFSYM(LDF_EXPSKIP, "expr_skip")
DEFSYM(LDF_LIT, "lit")
DEFSYM(LDF_PUNTIL, "parse_until")
DEFSYM(LDF_REVAL, "run_eval")
DEFSYM(LDF_SKIP, "skip")
DEFSYM(LDF_TRIG, "trigger")
DEFSYM(NE, "/=")
DEFSYM(LE, "<=")
DEFSYM(LT, "<")
DEFSYM(PLUS, "+")
DEFSYM(QM, "?")
DEFSYM(SLASH, "/")

#undef DEFSYM

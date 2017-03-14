#ifdef DECLARING
#   define DEFSYM(__name, __s) rbb::object SY_##__name = rbb::symbol(__s);
#else
#   define DEFSYM(__name, __s) extern rbb::object SY_##__name;
#endif

DEFSYM(ASTER, "*")
DEFSYM(BR_DOL, "[$]")
DEFSYM(BR_EXCL, "[!]")
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
DEFSYM(NE, "/=")
DEFSYM(LE, "<=")
DEFSYM(LT, "<")
DEFSYM(PLUS, "+")
DEFSYM(QM, "?")
DEFSYM(SLASH, "/")

#undef DEFSYM

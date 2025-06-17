/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 4 "source/l25.y"


#ifndef YYLTYPE_IS_DECLARED
#define YYLTYPE_IS_DECLARED 1
typedef struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} YYLTYPE;
#endif

extern YYLTYPE yylloc;
int yylex();
void yyerror(const char*);
void yyerror(YYLTYPE* yylloc, const char*);

#include <bit>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct Code
{
    std::string op;
    long long val;
};

int preStk;
std::vector<int> codeStk;
std::vector<Code> code, codePre;

int emit(const std::string& op, long long val = 0)
{
    int idx = code.size();
    code.push_back({op, val});
    return idx;
}
int emitPre(const std::string& op, long long val = 0)
{
    int idx = preStk;
    codePre.push_back({op, val});
    if (op == "ADDSP")
        preStk += val;
    else
        preStk++;
    return idx;
}

enum TypeKind
{
    TYPE_INT,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_VOID,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_PTR,
    TYPE_ARR,
    TYPE_FUNC,
    TYPE_STRUCT
};
struct TypeInfo
{
    std::string name;
    TypeKind kind;
    int size, index;
};

std::vector<TypeInfo> typeTable;
std::map<int, int> ptrTypeMap;
std::map<int, std::map<int, int>> arrTypeMap;
std::map<int, std::map<std::vector<int>, int>> funcTypeMap;

void initBasicTypes()
{
    typeTable.push_back({"int", TYPE_INT, 1, -1});
    typeTable.push_back({"bool", TYPE_BOOL, 1, -1});
    typeTable.push_back({"char", TYPE_CHAR, 1, -1});
    typeTable.push_back({"void", TYPE_VOID, 0, -1});
    typeTable.push_back({"float", TYPE_FLOAT, 1, -1});
    typeTable.push_back({"string", TYPE_STRING, 256, -1});
}

struct StructDef
{
    struct Field
    {
        int type;
        int offset;
    };
    std::map<std::string, Field> field;
};
struct PtrDef
{
    int baseType;
};
struct ArrDef 
{
    int baseType;
    int length;
};
struct FuncDef
{
    int retType;
    std::vector<int> argType;
    int argSize;
};

std::vector<PtrDef> ptrDef;
std::vector<ArrDef> arrDef;
std::vector<FuncDef> funcDef;
std::vector<StructDef> structDef;

int ptrType(int baseType)
{
    if (ptrTypeMap.count(baseType))
        return ptrTypeMap[baseType];
    PtrDef def = {baseType};
    int ptrIdx = ptrDef.size();
    ptrDef.push_back(def);

    TypeInfo info =
    {
        typeTable[baseType].name + "*",
        TYPE_PTR,
        1,
        ptrIdx
    };
    int type = typeTable.size();
    typeTable.push_back(info);
    return ptrTypeMap[baseType] = type;
}
int arrType(int baseType, int length)
{
    if (arrTypeMap.count(baseType) && arrTypeMap[baseType].count(length))
        return arrTypeMap[baseType][length];
    ArrDef def = {baseType, length};
    int arrIdx = arrDef.size();
    arrDef.push_back(def);

    TypeInfo info =
    {
        typeTable[baseType].name + "[" + std::to_string(length) + "]",
        TYPE_ARR,
        typeTable[baseType].size * length,
        arrIdx
    };
    int type = typeTable.size();
    typeTable.push_back(info);
    return arrTypeMap[baseType][length] = type;
}
int funcType(int retType, const std::vector<int>& argType)
{
    if (funcTypeMap.count(retType) && funcTypeMap[retType].count(argType))
        return funcTypeMap[retType][argType];
    FuncDef def = {retType, argType, 0};
    for (int type: argType)
        def.argSize += typeTable[type].size;
    int funcIdx = funcDef.size();
    funcDef.push_back(def);

    std::string name = typeTable[retType].name + "(";
    for (int type: argType)
        name += typeTable[type].name + ", ";
    if (!argType.empty())
        name.pop_back(), name.pop_back();
    name += ")";

    TypeInfo info = {name, TYPE_FUNC, 1, funcIdx};
    int type = typeTable.size();
    typeTable.push_back(info);
    return funcTypeMap[retType][argType] = type;
}
int structType(const std::string& name)
{
    TypeInfo info = {"struct " + name, TYPE_STRUCT, 0, 0};
    int type = typeTable.size();
    typeTable.push_back(info);
    return type;
}
void fixStructType(int type,
    const std::map<std::string, StructDef::Field>& field)
{
    StructDef def = {field};
    int structIdx = structDef.size();
    structDef.push_back(def);

    int size = 0;
    for (auto& [_, f]: structDef.back().field)
        f.offset = size, size += typeTable[f.type].size;

    typeTable[type].size = size;
    typeTable[type].index = structIdx;
}
enum ScopeKind
{
    SCOPE_LOCAL,
    SCOPE_MEMBER,
    SCOPE_GLOBAL
};
struct Sym
{
    int type, addr;
    ScopeKind kind;
};

int curLocal;
int entType, entAddr;
std::vector<int> paramStk;
std::vector<std::vector<int>> typeStk;
std::vector<std::map<std::string, StructDef::Field>> fieldStk;

std::vector<std::map<std::string, Sym>> scopes(1);

Sym& find(const std::string& id)
{
    for (int i = scopes.size(); i--;)
        if (scopes[i].count(id))
            return scopes[i][id];
    yyerror(("syntax error, " + id + " is undeclared").c_str());
    exit(1);
}
void declare(const std::string& id, int type, int addr, ScopeKind kind)
{
    if (scopes.back().count(id))
    {
        yyerror(("syntax error, " + id + " is redeclared").c_str());
        exit(1);
    }
    scopes.back()[id] = {type, addr, kind};
}

std::string unescapeLiteral(const char *raw)
{
    int len = strlen(raw);
    std::string out;
    out.reserve(len > 2 ? len - 2 : 0);
    for (int i = 1; i + 1 < len; ++i) {
        if (raw[i] == '\\' && i + 1 + 1 < len) {
            switch (raw[i + 1]) {
                case 'n':
                    out.push_back('\n');
                    break;
                case 't':
                    out.push_back('\t');
                    break;
                case '\\':
                    out.push_back('\\');
                    break;
                case '"':
                    out.push_back('"');
                    break;
                default:
                    out.push_back(raw[i + 1]);
            }
            i++;
        } else {
            out.push_back(raw[i]);
        }
    }
    return out;
}

void generateCode()
{
    for (const auto& [op, val] : codePre)
        if (op == "PUSH")
            std::cout << op << " " << val + codePre.size() << std::endl;
        else
            std::cout << op << " " << val << std::endl;
    for (const auto& [op, val] : code)
        if (op == "JMP" || op == "JPC" || op == "PUSHE")
            std::cout << op << " " << val + codePre.size() << std::endl;
        else
            std::cout << op << " " << val << std::endl;
    return;
}


#line 356 "build/l25.tab.cc"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "l25.tab.hh"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_PROGRAM = 3,                    /* PROGRAM  */
  YYSYMBOL_STRUCT = 4,                     /* STRUCT  */
  YYSYMBOL_FUNC = 5,                       /* FUNC  */
  YYSYMBOL_MAIN = 6,                       /* MAIN  */
  YYSYMBOL_RETURN = 7,                     /* RETURN  */
  YYSYMBOL_LET = 8,                        /* LET  */
  YYSYMBOL_INT = 9,                        /* INT  */
  YYSYMBOL_BOOL = 10,                      /* BOOL  */
  YYSYMBOL_CHAR = 11,                      /* CHAR  */
  YYSYMBOL_VOID = 12,                      /* VOID  */
  YYSYMBOL_FLOAT = 13,                     /* FLOAT  */
  YYSYMBOL_STRING = 14,                    /* STRING  */
  YYSYMBOL_IF = 15,                        /* IF  */
  YYSYMBOL_ELSE = 16,                      /* ELSE  */
  YYSYMBOL_WHILE = 17,                     /* WHILE  */
  YYSYMBOL_EQ = 18,                        /* EQ  */
  YYSYMBOL_NEQ = 19,                       /* NEQ  */
  YYSYMBOL_LE = 20,                        /* LE  */
  YYSYMBOL_GE = 21,                        /* GE  */
  YYSYMBOL_AND = 22,                       /* AND  */
  YYSYMBOL_OR = 23,                        /* OR  */
  YYSYMBOL_LSH = 24,                       /* LSH  */
  YYSYMBOL_RSH = 25,                       /* RSH  */
  YYSYMBOL_TRY = 26,                       /* TRY  */
  YYSYMBOL_CATCH = 27,                     /* CATCH  */
  YYSYMBOL_INPUT = 28,                     /* INPUT  */
  YYSYMBOL_OUTPUT = 29,                    /* OUTPUT  */
  YYSYMBOL_ALLOC = 30,                     /* ALLOC  */
  YYSYMBOL_FREE = 31,                      /* FREE  */
  YYSYMBOL_ARROW = 32,                     /* ARROW  */
  YYSYMBOL_THIS = 33,                      /* THIS  */
  YYSYMBOL_SIZEOF = 34,                    /* SIZEOF  */
  YYSYMBOL_IDENT = 35,                     /* IDENT  */
  YYSYMBOL_INT_CONST = 36,                 /* INT_CONST  */
  YYSYMBOL_BOOL_CONST = 37,                /* BOOL_CONST  */
  YYSYMBOL_CHAR_CONST = 38,                /* CHAR_CONST  */
  YYSYMBOL_VOID_CONST = 39,                /* VOID_CONST  */
  YYSYMBOL_FLOAT_CONST = 40,               /* FLOAT_CONST  */
  YYSYMBOL_STRING_CONST = 41,              /* STRING_CONST  */
  YYSYMBOL_42_ = 42,                       /* '='  */
  YYSYMBOL_43_ = 43,                       /* '<'  */
  YYSYMBOL_44_ = 44,                       /* '>'  */
  YYSYMBOL_45_ = 45,                       /* '&'  */
  YYSYMBOL_46_ = 46,                       /* '|'  */
  YYSYMBOL_47_ = 47,                       /* '^'  */
  YYSYMBOL_48_ = 48,                       /* '+'  */
  YYSYMBOL_49_ = 49,                       /* '-'  */
  YYSYMBOL_50_ = 50,                       /* '*'  */
  YYSYMBOL_51_ = 51,                       /* '/'  */
  YYSYMBOL_UNARY = 52,                     /* UNARY  */
  YYSYMBOL_53_ = 53,                       /* '['  */
  YYSYMBOL_54_ = 54,                       /* '.'  */
  YYSYMBOL_55_ = 55,                       /* '{'  */
  YYSYMBOL_56_ = 56,                       /* '}'  */
  YYSYMBOL_57_ = 57,                       /* ':'  */
  YYSYMBOL_58_ = 58,                       /* ';'  */
  YYSYMBOL_59_ = 59,                       /* '('  */
  YYSYMBOL_60_ = 60,                       /* ')'  */
  YYSYMBOL_61_ = 61,                       /* ','  */
  YYSYMBOL_62_ = 62,                       /* ']'  */
  YYSYMBOL_63_ = 63,                       /* '%'  */
  YYSYMBOL_64_ = 64,                       /* '~'  */
  YYSYMBOL_65_ = 65,                       /* '!'  */
  YYSYMBOL_YYACCEPT = 66,                  /* $accept  */
  YYSYMBOL_program = 67,                   /* program  */
  YYSYMBOL_68_1 = 68,                      /* $@1  */
  YYSYMBOL_69_2 = 69,                      /* $@2  */
  YYSYMBOL_global_list_opt = 70,           /* global_list_opt  */
  YYSYMBOL_declare_def = 71,               /* declare_def  */
  YYSYMBOL_struct_def = 72,                /* struct_def  */
  YYSYMBOL_73_3 = 73,                      /* $@3  */
  YYSYMBOL_field_list_opt = 74,            /* field_list_opt  */
  YYSYMBOL_field_list = 75,                /* field_list  */
  YYSYMBOL_field_def = 76,                 /* field_def  */
  YYSYMBOL_func_def = 77,                  /* func_def  */
  YYSYMBOL_78_4 = 78,                      /* $@4  */
  YYSYMBOL_79_5 = 79,                      /* $@5  */
  YYSYMBOL_80_6 = 80,                      /* $@6  */
  YYSYMBOL_81_7 = 81,                      /* $@7  */
  YYSYMBOL_arg_list_opt = 82,              /* arg_list_opt  */
  YYSYMBOL_arg_list = 83,                  /* arg_list  */
  YYSYMBOL_arg_def = 84,                   /* arg_def  */
  YYSYMBOL_type_spec = 85,                 /* type_spec  */
  YYSYMBOL_86_8 = 86,                      /* $@8  */
  YYSYMBOL_type_spec_list_opt = 87,        /* type_spec_list_opt  */
  YYSYMBOL_type_spec_list = 88,            /* type_spec_list  */
  YYSYMBOL_stmt_list = 89,                 /* stmt_list  */
  YYSYMBOL_90_9 = 90,                      /* $@9  */
  YYSYMBOL_91_10 = 91,                     /* $@10  */
  YYSYMBOL_stmt = 92,                      /* stmt  */
  YYSYMBOL_declare_stmt = 93,              /* declare_stmt  */
  YYSYMBOL_assign_stmt = 94,               /* assign_stmt  */
  YYSYMBOL_const_expr = 95,                /* const_expr  */
  YYSYMBOL_left_expr = 96,                 /* left_expr  */
  YYSYMBOL_right_expr = 97,                /* right_expr  */
  YYSYMBOL_compare_expr = 98,              /* compare_expr  */
  YYSYMBOL_bitwise_expr = 99,              /* bitwise_expr  */
  YYSYMBOL_bitwise_term = 100,             /* bitwise_term  */
  YYSYMBOL_arith_expr = 101,               /* arith_expr  */
  YYSYMBOL_arith_term = 102,               /* arith_term  */
  YYSYMBOL_factor = 103,                   /* factor  */
  YYSYMBOL_if_stmt = 104,                  /* if_stmt  */
  YYSYMBOL_105_11 = 105,                   /* $@11  */
  YYSYMBOL_106_12 = 106,                   /* $@12  */
  YYSYMBOL_else_opt = 107,                 /* else_opt  */
  YYSYMBOL_108_13 = 108,                   /* $@13  */
  YYSYMBOL_while_stmt = 109,               /* while_stmt  */
  YYSYMBOL_110_14 = 110,                   /* $@14  */
  YYSYMBOL_111_15 = 111,                   /* $@15  */
  YYSYMBOL_try_catch_stmt = 112,           /* try_catch_stmt  */
  YYSYMBOL_113_16 = 113,                   /* $@16  */
  YYSYMBOL_114_17 = 114,                   /* $@17  */
  YYSYMBOL_115_18 = 115,                   /* $@18  */
  YYSYMBOL_func_call = 116,                /* func_call  */
  YYSYMBOL_117_19 = 117,                   /* $@19  */
  YYSYMBOL_118_20 = 118,                   /* $@20  */
  YYSYMBOL_119_21 = 119,                   /* $@21  */
  YYSYMBOL_param_list_opt = 120,           /* param_list_opt  */
  YYSYMBOL_param_list = 121,               /* param_list  */
  YYSYMBOL_input_call = 122,               /* input_call  */
  YYSYMBOL_in_arg_list = 123,              /* in_arg_list  */
  YYSYMBOL_output_call = 124,              /* output_call  */
  YYSYMBOL_out_arg_list = 125,             /* out_arg_list  */
  YYSYMBOL_alloc_call = 126,               /* alloc_call  */
  YYSYMBOL_free_call = 127                 /* free_call  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  4
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   971

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  66
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  62
/* YYNRULES -- Number of rules.  */
#define YYNRULES  140
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  273

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   297


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    65,     2,     2,     2,    63,    45,     2,
      59,    60,    50,    48,    61,    49,    54,    51,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    57,    58,
      43,    42,    44,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    53,     2,    62,    47,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    55,    46,    56,    64,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    52
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   327,   327,   334,   326,   365,   366,   367,   368,   372,
     381,   380,   401,   402,   406,   407,   411,   426,   437,   447,
     456,   425,   489,   490,   494,   495,   499,   508,   512,   516,
     520,   524,   528,   532,   543,   547,   552,   551,   563,   564,
     568,   572,   576,   583,   584,   586,   585,   594,   593,   604,
     605,   606,   607,   608,   609,   613,   614,   615,   619,   623,
     628,   646,   660,   692,   697,   702,   707,   711,   717,   729,
     737,   746,   758,   793,   812,   842,   858,   874,   881,   903,
     925,   947,   969,   997,  1025,  1032,  1048,  1064,  1080,  1087,
    1103,  1119,  1126,  1173,  1219,  1226,  1261,  1283,  1299,  1306,
    1329,  1333,  1337,  1421,  1425,  1429,  1433,  1447,  1461,  1473,
    1493,  1500,  1492,  1508,  1514,  1513,  1538,  1543,  1537,  1566,
    1573,  1577,  1565,  1602,  1601,  1638,  1637,  1699,  1698,  1772,
    1773,  1777,  1793,  1812,  1816,  1857,  1901,  1905,  1934,  1966,
    1974
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "PROGRAM", "STRUCT",
  "FUNC", "MAIN", "RETURN", "LET", "INT", "BOOL", "CHAR", "VOID", "FLOAT",
  "STRING", "IF", "ELSE", "WHILE", "EQ", "NEQ", "LE", "GE", "AND", "OR",
  "LSH", "RSH", "TRY", "CATCH", "INPUT", "OUTPUT", "ALLOC", "FREE",
  "ARROW", "THIS", "SIZEOF", "IDENT", "INT_CONST", "BOOL_CONST",
  "CHAR_CONST", "VOID_CONST", "FLOAT_CONST", "STRING_CONST", "'='", "'<'",
  "'>'", "'&'", "'|'", "'^'", "'+'", "'-'", "'*'", "'/'", "UNARY", "'['",
  "'.'", "'{'", "'}'", "':'", "';'", "'('", "')'", "','", "']'", "'%'",
  "'~'", "'!'", "$accept", "program", "$@1", "$@2", "global_list_opt",
  "declare_def", "struct_def", "$@3", "field_list_opt", "field_list",
  "field_def", "func_def", "$@4", "$@5", "$@6", "$@7", "arg_list_opt",
  "arg_list", "arg_def", "type_spec", "$@8", "type_spec_list_opt",
  "type_spec_list", "stmt_list", "$@9", "$@10", "stmt", "declare_stmt",
  "assign_stmt", "const_expr", "left_expr", "right_expr", "compare_expr",
  "bitwise_expr", "bitwise_term", "arith_expr", "arith_term", "factor",
  "if_stmt", "$@11", "$@12", "else_opt", "$@13", "while_stmt", "$@14",
  "$@15", "try_catch_stmt", "$@16", "$@17", "$@18", "func_call", "$@19",
  "$@20", "$@21", "param_list_opt", "param_list", "input_call",
  "in_arg_list", "output_call", "out_arg_list", "alloc_call", "free_call", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-91)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-105)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      45,   -32,    57,   -19,   -91,   -91,   -91,   124,    30,    38,
      26,    50,   -91,   -91,   -91,    37,    28,   -91,    65,   -91,
     -91,   849,   917,    78,    96,   102,   917,   109,   -91,   -91,
     -91,   -91,   -91,   -91,    87,    97,   111,   101,   114,   116,
     123,   125,   127,   -91,   -91,   -91,   -91,   -91,   -91,   906,
     906,   906,   906,   -91,   906,   906,   906,    29,   323,   110,
     -91,   -91,   -91,    17,   156,   -91,   -91,   -91,   158,   -91,
     -91,   163,   -91,    43,   144,   154,    78,   -91,   161,   151,
     152,   -91,   -91,    36,   -41,   906,   -91,   -91,   906,   906,
     906,   906,   917,   -91,   -14,   -91,   -91,   -12,   156,   156,
     156,   849,   -15,   -91,    80,    83,    86,    12,   156,   156,
     156,   -91,   183,   906,   -91,   166,   171,   -91,   906,   906,
     189,   197,   917,   -91,   -91,   917,   177,    96,   -91,   906,
     917,   -13,   906,   849,   -14,    81,   131,   103,    -9,    -2,
      16,   906,   381,   906,   906,   -91,   906,   906,   906,   906,
     906,   906,   906,   906,   906,   906,   906,   906,   906,   906,
     906,   906,   178,     0,   849,   -91,   -91,   131,   -18,   180,
     184,    53,    43,   -91,   -91,   195,   131,    -4,   190,     4,
     439,   -91,   906,   -91,   906,   -91,   -91,   -91,   131,   186,
     187,   194,   -91,   -91,   105,   105,   105,   105,   105,   105,
      83,    83,    83,    86,    86,    12,    12,   156,   156,   156,
     -91,   -91,   497,   -91,   -91,   -91,   -91,   917,   -91,    43,
     193,   199,   906,   -91,   201,   -91,   -14,   131,   -91,   906,
     -91,   196,   906,   906,   -38,   -91,   917,   131,   849,   -91,
     231,   131,   -91,   203,   204,   -91,    43,   555,   849,   206,
     -91,   -91,   849,   -91,   613,   -91,   675,   249,   -91,   849,
     906,   212,   -91,   733,    10,   -91,   -91,   -91,   849,   213,
     791,   -91,   -91
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     1,     2,     5,     0,     0,     0,
       0,     0,     6,     7,     8,     0,     0,     3,     0,    10,
      17,     0,     0,    12,    22,     0,     0,     0,    27,    28,
      29,    30,    31,    32,     0,     0,     0,     0,     0,     0,
       0,     0,    70,    63,    64,    65,    66,    67,    68,     0,
       0,     0,     0,    45,     0,     0,     0,     0,     0,     0,
      49,    50,   100,    99,     0,    51,    52,    53,    54,    55,
      56,    57,    58,     9,     0,     0,    13,    14,     0,     0,
      23,    24,    33,     0,     0,     0,   116,   119,     0,     0,
       0,     0,     0,   123,   105,   103,   104,    99,   107,   106,
      71,     0,     0,    77,    84,    88,    91,    94,    98,   108,
     109,    34,     0,     0,    47,     0,     0,    43,     0,     0,
       0,     0,     0,    11,    15,     0,     0,     0,    36,     0,
       0,     0,     0,     0,   134,     0,   137,     0,     0,     0,
       0,   129,     0,     0,     0,   101,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     4,    44,    62,     0,    73,
      74,     0,    26,    18,    25,    38,    61,    59,     0,     0,
       0,   133,     0,   136,     0,   139,   140,    69,   131,     0,
     130,     0,    75,    76,    82,    83,    80,    81,    78,    79,
      85,    86,    87,    89,    90,    92,    93,    95,    96,    97,
      35,   102,     0,    72,   125,   127,    16,     0,    42,    40,
       0,    39,     0,   110,     0,   120,   135,   138,   124,     0,
      46,     0,   129,   129,     0,    37,     0,    60,     0,   117,
       0,   132,    48,     0,     0,    19,    41,     0,     0,     0,
     126,   128,     0,   111,     0,   121,     0,   113,   118,     0,
       0,     0,   112,     0,     0,   114,   122,    20,     0,     0,
       0,    21,   115
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,
     198,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   145,   -20,
     -91,   -91,   -91,   -90,   -91,   -91,   -33,   -91,   -91,   -91,
       3,   -35,    33,   -31,    18,    25,    35,   -21,   -91,   -91,
     -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,   -91,
      32,   -91,   -91,   -91,   -30,   -91,   -91,   -91,   -91,   -91,
      56,   -91
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     2,     6,    21,     7,    12,    13,    23,    75,    76,
      77,    14,    24,   217,   252,   269,    79,    80,    81,    57,
     175,   220,   221,    58,   101,   164,    59,    60,    61,    62,
      97,   188,   103,   104,   105,   106,   107,   108,    65,   238,
     257,   262,   268,    66,   132,   248,    67,   133,   240,   259,
      95,   141,   232,   233,   189,   190,    69,   135,    70,   137,
      96,    72
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      64,   129,    73,     3,   143,   144,    83,   143,   144,   143,
     144,   142,   111,   143,   144,   112,   130,   245,   -99,   102,
     143,   144,   143,   144,    63,   116,   143,   144,    64,    98,
      99,   100,   143,   144,   109,   110,     5,    64,   222,   119,
     120,   119,   120,   180,   213,   145,   111,   178,     1,   112,
     131,   185,    94,    68,   136,   138,   139,     4,   186,   118,
     211,    63,   159,   160,   224,    15,   111,    64,   267,   112,
     119,   120,   140,    16,   212,   161,   187,    71,   163,   111,
      64,    17,   112,   167,   168,    18,   111,    20,   113,   112,
      68,   134,    19,   111,   176,   128,   112,   179,   146,   147,
     148,   149,   171,   111,    63,   172,   112,   155,   156,   116,
     177,   216,    64,    74,    71,   194,   195,   196,   197,   198,
     199,    64,    22,   150,   151,   152,   153,   154,     8,     9,
      10,    78,    11,    68,   157,   158,    63,    82,   207,   208,
     209,   181,   182,    64,    84,    63,    85,   116,   247,   227,
     152,   153,   154,   143,   144,   219,    86,    71,   254,    64,
      88,    64,   256,   183,   184,    68,    87,    63,   117,   263,
     200,   201,   202,    89,    68,    90,   192,   193,   270,   116,
     203,   204,    91,    63,    92,   226,    93,   237,   121,    71,
    -103,    64,   205,   206,   241,  -104,    68,   234,    71,    25,
      26,   122,   243,   244,    28,    29,    30,    31,    32,    33,
     123,   126,    68,   127,   116,    63,   246,    64,   125,   162,
      71,   116,   165,   116,   169,   264,    64,    64,   218,   166,
     116,    64,   170,    64,   173,    64,    71,   116,    64,   214,
     210,    63,    64,   215,    68,   223,   228,    64,   229,    64,
      63,    63,   230,   235,   242,    63,   239,    63,   249,    63,
     236,   255,    63,   250,   251,   261,    63,   265,    71,   271,
      68,    63,   174,    63,   124,     0,     0,     0,     0,    68,
      68,     0,     0,     0,    68,     0,    68,     0,    68,     0,
       0,    68,     0,     0,    71,    68,     0,     0,     0,     0,
      68,     0,    68,    71,    71,     0,     0,     0,    71,     0,
      71,     0,    71,     0,     0,    71,     0,     0,     0,    71,
       0,     0,     0,     0,    71,     0,    71,    25,    26,     0,
       0,    27,    28,    29,    30,    31,    32,    33,    34,     0,
      35,     0,     0,     0,     0,     0,     0,     0,     0,    36,
       0,    37,    38,    39,    40,     0,     0,    41,    42,    43,
      44,    45,    46,    47,    48,     0,     0,     0,    49,     0,
       0,    50,    51,    52,     0,     0,     0,     0,   114,   115,
       0,     0,    54,     0,     0,    25,    26,    55,    56,    27,
      28,    29,    30,    31,    32,    33,    34,     0,    35,     0,
       0,     0,     0,     0,     0,     0,     0,    36,     0,    37,
      38,    39,    40,     0,     0,    41,    42,    43,    44,    45,
      46,    47,    48,     0,     0,     0,    49,     0,     0,    50,
      51,    52,     0,     0,     0,     0,   114,   191,     0,     0,
      54,     0,     0,    25,    26,    55,    56,    27,    28,    29,
      30,    31,    32,    33,    34,     0,    35,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,    37,    38,    39,
      40,     0,     0,    41,    42,    43,    44,    45,    46,    47,
      48,     0,     0,     0,    49,     0,     0,    50,    51,    52,
       0,     0,     0,     0,   114,   225,     0,     0,    54,     0,
       0,    25,    26,    55,    56,    27,    28,    29,    30,    31,
      32,    33,    34,     0,    35,     0,     0,     0,     0,     0,
       0,     0,     0,    36,     0,    37,    38,    39,    40,     0,
       0,    41,    42,    43,    44,    45,    46,    47,    48,     0,
       0,     0,    49,     0,     0,    50,    51,    52,     0,     0,
       0,     0,   114,   231,     0,     0,    54,     0,     0,    25,
      26,    55,    56,    27,    28,    29,    30,    31,    32,    33,
      34,     0,    35,     0,     0,     0,     0,     0,     0,     0,
       0,    36,     0,    37,    38,    39,    40,     0,     0,    41,
      42,    43,    44,    45,    46,    47,    48,     0,     0,     0,
      49,     0,     0,    50,    51,    52,     0,     0,     0,     0,
     114,   253,     0,     0,    54,     0,     0,    25,    26,    55,
      56,    27,    28,    29,    30,    31,    32,    33,    34,     0,
      35,     0,     0,     0,     0,     0,     0,     0,     0,    36,
       0,    37,    38,    39,    40,     0,     0,    41,    42,    43,
      44,    45,    46,    47,    48,     0,     0,     0,    49,     0,
       0,    50,    51,    52,     0,     0,     0,     0,   114,   258,
       0,     0,    54,     0,     0,     0,     0,    55,    56,    25,
      26,     0,   260,    27,    28,    29,    30,    31,    32,    33,
      34,     0,    35,     0,     0,     0,     0,     0,     0,     0,
       0,    36,     0,    37,    38,    39,    40,     0,     0,    41,
      42,    43,    44,    45,    46,    47,    48,     0,     0,     0,
      49,     0,     0,    50,    51,    52,     0,     0,     0,     0,
     114,     0,     0,     0,    54,     0,     0,    25,    26,    55,
      56,    27,    28,    29,    30,    31,    32,    33,    34,     0,
      35,     0,     0,     0,     0,     0,     0,     0,     0,    36,
       0,    37,    38,    39,    40,     0,     0,    41,    42,    43,
      44,    45,    46,    47,    48,     0,     0,     0,    49,     0,
       0,    50,    51,    52,     0,     0,     0,     0,   114,   266,
       0,     0,    54,     0,     0,    25,    26,    55,    56,    27,
      28,    29,    30,    31,    32,    33,    34,     0,    35,     0,
       0,     0,     0,     0,     0,     0,     0,    36,     0,    37,
      38,    39,    40,     0,     0,    41,    42,    43,    44,    45,
      46,    47,    48,     0,     0,     0,    49,     0,     0,    50,
      51,    52,     0,     0,     0,     0,   114,   272,     0,     0,
      54,     0,     0,    25,    26,    55,    56,    27,    28,    29,
      30,    31,    32,    33,    34,     0,    35,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,    37,    38,    39,
      40,     0,     0,    41,    42,    43,    44,    45,    46,    47,
      48,     0,     0,     0,    49,     0,     0,    50,    51,    52,
       0,     0,     0,     0,    53,     0,     0,     0,    54,     0,
      25,    26,     0,    55,    56,    28,    29,    30,    31,    32,
      33,    25,    26,     0,     0,     0,    28,    29,    30,    31,
      32,    33,     0,     0,     0,     0,    39,     0,     0,     0,
      41,    42,    43,    44,    45,    46,    47,    48,     0,     0,
       0,    49,     0,     0,    50,    51,    52,     0,     0,     0,
       0,     0,     0,     0,     0,    54,     0,     0,     0,     0,
      55,    56
};

static const yytype_int16 yycheck[] =
{
      21,    42,    22,    35,    22,    23,    26,    22,    23,    22,
      23,   101,    50,    22,    23,    53,    57,    55,    32,    54,
      22,    23,    22,    23,    21,    58,    22,    23,    49,    50,
      51,    52,    22,    23,    55,    56,    55,    58,    42,    53,
      54,    53,    54,   133,    62,    60,    50,    60,     3,    53,
      85,    60,    49,    21,    89,    90,    91,     0,    60,    42,
      60,    58,    50,    51,    60,    35,    50,    88,    58,    53,
      53,    54,    92,    35,   164,    63,    60,    21,   113,    50,
     101,    55,    53,   118,   119,    35,    50,    59,    59,    53,
      58,    88,    55,    50,   129,    59,    53,   132,    18,    19,
      20,    21,   122,    50,   101,   125,    53,    24,    25,   142,
     130,    58,   133,    35,    58,   146,   147,   148,   149,   150,
     151,   142,    57,    43,    44,    45,    46,    47,     4,     5,
       6,    35,     8,   101,    48,    49,   133,    35,   159,   160,
     161,    60,    61,   164,    35,   142,    59,   180,   238,   184,
      45,    46,    47,    22,    23,   175,    59,   101,   248,   180,
      59,   182,   252,    60,    61,   133,    55,   164,    58,   259,
     152,   153,   154,    59,   142,    59,   143,   144,   268,   212,
     155,   156,    59,   180,    59,   182,    59,   222,    32,   133,
      32,   212,   157,   158,   229,    32,   164,   217,   142,     4,
       5,    57,   232,   233,     9,    10,    11,    12,    13,    14,
      56,    60,   180,    61,   247,   212,   236,   238,    57,    36,
     164,   254,    56,   256,    35,   260,   247,   248,    33,    58,
     263,   252,    35,   254,    57,   256,   180,   270,   259,    59,
      62,   238,   263,    59,   212,    55,    60,   268,    61,   270,
     247,   248,    58,    60,    58,   252,    55,   254,    27,   256,
      61,    55,   259,    60,    60,    16,   263,    55,   212,    56,
     238,   268,   127,   270,    76,    -1,    -1,    -1,    -1,   247,
     248,    -1,    -1,    -1,   252,    -1,   254,    -1,   256,    -1,
      -1,   259,    -1,    -1,   238,   263,    -1,    -1,    -1,    -1,
     268,    -1,   270,   247,   248,    -1,    -1,    -1,   252,    -1,
     254,    -1,   256,    -1,    -1,   259,    -1,    -1,    -1,   263,
      -1,    -1,    -1,    -1,   268,    -1,   270,     4,     5,    -1,
      -1,     8,     9,    10,    11,    12,    13,    14,    15,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,
      -1,    28,    29,    30,    31,    -1,    -1,    34,    35,    36,
      37,    38,    39,    40,    41,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    -1,    -1,    -1,    -1,    55,    56,
      -1,    -1,    59,    -1,    -1,     4,     5,    64,    65,     8,
       9,    10,    11,    12,    13,    14,    15,    -1,    17,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,    28,
      29,    30,    31,    -1,    -1,    34,    35,    36,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    -1,    -1,    -1,    -1,    55,    56,    -1,    -1,
      59,    -1,    -1,     4,     5,    64,    65,     8,     9,    10,
      11,    12,    13,    14,    15,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    26,    -1,    28,    29,    30,
      31,    -1,    -1,    34,    35,    36,    37,    38,    39,    40,
      41,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      -1,    -1,    -1,    -1,    55,    56,    -1,    -1,    59,    -1,
      -1,     4,     5,    64,    65,     8,     9,    10,    11,    12,
      13,    14,    15,    -1,    17,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    26,    -1,    28,    29,    30,    31,    -1,
      -1,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
      -1,    -1,    45,    -1,    -1,    48,    49,    50,    -1,    -1,
      -1,    -1,    55,    56,    -1,    -1,    59,    -1,    -1,     4,
       5,    64,    65,     8,     9,    10,    11,    12,    13,    14,
      15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    26,    -1,    28,    29,    30,    31,    -1,    -1,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    -1,    -1,    -1,    -1,
      55,    56,    -1,    -1,    59,    -1,    -1,     4,     5,    64,
      65,     8,     9,    10,    11,    12,    13,    14,    15,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,
      -1,    28,    29,    30,    31,    -1,    -1,    34,    35,    36,
      37,    38,    39,    40,    41,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    -1,    -1,    -1,    -1,    55,    56,
      -1,    -1,    59,    -1,    -1,    -1,    -1,    64,    65,     4,
       5,    -1,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    -1,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    26,    -1,    28,    29,    30,    31,    -1,    -1,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
      45,    -1,    -1,    48,    49,    50,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    -1,    59,    -1,    -1,     4,     5,    64,
      65,     8,     9,    10,    11,    12,    13,    14,    15,    -1,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,
      -1,    28,    29,    30,    31,    -1,    -1,    34,    35,    36,
      37,    38,    39,    40,    41,    -1,    -1,    -1,    45,    -1,
      -1,    48,    49,    50,    -1,    -1,    -1,    -1,    55,    56,
      -1,    -1,    59,    -1,    -1,     4,     5,    64,    65,     8,
       9,    10,    11,    12,    13,    14,    15,    -1,    17,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,    28,
      29,    30,    31,    -1,    -1,    34,    35,    36,    37,    38,
      39,    40,    41,    -1,    -1,    -1,    45,    -1,    -1,    48,
      49,    50,    -1,    -1,    -1,    -1,    55,    56,    -1,    -1,
      59,    -1,    -1,     4,     5,    64,    65,     8,     9,    10,
      11,    12,    13,    14,    15,    -1,    17,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    26,    -1,    28,    29,    30,
      31,    -1,    -1,    34,    35,    36,    37,    38,    39,    40,
      41,    -1,    -1,    -1,    45,    -1,    -1,    48,    49,    50,
      -1,    -1,    -1,    -1,    55,    -1,    -1,    -1,    59,    -1,
       4,     5,    -1,    64,    65,     9,    10,    11,    12,    13,
      14,     4,     5,    -1,    -1,    -1,     9,    10,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    30,    -1,    -1,    -1,
      34,    35,    36,    37,    38,    39,    40,    41,    -1,    -1,
      -1,    45,    -1,    -1,    48,    49,    50,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    59,    -1,    -1,    -1,    -1,
      64,    65
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,    67,    35,     0,    55,    68,    70,     4,     5,
       6,     8,    71,    72,    77,    35,    35,    55,    35,    55,
      59,    69,    57,    73,    78,     4,     5,     8,     9,    10,
      11,    12,    13,    14,    15,    17,    26,    28,    29,    30,
      31,    34,    35,    36,    37,    38,    39,    40,    41,    45,
      48,    49,    50,    55,    59,    64,    65,    85,    89,    92,
      93,    94,    95,    96,   103,   104,   109,   112,   116,   122,
     124,   126,   127,    85,    35,    74,    75,    76,    35,    82,
      83,    84,    35,    85,    35,    59,    59,    55,    59,    59,
      59,    59,    59,    59,    96,   116,   126,    96,   103,   103,
     103,    90,    97,    98,    99,   100,   101,   102,   103,   103,
     103,    50,    53,    59,    55,    56,    92,    58,    42,    53,
      54,    32,    57,    56,    76,    57,    60,    61,    59,    42,
      57,    97,   110,   113,    96,   123,    97,   125,    97,    97,
      85,   117,    89,    22,    23,    60,    18,    19,    20,    21,
      43,    44,    45,    46,    47,    24,    25,    48,    49,    50,
      51,    63,    36,    97,    91,    56,    58,    97,    97,    35,
      35,    85,    85,    57,    84,    86,    97,    85,    60,    97,
      89,    60,    61,    60,    61,    60,    60,    60,    97,   120,
     121,    56,    98,    98,    99,    99,    99,    99,    99,    99,
     100,   100,   100,   101,   101,   102,   102,   103,   103,   103,
      62,    60,    89,    62,    59,    59,    58,    79,    33,    85,
      87,    88,    42,    55,    60,    56,    96,    97,    60,    61,
      58,    56,   118,   119,    85,    60,    61,    97,   105,    55,
     114,    97,    58,   120,   120,    55,    85,    89,   111,    27,
      60,    60,    80,    56,    89,    55,    89,   106,    56,   115,
       7,    16,   107,    89,    97,    55,    56,    58,   108,    81,
      89,    56,    56
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    66,    68,    69,    67,    70,    70,    70,    70,    71,
      73,    72,    74,    74,    75,    75,    76,    78,    79,    80,
      81,    77,    82,    82,    83,    83,    84,    85,    85,    85,
      85,    85,    85,    85,    85,    85,    86,    85,    87,    87,
      88,    88,    88,    89,    89,    90,    89,    91,    89,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    93,
      93,    93,    94,    95,    95,    95,    95,    95,    95,    95,
      96,    96,    96,    96,    96,    97,    97,    97,    98,    98,
      98,    98,    98,    98,    98,    99,    99,    99,    99,   100,
     100,   100,   101,   101,   101,   102,   102,   102,   102,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     105,   106,   104,   107,   108,   107,   110,   111,   109,   113,
     114,   115,   112,   117,   116,   118,   116,   119,   116,   120,
     120,   121,   121,   122,   123,   123,   124,   125,   125,   126,
     127
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     0,    11,     0,     2,     2,     2,     4,
       0,     6,     0,     1,     1,     2,     4,     0,     0,     0,
       0,    17,     0,     1,     1,     3,     3,     1,     1,     1,
       1,     1,     1,     2,     2,     4,     0,     6,     0,     1,
       1,     3,     1,     2,     3,     0,     5,     0,     6,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     4,
       6,     4,     3,     1,     1,     1,     1,     1,     1,     4,
       1,     2,     4,     3,     3,     3,     3,     1,     3,     3,
       3,     3,     3,     3,     1,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     1,     3,     3,     3,     1,     1,
       1,     3,     4,     1,     1,     2,     2,     2,     2,     2,
       0,     0,    10,     0,     0,     5,     0,     0,     9,     0,
       0,     0,    11,     0,     5,     0,     7,     0,     7,     0,
       1,     1,     3,     4,     1,     3,     4,     1,     3,     4,
       4
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;
      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* $@1: %empty  */
#line 327 "source/l25.y"
    {
        initBasicTypes();

        int jmp = emit("JMP");
        codeStk.push_back(jmp);
    }
#line 2198 "build/l25.tab.cc"
    break;

  case 3: /* $@2: %empty  */
#line 334 "source/l25.y"
    {
        int jmp = codeStk.back();
        codeStk.pop_back();
        code[jmp].val = code.size();
        
        scopes.push_back({});

        curLocal = 0;
        
        emit("PUSHFP");
        emit("MOVFP");
        int addsp = emit("ADDSP");
        codeStk.push_back(addsp);
    }
#line 2217 "build/l25.tab.cc"
    break;

  case 4: /* program: PROGRAM IDENT '{' $@1 global_list_opt MAIN '{' $@2 stmt_list '}' '}'  */
#line 349 "source/l25.y"
    {
        scopes.pop_back();

        int addsp = codeStk.back();
        codeStk.pop_back();
        code[addsp].val = curLocal;

        emit("ADDSP", -curLocal);
        emit("POPFP");

        generateCode();
        exit(0);
    }
#line 2235 "build/l25.tab.cc"
    break;

  case 5: /* global_list_opt: %empty  */
#line 365 "source/l25.y"
                  {}
#line 2241 "build/l25.tab.cc"
    break;

  case 9: /* declare_def: LET IDENT ':' type_spec  */
#line 373 "source/l25.y"
    {
        declare((yyvsp[-2].sval), (yyvsp[0].ival),
            emitPre("ADDSP", typeTable[(yyvsp[0].ival)].size), SCOPE_GLOBAL);
    }
#line 2250 "build/l25.tab.cc"
    break;

  case 10: /* $@3: %empty  */
#line 381 "source/l25.y"
    {
        entType = structType((yyvsp[-1].sval));
        declare((yyvsp[-1].sval), entType, 0, SCOPE_GLOBAL);

        scopes.push_back({});

        fieldStk.push_back({});
    }
#line 2263 "build/l25.tab.cc"
    break;

  case 11: /* struct_def: STRUCT IDENT '{' $@3 field_list_opt '}'  */
#line 390 "source/l25.y"
    {
        fixStructType(entType, fieldStk.back());
        entType = TYPE_VOID;

        scopes.pop_back();

        fieldStk.pop_back();
    }
#line 2276 "build/l25.tab.cc"
    break;

  case 12: /* field_list_opt: %empty  */
#line 401 "source/l25.y"
                  {}
#line 2282 "build/l25.tab.cc"
    break;

  case 16: /* field_def: IDENT ':' type_spec ';'  */
#line 412 "source/l25.y"
    {
        if ((yyvsp[-1].ival) == entType)
        {
            yyerror(("syntax error, expected complete type, "
                     "found \"" + typeTable[(yyvsp[-1].ival)].name + "\"").c_str());
            exit(1);
        }
        declare((yyvsp[-3].sval), (yyvsp[-1].ival), 0, SCOPE_LOCAL);
        fieldStk.back()[(yyvsp[-3].sval)] = {int((yyvsp[-1].ival)), -1};
    }
#line 2297 "build/l25.tab.cc"
    break;

  case 17: /* $@4: %empty  */
#line 426 "source/l25.y"
    {
        scopes.push_back({});

        curLocal = 0;

        typeStk.push_back({});

        entAddr = emitPre("PUSH", code.size());

    }
#line 2312 "build/l25.tab.cc"
    break;

  case 18: /* $@5: %empty  */
#line 437 "source/l25.y"
    {
        emit("PUSHFP");
        emit("MOVFP");

        int addsp = emit("ADDSP");
        codeStk.push_back(addsp);

        curLocal = 0;
    }
#line 2326 "build/l25.tab.cc"
    break;

  case 19: /* $@6: %empty  */
#line 447 "source/l25.y"
    {
        int type = funcType((yyvsp[-1].ival), typeStk.back());
        declare((yyvsp[-8].sval), type, entAddr, SCOPE_GLOBAL);

        for (auto& [name, s]: scopes.back())
            if (name != (yyvsp[-8].sval))
                s.addr -= 2 + funcDef[typeTable[type].index].argSize;
    }
#line 2339 "build/l25.tab.cc"
    break;

  case 20: /* $@7: %empty  */
#line 456 "source/l25.y"
    {
        if ((yyvsp[-6].ival) != (yyvsp[-1].ival))
        {
            yyerror(("syntax error, expected \"" + typeTable[(yyvsp[-6].ival)].name +
                     "\", found \"" + typeTable[(yyvsp[-1].ival)].name + "\"").c_str());
            exit(1);
        }

        scopes.pop_back();

        int addsp = codeStk.back();
        codeStk.pop_back();
        int type = funcType((yyvsp[-6].ival), typeStk.back());
        declare((yyvsp[-13].sval), type, entAddr, SCOPE_GLOBAL);
        code[addsp].val = curLocal;

        for (int i = 0; i < typeTable[(yyvsp[-6].ival)].size; i++)
        {
            emit("PADDRL", -3 - funcDef[typeTable[type].index].argSize - i);
            emit("SWAP", 1);
            emit("STR");
        }

        emit("ADDSP", -curLocal);
        emit("POPFP");
        emit("RET");
   
        typeStk.pop_back();
    }
#line 2373 "build/l25.tab.cc"
    break;

  case 22: /* arg_list_opt: %empty  */
#line 489 "source/l25.y"
                  {}
#line 2379 "build/l25.tab.cc"
    break;

  case 26: /* arg_def: IDENT ':' type_spec  */
#line 500 "source/l25.y"
    {
        declare((yyvsp[-2].sval), (yyvsp[0].ival), curLocal, SCOPE_LOCAL);
        curLocal += typeTable[(yyvsp[0].ival)].size;
        typeStk.back().push_back((yyvsp[0].ival));
    }
#line 2389 "build/l25.tab.cc"
    break;

  case 27: /* type_spec: INT  */
#line 509 "source/l25.y"
    {
        (yyval.ival) = TYPE_INT;
    }
#line 2397 "build/l25.tab.cc"
    break;

  case 28: /* type_spec: BOOL  */
#line 513 "source/l25.y"
    {
        (yyval.ival) = TYPE_BOOL;
    }
#line 2405 "build/l25.tab.cc"
    break;

  case 29: /* type_spec: CHAR  */
#line 517 "source/l25.y"
    {
        (yyval.ival) = TYPE_CHAR;
    }
#line 2413 "build/l25.tab.cc"
    break;

  case 30: /* type_spec: VOID  */
#line 521 "source/l25.y"
    {
        (yyval.ival) = TYPE_VOID;
    }
#line 2421 "build/l25.tab.cc"
    break;

  case 31: /* type_spec: FLOAT  */
#line 525 "source/l25.y"
    {
        (yyval.ival) = TYPE_FLOAT;
    }
#line 2429 "build/l25.tab.cc"
    break;

  case 32: /* type_spec: STRING  */
#line 529 "source/l25.y"
    {
        (yyval.ival) = TYPE_STRING;
    }
#line 2437 "build/l25.tab.cc"
    break;

  case 33: /* type_spec: STRUCT IDENT  */
#line 533 "source/l25.y"
    {
        Sym& s = find((yyvsp[0].sval));
        if (typeTable[s.type].kind != TYPE_STRUCT)
        {
            yyerror(("syntax error, expected struct type, "
                     "found \"" + typeTable[s.type].name + "\"").c_str());
            exit(1);
        }
        (yyval.ival) = s.type;
    }
#line 2452 "build/l25.tab.cc"
    break;

  case 34: /* type_spec: type_spec '*'  */
#line 544 "source/l25.y"
    {
        (yyval.ival) = ptrType((yyvsp[-1].ival));
    }
#line 2460 "build/l25.tab.cc"
    break;

  case 35: /* type_spec: type_spec '[' INT_CONST ']'  */
#line 548 "source/l25.y"
    {
        (yyval.ival) = arrType((yyvsp[-3].ival), (yyvsp[-1].ival));
    }
#line 2468 "build/l25.tab.cc"
    break;

  case 36: /* $@8: %empty  */
#line 552 "source/l25.y"
    {
        typeStk.push_back({});
    }
#line 2476 "build/l25.tab.cc"
    break;

  case 37: /* type_spec: FUNC type_spec '(' $@8 type_spec_list_opt ')'  */
#line 556 "source/l25.y"
    {
        (yyval.ival) = funcType((yyvsp[-4].ival), typeStk.back());
        typeStk.pop_back();
    }
#line 2485 "build/l25.tab.cc"
    break;

  case 38: /* type_spec_list_opt: %empty  */
#line 563 "source/l25.y"
                  {}
#line 2491 "build/l25.tab.cc"
    break;

  case 40: /* type_spec_list: type_spec  */
#line 569 "source/l25.y"
    {
        typeStk.back().push_back((yyvsp[0].ival));
    }
#line 2499 "build/l25.tab.cc"
    break;

  case 41: /* type_spec_list: type_spec_list ',' type_spec  */
#line 573 "source/l25.y"
    {
        typeStk.back().push_back((yyvsp[0].ival));
    }
#line 2507 "build/l25.tab.cc"
    break;

  case 42: /* type_spec_list: THIS  */
#line 577 "source/l25.y"
    {
        typeStk.back().push_back(ptrType(entType));
    }
#line 2515 "build/l25.tab.cc"
    break;

  case 45: /* $@9: %empty  */
#line 586 "source/l25.y"
    {
        scopes.push_back({});
    }
#line 2523 "build/l25.tab.cc"
    break;

  case 46: /* stmt_list: '{' $@9 stmt_list '}' ';'  */
#line 590 "source/l25.y"
    {
        scopes.pop_back();
    }
#line 2531 "build/l25.tab.cc"
    break;

  case 47: /* $@10: %empty  */
#line 594 "source/l25.y"
    {
        scopes.push_back({});
    }
#line 2539 "build/l25.tab.cc"
    break;

  case 48: /* stmt_list: stmt_list '{' $@10 stmt_list '}' ';'  */
#line 598 "source/l25.y"
    {
        scopes.pop_back();
    }
#line 2547 "build/l25.tab.cc"
    break;

  case 54: /* stmt: func_call  */
#line 610 "source/l25.y"
    {
        emit("ADDSP", -typeTable[(yyvsp[0].ival)].size);
    }
#line 2555 "build/l25.tab.cc"
    break;

  case 57: /* stmt: alloc_call  */
#line 616 "source/l25.y"
    {
        emit("ADDSP", -typeTable[(yyvsp[0].ival)].size);
    }
#line 2563 "build/l25.tab.cc"
    break;

  case 59: /* declare_stmt: LET IDENT ':' type_spec  */
#line 624 "source/l25.y"
    {
        declare((yyvsp[-2].sval), (yyvsp[0].ival), curLocal, SCOPE_LOCAL);
        curLocal += typeTable[(yyvsp[0].ival)].size;
    }
#line 2572 "build/l25.tab.cc"
    break;

  case 60: /* declare_stmt: LET IDENT ':' type_spec '=' right_expr  */
#line 629 "source/l25.y"
    {
        if ((yyvsp[-2].ival) != (yyvsp[0].ival))
        {
            yyerror(("syntax error, type dismatch, expected \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\", found \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
        declare((yyvsp[-4].sval), (yyvsp[-2].ival), curLocal, SCOPE_LOCAL);
        for (int i = typeTable[(yyvsp[-2].ival)].size; i--;)
        {
            emit("PADDRL", curLocal + i);
            emit("SWAP", 1);
            emit("STR");
        }
        curLocal += typeTable[(yyvsp[-2].ival)].size;
    }
#line 2594 "build/l25.tab.cc"
    break;

  case 61: /* declare_stmt: LET IDENT '=' right_expr  */
#line 647 "source/l25.y"
    {
        declare((yyvsp[-2].sval), (yyvsp[0].ival), curLocal, SCOPE_LOCAL);
        for (int i = typeTable[(yyvsp[0].ival)].size; i--;)
        {
            emit("PADDRL", curLocal + i);
            emit("SWAP", 1);
            emit("STR");
        }
        curLocal += typeTable[(yyvsp[0].ival)].size;
    }
#line 2609 "build/l25.tab.cc"
    break;

  case 62: /* assign_stmt: left_expr '=' right_expr  */
#line 661 "source/l25.y"
    {
        if ((yyvsp[-2].ival) != (yyvsp[0].ival))
        {
            yyerror(("syntax error, type dismatch, expected \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\", found \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
        if (typeTable[(yyvsp[0].ival)].size == 1)
            emit("STR");
        else
        {
            emit("PUSH");
            emit("SWAP", typeTable[(yyvsp[0].ival)].size + 1);
            for (int i = typeTable[(yyvsp[0].ival)].size; i--;)
            {
                emit("DUP");
                emit("SWAP", 2);
                emit("SWAP", 1);
                emit("PUSH", i);
                emit("ADD");
                emit("SWAP", 1);
                emit("STR");
            }
            emit("POP");
            emit("POP");
        }
    }
#line 2642 "build/l25.tab.cc"
    break;

  case 63: /* const_expr: INT_CONST  */
#line 693 "source/l25.y"
    {
        emit("PUSH", (yyvsp[0].ival));
        (yyval.ival) = TYPE_INT;
    }
#line 2651 "build/l25.tab.cc"
    break;

  case 64: /* const_expr: BOOL_CONST  */
#line 698 "source/l25.y"
    {
        emit("PUSH", (yyvsp[0].ival));
        (yyval.ival) = TYPE_BOOL;
    }
#line 2660 "build/l25.tab.cc"
    break;

  case 65: /* const_expr: CHAR_CONST  */
#line 703 "source/l25.y"
    {
        emit("PUSH", (yyvsp[0].cval));
        (yyval.ival) = TYPE_CHAR;
    }
#line 2669 "build/l25.tab.cc"
    break;

  case 66: /* const_expr: VOID_CONST  */
#line 708 "source/l25.y"
    {
        (yyval.ival) = TYPE_VOID;
    }
#line 2677 "build/l25.tab.cc"
    break;

  case 67: /* const_expr: FLOAT_CONST  */
#line 712 "source/l25.y"
    {
        double f = (yyvsp[0].fval);
        emit("PUSH", std::bit_cast<long long>(f));
        (yyval.ival) = TYPE_FLOAT;
    }
#line 2687 "build/l25.tab.cc"
    break;

  case 68: /* const_expr: STRING_CONST  */
#line 718 "source/l25.y"
    {
        std::string str = unescapeLiteral((yyvsp[0].sval));
        for (int i = 0; i < 256; i++)
        {
            if (i < str.size())
                emit("PUSH", str[i]);
            else
                emit("PUSH", 0);
        }
        (yyval.ival) = TYPE_STRING;
    }
#line 2703 "build/l25.tab.cc"
    break;

  case 69: /* const_expr: SIZEOF '(' type_spec ')'  */
#line 730 "source/l25.y"
    {
        emit("PUSH", typeTable[(yyvsp[-1].ival)].size);
        (yyval.ival) = TYPE_INT;
    }
#line 2712 "build/l25.tab.cc"
    break;

  case 70: /* left_expr: IDENT  */
#line 738 "source/l25.y"
    {
        Sym& s = find((yyvsp[0].sval));
        (yyval.ival) = s.type;
        if (s.kind == SCOPE_LOCAL)
            emit("PADDRL", s.addr);
        else
            emit("PUSH", s.addr);
    }
#line 2725 "build/l25.tab.cc"
    break;

  case 71: /* left_expr: '*' factor  */
#line 747 "source/l25.y"
    {
        if (typeTable[(yyvsp[0].ival)].kind != TYPE_PTR)
        {
            yyerror(("syntax error, indirection expected pointer type, "
                     "found \"" + typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
        int ptrIdx = typeTable[(yyvsp[0].ival)].index;
        (yyval.ival) = ptrDef[ptrIdx].baseType;

    }
#line 2741 "build/l25.tab.cc"
    break;

  case 72: /* left_expr: left_expr '[' right_expr ']'  */
#line 759 "source/l25.y"
    {
        if (typeTable[(yyvsp[-1].ival)].kind != TYPE_INT)
        {
            yyerror(("syntax error, expected subscriptable type, "
                     "found \"" + typeTable[(yyvsp[-1].ival)].name + "\"").c_str());
            exit(1);
        }
        if (typeTable[(yyvsp[-3].ival)].kind == TYPE_PTR)
        {
            int ptrIdx = typeTable[(yyvsp[-3].ival)].index;
            (yyval.ival) = ptrDef[ptrIdx].baseType;
            emit("SWAP", 1);
            emit("LOD");
            emit("SWAP", 1);
            emit("PADDRA", typeTable[ptrDef[ptrIdx].baseType].size);
        }
        else if (typeTable[(yyvsp[-3].ival)].kind == TYPE_ARR)
        {
            int arrIdx = typeTable[(yyvsp[-3].ival)].index;
            (yyval.ival) = arrDef[arrIdx].baseType;
            emit("PADDRA", typeTable[arrDef[arrIdx].baseType].size);
        }
        else if (typeTable[(yyvsp[-3].ival)].kind == TYPE_STRING)
        {
            (yyval.ival) = TYPE_CHAR;
            emit("PADDRA", 1);
        }
        else
        {
            yyerror(("syntax error, index access expected array-like type, "
                     "found \"" + typeTable[(yyvsp[-3].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 2780 "build/l25.tab.cc"
    break;

  case 73: /* left_expr: left_expr '.' IDENT  */
#line 794 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind != TYPE_STRUCT)
        {
            yyerror(("syntax error, field access expected struct type, "
                     "found \"" + typeTable[(yyvsp[-2].ival)].name + "\"").c_str());
            exit(1);
        }
        int structIdx = typeTable[(yyvsp[-2].ival)].index;
        auto it = structDef[structIdx].field.find((yyvsp[0].sval));
        if (it == structDef[structIdx].field.end())
        {
            yyerror(("syntax error, struct \"" + typeTable[(yyvsp[-2].ival)].name +
                     "\" has no field \"" + (yyvsp[0].sval) + "\"").c_str());
            exit(1);
        }
        (yyval.ival) = it->second.type;
        emit("PADDRF", it->second.offset);
    }
#line 2803 "build/l25.tab.cc"
    break;

  case 74: /* left_expr: factor ARROW IDENT  */
#line 813 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind != TYPE_PTR)
        {
            yyerror(("syntax error, indirection expected pointer type, "
                     "found \"" + typeTable[(yyvsp[-2].ival)].name + "\"").c_str());
            exit(1);
        }
        int ptrIdx = typeTable[(yyvsp[-2].ival)].index;
        int type = ptrDef[ptrIdx].baseType;
        if (typeTable[type].kind != TYPE_STRUCT)
        {
            yyerror(("syntax error, field access expected struct type, "
                     "found \"" + typeTable[type].name + "\"").c_str());
            exit(1);
        }
        int structIdx = typeTable[type].index;
        auto it = structDef[structIdx].field.find((yyvsp[0].sval));
        if (it == structDef[structIdx].field.end())
        {
            yyerror(("syntax error, struct \"" + typeTable[(yyvsp[-2].ival)].name +
                     "\" has no field \"" + (yyvsp[0].sval) + "\"").c_str());
            exit(1);
        }
        (yyval.ival) = it->second.type;
        emit("PADDRF", it->second.offset);
    }
#line 2834 "build/l25.tab.cc"
    break;

  case 75: /* right_expr: right_expr AND compare_expr  */
#line 843 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_BOOL &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_BOOL)
        {
            emit("ANDB");
            (yyval.ival) = TYPE_BOOL;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" && \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 2854 "build/l25.tab.cc"
    break;

  case 76: /* right_expr: right_expr OR compare_expr  */
#line 859 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_BOOL &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_BOOL)
        {
            emit("ORB");
            (yyval.ival) = TYPE_BOOL;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" || \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 2874 "build/l25.tab.cc"
    break;

  case 77: /* right_expr: compare_expr  */
#line 875 "source/l25.y"
    {
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 2882 "build/l25.tab.cc"
    break;

  case 78: /* compare_expr: bitwise_expr '<' bitwise_expr  */
#line 882 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("LT");
            (yyval.ival) = TYPE_BOOL;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("LTF");
            (yyval.ival) = TYPE_BOOL;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" < \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 2908 "build/l25.tab.cc"
    break;

  case 79: /* compare_expr: bitwise_expr '>' bitwise_expr  */
#line 904 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("GT");
            (yyval.ival) = TYPE_BOOL;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("GTF");
            (yyval.ival) = TYPE_BOOL;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" > \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 2934 "build/l25.tab.cc"
    break;

  case 80: /* compare_expr: bitwise_expr LE bitwise_expr  */
#line 926 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("LE");
            (yyval.ival) = TYPE_BOOL;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("LEF");
            (yyval.ival) = TYPE_BOOL;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" <= \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 2960 "build/l25.tab.cc"
    break;

  case 81: /* compare_expr: bitwise_expr GE bitwise_expr  */
#line 948 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("GE");
            (yyval.ival) = TYPE_BOOL;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("GEF");
            (yyval.ival) = TYPE_BOOL;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" >= \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 2986 "build/l25.tab.cc"
    break;

  case 82: /* compare_expr: bitwise_expr EQ bitwise_expr  */
#line 970 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("EQ");
            (yyval.ival) = TYPE_BOOL;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("EQF");
            (yyval.ival) = TYPE_BOOL;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_PTR &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_PTR && (yyvsp[-2].ival) == (yyvsp[0].ival))
        {
            emit("EQ");
            (yyval.ival) = TYPE_BOOL;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" == \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3018 "build/l25.tab.cc"
    break;

  case 83: /* compare_expr: bitwise_expr NEQ bitwise_expr  */
#line 998 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("NEQ");
            (yyval.ival) = TYPE_BOOL;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("NEQF");
            (yyval.ival) = TYPE_BOOL;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_PTR &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_PTR && (yyvsp[-2].ival) == (yyvsp[0].ival))
        {
            emit("NEQ");
            (yyval.ival) = TYPE_BOOL;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" != \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3050 "build/l25.tab.cc"
    break;

  case 84: /* compare_expr: bitwise_expr  */
#line 1026 "source/l25.y"
    {
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3058 "build/l25.tab.cc"
    break;

  case 85: /* bitwise_expr: bitwise_expr '&' bitwise_term  */
#line 1033 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("AND");
            (yyval.ival) = TYPE_INT;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" & \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3078 "build/l25.tab.cc"
    break;

  case 86: /* bitwise_expr: bitwise_expr '|' bitwise_term  */
#line 1049 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("OR");
            (yyval.ival) = TYPE_INT;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" | \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3098 "build/l25.tab.cc"
    break;

  case 87: /* bitwise_expr: bitwise_expr '^' bitwise_term  */
#line 1065 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("XOR");
            (yyval.ival) = TYPE_INT;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" ^ \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3118 "build/l25.tab.cc"
    break;

  case 88: /* bitwise_expr: bitwise_term  */
#line 1081 "source/l25.y"
    {
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3126 "build/l25.tab.cc"
    break;

  case 89: /* bitwise_term: bitwise_term LSH arith_expr  */
#line 1088 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("LSH");
            (yyval.ival) = TYPE_INT;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" << \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3146 "build/l25.tab.cc"
    break;

  case 90: /* bitwise_term: bitwise_term RSH arith_expr  */
#line 1104 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("RSH");
            (yyval.ival) = TYPE_INT;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" >> \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3166 "build/l25.tab.cc"
    break;

  case 91: /* bitwise_term: arith_expr  */
#line 1120 "source/l25.y"
    {
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3174 "build/l25.tab.cc"
    break;

  case 92: /* arith_expr: arith_expr '+' arith_term  */
#line 1127 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("ADD");
            (yyval.ival) = TYPE_INT;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("ADDF");
            (yyval.ival) = TYPE_FLOAT;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_PTR &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("PUSH",
                 typeTable[ptrDef[typeTable[(yyvsp[-2].ival)].index].baseType].size);
            emit("MUL");
            emit("ADD");
            (yyval.ival) = (yyvsp[-2].ival);
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_PTR &&
                 typeTable[(yyvsp[-2].ival)].kind == TYPE_INT)
        {
            emit("SWAP", 1);
            emit("PUSH",
                 typeTable[ptrDef[typeTable[(yyvsp[0].ival)].index].baseType].size);
            emit("MUL");
            emit("ADD");
            (yyval.ival) = (yyvsp[0].ival);
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_STRING &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_STRING)
        {
            emit("ADDS");
            (yyval.ival) = TYPE_STRING;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" + \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3225 "build/l25.tab.cc"
    break;

  case 93: /* arith_expr: arith_expr '-' arith_term  */
#line 1174 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("SUB");
            (yyval.ival) = TYPE_INT;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("SUBF");
            (yyval.ival) = TYPE_FLOAT;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_PTR &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("PUSH",
                 typeTable[ptrDef[typeTable[(yyvsp[-2].ival)].index].baseType].size);
            emit("MUL");
            emit("SUB");
            (yyval.ival) = (yyvsp[-2].ival);
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_PTR &&
                 typeTable[(yyvsp[-2].ival)].kind == TYPE_INT)
        {
            emit("PUSH",
                 typeTable[ptrDef[typeTable[(yyvsp[0].ival)].index].baseType].size);
            emit("MUL");
            emit("SUB");
            (yyval.ival) = (yyvsp[0].ival);
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_PTR &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_PTR)
        {
            emit("SUB");
            (yyval.ival) = TYPE_INT;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" - \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3275 "build/l25.tab.cc"
    break;

  case 94: /* arith_expr: arith_term  */
#line 1220 "source/l25.y"
    {
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3283 "build/l25.tab.cc"
    break;

  case 95: /* arith_term: arith_term '*' factor  */
#line 1227 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("MUL");
            (yyval.ival) = TYPE_INT;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("MULF");
            (yyval.ival) = TYPE_FLOAT;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_STRING &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("MULS");
            (yyval.ival) = TYPE_STRING;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_STRING)
        {
            emit("SWAP", 1);
            emit("MULS");
            (yyval.ival) = TYPE_STRING;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" * \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3322 "build/l25.tab.cc"
    break;

  case 96: /* arith_term: arith_term '/' factor  */
#line 1262 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("DIV");
            (yyval.ival) = TYPE_INT;
        }
        else if (typeTable[(yyvsp[-2].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("DIVF");
            (yyval.ival) = TYPE_FLOAT;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" / \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3348 "build/l25.tab.cc"
    break;

  case 97: /* arith_term: arith_term '%' factor  */
#line 1284 "source/l25.y"
    {
        if (typeTable[(yyvsp[-2].ival)].kind == TYPE_INT &&
            typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("MOD");
            (yyval.ival) = TYPE_INT;
        }
        else
        {
            yyerror(("syntax error, type dismatch, \"" +
                     typeTable[(yyvsp[-2].ival)].name + "\" % \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3368 "build/l25.tab.cc"
    break;

  case 98: /* arith_term: factor  */
#line 1300 "source/l25.y"
    {
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3376 "build/l25.tab.cc"
    break;

  case 99: /* factor: left_expr  */
#line 1307 "source/l25.y"
    {
        if (typeTable[(yyvsp[0].ival)].size == 1)
            emit("LOD", 0);
        else
        {
            emit("ADDSP", typeTable[(yyvsp[0].ival)].size);
            emit("PUSH");
            emit("SWAP", typeTable[(yyvsp[0].ival)].size + 1);
            for (int i = 0; i < typeTable[(yyvsp[0].ival)].size; i++)
            {
                emit("DUP");
                emit("PUSH", i);
                emit("ADD");
                emit("LOD");
                emit("SWAP", typeTable[(yyvsp[0].ival)].size - i + 2);
                emit("POP");
            }
            emit("POP");
            emit("POP");
        }
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3403 "build/l25.tab.cc"
    break;

  case 100: /* factor: const_expr  */
#line 1330 "source/l25.y"
    {
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3411 "build/l25.tab.cc"
    break;

  case 101: /* factor: '(' right_expr ')'  */
#line 1334 "source/l25.y"
    {
        (yyval.ival) = (yyvsp[-1].ival);
    }
#line 3419 "build/l25.tab.cc"
    break;

  case 102: /* factor: type_spec '(' right_expr ')'  */
#line 1338 "source/l25.y"
    {
        if ((yyvsp[-1].ival) == (yyvsp[-3].ival))
            ;
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_INT &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_FLOAT)
        {
            emit("I2F");
        }
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_INT)
        {
            emit("F2I");
        }
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_INT &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_BOOL)
        {
            emit("I2B");
        }
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_BOOL &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_INT)
        {
            emit("B2I");
        }
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_INT &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_CHAR)
        {
            emit("I2C");
        }
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_CHAR &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_INT)
        {
            emit("C2I");
        }
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_BOOL)
        {
            emit("F2B");
        }
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_BOOL &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_FLOAT)
        {
            emit("B2F");
        }
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_FLOAT &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_CHAR)
        {
            emit("F2I");
            emit("I2C");
        }
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_CHAR &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_FLOAT)
        {
            emit("C2I");
            emit("I2F");
        }
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_CHAR &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_BOOL)
        {
            emit("C2I");
            emit("I2B");
        }
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_BOOL &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_CHAR)
        {
            emit("B2I");
            emit("I2C");
        }
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_PTR &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_PTR)
            ;
        else if (typeTable[(yyvsp[-1].ival)].kind == TYPE_VOID &&
                 typeTable[(yyvsp[-3].ival)].kind == TYPE_PTR)
        {
            emit("PUSH");
        }
        else
        {
            yyerror(("syntax error, unsupported cast from \"" +
                     typeTable[(yyvsp[-1].ival)].name + "\" to \"" +
                     typeTable[(yyvsp[-3].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 3507 "build/l25.tab.cc"
    break;

  case 103: /* factor: func_call  */
#line 1422 "source/l25.y"
    {
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3515 "build/l25.tab.cc"
    break;

  case 104: /* factor: alloc_call  */
#line 1426 "source/l25.y"
    {
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3523 "build/l25.tab.cc"
    break;

  case 105: /* factor: '&' left_expr  */
#line 1430 "source/l25.y"
    {
        (yyval.ival) = ptrType((yyvsp[0].ival));
    }
#line 3531 "build/l25.tab.cc"
    break;

  case 106: /* factor: '-' factor  */
#line 1434 "source/l25.y"
    {
        if (typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
            emit("NEG");
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
            emit("NEGF");
        else
        {
            yyerror(("syntax error, type dismatch, -\"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3549 "build/l25.tab.cc"
    break;

  case 107: /* factor: '+' factor  */
#line 1448 "source/l25.y"
    {
        if (typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
            ;
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
            ;
        else
        {
            yyerror(("syntax error, type dismatch, +\"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3567 "build/l25.tab.cc"
    break;

  case 108: /* factor: '~' factor  */
#line 1462 "source/l25.y"
    {
        if (typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
            emit("COMPL");
        else
        {
            yyerror(("syntax error, type dismatch, ~\"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3583 "build/l25.tab.cc"
    break;

  case 109: /* factor: '!' factor  */
#line 1474 "source/l25.y"
    {
        if (typeTable[(yyvsp[0].ival)].kind == TYPE_BOOL)
        {
            emit("PUSH", 1);
            emit("SUB");
            (yyval.ival) = TYPE_BOOL;
        }
        else
        {
            yyerror(("syntax error, type dismatch, !\"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
        (yyval.ival) = (yyvsp[0].ival);
    }
#line 3603 "build/l25.tab.cc"
    break;

  case 110: /* $@11: %empty  */
#line 1493 "source/l25.y"
    {
        int jpc = emit("JPC");
        codeStk.push_back(jpc);

        scopes.push_back({});
    }
#line 3614 "build/l25.tab.cc"
    break;

  case 111: /* $@12: %empty  */
#line 1500 "source/l25.y"
    {
        scopes.pop_back();
    }
#line 3622 "build/l25.tab.cc"
    break;

  case 113: /* else_opt: %empty  */
#line 1508 "source/l25.y"
    {
        int jpc = codeStk.back();
        codeStk.pop_back();
        code[jpc].val = code.size();
    }
#line 3632 "build/l25.tab.cc"
    break;

  case 114: /* $@13: %empty  */
#line 1514 "source/l25.y"
    {   
        int jpc = codeStk.back();
        codeStk.pop_back();

        int jmp = emit("JMP");

        code[jpc].val = code.size();

        codeStk.push_back(jmp);

        scopes.push_back({});
    }
#line 3649 "build/l25.tab.cc"
    break;

  case 115: /* else_opt: ELSE '{' $@13 stmt_list '}'  */
#line 1527 "source/l25.y"
    {
        scopes.pop_back();

        int jmp = codeStk.back();
        codeStk.pop_back();
        code[jmp].val = code.size();
    }
#line 3661 "build/l25.tab.cc"
    break;

  case 116: /* $@14: %empty  */
#line 1538 "source/l25.y"
    {
        int top = code.size();
        codeStk.push_back(top);
    }
#line 3670 "build/l25.tab.cc"
    break;

  case 117: /* $@15: %empty  */
#line 1543 "source/l25.y"
    {
        int jpc = emit("JPC");
        codeStk.push_back(jpc);

        scopes.push_back({});
    }
#line 3681 "build/l25.tab.cc"
    break;

  case 118: /* while_stmt: WHILE '(' $@14 right_expr ')' '{' $@15 stmt_list '}'  */
#line 1550 "source/l25.y"
    {
        scopes.pop_back();

        int jpc = codeStk.back();
        codeStk.pop_back();

        int top = codeStk.back();
        codeStk.pop_back();

        emit("JMP", top);
        code[jpc].val = code.size();
    }
#line 3698 "build/l25.tab.cc"
    break;

  case 119: /* $@16: %empty  */
#line 1566 "source/l25.y"
    {
        int pushe = emit("PUSHE");
        codeStk.push_back(pushe);

        scopes.push_back({});
    }
#line 3709 "build/l25.tab.cc"
    break;

  case 120: /* $@17: %empty  */
#line 1573 "source/l25.y"
    {
        scopes.pop_back();
    }
#line 3717 "build/l25.tab.cc"
    break;

  case 121: /* $@18: %empty  */
#line 1577 "source/l25.y"
    {
        emit("POPE");
        
        int jmp = emit("JMP");

        int pushe = codeStk.back();
        codeStk.pop_back();
        code[pushe].val = code.size();

        codeStk.push_back(jmp);

        scopes.push_back({});
    }
#line 3735 "build/l25.tab.cc"
    break;

  case 122: /* try_catch_stmt: TRY '{' $@16 stmt_list '}' $@17 CATCH '{' $@18 stmt_list '}'  */
#line 1591 "source/l25.y"
    {   
        scopes.pop_back();

        int jmp = codeStk.back();
        codeStk.pop_back();
        code[jmp].val = code.size();
    }
#line 3747 "build/l25.tab.cc"
    break;

  case 123: /* $@19: %empty  */
#line 1602 "source/l25.y"
    {
        Sym& s = find((yyvsp[-1].sval));
        if (typeTable[s.type].kind != TYPE_FUNC)
        {
            yyerror(("syntax error, expected function type, "
                     "found \"" + typeTable[s.type].name + "\"").c_str());
            exit(1);
        }

        paramStk.push_back(0);
        typeStk.push_back(funcDef[typeTable[s.type].index].argType);

        emit("ADDSP",
             typeTable[funcDef[typeTable[s.type].index].retType].size);
    }
#line 3767 "build/l25.tab.cc"
    break;

  case 124: /* func_call: IDENT '(' $@19 param_list_opt ')'  */
#line 1618 "source/l25.y"
    {
        Sym& s = find((yyvsp[-4].sval));
        if (paramStk.back() < typeStk.back().size())
        {
            yyerror("syntax error, too few parameters");
            exit(1);
        }
        paramStk.pop_back();
        typeStk.pop_back();

        if (s.kind == SCOPE_LOCAL)
            emit("PADDRL", s.addr);
        else
            emit("PUSH", s.addr);
        emit("LOD");
        emit("CALL");
        emit("ADDSP", -funcDef[typeTable[s.type].index].argSize);
        (yyval.ival) = funcDef[typeTable[s.type].index].retType;
    }
#line 3791 "build/l25.tab.cc"
    break;

  case 125: /* $@20: %empty  */
#line 1638 "source/l25.y"
    {
        if (typeTable[(yyvsp[-3].ival)].kind != TYPE_STRUCT)
        {
            yyerror(("syntax error, field access expected struct type, "
                     "found \"" + typeTable[(yyvsp[-3].ival)].name + "\"").c_str());
            exit(1);
        }
        int structIdx = typeTable[(yyvsp[-3].ival)].index;
        auto it = structDef[structIdx].field.find((yyvsp[-1].sval));
        if (it == structDef[structIdx].field.end())
        {
            yyerror(("syntax error, struct \"" + typeTable[(yyvsp[-3].ival)].name +
                     "\" has no field \"" + (yyvsp[-1].sval) + "\"").c_str());
            exit(1);
        }
        emit("DUP");
        emit("PADDRF", it->second.offset);

        int retType = funcDef[typeTable[it->second.type].index].retType;

        paramStk.push_back(1);
        typeStk.push_back(funcDef[typeTable[it->second.type].index].argType);

        emit("ADDSP", typeTable[retType].size);
        emit("PUSH");
        emit("SWAP", typeTable[retType].size + 2);
    }
#line 3823 "build/l25.tab.cc"
    break;

  case 126: /* func_call: left_expr '.' IDENT '(' $@20 param_list_opt ')'  */
#line 1666 "source/l25.y"
    {
        int structIdx = typeTable[(yyvsp[-6].ival)].index;
        auto it = structDef[structIdx].field.find((yyvsp[-4].sval));

        if (paramStk.back() < typeStk.back().size())
        {
            yyerror("syntax error, too few parameters");
            exit(1);
        }
        paramStk.pop_back();
        typeStk.pop_back();

        int retType = funcDef[typeTable[it->second.type].index].retType;

        emit("PUSH");
        emit("SWAP", typeTable[retType].size +
            funcDef[typeTable[it->second.type].index].argSize + 1);
        emit("LOD");
        emit("CALL");
        emit("ADDSP", -funcDef[typeTable[it->second.type].index].argSize);

        emit("PUSH");
        for (int i = 0; i < typeTable[retType].size; i++)
        {
            emit("SWAP", typeTable[retType].size - i);
            emit("SWAP", typeTable[retType].size - i + 2);
        }
        emit("POP");
        emit("POP");
        emit("POP");
        (yyval.ival) = retType;
    }
#line 3860 "build/l25.tab.cc"
    break;

  case 127: /* $@21: %empty  */
#line 1699 "source/l25.y"
    {
        if (typeTable[(yyvsp[-3].ival)].kind != TYPE_PTR)
        {
            yyerror(("syntax error, indirection expected pointer type, "
                     "found \"" + typeTable[(yyvsp[-3].ival)].name + "\"").c_str());
            exit(1);
        }
        int ptrIdx = typeTable[(yyvsp[-3].ival)].index;
        int type = ptrDef[ptrIdx].baseType;
        if (typeTable[type].kind != TYPE_STRUCT)
        {
            yyerror(("syntax error, field access expected struct type, "
                     "found \"" + typeTable[type].name + "\"").c_str());
            exit(1);
        }
        int structIdx = typeTable[type].index;
        auto it = structDef[structIdx].field.find((yyvsp[-1].sval));
        if (it == structDef[structIdx].field.end())
        {
            yyerror(("syntax error, struct \"" + typeTable[(yyvsp[-3].ival)].name +
                     "\" has no field \"" + (yyvsp[-1].sval) + "\"").c_str());
            exit(1);
        }
        emit("DUP");
        emit("PADDRF", it->second.offset);

        int retType = funcDef[typeTable[it->second.type].index].retType;

        paramStk.push_back(1);
        typeStk.push_back(funcDef[typeTable[it->second.type].index].argType);

        emit("ADDSP", typeTable[retType].size);
        emit("PUSH");
        emit("SWAP", typeTable[retType].size + 2);
    }
#line 3900 "build/l25.tab.cc"
    break;

  case 128: /* func_call: factor ARROW IDENT '(' $@21 param_list_opt ')'  */
#line 1735 "source/l25.y"
    {
        int ptrIdx = typeTable[(yyvsp[-6].ival)].index;
        int type = ptrDef[ptrIdx].baseType;
        int structIdx = typeTable[type].index;
        auto it = structDef[structIdx].field.find((yyvsp[-4].sval));

        if (paramStk.back() < typeStk.back().size())
        {
            yyerror("syntax error, too few parameters");
            exit(1);
        }
        paramStk.pop_back();
        typeStk.pop_back();

        int retType = funcDef[typeTable[it->second.type].index].retType;

        emit("PUSH");
        emit("SWAP", typeTable[retType].size +
            funcDef[typeTable[it->second.type].index].argSize + 1);
        emit("LOD");
        emit("CALL");
        emit("ADDSP", -funcDef[typeTable[it->second.type].index].argSize);

        emit("PUSH");
        for (int i = 0; i < typeTable[retType].size; i++)
        {
            emit("SWAP", typeTable[retType].size - i);
            emit("SWAP", typeTable[retType].size - i + 2);
        }
        emit("POP");
        emit("POP");
        emit("POP");
        (yyval.ival) = retType;
    }
#line 3939 "build/l25.tab.cc"
    break;

  case 129: /* param_list_opt: %empty  */
#line 1772 "source/l25.y"
                  {}
#line 3945 "build/l25.tab.cc"
    break;

  case 131: /* param_list: right_expr  */
#line 1778 "source/l25.y"
    {
        if (paramStk.back() >= typeStk.back().size())
        {
            yyerror("syntax error, too many parameters");
            exit(1);
        }
        if ((yyvsp[0].ival) != typeStk.back()[paramStk.back()])
        {
            yyerror(("syntax error, expected \"" +
                     typeTable[typeStk.back()[paramStk.back()]].name +
                     ", found \"" + typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
        paramStk.back()++;
    }
#line 3965 "build/l25.tab.cc"
    break;

  case 132: /* param_list: param_list ',' right_expr  */
#line 1794 "source/l25.y"
    {
        if (paramStk.back() >= typeStk.back().size())
        {
            yyerror("syntax error, too many parameters");
            exit(1);
        }
        if ((yyvsp[0].ival) != typeStk.back()[paramStk.back()])
        {
            yyerror(("syntax error, expected \"" +
                     typeTable[typeStk.back()[paramStk.back()]].name +
                     ", found \"" + typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
        paramStk.back()++;
    }
#line 3985 "build/l25.tab.cc"
    break;

  case 134: /* in_arg_list: left_expr  */
#line 1817 "source/l25.y"
    {
        if (typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("SCAN");
            emit("STR");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("SCANF");
            emit("STR");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_CHAR)
        {
            emit("GET");
            emit("STR");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_STRING)
        {
            emit("GETS");
            emit("PUSH");
            emit("SWAP", 257);
            for (int i = 256; i--;)
            {
                emit("DUP");
                emit("SWAP", 2);
                emit("SWAP", 1);
                emit("PUSH", i);
                emit("ADD");
                emit("SWAP", 1);
                emit("STR");
            }
            emit("POP");
            emit("POP");
        }
        else {
            yyerror(("syntax error, unsupported input type \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 4030 "build/l25.tab.cc"
    break;

  case 135: /* in_arg_list: in_arg_list ',' left_expr  */
#line 1858 "source/l25.y"
    {
        if (typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("SCAN");
            emit("STR");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("SCANF");
            emit("STR");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_CHAR)
        {
            emit("GET");
            emit("STR");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_STRING)
        {
            emit("GETS");
            emit("PUSH");
            emit("SWAP", 257);
            for (int i = 256; i--;)
            {
                emit("DUP");
                emit("SWAP", 2);
                emit("SWAP", 1);
                emit("PUSH", i);
                emit("ADD");
                emit("SWAP", 1);
                emit("STR");
            }
            emit("POP");
            emit("POP");
        }
        else {
            yyerror(("syntax error, unsupported input type \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 4075 "build/l25.tab.cc"
    break;

  case 137: /* out_arg_list: right_expr  */
#line 1906 "source/l25.y"
    {
        if (typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("PRINT");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("PRINTF");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_CHAR)
        {
            emit("PUT");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_STRING)
        {
            emit("PUTS");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_PTR &&
                 ptrDef[typeTable[(yyvsp[0].ival)].index].baseType == TYPE_VOID)
        {
            emit("PRINT");
        }
        else {
            yyerror(("syntax error, unsupported output type \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 4108 "build/l25.tab.cc"
    break;

  case 138: /* out_arg_list: out_arg_list ',' right_expr  */
#line 1935 "source/l25.y"
    {
        if (typeTable[(yyvsp[0].ival)].kind == TYPE_INT)
        {
            emit("PRINT");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_FLOAT)
        {
            emit("PRINTF");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_CHAR)
        {
            emit("PUT");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_STRING)
        {
            emit("PUTS");
        }
        else if (typeTable[(yyvsp[0].ival)].kind == TYPE_PTR &&
                 ptrDef[typeTable[(yyvsp[0].ival)].index].baseType == TYPE_VOID)
        {
            emit("PRINT");
        }
        else {
            yyerror(("syntax error, unsupported output type \"" +
                     typeTable[(yyvsp[0].ival)].name + "\"").c_str());
            exit(1);
        }
    }
#line 4141 "build/l25.tab.cc"
    break;

  case 139: /* alloc_call: ALLOC '(' right_expr ')'  */
#line 1967 "source/l25.y"
    {
        emit("ALLOC");
        (yyval.ival) = ptrType(TYPE_VOID);
    }
#line 4150 "build/l25.tab.cc"
    break;

  case 140: /* free_call: FREE '(' right_expr ')'  */
#line 1975 "source/l25.y"
    {
        emit("FREE");
    }
#line 4158 "build/l25.tab.cc"
    break;


#line 4162 "build/l25.tab.cc"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 1980 "source/l25.y"


void yyerror(const char* msg)
{
    return yyerror(&yylloc, msg);
}
void yyerror(YYLTYPE* yylloc, const char* msg)
{
    std::cerr << "Compile error at line " << yylloc->first_line
              << ", column " << yylloc->first_column << ": "
              <<  msg << std::endl;
}

int main()
{
    yyparse();
    return 0;
}

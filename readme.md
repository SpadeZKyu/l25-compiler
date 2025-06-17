# 编译原理作业

SpadeZ

## 一、项目概述

#### 项目名称
L25 语言编译器与类 Pcode 解释器

#### 项目背景
基于已经提供的 EBNF 描述的 L25 语言，扩展了更丰富的类型系统，支持基本数据类型（整型、布尔、字符、浮点、定长字符串）、复合类型组合（静态数组、结构体、指针、函数）和严格的类型检查，实现了基本的内存管理（伙伴分配器）。本项目基于 Flex/Bison/C++ 实现完整编译流程：

1. **词法分析**：`l25.l`，生成 `l25.lex.cc`；
2. **语法分析**：`l25.y`，生成 `l25.tab.cc`；
3. **代码生成**：在 Bison 动作中调用 `emit(...)` 生成类 Pcode 中间代码；
4. **解释执行**：`pcode.cc` 实现虚拟机，读取中间代码并执行。

项目中也给出了使用扩展后的 L25 语言编写的字典类型，支持插入、删除、查找和遍历等基本操作。

## 二、项目目录结构

```plaintext
.
├─ source/
│  ├─ l25.l                 // Flex 词法文件
│  ├─ l25.y                 // Bison 语法文件
│  └─ pcode.cc              // Pcode 虚拟机实现
├─ build/
│  ├─ l25.lex.cc            // Flex 生成 C++ 源文件
│  ├─ l25.tab.cc            // Bison 生成 C++ 源文件
│  ├─ l25.tab.hh            // Bison 生成 C++ 头文件
│  └─ compiler              // 编译器 Linux 可执行文件
│  └─ interpreter           // 解释器 Linux 可执行文件
│  └─ interpreterd          // 解释器（输出中间代码与栈帧变化）
├─ sample/
│  ├─ sort.l25              // 示例 L25 源程序：可传参比较函数的 sort
│  ├─ sort.pcode
│  ├─ ntt.l25               // 示例 L25 源程序：NTT 实现多项式乘法
│  ├─ ntt.pcode
│  ├─ map.l25               // 示例 L25 源程序：平衡树实现 Map
│  └─ map.pcode
├─ extension            
│  ├─ l25-vscode-0.1.1.vsix // 语言服务器 Linux VSCode 插件
│  └─ ...
├─ build.sh                 // 构建脚本
├─ run.sh                   // 编译并执行脚本
└─ readme.md                // 作业文档
```

## 三、扩展的 L25 语言规范

#### 1. 基本类型
`int, bool, char, void, float, string`

#### 2. 复合类型
静态数组、结构体、指针、函数

#### 3. 变量声明
```
let x: int[2];  // 定义静态数组变量
let y: int = 1; // 定义基本类型并初始化
let pi = 3.14;  // 自动推导类型为 float
```

#### 4. 结构体声明
```
struct node
{
    val: int;
    son: struct node*[2];     // 结构体指针实现二叉树结构
    push_up: func void(this); // 函数变量模拟成员函数，但需要手动赋值
}
```

#### 5. 函数声明
```
func swap(p: int*, q: int*): void
{
    let tmp = *p;
    *p = *q;
    *q = tmp;
    return default; // func 体里最后必须有 return
}
```

#### 6. 控制流
```
if (cond) { ... } else { ... };
while (cond) { ... };
try { ... } catch { ... };
```

#### 7. 内存管理
```
let p = struct node*(alloc(sizeof(struct node)));
free(p);
```

#### 8. I/O 操作
```
input(x, y);
output(x, ' ', y, '\n');
```

#### 9. 程序结构
```
program useless_ident {
    // 全局变量、结构体和函数
    main {
        // 主程序
    }
}
```

## 四、词法分析（Flex）

**文件**：`source/l25.l`

**扩展的 L25 语言关键字**：

```
program struct this func main return
let int bool char void float string
if else while try catch
input output alloc free sizeof
true false default // bool 和 void 类型常量
```
检查词法错误，同时记录 `yylloc`(`first_line/column`、`last_line/column`) 在错误报告模板中提供精确的位置信息。

## 五、语法分析（Bison）

**文件**：`source/l25.y`

**扩展的 L25 语言文法**：

```
%%
program
:   PROGRAM IDENT '{' global_list_opt MAIN '{' stmt_list '}' '}'
;
global_list_opt
:   {}
|   global_list_opt declare_def
|   global_list_opt struct_def
|   global_list_opt func_def
;
declare_def // 全局变量声明
:   LET IDENT ':' type_spec
;
struct_def // 结构体声明
:   STRUCT IDENT '{' field_list_opt '}'
;
field_list_opt
:   {}
|   field_list
;
field_list
:   field_def
|   field_list field_def
;
field_def
:   IDENT ':' type_spec ';'
;
func_def // 函数声明
:   FUNC IDENT '(' arg_list_opt ')' ':' type_spec '{' stmt_list RETURN right_expr ';' '}'
;
arg_list_opt
:   {}
|   arg_list
;
arg_list
:   arg_def
|   arg_list ',' arg_def
;
arg_def
:   IDENT ':' type_spec
;
type_spec // 类型系统
:   INT
|   BOOL
|   CHAR
|   VOID
|   FLOAT
|   STRING
|   STRUCT IDENT
|   type_spec '*'
|   type_spec '[' INT_CONST ']'
|   FUNC type_spec '(' type_spec_list_opt ')'
;
type_spec_list_opt
:   {}
|   type_spec_list
;
type_spec_list
:   type_spec
|   type_spec_list ',' type_spec
|   THIS
;
stmt_list
:   stmt ';'
|   stmt_list stmt ';'
|   '{' stmt_list '}' ';'
|   stmt_list '{' stmt_list '}' ';'
;
stmt
:   declare_stmt
|   assign_stmt
|   if_stmt
|   while_stmt
|   try_catch_stmt
|   func_call
|   input_call
|   output_call
|   alloc_call
|   free_call
;
declare_stmt
:   LET IDENT ':' type_spec
|   LET IDENT ':' type_spec '=' right_expr
|   LET IDENT '=' right_expr
assign_stmt
:   left_expr '=' right_expr
;
const_expr // 常量表达式
:   INT_CONST
|   BOOL_CONST
|   CHAR_CONST
|   VOID_CONST
|   FLOAT_CONST
|   STRING_CONST
|   SIZEOF '(' type_spec ')'
;
left_expr // 左值表达式，在栈中存放起始地址
:   IDENT
|   '*' factor %prec UNARY // 解引用为左值
|   left_expr '[' right_expr ']'
|   left_expr '.' IDENT
|   factor ARROW IDENT
;
right_expr // 右值表达式，在栈中存放完整内容
:   right_expr AND compare_expr
|   right_expr OR compare_expr
|   compare_expr
;
compare_expr
:   bitwise_expr '<' bitwise_expr
|   bitwise_expr '>' bitwise_expr
|   bitwise_expr LE bitwise_expr
|   bitwise_expr GE bitwise_expr
|   bitwise_expr EQ bitwise_expr
|   bitwise_expr NEQ bitwise_expr
|   bitwise_expr
;
bitwise_expr
:   bitwise_expr '&' bitwise_term
|   bitwise_expr '|' bitwise_term
|   bitwise_expr '^' bitwise_term
|   bitwise_term
;
bitwise_term
:   bitwise_term LSH arith_expr
|   bitwise_term RSH arith_expr
|   arith_expr
;
arith_expr
:   arith_expr '+' arith_term
|   arith_expr '-' arith_term
|   arith_term
;
arith_term
:   arith_term '*' factor
|   arith_term '/' factor
|   arith_term '%' factor
|   factor
;
factor // 右值表达式基本单元
:   left_expr
|   const_expr
|   '(' right_expr ')'
|   type_spec '(' right_expr ')'
|   func_call
|   alloc_call
|   '&' left_expr %prec UNARY // 取地址为右值
|   '-' factor %prec UNARY
|   '+' factor %prec UNARY
|   '~' factor %prec UNARY
|   '!' factor %prec UNARY
;
if_stmt
:   IF '(' right_expr ')' '{' stmt_list '}' else_opt
;
else_opt
:   {}
|   ELSE '{' stmt_list '}'
;
while_stmt
:   WHILE '(' right_expr ')' '{' stmt_list '}'
try_catch_stmt
:   TRY '{' stmt_list '}' CATCH '{' stmt_list '}'
func_call // 函数调用，结构体的函数成员变量可类似成员函数调用
:   IDENT '(' param_list_opt ')'
|   left_expr '.' IDENT '(' param_list_opt ')'
|   factor ARROW IDENT '(' param_list_opt ')'
;
param_list_opt
:   {}
|   param_list
;
param_list
:   right_expr
|   param_list ',' right_expr
;
input_call
:   INPUT '(' in_arg_list ')'
;
in_arg_list
:   left_expr
|   in_arg_list ',' left_expr
;
output_call
:   OUTPUT '(' out_arg_list ')'
;
out_arg_list
:   right_expr
|   out_arg_list ',' right_expr
;
alloc_call // 分配堆内存，返回 void*
:   ALLOC '(' right_expr ')'
;
free_call // 释放堆内存，接受任何指针
:   FREE '(' right_expr ')'
;

```

`type_spec，xxx_call，xxx_expr` 都记录类型信息，左值/右值分离、提供严格的类型检查和详细错误信息，同时使用符号表按作用域管理变量，检测未定义与重定义错误，统一 `Compile error` 输出。

在 Bison 规则中调用 `emit(op, val)` 生成中间代码，使用 `entXxx, xxxStk` 管理跳转地址等信息并回填。

## 六、中间代码（类 Pcode）

**使用的类 Pcode 指令**：

#### 1. 栈顶元素及栈帧指令
|指令|作用|
|---|---|
|PUSH val|将常量 val 压入栈顶|
|POP|弹出栈顶元素并丢弃|
|DUP|复制栈顶元素并压入栈顶|
|SWAP n|交换栈顶元素与栈顶第 n+1 个元素（n=0 时无操作）
|PUSHFP|将当前帧指针 FP 压入栈顶|
|MOVFP|设置 FP=SP，开始新的栈帧|
|POPFP|弹出栈顶值并恢复为上一帧指针 FP|
|ADDSP n|设置 SP+=n，分配或回收栈空间|

#### 2. 内存相关指令
|指令|作用|
|---|---|
|PADDRL offset|弹出偏移值，计算局部地址 FP+offset 压入栈顶|
|PADDRA size|弹出索引和基地址，计算数组元素地址 base + index * size 压入栈顶|
|PADDRF offset|弹出基地址，计算结构体字段地址 base + offset 压入栈顶|
|LOD|弹出地址，加载内存（栈或堆）中的值压入栈顶|
|STR|弹出值和地址，将值存储到内存（栈或堆）|
|ALLOC|弹出所需字节数，调用分配器获得堆地址压入栈顶|
|FREE|弹出地址，释放堆内存|

#### 3. 控制流指令
|指令|作用|
|---|---|
|JMP target|无条件跳转到指令索引 target|
|JPC target|弹出栈顶值并，为 0 时跳转到指令索引 target|
|CALL|弹出跳转地址 addr，压入返回地址 PC+1，然后设置 PC = addr|
|RET|弹出跳转地址 addr，然后设置 PC = addr|
|PUSHE handler|设置异常处理帧，handler 为处理地址|
|POPE|弹出最近的异常处理帧|

#### 4. 基本类型指令
|指令|作用|
|---|---|
|ADD/SUB/MUL/DIV/MOD|弹出两个整数，执行算术运算，结果压入栈顶|
|ADDF/SUBF/MULF/DIVF|弹出两个浮点数，执行算术运算，结果压入栈顶|
|LT/GT/LE/GE/EQ/NEQ|弹出两个整数，执行比较运算，结果压入栈顶|
|LTF/GTF/LEF/GEF/EQF/NEQF|弹出两个浮点数，执行比较运算，结果压入栈顶|
|AND/OR/XOR/LSH/RSH|弹出两个整数，执行位运算，结果压入栈顶|
|ANDB/ORB|弹出两个布尔值，执行逻辑运算，结果压入栈顶|
|NEG/COMPL|弹出两个整数，执行取负/取反，结果压入栈顶|
|NEGF|弹出两个浮点数，执行取负，结果压入栈顶|
|ADDS|弹出两个定长字符串，进行拼接，结果压入栈顶|
|MULS|弹出定长字符串和次数，进行重复，结果压入栈顶|
|I2F/F2I|整数与浮点互转|
|I2B/B2I|整数与布尔互转|
|I2C/C2I|整数与字符互转|
|F2B/B2F|浮点与布尔互转|
|SCAN/SCANF/GET/GETS|通过标准输入流输入|
|PRINT/PRINTF/PUT/PUTS|通过标准输出流输出|

除字符串操作（ADDS, MULS）需要内部循环处理外，其余指令均为单步执行，无需在虚拟机中维护循环逻辑。除无条件跳转 JMP 和条件跳转 JPC 外，不提供其他复杂跳转指令。

精简的指令执行模型减少分支预测和循环开销，提高解释器执行效率。通过异常处理帧和明确跳转，能够快速恢复运行状态，提升程序健壮性。

## 七、解释器实现（C++）

**文件**：`source/pcode.cc`

**虚拟机架构**：
1. 程序计数器 `PC`：指向当前指令索引；
2. 栈指针 `SP` & 帧指针 `FP`：管理函数调用栈帧与局部变量。

**内存模型**：
1. 使用 `std::vector<long long>` 虚拟栈空间；
2. 使用 `std::map<int, long long>` 虚拟堆空间；
3. 使用伙伴分配器分配堆空间。

**异常处理**：
1. 使用 `std::vector<ExceptionFrame>` 记录 `handlerPC, savedSP, savedFP`；
2. `DIV/MOD/DIVF` 检测除零，通过 `handle_divide_by_zero()` 恢复到最近的异常帧。

**调试支持**：
1. 在 #ifndef NDEBUG 块中输出执行计数 (cnt)、SP/FP/PC、op/val 信息；
2. 统一 `Runtime error` 输出，设置精确异常位置与原因。


## 八、构建与运行

#### 环境依赖

**Flex, Bison, C++20**

#### 构建命令
```
./build.sh
```
或

```
bison -o build/l25.tab.cc -d source/l25.y
flex -o build/l25.lex.cc -d source/l25.l
g++ -std=c++20 -Wno-free-nonheap-object \
    -o build/compiler build/l25.tab.cc build/l25.lex.cc
g++ -std=c++20 -DNDEBUG \
    -o build/interpreterd source/pcode.cc
g++ -std=c++20 \
    -o build/interpreter source/pcode.cc
```

**输出**：`build/compiler, build/interpreter, build/interpreterd`

#### 编译与执行

```
./run.sh sample/map.l25 -o sample/map.pcode
./run.sh sample/map.l25 -o sample/map.pcode -d # 调试模式
```
或
```
build/compiler < sample/map.l25 > sample/map.pcode
build/interpreter sample/map.pcode
build/interpreterd sample/map.pcode # 调试模式
```
**输出**：中间代码 `sample/map.pcode`

解释器通过标准输入输出进行交互

## 九、总结与展望

本项目基于 Flex/Bison 完成 L25 语言编译前端，生成类 Pcode 中间代码，并设计了功能完备的解释器。后续可扩展：

1. **代码优化**：常量折叠、减少栈空间移动
2. **冲突消除**：不依赖优先级解决 `*a[i],*a.x,&a->x` 三种 LALR(1) 文法下的冲突
3. **语言特性**：面向对象支持、泛型支持
4. **JIT 编译**：在虚拟机基础上动态编译为本地指令

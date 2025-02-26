`bison -d count_chars.y`

This creates count_chars.tab.c and count_chars.tab.h.

`flex count_chars.l`

This creates lex.yy.c.

`gcc count_chars.tab.c lex.yy.c -o count_chars -lfl`

`./count_chars < your_input_file.txt`
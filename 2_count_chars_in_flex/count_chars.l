%{
#include "count_chars.tab.h"
%}

%%

.         { return CHARACTER; }
<<EOF>>   { return 0; }

%%

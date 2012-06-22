class JS
  # from http://www.h4.dion.ne.jp/~unkai/js/parser.jsy

#prechigh
#preclow

rule

program
	: source_elements
	;

function_declaration
	: FUNCTION IDENTIFIER '(' formal_parameter_list__opt ')' '{' function_body '}'
	;

function_expression
	: FUNCTION identifier__opt '(' formal_parameter_list__opt ')' '{' function_body '}'
	;

formal_parameter_list
	: IDENTIFIER
	| formal_parameter_list ',' IDENTIFIER
	;

function_body
	:
	| source_elements
	;

source_elements
	: source_element
	| source_elements source_element
	;

source_element
	: statement
	| function_declaration
	;

formal_parameter_list__opt
	:
	| formal_parameter_list
	;

identifier__opt
	:
	| IDENTIFIER
	;

statement
	: block
	| variable_statement
	| empty_statement
	| expression_statement
	| if_statement
	| iteration_statement
	| continue_statement
	| break_statement
	| return_statement
	| with_statement
	| labelled_statement
	| switch_statement
	| throw_statement
	| try_statement
	;

block
	: '{' '}'
	| '{' statement_list '}'
	;

statement_list
	: statement
	| statement_list statement
	;

variable_statement : VAR variable_declaration_list ';' { ep"varstat "; push(:var, [:vardeclist]) }
    | VAR variable_declaration_list { ep"varstat-wo-semi "; push(:var, [:vardeclist]) }
	;

variable_declaration_list
: variable_declaration { ep"vardeclist-firstdecl " }
| variable_declaration_list ',' variable_declaration { ep"vardeclist-appenddecl " }
;

variable_declaration_list_no_in
: variable_declaration_no_in { ep "vardeclist-no-in-first " }
| variable_declaration_list_no_in ',' variable_declaration_no_in { ep"vardeclist-no-in-append " }
;

variable_declaration
	: IDENTIFIER initialiser__opt
	;

variable_declaration_no_in
	: IDENTIFIER initialiser_no_in__opt
	;

initialiser
	: '=' assignment_expression
	;

initialiser_no_in
	: '=' assignment_expression_no_in
	;

empty_statement
	: ';'
	;

expression_statement
	: expression ';'
	;

if_statement
	: IF '(' expression ')' statement ELSE statement
	| IF '(' expression ')' statement
	;

iteration_statement
	: DO statement WHILE '(' expression ')' ';'
	| WHILE '(' expression ')' statement
	| FOR '(' expression_no_in__opt ';' expression__opt ';' expression__opt ')' statement
	| FOR '(' VAR variable_declaration_list_no_in ';' expression__opt ';' expression__opt ')' statement
	| FOR '(' left_hand_side_expression IN expression ')' statement
	| FOR '(' VAR variable_declaration_no_in IN expression ')' statement
	;

continue_statement
	: CONTINUE identifier__opt ';'
	;

break_statement
	: BREAK identifier__opt ';'
	;

return_statement
	: RETURN expression__opt ';'
	;

with_statement
	: WITH '(' expression ')' statement
	;

switch_statement
	: SWITCH '(' expression ')' case_block
	;

case_block
	: '{' case_clauses__opt '}'
	| '{' case_clauses__opt default_clause case_clauses__opt '}'
	;

case_clauses
	: case_clause
	| case_clauses case_clause
	;

case_clause
	: CASE expression ':' statement_list__opt
	;

default_clause
	: DEFAULT ':' statement_list__opt
	;

labelled_statement
	: IDENTIFIER ':' statement
	;

throw_statement
	: THROW expression ';'
	;

try_statement
	: TRY block catch
	| TRY block finally
	| TRY block catch finally
	;

catch
	: CATCH '(' IDENTIFIER ')' block
	;

finally
	: FINALLY block
	;

statement_list__opt
	:
	| statement_list
	;

initialiser__opt
	:
	| initialiser
	;

initialiser_no_in__opt
	:
	| initialiser_no_in
	;

case_clauses__opt
	:
	| case_clauses
	;

literal
	: null_literal
	| boolean_literal
	| NUMERIC_LITERAL
	| STRING_LITERAL
	| REGEXP_LITERAL
	;

null_literal
	: NULL
	;

boolean_literal
	: TRUE
	| FALSE
	;

primary_expression
	: THIS
	| IDENTIFIER
	| literal
	| array_literal
	| object_literal
	| '(' expression ')'
	;

array_literal
	: '[' elision__opt ']'
	| '[' element_list ']'
	| '[' element_list ',' elision__opt ']'
	;

element_list
	: elision__opt assignment_expression
	| element_list ',' elision__opt assignment_expression
	;

elision
	: ','
	| elision ','
	;

object_literal
	: '{' property_name_and_value_list__opt '}'
	;

property_name_and_value_list__opt
	:
	| property_name_and_value_list
	;

property_name_and_value_list
	: property_name ':' assignment_expression
	| property_name_and_value_list ',' property_name ':' assignment_expression
	;

property_name
	: IDENTIFIER
	| STRING_LITERAL
	| NUMERIC_LITERAL
	;

member_expression
	: primary_expression
	| function_expression
	| member_expression '[' expression ']'
	| member_expression '.' IDENTIFIER
	| NEW member_expression arguments
	;

new_expression
	: member_expression
	| NEW new_expression
	;

call_expression
	: member_expression arguments
	| call_expression arguments
	| call_expression '[' expression ']'
	| call_expression '.' IDENTIFIER
	;

arguments
	: '(' ')'
	| '(' argument_list ')'
	;

argument_list
	: assignment_expression
	| argument_list ',' assignment_expression
	;

left_hand_side_expression
	: new_expression
	| call_expression
	;

postfix_expression
	: left_hand_side_expression
	| left_hand_side_expression INCREMENT
	| left_hand_side_expression DECREMENT
	;

unary_expression
	: postfix_expression
	| DELETE unary_expression
	| VOID unary_expression
	| TYPEOF unary_expression
	| INCREMENT unary_expression
	| DECREMENT unary_expression
	| '+' unary_expression
	| '-' unary_expression
	| '~' unary_expression
	| '!' unary_expression
	;

multiplicative_expression
	: unary_expression
	| multiplicative_expression '*' unary_expression
	| multiplicative_expression '/' unary_expression
	| multiplicative_expression '%' unary_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

shift_expression
	: additive_expression
	| shift_expression SHIFT_LEFT additive_expression
	| shift_expression SHIFT_RIGHT additive_expression
	| shift_expression U_SHIFT_RIGHT additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LESS_EQUAL shift_expression
	| relational_expression GRATER_EQUAL shift_expression
	| relational_expression INSTANCEOF shift_expression
	| relational_expression IN shift_expression
	;

relational_expression_no_in
	: shift_expression
	| relational_expression_no_in '<' shift_expression
	| relational_expression_no_in '>' shift_expression
	| relational_expression_no_in LESS_EQUAL shift_expression
	| relational_expression_no_in GRATER_EQUAL shift_expression
	| relational_expression_no_in INSTANCEOF shift_expression
	;

equality_expression
	: relational_expression
	| equality_expression EQUAL relational_expression
	| equality_expression NOT_EQUAL relational_expression
	| equality_expression EQ relational_expression
	| equality_expression NOT_EQ relational_expression
	;

equality_expression_no_in
	: relational_expression_no_in
	| equality_expression_no_in EQUAL relational_expression_no_in
	| equality_expression_no_in NOT_EQUAL relational_expression_no_in
	| equality_expression_no_in EQ relational_expression_no_in
	| equality_expression_no_in NOT_EQ relational_expression_no_in
	;

bitwise_and_expression
	: equality_expression
	| bitwise_and_expression '&' equality_expression
	;

bitwise_and_expression_no_in
	: equality_expression_no_in
	| bitwise_and_expression_no_in '&' equality_expression_no_in
	;

bitwise_xor_expression
	: bitwise_and_expression
	| bitwise_xor_expression '^' bitwise_and_expression
	;

bitwise_xor_expression_no_in
	: bitwise_and_expression_no_in
	| bitwise_xor_expression_no_in '^' bitwise_and_expression_no_in
	;

bitwise_or_expression
	: bitwise_xor_expression
	| bitwise_or_expression '|' bitwise_xor_expression
	;

bitwise_or_expression_no_in
	: bitwise_xor_expression_no_in
	| bitwise_or_expression_no_in '|' bitwise_xor_expression_no_in
	;

logical_and_expression
	: bitwise_or_expression
	| logical_and_expression LOGICAL_AND bitwise_or_expression
	;

logical_and_expression_no_in
	: bitwise_or_expression_no_in
	| logical_and_expression_no_in LOGICAL_AND bitwise_or_expression_no_in
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression LOGICAL_OR logical_and_expression
	;

logical_or_expression_no_in
	: logical_and_expression_no_in
	| logical_or_expression_no_in LOGICAL_OR logical_and_expression_no_in
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' assignment_expression ':' assignment_expression
	;

conditional_expression_no_in
	: logical_or_expression_no_in
	| logical_or_expression_no_in '?' assignment_expression_no_in ':' assignment_expression_no_in
	;

assignment_expression
	: conditional_expression
	| left_hand_side_expression assignment_operator assignment_expression
	;

assignment_expression_no_in
	: conditional_expression_no_in
	| left_hand_side_expression assignment_operator assignment_expression_no_in
	;

assignment_operator
	: '='
	| MUL_LET
	| DIV_LET
	| MOD_LET
	| ADD_LET
	| SUB_LET
	| SHIFT_LEFT_LET
	| SHIFT_RIGHT_LET
	| U_SHIFT_RIGHT_LET
	| AND_LET
	| NOT_LET
	| OR_LET
	;

expression
	: assignment_expression
	| expression ',' assignment_expression
	;

expression_no_in
	: assignment_expression_no_in
	| expression_no_in ',' assignment_expression_no_in
	;

expression_no_in__opt
	:
	| expression_no_in
	;

expression__opt
	:
	| expression
	;

elision__opt
	:
	| elision
	;

end

---- header = header.rb
---- inner = inner.rb
---- footer = footer.rb

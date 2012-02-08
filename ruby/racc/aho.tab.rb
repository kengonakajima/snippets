require 'racc/parser'

class Gen < ::Racc::Parser

##### racc 1.3.9 generates ###

racc_reduce_table = [
 0, 0, :racc_error,
 0, 3, :_reduce_none ]

racc_reduce_n = 2

racc_shift_n = 4

racc_action_table = [
     2,     3 ]

racc_action_check = [
     1,     2 ]

racc_action_pointer = [
   nil,     0,     1,   nil ]

racc_action_default = [
    -1,    -2,    -2,     4 ]

racc_goto_table = [
     1 ]

racc_goto_check = [
     1 ]

racc_goto_pointer = [
   nil,     0 ]

racc_goto_default = [
   nil,   nil ]

racc_token_table = {
 false => 0,
 Object.new => 1 }

racc_use_result_var = true

racc_nt_base = 2

Racc_arg = [
 racc_action_table,
 racc_action_check,
 racc_action_default,
 racc_action_pointer,
 racc_goto_table,
 racc_goto_check,
 racc_goto_default,
 racc_goto_pointer,
 racc_nt_base,
 racc_reduce_table,
 racc_token_table,
 racc_shift_n,
 racc_reduce_n,
 racc_use_result_var ]

Racc_token_to_s_table = [
'$end',
'error',
'$start',
'all']

Racc_debug_parser = false

##### racc system variables end #####

 # reduce 0 omitted

 # reduce 1 omitted

 def _reduce_none( val, _values, result )
  result
 end

end   # class Gen

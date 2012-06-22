module Narcissus
  class Node

    attr_accessor :type, :value, :lineno, :start, :end, :tokenizer, :initializer
    attr_accessor :name, :params, :fun_decls, :var_decls, :body, :function_form
    attr_accessor :assign_op, :expression, :condition, :then_part, :else_part
    attr_accessor :read_only, :is_loop, :setup, :postfix, :update, :exception
    attr_accessor :object, :iterator, :var_decl, :label, :target, :try_block
    attr_accessor :catch_clauses, :var_name, :guard, :block, :discriminant, :cases
    attr_accessor :default_index, :case_label, :statements, :statement

    attr_accessor :children

    def initialize(t, type = nil)
      if token = t.token
        if type != nil
          @type = type
        else
          @type = token.type
        end
        @value = token.value
        @lineno = token.lineno
        @start = token.start
        @end = token.end
      else
        @type = type
        @lineno = t.lineno
      end
      @tokenizer = t
      @children = []
      #for (var i = 2; i < arguments.length; i++)
      #this.push(arguments[i]);
    end

    # Always use push to add operands to an expression, to update start and end.
    def push(kid)
      @start = kid.start if kid.start && @start && kid.start < @start
      @end = kid.end if kid.end && @end && @end < kid.end
      @children.push(kid)
    end

    def get_source
      @tokenizer.source.slice(@start, @end)
    end

    def filename
      @tokenizer.filename
    end
  end
end

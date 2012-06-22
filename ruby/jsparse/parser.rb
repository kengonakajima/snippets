require 'enumerator'
require 'data'
require 'tokenizer'
require 'node'

module Narcissus
  class SyntaxError
    def initialize(msg, tokenizer)
      puts msg
      puts "on line " + tokenizer.lineno.to_s
    end
  end

  class Token
    attr_accessor :type, :value, :start, :end, :lineno, :assign_op
  end

  class CompilerContext
    attr_accessor :in_function, :stmt_stack, :fun_decls, :var_decls
    attr_accessor :bracket_level, :curly_level, :paren_level, :hook_level
    attr_accessor :ecma_strict_mode, :in_for_loop_init

    def initialize(in_function)
      @in_function = in_function
      @stmt_stack = []
      @fun_decls = []
      @var_decls = []
      
      @bracket_level = @curly_level = @paren_level = @hook_level = 0
      @ecma_strict_mode = @in_for_loop_init = false
    end
  end

  def self.script(t, x)
    n = statements(t, x)
    n.type = CONSTS["SCRIPT"]
    n.fun_decls = x.fun_decls
    n.var_decls = x.var_decls
    return n
  end

  # Statement stack and nested statement handler.
  # nb. Narcissus allowed a function reference, here we use statement explicitly
  def self.nest(t, x, node, end_ = nil)
    x.stmt_stack.push(node)
    n = statement(t, x)
    x.stmt_stack.pop
    end_ and t.must_match(end_)
    return n
  end

  def self.statements(t, x)
    n = Node.new(t, CONSTS["BLOCK"])
    x.stmt_stack.push(n)
    n.push(statement(t, x)) while !t.done and t.peek != CONSTS["RIGHT_CURLY"]
    x.stmt_stack.pop
    return n
  end

  def self.block(t, x)
    t.must_match(CONSTS["LEFT_CURLY"])
    n = statements(t, x)
    t.must_match(CONSTS["RIGHT_CURLY"])
    return n
  end

  DECLARED_FORM = 0
  EXPRESSED_FORM = 1
  STATEMENT_FORM = 2

  def self.statement(t, x)
    tt = t.get

    # Cases for statements ending in a right curly return early, avoiding the
    # common semicolon insertion magic after this switch.
    case tt
    when CONSTS["FUNCTION"]
      return function_definition(t, x, true, 
        (x.stmt_stack.length > 1) && STATEMENT_FORM || DECLARED_FORM)

    when CONSTS["LEFT_CURLY"]
      n = statements(t, x)
      t.must_match(CONSTS["RIGHT_CURLY"])
      return n
      
    when CONSTS["IF"]
      n = Node.new(t)
      n.condition = paren_expression(t, x)
      x.stmt_stack.push(n)
      n.then_part = statement(t, x)
      n.else_part = t.match(CONSTS["ELSE"]) ? statement(t, x) : nil
      x.stmt_stack.pop()
      return n

    when CONSTS["SWITCH"]
      n = Node.new(t)
      t.must_match(CONSTS["LEFT_PAREN"])
      n.discriminant = expression(t, x)
      t.must_match(CONSTS["RIGHT_PAREN"])
      n.cases = []
      n.default_index = -1
      x.stmt_stack.push(n)
      t.must_match(CONSTS["LEFT_CURLY"])
      while (tt = t.get) != CONSTS["RIGHT_CURLY"]
        case tt
        when CONSTS["DEFAULT"], CONSTS["CASE"]
          if tt == CONSTS["DEFAULT"] and n.default_index >= 0
            raise SyntaxError.new("More than one switch default", t)
          end
          n2 = Node.new(t)
          if tt == CONSTS["DEFAULT"]
            n.default_index = n.cases.length
          else
            n2.case_label = expression(t, x, CONSTS["COLON"])
          end
          
        else
          raise SyntaxError.new("Invalid switch case", t)
        end
        t.must_match(CONSTS["COLON"])
        n2.statements = Node.new(t, CONSTS["BLOCK"])
        while (tt = t.peek) != CONSTS["CASE"] and tt != CONSTS["DEFAULT"] and tt != CONSTS["RIGHT_CURLY"]
          n2.statements.push(statement(t, x))
        end
        n.cases.push(n2)
      end
      x.stmt_stack.pop
      return n
      
    when CONSTS["FOR"]
      n = Node.new(t)
      n.is_loop = true
      t.must_match(CONSTS["LEFT_PAREN"])
      if (tt = t.peek) != CONSTS["SEMICOLON"]
        x.in_for_loop_init = true
        if tt == CONSTS["VAR"] or tt == CONSTS["CONST"]
          t.get
          n2 = variables(t, x)
        else
          n2 = expression(t, x)
        end
        x.in_for_loop_init = false
      end
      if n2 and t.match(CONSTS["IN"])
        n.type = CONSTS["FOR_IN"]
        if n2.type == CONSTS["VAR"]
          if n2.length != 1
            raise SyntaxError.new("Invalid for..in left-hand side", t)
          end
          # NB: n2[0].type == IDENTIFIER and n2[0].value == n2[0].name.
          n.iterator = n2[0]
          n.var_decl = n2
        else
          n.iterator = n2
          n.var_decl = nil
        end
        n.object = expression(t, x)
      else
        n.setup = n2 or nil
        t.must_match(CONSTS["SEMICOLON"])
        n.condition = (t.peek == CONSTS["SEMICOLON"]) ? nil : expression(t, x)
        t.must_match(CONSTS["SEMICOLON"])
        n.update = (t.peek == CONSTS["RIGHT_PAREN"]) ? nil : expression(t, x)
      end
      t.must_match(CONSTS["RIGHT_PAREN"])
      n.body = nest(t, x, n)
      return n
      
    when CONSTS["WHILE"]
      n = Node.new(t)
      n.is_loop = true
      n.condition = paren_expression(t, x)
      n.body = nest(t, x, n)
      return n
      
    when CONSTS["DO"]
      n = Node.new(t)
      n.is_loop = true
      n.body = nest(t, x, n, CONSTS["WHILE"])
      n.condition = paren_expression(t, x)
      if !x.ecma_strict_mode
        # <script language="JavaScript"> (without version hints) may need
        # automatic semicolon insertion without a newline after do-while.
        # See http://bugzilla.mozilla.org/show_bug.cgi?id=238945.
        t.match(CONSTS["SEMICOLON"])
        return n
      end
      
    when CONSTS["BREAK"], CONSTS["CONTINUE"]
      n = Node.new(t)
      if t.peek_on_same_line == CONSTS["IDENTIFIER"]
        t.get
        n.label = t.token.value
      end
      ss = x.stmt_stack
      i = ss.length
      label = n.label
      if label
        begin
          i -= 1
          raise SyntaxError.new("Label not found", t) if i < 0
        end while (ss[i].label != label)
      else
        begin
          i -= 1
          raise SyntaxError.new("Invalid " + ((tt == CONSTS["BREAK"]) and "break" or "continue"), t) if i < 0
        end while !ss[i].is_loop and (tt != CONSTS["BREAK"] or ss[i].type != CONSTS["SWITCH"])
      end
      n.target = ss[i]
      
    when CONSTS["TRY"]
      n = Node.new(t)
      n.try_block = block(t, x)
      n.catch_clauses = []
      while t.match(CONSTS["CATCH"])
        n2 = Node.new(t)
        t.must_match(CONSTS["LEFT_PAREN"])
        n2.var_name = t.must_match(CONSTS["IDENTIFIER"]).value
        if t.match(CONSTS["IF"])
          raise SyntaxError.new("Illegal catch guard", t) if x.ecma_strict_mode
          if n.catch_clauses.length and !n.catch_clauses.last.guard
            raise SyntaxError.new("Guarded catch after unguarded", t)
          end
          n2.guard = expression(t, x)
        else
          n2.guard = nil
        end
        t.must_match(CONSTS["RIGHT_PAREN"])
        n2.block = block(t, x)
        n.catch_clauses.push(n2)
      end
      n.finallyBlock = block(t, x) if t.match(CONSTS["FINALLY"])
      if !n.catch_clauses.length and !n.finallyBlock
        raise SyntaxError.new("Invalid try statement", t)
      end
      return n
      
    when CONSTS["CATCH"]
    when CONSTS["FINALLY"]
      raise SyntaxError.new(tokens[tt] + " without preceding try", t)
      
    when CONSTS["THROW"]
      n = Node.new(t)
      n.exception = expression(t, x)
      
    when CONSTS["RETURN"]
      raise SyntaxError.new("Invalid return", t) unless x.in_function
      n = Node.new(t)
      tt = t.peek_on_same_line
      if tt != CONSTS["END"] and tt != CONSTS["NEWLINE"] and tt != CONSTS["SEMICOLON"] and tt != CONSTS["RIGHT_CURLY"]
        n.value = expression(t, x)
      end
      
    when CONSTS["WITH"]
      n = Node.new(t)
      n.object = paren_expression(t, x)
      n.body = nest(t, x, n)
      return n
      
    when CONSTS["VAR"], CONSTS["CONST"]
      n = variables(t, x)
      
    when CONSTS["DEBUGGER"]
      n = Node.new(t)
      
    when CONSTS["NEWLINE"], CONSTS["SEMICOLON"]
      n = Node.new(t, CONSTS["SEMICOLON"])
      n.expression = nil
      return n

    else
      if tt == CONSTS["IDENTIFIER"] and t.peek == CONSTS["COLON"]
        label = t.token.value
        ss = x.stmt_stack
        (ss.length - 1).times do |i|
          raise SyntaxError.new("Duplicate label", t) if ss[i].label == label
        end
        t.get
        n = Node.new(t, CONSTS["LABEL"])
        n.label = label
        n.statement = nest(t, x, n)
        return n
      end

      t.unget
      n = Node.new(t, CONSTS["SEMICOLON"])
      n.expression = expression(t, x)
      n.end = n.expression.end
    end

    if t.lineno == t.token.lineno
      tt = t.peek_on_same_line
      if tt != CONSTS["END"] and tt != CONSTS["NEWLINE"] and tt != CONSTS["SEMICOLON"] and tt != CONSTS["RIGHT_CURLY"]
        raise SyntaxError.new("Missing ; before statement", t)
      end
    end
    t.match(CONSTS["SEMICOLON"])
    return n
  end


  def self.function_definition (t, x, requireName, function_form)
    f = Node.new(t)
    if f.type != CONSTS["FUNCTION"]
      f.type = (f.value == "get") and CONSTS["GETTER"] or CONSTS["SETTER"]
    end
    if t.match(CONSTS["IDENTIFIER"])
      f.name = t.token.value
    elsif requireName
      raise SyntaxError.new("Missing function identifier", t)
    end
    t.must_match(CONSTS["LEFT_PAREN"])
    f.params = []
    while (tt = t.get) != CONSTS["RIGHT_PAREN"]
      raise SyntaxError.new("Missing formal parameter", t) unless tt == CONSTS["IDENTIFIER"]
      f.params.push(t.token.value)
      t.must_match(CONSTS["COMMA"]) unless t.peek == CONSTS["RIGHT_PAREN"]
    end
    
    t.must_match(CONSTS["LEFT_CURLY"])
    x2 = CompilerContext.new(true)
    f.body = script(t, x2)
    t.must_match(CONSTS["RIGHT_CURLY"])
    f.end = t.token.end
    f.function_form = function_form
    x.fun_decls.push(f) if function_form == CONSTS["DECLARED_FORM"]
    return f
  end


  def self.variables(t, x)
    n = Node.new(t)

    begin
      t.must_match(CONSTS["IDENTIFIER"])
      n2 = Node.new(t)
      n2.name = n2.value
      if t.match(CONSTS["ASSIGN"])
        raise SyntaxError.new("Invalid variable initialization", t) if t.token.assign_op
        n2.initializer = expression(t, x, CONSTS["COMMA"])
      end
      n2.read_only = (n.type == CONSTS["CONST"])
      n.push(n2)
      x.var_decls.push(n2)
    end while t.match(CONSTS["COMMA"])
    return n
  end


  def self.paren_expression (t, x)
    t.must_match(CONSTS["LEFT_PAREN"])
    n = expression(t, x)
    t.must_match(CONSTS["RIGHT_PAREN"])
    return n
  end


  def self.reduce(operators, operands, t)
    n = operators.pop
    op = n.type
    arity = OP_ARITY[op]
    if arity == -2
      if operands.length >= 2
        # Flatten left-associative trees.
        left = operands[operands.length - 2]
        
        if left.type == op
          right = operands.pop
          left.push(right)
          return left
        end
      end
      arity = 2
    end
    
    # Always use push to add operands to n, to update start and end.
    a = operands.slice!(operands.length - arity, operands.length)

    arity.times do |i|
      n.push(a[i])
    end
    
    # Include closing bracket or postfix operator in [start,end).
    n.end = t.token.end if n.end < t.token.end
    
    operands.push(n)
    return n
  end


  def self.expression(t, x, stop = nil)
    operators = []
    operands = []
    bl = x.bracket_level
    cl = x.curly_level
    pl = x.paren_level
    hl = x.hook_level
    
    gotoloopContinue = false
    until gotoloopContinue or (t.token and t.token.type == CONSTS["END"])
      gotoloopContinue = catch(:gotoloop) do
        #loop:
        while (tt = t.get) != CONSTS["END"]
          # Stop only if tt matches the optional stop parameter, and that
          # token is not quoted by some kind of bracket.
          if tt == stop and x.bracket_level == bl and x.curly_level == cl and x.paren_level == pl and x.hook_level == hl
            throw :gotoloop, true
          end
          
          case tt
          when CONSTS["SEMICOLON"]
            # NB: cannot be empty, statement handled that.
            throw :gotoloop, true;
            
          when CONSTS["ASSIGN"], CONSTS["HOOK"], CONSTS["COLON"]
            if t.scan_operand
              throw :gotoloop, true
            end
            
            # Use >, not >=, for right-associative ASSIGN and HOOK/COLON.
            while operators.length > 0 && OP_PRECEDENCE[operators.last.type] && OP_PRECEDENCE[operators.last.type] > OP_PRECEDENCE[tt]
              reduce(operators, operands, t)
            end
            if tt == CONSTS["COLON"]
              n = operators.last
              raise SyntaxError.new("Invalid label", t) if n.type != CONSTS["HOOK"]
              n.type = CONSTS["CONDITIONAL"]
              x.hook_level -= 1
            else
              operators.push(Node.new(t))
              if tt == CONSTS["ASSIGN"]
                operands.last.assign_op = t.token.assign_op
              else
                x.hook_level += 1 # tt == HOOK
              end
            end
            t.scan_operand = true
            
          when CONSTS["COMMA"],
            # Treat comma as left-associative so reduce can fold left-heavy
            # COMMA trees into a single array.
            CONSTS["OR"], CONSTS["AND"], CONSTS["BITWISE_OR"], CONSTS["BITWISE_XOR"],
            CONSTS["BITWISE_AND"], CONSTS["EQ"], CONSTS["NE"], CONSTS["STRICT_EQ"],
            CONSTS["STRICT_NE"], CONSTS["LT"], CONSTS["LE"], CONSTS["GE"],
            CONSTS["GT"], CONSTS["INSTANCEOF"], CONSTS["LSH"], CONSTS["RSH"],
            CONSTS["URSH"], CONSTS["PLUS"], CONSTS["MINUS"], CONSTS["MUL"],
            CONSTS["DIV"], CONSTS["MOD"], CONSTS["DOT"], CONSTS["IN"]

            # An in operator should not be parsed if we're parsing the head of
            # a for (...) loop, unless it is in the then part of a conditional
            # expression, or parenthesized somehow.
            if tt == CONSTS["IN"] and x.in_for_loop_init and x.hook_level == 0 and x.bracket_level == 0 and x.curly_level == 0 and x.paren_level == 0
              throw :gotoloop, true
            end
            
            if t.scan_operand
              throw :gotoloop, true
            end

            reduce(operators, operands, t) while operators.length > 0 && OP_PRECEDENCE[operators.last.type] && OP_PRECEDENCE[operators.last.type] >= OP_PRECEDENCE[tt]

            if tt == CONSTS["DOT"]
              t.must_match(CONSTS["IDENTIFIER"])
              node = Node.new(t, CONSTS["DOT"])
              node.push(operands.pop)
              node.push(Node.new(t))
              operands.push(node)
            else
              operators.push(Node.new(t))
              t.scan_operand = true
            end
            
          when CONSTS["DELETE"], CONSTS["VOID"], CONSTS["TYPEOF"], CONSTS["NOT"],
            CONSTS["BITWISE_NOT"], CONSTS["UNARY_PLUS"], CONSTS["UNARY_MINUS"],
            CONSTS["NEW"]

            if !t.scan_operand
              throw :gotoloop, true
            end
            operators.push(Node.new(t))
            
          when CONSTS["INCREMENT"], CONSTS["DECREMENT"]
            if t.scan_operand
              operators.push(Node.new(t)) # prefix increment or decrement
            else
              # Use >, not >=, so postfix has higher precedence than prefix.
              reduce(operators, operands, t) while operators.length > 0 && OP_PRECEDENCE[operators.last.type] && OP_PRECEDENCE[operators.last.type] > OP_PRECEDENCE[tt]
              n = Node.new(t, tt)
              n.push(operands.pop)
              n.postfix = true
              operands.push(n)
            end
            
          when CONSTS["FUNCTION"]
            if !t.scan_operand
              throw :gotoloop, true
            end
            operands.push(function_definition(t, x, false, CONSTS["EXPRESSED_FORM"]))
            t.scan_operand = false
            
          when CONSTS["NULL"], CONSTS["THIS"], CONSTS["TRUE"], CONSTS["FALSE"],
            CONSTS["IDENTIFIER"], CONSTS["NUMBER"], CONSTS["STRING"],
            CONSTS["REGEXP"]

            if !t.scan_operand
              throw :gotoloop, true
            end
            operands.push(Node.new(t))
            t.scan_operand = false
            
          when CONSTS["LEFT_BRACKET"]
            if t.scan_operand
              # Array initialiser.  Parse using recursive descent, as the
              # sub-grammar here is not an operator grammar.
              n = Node.new(t, CONSTS["ARRAY_INIT"])
              while (tt = t.peek) != CONSTS["RIGHT_BRACKET"]
                if tt == CONSTS["COMMA"]
                  t.get
                  n.push(nil)
                  next
                end
                n.push(expression(t, x, CONSTS["COMMA"]))
                break if !t.match(CONSTS["COMMA"])
              end
              t.must_match(CONSTS["RIGHT_BRACKET"])
              operands.push(n)
              t.scan_operand = false
            else
              # Property indexing operator.
              operators.push(Node.new(t, CONSTS["INDEX"]))
              t.scan_operand = true
              x.bracket_level += 1
            end
            
          when CONSTS["RIGHT_BRACKET"]
            if t.scan_operand or x.bracket_level == bl
              throw :gotoloop, true
            end
            while reduce(operators, operands, t).type != CONSTS["INDEX"]
              nil
            end
            x.bracket_level -= 1
            
          when CONSTS["LEFT_CURLY"]
            if !t.scan_operand
              throw :gotoloop, true
            end
            # Object initialiser.  As for array initialisers (see above),
            # parse using recursive descent.
            x.curly_level += 1
            n = Node.new(t, CONSTS["OBJECT_INIT"])

            catch(:gotoobject_init) do
              #object_init:
              if !t.match(CONSTS["RIGHT_CURLY"])
                begin
                  tt = t.get
                  if (t.token.value == "get" or t.token.value == "set") and t.peek == CONSTS["IDENTIFIER"]
                    raise SyntaxError.new("Illegal property accessor", t) if x.ecma_strict_mode
                    n.push(function_definition(t, x, true, CONSTS["EXPRESSED_FORM"]))
                  else
                    case tt
                    when CONSTS["IDENTIFIER"], CONSTS["NUMBER"], CONSTS["STRING"]
                      id = Node.new(t)
                      
                    when CONSTS["RIGHT_CURLY"]
                      raise SyntaxError.new("Illegal trailing ,", t) if x.ecma_strict_mode
                      throw :gotoobject_init
                      
                    else
                      raise SyntaxError.new("Invalid property name", t)
                    end
                    t.must_match(CONSTS["COLON"])
                    n2 = Node.new(t, CONSTS["PROPERTY_INIT"])
                    n2.push(id)
                    n2.push(expression(t, x, CONSTS["COMMA"]))
                    n.push(n2)
                  end
                end while t.match(CONSTS["COMMA"])
                t.must_match(CONSTS["RIGHT_CURLY"])
              end
              operands.push(n)
              t.scan_operand = false
              x.curly_level -= 1
            end

          when CONSTS["RIGHT_CURLY"]
            raise SyntaxError.new("PANIC: right curly botch", t) if !t.scan_operand and x.curly_level != cl
            throw :gotoloop, true
            
          when CONSTS["LEFT_PAREN"]
            if t.scan_operand
              operators.push(Node.new(t, CONSTS["GROUP"]))
            else
              reduce(operators, operands, t) while operators.length > 0 && OP_PRECEDENCE[operators.last.type] && OP_PRECEDENCE[operators.last.type] > OP_PRECEDENCE[CONSTS["NEW"]]
              # Handle () now, to regularize the n-ary case for n > 0.
              # We must set scan_operand in case there are arguments and
              # the first one is a regexp or unary+/-.
              n = operators.last
              t.scan_operand = true
              if t.match(CONSTS["RIGHT_PAREN"])
                if n && n.type == CONSTS["NEW"]
                  operators.pop
                  n.push(operands.pop)
                else
                  n = Node.new(t, CONSTS["CALL"])
                  n.push(operands.pop)
                  n.push(Node.new(t, CONSTS["LIST"]))
                end
                operands.push(n)
                t.scan_operand = false
                #puts "woah"
                break
              end
              if n && n.type == CONSTS["NEW"]
                n.type = CONSTS["NEW_WITH_ARGS"]
              else
                operators.push(Node.new(t, CONSTS["CALL"]))
              end
            end
            x.paren_level += 1
            
          when CONSTS["RIGHT_PAREN"]
            if t.scan_operand or x.paren_level == pl
              throw :gotoloop, true
            end
            while (tt = reduce(operators, operands, t).type) != CONSTS["GROUP"] \
              and tt != CONSTS["CALL"] and tt != CONSTS["NEW_WITH_ARGS"]
              nil
            end
            if tt != CONSTS["GROUP"]
              n = operands.last
              if n.children[1].type != CONSTS["COMMA"]
                n2 = n.children[1]
                n.children[1] = Node.new(t, CONSTS["LIST"])
                n.children[1].push(n2)
              else
                n.children[1].type = CONSTS["LIST"]
              end
            end
            x.paren_level -= 1
            
            # Automatic semicolon insertion means we may scan across a newline
            # and into the beginning of another statement.  If so, break out of
            # the while loop and let the t.scan_operand logic handle errors.
          else
            throw :gotoloop, true
          end
        end

      end
    end

    raise SyntaxError.new("Missing : after ?", t) if x.hook_level != hl
    raise SyntaxError.new("Missing operand", t) if t.scan_operand
    
    # Resume default mode, scanning for operands, not operators.
    t.scan_operand = true
    t.unget
    reduce(operators, operands, t) while operators.length > 0
    return operands.pop
  end

  def self.parse(source, filename, line = 1)
    t = Tokenizer.new(source, filename, line)
    x = CompilerContext.new(false)
    n = script(t, x)
    raise SyntaxError.new("Syntax error", t) if !t.done
    return n
  end

end

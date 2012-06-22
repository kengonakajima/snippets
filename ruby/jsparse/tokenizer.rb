module Narcissus
  class Tokenizer

    attr_accessor :cursor, :source, :tokens, :token_index, :lookahead
    attr_accessor :scan_newlines, :scan_operand, :filename, :lineno

    def initialize(source, filename, line)
      @cursor = 0
      @source = source.to_s
      @tokens = []
      @token_index = 0
      @lookahead = 0
      @scan_newlines = false
      @scan_operand = true
      @filename = filename or ""
      @lineno = line or 1
    end

    def input
      return @source.slice(@cursor, @source.length - @cursor)
    end

    def done
      return self.peek == CONSTS["END"];
    end

    def token
      return @tokens[@token_index];
    end
    
    def match(tt)
      got = self.get
      #puts got
      #puts tt
      return got == tt || self.unget
    end
    
    def must_match(tt)
      raise SyntaxError.new("Missing " + TOKENS[tt].downcase, self) unless self.match(tt)
      return self.token
    end

    def peek
      if @lookahead > 0
        #tt = @tokens[(@token_index + @lookahead)].type
        tt = @tokens[(@token_index + @lookahead) & 3].type
      else
        tt = self.get
        self.unget
      end
      return tt
    end
    
    def peek_on_same_line
      @scan_newlines = true;
      tt = self.peek
      @scan_newlines = false;
      return tt
    end

    def get
      while @lookahead > 0
        @lookahead -= 1
        @token_index = (@token_index + 1) & 3
        token = @tokens[@token_index]
        return token.type if token.type != CONSTS["NEWLINE"] || @scan_newlines
      end
      
      while true
        input = self.input

        if @scan_newlines
          match = /\A[ \t]+/.match(input)
        else
          match = /\A\s+/.match(input)
        end
        
        if match
          spaces = match[0]
          @cursor += spaces.length
          @lineno += spaces.count("\n")
          input = self.input
        end
        
        match = /\A\/(?:\*(?:.)*?\*\/|\/[^\n]*)/m.match(input)
        break unless match
        comment = match[0]
        @cursor += comment.length
        @lineno += comment.count("\n")
      end
      
      #puts input
      
      @token_index = (@token_index + 1) & 3
      token = @tokens[@token_index]
      (@tokens[@token_index] = token = Token.new) unless token
      if input.length == 0
        #puts "end!!!"
        return (token.type = CONSTS["END"])
      end

      cursor_advance = 0
      if (match = FP_REGEXP.match(input))
        token.type = CONSTS["NUMBER"]
        token.value = match[0].to_f
      elsif (match = /\A0[xX][\da-fA-F]+|\A0[0-7]*|\A\d+/.match(input))
        token.type = CONSTS["NUMBER"]
        token.value = match[0].to_i
      elsif (match = /\A(\w|\$)+/.match(input))
        id = match[0]
        token.type = KEYWORDS[id] || CONSTS["IDENTIFIER"]
        token.value = id
      elsif (match = /\A"(?:\\.|[^"])*"|\A'(?:[^']|\\.)*'/.match(input))
        token.type = CONSTS["STRING"]
        token.value = match[0].to_s
      elsif @scan_operand and (match = /\A\/((?:\\.|[^\/])+)\/([gi]*)/.match(input))
        token.type = CONSTS["REGEXP"]
        token.value = Regexp.new(match[1], match[2])
      elsif (match = OP_REGEXP.match(input))
        op = match[0]
        if ASSIGN_OPS_HASH[op] && input[op.length, 1] == '='
          token.type = CONSTS["ASSIGN"]
          token.assign_op = CONSTS[OPERATOR_TYPE_NAMES[op]]
          cursor_advance = 1 # length of '='
        else
          #puts CONSTS[OPERATOR_TYPE_NAMES[op]].to_s + " " + OPERATOR_TYPE_NAMES[op] + " " + op
          token.type = CONSTS[OPERATOR_TYPE_NAMES[op]]
          if @scan_operand and (token.type == CONSTS["PLUS"] || token.type == CONSTS["MINUS"])
            token.type += CONSTS["UNARY_PLUS"] - CONSTS["PLUS"]
          end
          token.assign_op = nil
        end
        token.value = op
      else
        raise SyntaxError.new("Illegal token", self)
      end

      token.start = @cursor
      @cursor += match[0].length + cursor_advance
      token.end = @cursor
      token.lineno = @lineno
      
      return token.type
    end

    def unget
      #puts "start: lookahead: " + @lookahead.to_s + " token_index: " + @token_index.to_s
      @lookahead += 1
      raise SyntaxError.new("PANIC: too much lookahead!", self) if @lookahead == 4
      @token_index = (@token_index - 1) & 3
      #puts "end:   lookahead: " + @lookahead.to_s + " token_index: " + @token_index.to_s
      return nil
    end

  end
end

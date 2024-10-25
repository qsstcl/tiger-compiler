%filenames = "scanner"

  %x COMMENT STRING
  %%

  // 
  [ \t]+ {adjust();}
  \n {adjust(); errormsg_->Newline();}

  // reserved signs
  "," {adjust(); return Parser::COMMA;}
  ":" {adjust(); return Parser::COLON;}
  ";" {adjust(); return Parser::SEMICOLON;}
  "(" {adjust(); return Parser::LPAREN;}
  ")" {adjust(); return Parser::RPAREN;}
  "{" {adjust(); return Parser::LBRACE;}
  "}" {adjust(); return Parser::RBRACE;}
  "[" {adjust(); return Parser::LBRACK;}
  "]" {adjust(); return Parser::RBRACK;}
  "." {adjust(); return Parser::DOT;}
  "+" {adjust(); return Parser::PLUS;}
  "-" {adjust(); return Parser::MINUS;}
  "*" {adjust(); return Parser::TIMES;}
  "/" {adjust(); return Parser::DIVIDE;}
  "=" {adjust(); return Parser::EQ;}
  "<>" {adjust(); return Parser::NEQ;}
  "<" {adjust(); return Parser::LT;}
  "<=" {adjust(); return Parser::LE;}
  ">" {adjust(); return Parser::GT;}
  ">=" {adjust(); return Parser::GE;}
  "&" {adjust(); return Parser::AND;}
  "|" {adjust(); return Parser::OR;}
  ":=" {adjust(); return Parser::ASSIGN;}

  // reserved words
  "array" {adjust(); return Parser::ARRAY;}
  "if" {adjust(); return Parser::IF;}
  "then" {adjust(); return Parser::THEN;}
  "else" {adjust(); return Parser::ELSE;}
  "while" {adjust(); return Parser::WHILE;}
  "for" {adjust(); return Parser::FOR;}
  "to" {adjust(); return Parser::TO;}
  "do" {adjust(); return Parser::DO;}
  "let" {adjust(); return Parser::LET;}
  "in" {adjust(); return Parser::IN;}
  "end" {adjust(); return Parser::END;}
  "of" {adjust(); return Parser::OF;}
  "break" {adjust(); return Parser::BREAK;}
  "nil" {adjust(); return Parser::NIL;}
  "function" {adjust(); return Parser::FUNCTION;}
  "var" {adjust(); return Parser::VAR;}
  "type" {adjust(); return Parser::TYPE;}

  // Identifiers and Integers
  [a-zA-Z_][a-zA-Z0-9_]* {adjust();string_buf_ = matched(); return Parser::ID;} // if inside the string, update string_buf_
  [0-9]+ {adjust(); string_buf_ = matched();return Parser::INT;} 

  // handle comments
  "/*" {adjust(); comment_level_ = 0; begin(StartCondition_::COMMENT); }
  <COMMENT>{
    "*/"  {adjustStr(); if (comment_level_ > 0) comment_level_--; else begin(StartCondition_::INITIAL); }
    \n|.  {adjustStr(); } // ignore \n or any character inside the comment 
    "/*"  {adjustStr(); comment_level_ += 1; } // more layer 
  }

  // handle strings
  \" {adjust(); begin(StartCondition_::STRING); string_buf_.clear(); }
  <STRING>{
      \"    {adjustStr(); begin(StartCondition_::INITIAL); setMatched(string_buf_); return Parser::STRING; }
      \\n   {adjustStr(); string_buf_ += '\n'; }  //add \n
      \\t   {adjustStr(); string_buf_ += '\t'; }  //add \t
      \\\"  {adjustStr(); string_buf_ += '\"'; }  //add \"
      \\\\  {adjustStr(); string_buf_ += '\\'; }  //add \\
      \\[0-9]{3}      {adjustStr(); string_buf_ += (char)atoi(matched().c_str() + 1); } //add \ddd, convert it to a char(reprensented by ASCII)
      \\[ \f\n\t]+\\  {adjustStr(); } // ignore \f___f\  (dont add to buffer)
      \\\^[A-Z]       {adjustStr(); string_buf_ += matched()[2] - 'A' + 1; } // add control strings, ^C , ^Z .... etc
      .     {adjustStr(); string_buf_ += matched(); }
  }
  <<EOF>> {adjust(); return 0;}
  // handle error
  . {adjust(); errormsg_->tok_pos_;}

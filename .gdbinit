b tigermain

define run_with_input
  shell echo -e "1 a\n23 b" > input.txt
  run < input.txt
end

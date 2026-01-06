CPPFLAGS = -iquote $(headers_dir)
LDFLAGS := -lCppUTest

tests_dir := ./tests
src_dir := ./src
headers_dir = ./include

srcs := expr_translator.cpp lexer.cpp parser.cpp ast.cpp

srcs_abs_path := $(addprefix $(src_dir)/,$(srcs))

test_srcs := $(addprefix test_,$(srcs) all.cc)
test_srcs_abs_path := $(addprefix $(tests_dir)/,$(test_srcs))

headers := expr_translator.h lexer.h parser.h ast.h symbol_table.h

headers_abs_path := $(addprefix $(headers_dir)/,$(headers))

.PHONY : check
check :
	@c++ $(CPPFLAGS) $(test_srcs_abs_path) $(srcs_abs_path) \
				$(LDFLAGS) -o $(tests_dir)/test_all
	@$(tests_dir)/test_all


.PHONY : html
html :
	doxygen Doxyfile


.PHONY : clean
clean :
	rm -rf docs
	rm -f tests/test_all

CPPFLAGS = -iquote $(headers_dir)
CXXFLAGS := -Wall -Wextra
LDFLAGS := -lCppUTest

tests_dir := ./tests
src_dir := ./src
headers_dir = ./include

srcs := expr_translator.cpp lexer.cpp parser.cpp ast.cpp \
		parser_tester.cc symbol_table.cc

srcs_abs_path := $(addprefix $(src_dir)/,$(srcs))

test_srcs := $(addprefix test_,all.cc expr_translator.cpp lexer.cpp \
		parser.cc symbol_table.cc)
test_srcs_abs_path := $(addprefix $(tests_dir)/,$(test_srcs))

.PHONY : check
check :
	@c++ $(CPPFLAGS) $(CXXFLAGS) $(srcs_abs_path) $(test_srcs_abs_path) \
				$(LDFLAGS) -o $(tests_dir)/test_all
	@$(tests_dir)/test_all


.PHONY : html
html :
	doxygen Doxyfile


.PHONY : clean
clean :
	rm -rf docs
	rm -f tests/test_all

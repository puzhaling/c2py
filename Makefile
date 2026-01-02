CPPFLAGS = -iquote $(headers_dir)
LDFLAGS := -lCppUTest

tests_dir := ./tests
src_dir := ./src
headers_dir = ./include

srcs := file_walker.cc control_flow_translator.cpp \
		expr_translator.cpp lexer.cpp
srcs_abs_path := $(addprefix $(src_dir)/,$(srcs))

test_srcs := $(addprefix test_,$(srcs) all.cc)
test_srcs_abs_path := $(addprefix $(tests_dir)/,$(test_srcs))

headers := file_walker.hh expr_translator.h control_flow_translator.h \
			lexer.h
headers_abs_path := $(addprefix $(headers_dir)/,$(headers))

.PHONY : check
check :
	@c++ $(CPPFLAGS) $(test_srcs_abs_path) $(srcs_abs_path) \
				$(LDFLAGS) -o $(tests_dir)/test_all
	@$(tests_dir)/test_all


CXXFLAGS := -g
SHIELD_OBJS := shield_common.o shield_yacc.o shield_lex.o shield.o	\
shield_unsupported_exception.o shield_syntax_exception.o		\
shield_printable.o shield_select.o shield_identity.o shield_text.o	\
shield_create_table.o shield_insert.o shield_auto_increment.o		\
shield_create_index.o shield_drop_table.o shield_date_function.o	\
shield_interval.o shield_natural.o shield_print_package.o		\
shield_chain.o introspection_table.o				\
introspection_column.o introspection_column_type.o	\
shield_not_found_exception.o
SHIELD_LEX_TEST_OBJS := shield_lex_test.o 
INTROSPECTION_TEST_OBJS := introspection_test.o	\
introspection_table.o introspection.o
LDFLAGS := -ll

PROGRAMS := shield shield_lex_test shield_make_package_string

all: shield

shield: $(SHIELD_OBJS)
	g++ $(SHIELD_OBJS) -o $@ $(LDFLAGS)

shield_make_package_string: shield_make_package_string.cc
	g++ shield_make_package_string.cc -o $@

shield_lex_test: $(SHIELD_LEX_TEST_OBJS)
	g++ $(SHIELD_LEX_TEST_OBJS) -o $@ $(LDFLAGS)

introspection_test: $(INTROSPECTION_TEST_OBJS)
	g++ $(INTROSPECTION_TEST_OBJS) -o $@ $(LDFLAGS)

shield_lex.cc: shield_lex.y
	flex -i -oshield_lex.cc shield_lex.y 

shield_yacc.cc: shield_yacc.y
	bison -d shield_yacc.y -o shield_yacc.cc --report=state

shield_yacc.hh: shield_yacc.cc

shield_print_package.cc: shield_make_package_string shield_package.sql
	echo "#include \"shield.hh\"" >shield_print_package.cc
	echo "void shield::print_package ()" >>shield_print_package.cc
	echo "{" >>shield_print_package.cc
	echo -n "	cout << " >>shield_print_package.cc
	./shield_make_package_string < shield_package.sql >>shield_print_package.cc
	echo ";" >>shield_print_package.cc
	echo "}" >>shield_print_package.cc

shield.o: shield_yacc.hh

#
# Update dependencies
#
depend:
	makedepend -fMakefile.depend -Y $(subst .o,.cc,$(SHIELD_OBJS))
.PHONY: depend

#
# Include automatically generated dependencies (see depend target)
#

clean:
	rm -f *.o 
	rm -f shield_yacc.cc shield_yacc.hh 
	rm -f shield_lex.cc shield_lex.hh
	rm -f $(PROGRAMS)
	rm -f shield_print_package.cc

include Makefile.depend

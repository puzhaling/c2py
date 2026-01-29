c2py

Project Description:
	This project implements a translator that converts programs written in a subset of the C programming language into semantically equivalent Python code.
	The translator performs lexical, syntactic, and semantic analysis of the input C code, followed by the generation of valid Python source code that replicates the original program's logic and behavior.

Development Team:
	* Gorlatenko Maxim (@mqwl)
	* Kabanov Danila (@Danila121)  a.k.a Danili Kichibs
	* Puzhalin German (@puzhaling)
	* Porohov Danila (@blackmagicqqqqdwqd)
	
Testing:
	The project's components (lexer, parser, semantic analyzer) are thoroughly unit-tested using the CppUTest framework, ensuring reliability and correctness of the translation process.

Project Structure:
	src/ - Source code of the translator (C++)
	include/ - Header files
	tests/ - CppUTest test suites

Requirements:
	CppUTest - testing
	FLTK - user interface

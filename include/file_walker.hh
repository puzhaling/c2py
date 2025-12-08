#ifndef FILE_WALKER_HH
# define FILE_WALKER_HH

#include	<fstream>

class FileWalker {
public:
	FileWalker (char const *filename);

	bool is_file_opened () const;
	int get_next_char ();

private:
	std::fstream _fs;
};

#endif  /* !FILE_WALKER_HH */
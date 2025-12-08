#include	"file_walker.hh"

FileWalker::FileWalker (char const *filename)
	: _fs (filename)
{
}

bool
FileWalker::is_file_opened () const
{
	return _fs.is_open ();
}

int
FileWalker::get_next_char ()
{
	int retch = _fs.get ();
	return _fs.eof () ? -1 : retch;
}

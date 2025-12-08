#include	<memory>   			// for std::unique_ptr
#include	<exception>			// for std::exception
#include	<fstream>
#include	<cstdio>   			// for std::remove.
#include	<cstring>  			// for std::strlen
#include	<CppUTest/TestHarness.h>
#include	"file_walker.hh"

#define TEST_FILENAME_STR "testfile.txt"

static std::fstream
create_testfile_with_content (char const * filename,
				char const * content = nullptr)
{
	std::fstream created_file (filename,
			std::ios::in | std::ios::out | std::ios::trunc);

	if (!created_file.is_open ())
		FAIL ("Failed to create a testfile.");

	if (content)
		created_file << content;

	return created_file;
}

static auto
get_file_walker_on_file_with_next_name_and_content (char const * filename,
				char const * content = nullptr)
{
	(void )create_testfile_with_content (filename, content);

	std::unique_ptr<FileWalker> fw_sptr = nullptr;
	try {
		fw_sptr = std::make_unique<FileWalker> (FileWalker (filename));
	} catch (std::exception &) {
		FAIL ("Failed to create a FileWalker.");
	}

	return fw_sptr;
}

TEST_GROUP (file_walker_test_group)
{
};

TEST (file_walker_test_group, test_open_non_exist_file)
{
	FileWalker fw ("non_exist_file.txt");
	CHECK_EQUAL (false, fw.is_file_opened ());
}

TEST (file_walker_test_group, test_open_exist_file)
{
	auto fw_ptr =
		get_file_walker_on_file_with_next_name_and_content (TEST_FILENAME_STR);

	CHECK_EQUAL (true, fw_ptr->is_file_opened ());

	std::remove (TEST_FILENAME_STR);
}

TEST (file_walker_test_group, test_get_next_char)
{
	auto fw_ptr =
		get_file_walker_on_file_with_next_name_and_content (TEST_FILENAME_STR, "12345\n");

	int first_retch = fw_ptr->get_next_char ();

	CHECK (first_retch != -1);
	CHECK_EQUAL ('1', first_retch);
	CHECK_EQUAL ('2', fw_ptr->get_next_char ());
	CHECK_EQUAL ('3', fw_ptr->get_next_char ());
	CHECK_EQUAL ('4', fw_ptr->get_next_char ());
	CHECK_EQUAL ('5', fw_ptr->get_next_char ());
	CHECK_EQUAL ('\n', fw_ptr->get_next_char ());
	CHECK (-1 == fw_ptr->get_next_char ());
	CHECK (-1 == fw_ptr->get_next_char ());
	CHECK (-1 == fw_ptr->get_next_char ());

	std::remove (TEST_FILENAME_STR);
}

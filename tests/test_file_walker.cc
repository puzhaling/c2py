#include	<memory>   			// for std::unique_ptr
#include	<exception>			// for std::exception
#include	<fstream>
#include	<cstdio>   			// for std::remove.
#include	<cstring>  			// for std::strlen
#include	<CppUTest/TestHarness.h>
#include	"file_walker.hh"

TEST_GROUP (file_walker_test_group)
{
	char const * testfile_name = "testfile.txt";
	char const * testfile_content = "12345\n";
	std::unique_ptr<FileWalker> common_fw_ptr;

	void setup ()
	{
		std::ofstream created_file (testfile_name);
		if (!created_file.is_open ())
			FAIL ("Failed to create a test file.");

		created_file << testfile_content;

		try {
			common_fw_ptr =
				std::make_unique<FileWalker>(testfile_name);
		} catch (std::exception &) {
			FAIL ("common_fw has not been allocated before the test.");
		}
	}

	void teardown ()
	{
		common_fw_ptr.reset ();
		std::remove (testfile_name);
	}
};

TEST (file_walker_test_group, test_open_non_exist_file)
{
	FileWalker fw ("non_exist_file.txt");
	CHECK_EQUAL (false, fw.is_open ());
}

TEST (file_walker_test_group, test_open_exist_file)
{
	CHECK_EQUAL (true, common_fw_ptr->is_open ());
}

TEST (file_walker_test_group, test_get_next_char)
{
	int first_retch = common_fw_ptr->get_next_char ();

	CHECK (first_retch != -1);
	CHECK_EQUAL ('1', first_retch);
	CHECK_EQUAL ('2', common_fw_ptr->get_next_char ());
	CHECK_EQUAL ('3', common_fw_ptr->get_next_char ());
	CHECK_EQUAL ('4', common_fw_ptr->get_next_char ());
	CHECK_EQUAL ('5', common_fw_ptr->get_next_char ());
	CHECK_EQUAL ('\n', common_fw_ptr->get_next_char ());
	CHECK (-1 == common_fw_ptr->get_next_char ());
	CHECK (-1 == common_fw_ptr->get_next_char ());
	CHECK (-1 == common_fw_ptr->get_next_char ());
}

#include <phi/test/test_macros.hpp>

#include <DLX/VirtualFileSystem.hpp>
#include <phi/core/observer_ptr.hpp>
#include <cstdio>
#include <filesystem>

static constexpr const char temp_file_name[]{"dlxlib_native_test_file_ignore_me"};

PHI_GCC_SUPPRESS_WARNING("-Wabi-tag")

class temp_file
{
public:
    temp_file()
    {
        // Construct temporary file
        std::FILE* fh = std::fopen(get_file_path().c_str(), "w");
        REQUIRE(fh);
        std::fclose(fh);
    }

    ~temp_file()
    {
        // Cleanup the temporary file
        std::filesystem::remove(get_file_path());
    }

    [[nodiscard]] static std::string get_file_path() noexcept
    {
        return std::filesystem::temp_directory_path() / temp_file_name;
    }
};

TEST_CASE("VirtualFileSystem")
{
    SECTION("NativeFileHandle")
    {
        dlx::NativeFileHandle native{""};

        CHECK_FALSE(native.is_virtual());
    }

    SECTION("VirtualFileHandle")
    {
        dlx::VirtualFileHandle vhandle{""};

        CHECK(vhandle.is_virtual());
    }

    SECTION("VirtualFileSystem()")
    {
        dlx::VirtualFileSystem vfs;

        CHECK(vfs.GetNumberOfFileHandles() == 0u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 0u);
        CHECK(vfs.IsEmpty());

        CHECK(vfs.GetFileHandleLimit() == dlx::DefaultFileHandleLimit);
    }

    SECTION("FileExists")
    {
        dlx::VirtualFileSystem vfs;

        // Path doesn't exist
        CHECK_FALSE(vfs.FileExists(""));
        CHECK_FALSE(vfs.FileExists("/bin/DLXEmu"));

        // Test virtual file
        REQUIRE(vfs.CreateVirtualFile("virtual.test"));

        CHECK(vfs.FileExists("virtual.test"));

        // Test with native
        temp_file temp;
        REQUIRE(vfs.LinkFile(temp_file::get_file_path(), "native.test"));

        CHECK(vfs.FileExists("native.test"));
        CHECK_FALSE(vfs.FileExists(temp_file::get_file_path()));
    }

    SECTION("FileGet")
    {
        dlx::VirtualFileSystem vfs;

        // Get non existing file
        CHECK_FALSE(vfs.FileGet(""));
        CHECK_FALSE(vfs.FileGet("/bin/DLXEmu"));

        // Create virtual file
        REQUIRE(vfs.CreateVirtualFile("virtual.test"));

        // Get virtual file
        phi::observer_ptr<dlx::BasicFileHandle> handle = vfs.FileGet("virtual.test");
        REQUIRE(handle);

        CHECK(handle->is_virtual());

        // Test with native file
        temp_file temp;

        REQUIRE(vfs.LinkFile(temp_file::get_file_path(), "native.test"));

        handle = vfs.FileGet("native.test");
        REQUIRE(handle);

        CHECK_FALSE(handle->is_virtual());
    }

    SECTION("CreateVirtualFile")
    {
        dlx::VirtualFileSystem vfs;

        // Normal
        REQUIRE(vfs.CreateVirtualFile("test"));
        CHECK(vfs.FileExists("test"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 1u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 1u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);

        // With content
        REQUIRE(vfs.CreateVirtualFile("test2", "This file has some content"));
        CHECK(vfs.FileExists("test2"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 2u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 2u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);

        // Create file which already exists
        REQUIRE_FALSE(vfs.CreateVirtualFile("test"));
        REQUIRE_FALSE(vfs.CreateVirtualFile("test2"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 2u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 2u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);

        // Create a virtual file which already exists as a native file
        temp_file temp;
        REQUIRE(vfs.LinkFile(temp_file::get_file_path(), "native"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 3u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 2u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 1u);

        REQUIRE_FALSE(vfs.CreateVirtualFile("native"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 3u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 2u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 1u);

        // Check that FileHandleLimit is honored
        vfs.SetFileHandleLimit(3u);

        REQUIRE_FALSE(vfs.CreateVirtualFile("blocked_by_file_limit"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 3u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 2u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 1u);
    }

    SECTION("LinkFile")
    {
        dlx::VirtualFileSystem vfs;

        // Linking a file which doen't exist
        REQUIRE_FALSE(vfs.LinkFile("well_this_file_should_hopefully_not_exist_on_your_system_or_at_"
                                   "least_in_this_directory_but_yeah_who_knows",
                                   "file"));
        CHECK(vfs.IsEmpty());

        // Linking a file which exists
        temp_file temp;
        REQUIRE(vfs.LinkFile(temp_file::get_file_path(), "file"));
        CHECK(vfs.FileExists("file"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 1u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 0u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 1u);

        // Linking a file which already exists
        REQUIRE_FALSE(vfs.LinkFile(temp_file::get_file_path(), "file"));
        CHECK(vfs.FileExists("file"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 1u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 0u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 1u);

        // Linking the same file twice but under different names
        REQUIRE(vfs.LinkFile(temp_file::get_file_path(), "other_file"));
        CHECK(vfs.FileExists("file"));
        CHECK(vfs.FileExists("other_file"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 2u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 0u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 2u);

        // Check that FileHandleLimit is honored
        vfs.SetFileHandleLimit(2u);

        REQUIRE_FALSE(vfs.LinkFile(temp_file::get_file_path(), "blocked_by_file_handle_limit"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 2u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 0u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 2u);
    }

    SECTION("RemoveFile")
    {
        dlx::VirtualFileSystem vfs;

        // Deleting a file which doesn't exist
        REQUIRE_FALSE(vfs.RemoveFile("non-existing"));

        // Delete an existing virtual file
        REQUIRE(vfs.CreateVirtualFile("file"));
        CHECK(vfs.FileExists("file"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.CountNumberOfVirtualFiles() == 1u);
        CHECK(vfs.GetNumberOfFileHandles() == 1u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);

        REQUIRE(vfs.RemoveFile("file"));
        CHECK_FALSE(vfs.FileExists("file"));
        CHECK(vfs.IsEmpty());
        CHECK(vfs.CountNumberOfVirtualFiles() == 0u);
        CHECK(vfs.GetNumberOfFileHandles() == 0u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);

        // Delete an exiting native file
        temp_file temp;
        REQUIRE(vfs.LinkFile(temp_file::get_file_path(), "file"));

        REQUIRE(vfs.RemoveFile("file"));
        CHECK_FALSE(vfs.FileExists("file"));
        CHECK(vfs.IsEmpty());
        CHECK(vfs.CountNumberOfVirtualFiles() == 0u);
        CHECK(vfs.GetNumberOfFileHandles() == 0u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);
    }

    SECTION("Clear")
    {
        dlx::VirtualFileSystem vfs;

        REQUIRE(vfs.CreateVirtualFile("f1"));
        REQUIRE(vfs.CreateVirtualFile("f2"));
        REQUIRE(vfs.CreateVirtualFile("f3"));

        temp_file temp;
        REQUIRE(vfs.LinkFile(temp_file::get_file_path(), "f4"));
        REQUIRE(vfs.LinkFile(temp_file::get_file_path(), "f5"));

        CHECK(vfs.GetNumberOfFileHandles() == 5u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 2u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 3u);

        vfs.Clear();
        CHECK_FALSE(vfs.FileExists("f1"));
        CHECK_FALSE(vfs.FileExists("f2"));
        CHECK_FALSE(vfs.FileExists("f3"));
        CHECK_FALSE(vfs.FileExists("f4"));
        CHECK_FALSE(vfs.FileExists("f5"));

        CHECK(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 0u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 0u);
    }

    SECTION("SetFileHandleLimit")
    {
        dlx::VirtualFileSystem vfs;

        CHECK(vfs.GetFileHandleLimit() == dlx::DefaultFileHandleLimit);

        // Add some files
        REQUIRE(vfs.CreateVirtualFile("f1"));
        REQUIRE(vfs.CreateVirtualFile("f2"));
        REQUIRE(vfs.CreateVirtualFile("f3"));
        REQUIRE(vfs.CreateVirtualFile("f4"));
        REQUIRE(vfs.CreateVirtualFile("f5"));

        CHECK(vfs.GetNumberOfFileHandles() == 5u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 5u);

        // Higher than before
        vfs.SetFileHandleLimit(dlx::DefaultFileHandleLimit + 10u);
        CHECK(vfs.GetFileHandleLimit() == dlx::DefaultFileHandleLimit + 10u);
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 5u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 5u);

        // Lower but still in no deletions
        vfs.SetFileHandleLimit(6u);
        CHECK(vfs.GetFileHandleLimit() == 6u);
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 5u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 5u);

        // Reach the limit
        REQUIRE(vfs.CreateVirtualFile("f6"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 6u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 6u);

        // Not allowed to link or create new files anymore
        REQUIRE_FALSE(vfs.CreateVirtualFile("limited"));
        temp_file temp;
        REQUIRE_FALSE(vfs.LinkFile(temp_file::get_file_path(), "limited"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 6u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 6u);

        // Removing an element allows us to add another again
        REQUIRE(vfs.RemoveFile("f1"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 5u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 5u);

        REQUIRE(vfs.CreateVirtualFile("file"));
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 6u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 6u);

        // Limit set to lower than count need to delete elements now
        vfs.SetFileHandleLimit(3u);
        CHECK(vfs.GetFileHandleLimit() == 3u);
        CHECK_FALSE(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 3u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 3u);

        // Deleting all elements
        vfs.SetFileHandleLimit(0u);
        CHECK(vfs.GetFileHandleLimit() == 0u);
        CHECK(vfs.IsEmpty());
        CHECK(vfs.GetNumberOfFileHandles() == 0u);
        CHECK(vfs.CountNumberOfLinkedFiles() == 0u);
        CHECK(vfs.CountNumberOfVirtualFiles() == 0u);
    }
}

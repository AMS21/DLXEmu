#define _CRT_SECURE_NO_WARNINGS

#include "DLX/VirtualFileSystem.hpp"

#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/scope_guard.hpp>
#include <phi/core/scope_ptr.hpp>
#include <phi/core/types.hpp>
#include <phi/type_traits/to_underlying.hpp>
#include <cstdio>
#include <filesystem>

PHI_CLANG_SUPPRESS_WARNING("-Wunsafe-buffer-usage")

namespace dlx
{
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wswitch")
    PHI_CLANG_SUPPRESS_WARNING("-Wcovered-switch-default")
    PHI_GCC_SUPPRESS_WARNING("-Wreturn-type")
    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4702)

    PHI_ATTRIBUTE_CONST phi::string_view to_string_flags(const OpenModeFlags flags) noexcept
    {
        PHI_ASSERT(flags != OpenModeFlags::Invalid);
        PHI_ASSERT(phi::to_underlying(flags) > 0u &&
                   phi::to_underlying(flags) <= phi::to_underlying(OpenModeFlags::MaxFlagValue));

        switch (flags)
        {
            // 1 flags
            case OpenModeFlags::Read:
                return "r";
            case OpenModeFlags::Write:
                return "w";
            case OpenModeFlags::Append:
                return "a";

            // 2 flags
            case OpenModeFlags::Read | OpenModeFlags::Write:
                return "rw";
            case OpenModeFlags::Read | OpenModeFlags::Append:
                return "ra";

            case OpenModeFlags::Write | OpenModeFlags::Append:
                return "wa";

            // 3 flags
            case OpenModeFlags::Read | OpenModeFlags::Write | OpenModeFlags::Append:
                return "rwa";

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }

    PHI_MSVC_SUPPRESS_WARNING_POP()
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_POP()

    PHI_ATTRIBUTE_PURE OpenModeFlags parse_open_mode_flags(const char* string) noexcept
    {
        PHI_ASSERT(string != nullptr, "Cannot parse nullptr");

        OpenModeFlags flags = OpenModeFlags::Invalid;

        for (; *string != '\0'; ++string)
        {
            const char character = *string;

            switch (character)
            {
                case 'r':
                    flags |= OpenModeFlags::Read;
                    break;
                case 'w':
                    flags |= OpenModeFlags::Write;
                    break;
                case 'a':
                    flags |= OpenModeFlags::Append;
                    break;

                default:
                    return OpenModeFlags::Invalid;
            }
        }

        return flags;
    }

    phi::boolean BasicFileHandle::open(OpenModeFlags flags) noexcept
    {
        return open(to_string_flags(flags).data());
    }

    NativeFileHandle::NativeFileHandle(phi::string_view real_path) noexcept
        : m_RealPath{real_path}
    {}

    NativeFileHandle::~NativeFileHandle() noexcept
    {
        if (m_FileHandle)
        {
            (void)std::fclose(m_FileHandle.get());
        }
    }

    PHI_ATTRIBUTE_CONST phi::boolean NativeFileHandle::is_virtual() const noexcept
    {
        return false;
    }

    PHI_ATTRIBUTE_PURE phi::boolean NativeFileHandle::is_open() const noexcept
    {
        return m_FileHandle != nullptr;
    }

    phi::boolean NativeFileHandle::open(const char* flags) noexcept
    {
        m_FileHandle = std::fopen(m_RealPath.data(), flags);

        return m_FileHandle != nullptr;
    }

    phi::boolean NativeFileHandle::close() noexcept
    {
        if (!m_FileHandle)
        {
            return false;
        }

        int ret      = std::fclose(m_FileHandle.get());
        m_FileHandle = nullptr;

        // 0 means success
        return ret == 0;
    }

    phi::isize NativeFileHandle::read(phi::flat_ptr buffer, phi::usize number_of_bytes) noexcept
    {
        phi::size_t ret =
                std::fread(buffer.get(), 1u, number_of_bytes.unsafe(), m_FileHandle.get());

        if (ret == 0)
        {
            return -1;
        }

        return static_cast<phi::isize::value_type>(ret);
    }

    VirtualFileHandle::VirtualFileHandle(std::string content) noexcept
        : m_Content{phi::move(content)}
        , m_OpenFlags{OpenModeFlags::Invalid}
    {}

    PHI_ATTRIBUTE_CONST phi::boolean VirtualFileHandle::is_virtual() const noexcept
    {
        return true;
    }

    PHI_ATTRIBUTE_PURE phi::boolean VirtualFileHandle::is_open() const noexcept
    {
        return m_OpenFlags != OpenModeFlags::Invalid;
    }

    phi::boolean VirtualFileHandle::open(const char* flags) noexcept
    {
        // Can't open a file which is already opened
        if (is_open())
        {
            return false;
        }

        const OpenModeFlags open_flags = parse_open_mode_flags(flags);

        // Invalid flags string
        if (open_flags == OpenModeFlags::Invalid)
        {
            return false;
        }

        return open(open_flags);
    }

    phi::boolean VirtualFileHandle::open(OpenModeFlags flags) noexcept
    {
        PHI_ASSERT(flags != OpenModeFlags::Invalid);
        PHI_ASSERT(phi::to_underlying(flags) <= phi::to_underlying(OpenModeFlags::MaxFlagValue));

        // Can't open a file which is already opened
        if (is_open())
        {
            return false;
        }

        // Invalid OpenMode flags
        if (flags == OpenModeFlags::Invalid)
        {
            return false;
        }

        // Declare the "file" as opened
        m_OpenFlags = flags;

        return true;
    }

    phi::boolean VirtualFileHandle::close() noexcept
    {
        // Can't close a file which is not opened
        if (!is_open())
        {
            return false;
        }

        m_OpenFlags = OpenModeFlags::Invalid;
        return true;
    }

    phi::isize VirtualFileHandle::read(phi::flat_ptr buffer, phi::usize number_of_bytes) noexcept
    {
        if (!is_open())
        {
            return -1;
        }
        if (buffer == nullptr)
        {
            return -1;
        }
        if (number_of_bytes == 0u)
        {
            return -1;
        }

        phi::uint8_t* buffer_ptr = reinterpret_cast<phi::uint8_t*>(buffer.get());
        phi::usize    bytes_read = 0u;
        for (; bytes_read < number_of_bytes && bytes_read < m_Content.size(); ++buffer_ptr)
        {
            // Copy one byte to the target buffer
            *buffer_ptr = static_cast<phi::uint8_t>(m_Content[bytes_read.unsafe()]);
        }

        return static_cast<phi::isize::value_type>(bytes_read.unsafe());
    }

    VirtualFileSystem::VirtualFileSystem()
    {
        m_FileSystem.reserve(DefaultFileHandleLimit);
    }

    phi::boolean VirtualFileSystem::FileExists(const std::string& file_path) const noexcept
    {
        return m_FileSystem.contains(file_path);
    }

    phi::observer_ptr<BasicFileHandle> VirtualFileSystem::FileGet(
            const std::string& file_path) noexcept
    {
        if (FileExists(file_path))
        {
            auto res = m_FileSystem.find(file_path);
            PHI_ASSERT(res != m_FileSystem.end(), "Couldn't find although it should exist");

            return res->second;
        }

        return nullptr;
    }

    phi::boolean VirtualFileSystem::CreateVirtualFile(const std::string& file_path,
                                                      const std::string& file_content)
    {
        // Can't create file if it already exists
        if (FileExists(file_path))
        {
            return false;
        }

        if (GetNumberOfFileHandles() == GetFileHandleLimit())
        {
            return false;
        }

        PHI_ASSERT(GetNumberOfFileHandles() < GetFileHandleLimit());

        // Construct the new file and insert into the file system
        m_FileSystem.emplace(file_path, phi::make_not_null_scope<VirtualFileHandle>(file_content));

        return true;
    }

    phi::boolean VirtualFileSystem::LinkFile(const std::string& real_path,
                                             const std::string& virtual_path)
    {
        if (GetNumberOfFileHandles() == GetFileHandleLimit())
        {
            return false;
        }

        PHI_ASSERT(GetNumberOfFileHandles() < GetFileHandleLimit());

        // Can't link if the file already exists
        if (FileExists(virtual_path))
        {
            return false;
        }

        // Require Read/Write
        std::FILE* file_pointer = std::fopen(real_path.data(), "r+");
        if (file_pointer == nullptr)
        {
            return false;
        }

        auto file_pointer_guard = phi::make_scope_guard([&] { std::fclose(file_pointer); });

        m_FileSystem.emplace(virtual_path, phi::make_not_null_scope<NativeFileHandle>(real_path));

        return true;
    }

    phi::boolean VirtualFileSystem::RemoveFile(const std::string& file_path) noexcept
    {
        const phi::usize count = m_FileSystem.erase(file_path);
        PHI_ASSERT(count < 2u);

        return count == 1u;
    }

    PHI_ATTRIBUTE_PURE phi::boolean VirtualFileSystem::IsEmpty() const noexcept
    {
        return m_FileSystem.empty();
    }

    void VirtualFileSystem::Clear() noexcept
    {
        m_FileSystem.clear();
    }

    PHI_ATTRIBUTE_PURE phi::usize VirtualFileSystem::GetNumberOfFileHandles() const noexcept
    {
        return m_FileSystem.size();
    }

    phi::usize VirtualFileSystem::CountNumberOfLinkedFiles() const noexcept
    {
        phi::usize count{0u};
        for (const auto& iterator : m_FileSystem)
        {
            if (!iterator.second->is_virtual())
            {
                ++count;
            }
        }

        return count;
    }

    phi::usize VirtualFileSystem::CountNumberOfVirtualFiles() const noexcept
    {
        phi::usize count{0u};
        for (const auto& iterator : m_FileSystem)
        {
            if (iterator.second->is_virtual())
            {
                ++count;
            }
        }

        return count;
    }

    PHI_ATTRIBUTE_PURE phi::usize VirtualFileSystem::GetFileHandleLimit() const noexcept
    {
        return m_FileHandleLimit;
    }

    void VirtualFileSystem::SetFileHandleLimit(phi::usize new_limit) noexcept
    {
        m_FileHandleLimit = new_limit;

        if (new_limit <= m_FileSystem.size())
        {
            // Delete elements until we've reached the required size
            phi::usize count{m_FileSystem.size() - new_limit};
            for (auto it = m_FileSystem.begin(); count > 0u; --count)
            {
                PHI_ASSERT(it != m_FileSystem.end());
                it = m_FileSystem.erase(it);
            }

            PHI_ASSERT(m_FileSystem.size() == new_limit);
        }
        else
        {
            m_FileSystem.reserve(new_limit.unsafe());
        }

        PHI_ASSERT(m_FileSystem.size() <= new_limit);
    }
} // namespace dlx

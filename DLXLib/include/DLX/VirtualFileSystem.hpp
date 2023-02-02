#pragma once

#include <phi/compiler_support/warning.hpp>
#include <phi/container/dynamic_array.hpp>
#include <phi/container/string_view.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/flat_ptr.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/scope_ptr.hpp>
#include <phi/core/sized_types.hpp>
#include <phi/core/types.hpp>
#include <phi/type_traits/to_underlying.hpp>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

// TODO: Path normalization
// TODO: Path validation
// TODO: [Security] Option to disallow linking files with path traversal
// TODO: Cleanup includes

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wabi-tag")

namespace dlx
{
    enum class OpenModeFlags : phi::uint8_t
    {
        Invalid = 0,

        Read   = 1 << 0,
        Write  = 1 << 1,
        Append = 1 << 2,

        // Combinations
        ReadWrite   = Read | Write,
        ReadAppend  = Read | Append,
        WriteAppend = Write | Append,

        ReadWriteAppend = Read | Write | Append,

        MaxFlagValue = Read | Write | Append,
    };

    [[nodiscard]] constexpr OpenModeFlags operator|(const OpenModeFlags lhs,
                                                    const OpenModeFlags rhs) noexcept
    {
        return static_cast<OpenModeFlags>(phi::to_underlying(lhs) | phi::to_underlying(rhs));
    }

    constexpr OpenModeFlags& operator|=(OpenModeFlags& lhs, const OpenModeFlags rhs) noexcept
    {
        return lhs = lhs | rhs;
    }

    [[nodiscard]] constexpr OpenModeFlags operator&(const OpenModeFlags lhs,
                                                    const OpenModeFlags rhs) noexcept
    {
        return static_cast<OpenModeFlags>(phi::to_underlying(lhs) & phi::to_underlying(rhs));
    }

    constexpr OpenModeFlags& operator&=(OpenModeFlags& lhs, const OpenModeFlags rhs) noexcept
    {
        return lhs = lhs & rhs;
    }

    [[nodiscard]] phi::string_view to_string_flags(const OpenModeFlags flags) noexcept;

    [[nodiscard]] OpenModeFlags parse_open_mode_flags(const char* string) noexcept;

    class BasicFileHandle
    {
    public:
        virtual ~BasicFileHandle() noexcept = default;

        [[nodiscard]] virtual phi::boolean is_virtual() const noexcept = 0;

        [[nodiscard]] virtual phi::boolean is_open() const noexcept = 0;

        // Returns true on success
        virtual phi::boolean open(const char* flags) noexcept = 0;

        virtual phi::boolean open(OpenModeFlags flags) noexcept;

        // Returns true on success
        virtual phi::boolean close() noexcept = 0;

        // Return -1 on error
        //         0 on EOF
        //         number of bytes read on success
        virtual phi::isize read(phi::flat_ptr buffer, phi::usize number_of_bytes) noexcept = 0;
    };

    // File handle to a file from the actual operating system
    class NativeFileHandle final : public BasicFileHandle
    {
    public:
        using BasicFileHandle::open;

        NativeFileHandle(phi::string_view real_path) noexcept;

        ~NativeFileHandle() noexcept override;

        [[nodiscard]] phi::boolean is_virtual() const noexcept override;

        [[nodiscard]] phi::boolean is_open() const noexcept override;

        phi::boolean open(const char* flags) noexcept override;

        phi::boolean close() noexcept override;

        phi::isize read(phi::flat_ptr buffer, phi::usize number_of_bytes) noexcept override;

    private:
        phi::observer_ptr<std::FILE> m_FileHandle;
        phi::string_view             m_RealPath;
    };

    // File handle to a purely virtual file
    class VirtualFileHandle final : public BasicFileHandle
    {
    public:
        VirtualFileHandle(std::string content) noexcept;

        [[nodiscard]] phi::boolean is_virtual() const noexcept override;

        [[nodiscard]] phi::boolean is_open() const noexcept override;

        phi::boolean open(const char* flags) noexcept override;

        phi::boolean open(OpenModeFlags flags) noexcept override;

        phi::boolean close() noexcept override;

        phi::isize read(phi::flat_ptr buffer, phi::usize number_of_bytes) noexcept override;

    private:
        std::string   m_Content;
        OpenModeFlags m_OpenFlags; /// Invalid OpenMode indicates that the file was not opened
    };

    static constexpr const phi::size_t DefaultFileHandleLimit{50u};

    class VirtualFileSystem
    {
    public:
        VirtualFileSystem();

        VirtualFileSystem(const VirtualFileSystem&)            = delete;
        VirtualFileSystem& operator=(const VirtualFileSystem&) = delete;

        [[nodiscard]] phi::boolean FileExists(const std::string& file_path) const noexcept;

        [[nodiscard]] phi::observer_ptr<BasicFileHandle> FileGet(
                const std::string& file_path) noexcept;

        phi::boolean CreateVirtualFile(const std::string& file_path,
                                       const std::string& file_content = "");

        phi::boolean LinkFile(const std::string& real_path, const std::string& virtual_path);

        phi::boolean RemoveFile(const std::string& file_path) noexcept;

        [[nodiscard]] phi::boolean IsEmpty() const noexcept;

        void Clear() noexcept;

        [[nodiscard]] phi::usize GetNumberOfFileHandles() const noexcept;

        [[nodiscard]] phi::usize CountNumberOfLinkedFiles() const noexcept;

        [[nodiscard]] phi::usize CountNumberOfVirtualFiles() const noexcept;

        [[nodiscard]] phi::usize GetFileHandleLimit() const noexcept;

        void SetFileHandleLimit(phi::usize new_limit) noexcept;

    private:
        // Uses the file path as key to the actual file handle
        std::unordered_map<std::string, phi::not_null_scope_ptr<BasicFileHandle>> m_FileSystem;

        // Limits
        phi::usize m_FileHandleLimit{DefaultFileHandleLimit};
    };

} // namespace dlx

PHI_GCC_SUPPRESS_WARNING_POP()

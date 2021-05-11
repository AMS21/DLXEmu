#pragma once

#include "CodeEditor.hpp"
#include "MemoryViewer.hpp"
#include "RegisterViewer.hpp"
#include "Window.hpp"
#include <DLX/InstructionLibrary.hpp>
#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>
#include <DLX/Token.hpp>
#include <Phi/Core/Boolean.hpp>
#include <vector>

namespace dlxemu
{
    class Emulator
    {
        friend MemoryViewer;
        friend RegisterViewer;
        friend CodeEditor;

    public:
        Emulator() noexcept;

        phi::Boolean Initialize() noexcept;

        [[nodiscard]] phi::Boolean IsRunning() const noexcept;

        void MainLoop() noexcept;

        [[nodiscard]] dlx::Processor& GetProcessor() noexcept;

        [[nodiscard]] const dlx::InstructionLibrary& GetInstructionLibrary() const noexcept;

        [[nodiscard]] const dlx::ParsedProgram& GetProgram() const noexcept;

        void ParseProgram(std::string_view source) noexcept;

        void ParseProgram(std::vector<dlx::Token> tokens) noexcept;

    private:
        void RenderMenuBar() noexcept;

        void RenderControlPanel() noexcept;

        void RenderAbout() noexcept;

        void RenderOptionsMenu() noexcept;

    private:
        dlx::InstructionLibrary m_InstructionLibrary;
        dlx::Processor          m_Processor;
        dlx::ParsedProgram      m_DLXProgram;

        CodeEditor     m_CodeEditor;
        Window         m_Window;
        MemoryViewer   m_MemoryViewer;
        RegisterViewer m_RegisterViewer;

        // Menu
#if defined(PHI_DEBUG)
        bool m_ShowDemoWindow{false};
#endif
        bool m_ShowControlPanel{true};
        bool m_ShowMemoryViewer{true};
        bool m_ShowRegisterViewer{true};
        bool m_ShowAbout{false};
        bool m_ShowOptionsMenu{false};
    };
} // namespace dlxemu

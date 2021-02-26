#pragma once

#include "CodeEditor.hpp"
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
    public:
        Emulator() noexcept;

        phi::Boolean Initialize();

        [[nodiscard]] phi::Boolean IsRunning() const noexcept;

        void MainLoop();

        [[nodiscard]] dlx::Processor& GetProcessor() noexcept;

        [[nodiscard]] const dlx::InstructionLibrary& GetInstructionLibrary() const noexcept;

        [[nodiscard]] const dlx::ParsedProgram& GetProgram() const noexcept;

        void ParseProgram(std::string_view source) noexcept;

        void ParseProgram(std::vector<dlx::Token> tokens) noexcept;

    private:
        dlx::InstructionLibrary m_InstructionLibrary;
        dlx::Processor          m_Processor;
        dlx::ParsedProgram      m_DLXProgram;

        CodeEditor m_CodeEditor;
        Window     m_Window;
    };
} // namespace dlxemu

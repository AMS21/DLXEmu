#pragma once

#include "CodeEditor.hpp"
#include "DLX/TokenStream.hpp"
#include "DebugView.hpp"
#include "MemoryViewer.hpp"
#include "RegisterViewer.hpp"
#include "Window.hpp"
#include <DLX/InstructionLibrary.hpp>
#include <DLX/Parser.hpp>
#include <DLX/Processor.hpp>
#include <DLX/Token.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/sized_types.hpp>
#include <phi/core/types.hpp>
#include <vector>

namespace dlxemu
{
    class Emulator
    {
        friend MemoryViewer;
        friend RegisterViewer;
        friend CodeEditor;
        friend DebugView;

    public:
        enum class ShouldContinueInitilization : bool
        {
            No,
            Yes,
        };

        enum class ExecutionMode : phi::int8_t
        {
            None,
            StepThrough,
            SingleStep,
            Run,
        };

        Emulator() noexcept;

        [[nodiscard]] ShouldContinueInitilization HandleCommandLineArguments(phi::i32 argc,
                                                                             char** argv) noexcept;

        phi::boolean Initialize() noexcept;

        [[nodiscard]] phi::boolean IsRunning() const noexcept;

        void MainLoop() noexcept;

        [[nodiscard]] dlx::Processor& GetProcessor() noexcept;

        [[nodiscard]] const dlx::ParsedProgram& GetProgram() const noexcept;

        void ParseProgram(std::string_view source) noexcept;

        void ParseProgram(dlx::TokenStream& tokens) noexcept;

    private:
        void RenderMenuBar() noexcept;

        void RenderControlPanel() noexcept;

        void RenderAbout() noexcept;

        void RenderOptionsMenu() noexcept;

        void Update() noexcept;

        void SetExecutionMode(ExecutionMode mode) noexcept;

    private:
        dlx::Processor     m_Processor;
        dlx::ParsedProgram m_DLXProgram;

        CodeEditor     m_CodeEditor;
        Window         m_Window;
        MemoryViewer   m_MemoryViewer;
        RegisterViewer m_RegisterViewer;
#if defined(PHI_DEBUG)
        DebugView m_DebugView;
#endif

        ExecutionMode m_CurrentExecutionMode{ExecutionMode::None};
        double        m_LastExecTime{0.0};
        double        m_StepThroughDelayMS{0.5f};
        phi::boolean  m_DisableEditing{false};

        // Menu
#if defined(PHI_DEBUG)
        bool m_ShowDemoWindow{false};
        bool m_ShowDebugView{false};
#endif
        bool m_ShowControlPanel{true};
        bool m_ShowMemoryViewer{true};
        bool m_ShowRegisterViewer{true};
        bool m_ShowAbout{false};
        bool m_ShowOptionsMenu{false};
    };
} // namespace dlxemu

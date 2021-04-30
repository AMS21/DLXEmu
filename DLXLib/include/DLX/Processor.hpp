#pragma once

#include "DLX/FloatRegister.hpp"
#include "DLX/Instruction.hpp"
#include "DLX/IntRegister.hpp"
#include "DLX/MemoryBlock.hpp"
#include "DLX/RegisterNames.hpp"
#include "DLX/StatusRegister.hpp"
#include "Phi/Core/Boolean.hpp"
#include <Phi/Core/ObserverPtr.hpp>
#include <Phi/Core/ScopePtr.hpp>
#include <array>

namespace dlx
{
    struct ParsedProgram;

    enum class Exception
    {
        None,
        DivideByZero,
        Overflow,
        Underflow,
        Trap,
        Halt,
        UnknownLabel,
        BadShift,
        AddressOutOfBounds,
        RegisterOutOfBounds,
    };

    class Processor
    {
    public:
        Processor() noexcept;

        // Registers

        [[nodiscard]] IntRegister& GetIntRegister(IntRegisterID id) noexcept;

        [[nodiscard]] const IntRegister& GetIntRegister(IntRegisterID id) const noexcept;

        [[nodiscard]] phi::i32 IntRegisterGetSignedValue(IntRegisterID id) const noexcept;

        [[nodiscard]] phi::u32 IntRegisterGetUnsignedValue(IntRegisterID id) const noexcept;

        void IntRegisterSetSignedValue(IntRegisterID id, phi::i32 value) noexcept;

        void IntRegisterSetUnsignedValue(IntRegisterID id, phi::u32 value) noexcept;

        [[nodiscard]] FloatRegister& GetFloatRegister(FloatRegisterID id) noexcept;

        [[nodiscard]] const FloatRegister& GetFloatRegister(FloatRegisterID id) const noexcept;

        [[nodiscard]] phi::f32 FloatRegisterGetFloatValue(FloatRegisterID id) const noexcept;

        [[nodiscard]] phi::f64 FloatRegisterGetDoubleValue(FloatRegisterID id) noexcept;

        void FloatRegisterSetFloatValue(FloatRegisterID id, phi::f32 value) noexcept;

        void FloatRegisterSetDoubleValue(FloatRegisterID id, phi::f64 value) noexcept;

        [[nodiscard]] StatusRegister& GetFPSR() noexcept;

        [[nodiscard]] const StatusRegister& GetFPSR() const noexcept;

        [[nodiscard]] phi::Boolean GetFPSRValue() const noexcept;

        void SetFPSRValue(phi::Boolean value) noexcept;

        //

        void ExecuteInstruction(const Instruction& inst) noexcept;

        void LoadProgram(ParsedProgram& programm) noexcept;

        [[nodiscard]] phi::ObserverPtr<ParsedProgram> GetCurrentProgramm() const noexcept;

        void ExecuteCurrentProgram() noexcept;

        void ClearRegisters() noexcept;

        void ClearMemory() noexcept;

        void Raise(Exception exception) noexcept;

        [[nodiscard]] Exception GetLastRaisedException() const noexcept;

        [[nodiscard]] phi::Boolean IsHalted() const noexcept;

        [[nodiscard]] const MemoryBlock& GetMemory() const noexcept;

        [[nodiscard]] MemoryBlock& GetMemory() noexcept;

        [[nodiscard]] phi::u32 GetProgramCounter() const noexcept;

        void SetProgramCounter(phi::u32 new_pc) noexcept;

        [[nodiscard]] phi::u32 GetNextProgramCounter() const noexcept;

        void SetNextProgramCounter(phi::u32 new_npc) noexcept;

    private:
        phi::ObserverPtr<ParsedProgram> m_CurrentProgram;
        std::array<IntRegister, 32u>    m_IntRegisters;
        std::array<FloatRegister, 32u>  m_FloatRegisters;
        StatusRegister                  m_FPSR;

        MemoryBlock m_MemoryBlock;

        phi::u32   m_ProgramCounter{0u};
        phi::u32   m_NextProgramCounter{0u};
        phi::usize m_MaxNumberOfSteps{10'000u};

        Exception m_LastRaisedException{Exception::None};

        phi::Boolean m_Halted{false};

        RegisterAccessType m_CurrentInstructionAccessType{RegisterAccessType::Ignored};
    };
} // namespace dlx

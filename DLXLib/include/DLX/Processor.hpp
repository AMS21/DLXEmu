#pragma once

#include "DLX/FloatRegister.hpp"
#include "DLX/Instruction.hpp"
#include "DLX/IntRegister.hpp"
#include "DLX/MemoryBlock.hpp"
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
    };

    class Processor
    {
    public:
        Processor();

        IntRegister& GetIntRegister(IntRegisterID id);

        const IntRegister& GetIntRegister(IntRegisterID id) const;

        [[nodiscard]] phi::i32 IntRegisterGetSignedValue(IntRegisterID id) const;

        [[nodiscard]] phi::u32 IntRegisterGetUnsignedValue(IntRegisterID id) const;

        void IntRegisterSetSignedValue(IntRegisterID id, phi::i32 value);

        void IntRegisterSetUnsignedValue(IntRegisterID id, phi::u32 value);

        void ExecuteInstruction(const Instruction& inst);

        void LoadProgram(ParsedProgram* programm);

        [[nodiscard]] phi::ObserverPtr<ParsedProgram> GetCurrentProgramm() const noexcept;

        void ExecuteCurrentProgram();

        void ClearRegisters();

        void ClearMemory();

        void Raise(Exception exception);

        [[nodiscard]] Exception GetLastRaisedException() const noexcept;

        [[nodiscard]] phi::Boolean IsHalted() const noexcept;

        MemoryBlock m_MemoryBlock;

        phi::usize m_ProgramCounter{0u};
        phi::usize m_NextProgramCounter{0u};
        phi::usize m_MaxNumberOfSteps{10'000u};

    private:
        phi::ObserverPtr<ParsedProgram> m_CurrentProgram;
        std::array<IntRegister, 32u>    m_IntRegisters;
        // std::array<Register, 32> m_FloatRegisters;

        Exception m_LastRaisedException{Exception::None};

        phi::Boolean m_Halted{false};
    };
} // namespace dlx

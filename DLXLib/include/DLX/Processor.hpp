#pragma once

#include "DLX/EnumName.hpp"
#include "DLX/FloatRegister.hpp"
#include "DLX/Instruction.hpp"
#include "DLX/InstructionInfo.hpp"
#include "DLX/IntRegister.hpp"
#include "DLX/MemoryBlock.hpp"
#include "DLX/RegisterNames.hpp"
#include "DLX/StatusRegister.hpp"
#include <phi/core/boolean.hpp>
#include <phi/core/observer_ptr.hpp>
#include <phi/core/scope_ptr.hpp>
#include <array>

namespace dlx
{
    struct ParsedProgram;

#define DLX_ENUM_EXCEPTION                                                                         \
    DLX_ENUM_EXCEPTION_IMPL(None)                                                                  \
    DLX_ENUM_EXCEPTION_IMPL(DivideByZero)                                                          \
    DLX_ENUM_EXCEPTION_IMPL(Overflow)                                                              \
    DLX_ENUM_EXCEPTION_IMPL(Underflow)                                                             \
    DLX_ENUM_EXCEPTION_IMPL(Trap)                                                                  \
    DLX_ENUM_EXCEPTION_IMPL(Halt)                                                                  \
    DLX_ENUM_EXCEPTION_IMPL(UnknownLabel)                                                          \
    DLX_ENUM_EXCEPTION_IMPL(BadShift)                                                              \
    DLX_ENUM_EXCEPTION_IMPL(AddressOutOfBounds)                                                    \
    DLX_ENUM_EXCEPTION_IMPL(RegisterOutOfBounds)

    enum class Exception
    {
#define DLX_ENUM_EXCEPTION_IMPL(name) name,

        DLX_ENUM_EXCEPTION

#undef DLX_ENUM_EXCEPTION_IMPL

                NUMBER_OF_ELEMENTS,
    };

    template <>
    [[nodiscard]] constexpr std::string_view enum_name<Exception>(Exception value) noexcept
    {
        switch (value)
        {
#define DLX_ENUM_EXCEPTION_IMPL(name)                                                              \
    case Exception::name:                                                                          \
        return #name;

            DLX_ENUM_EXCEPTION

#undef DLX_ENUM_EXCEPTION_IMPL

            default:
                PHI_ASSERT_NOT_REACHED();
        }
    }

    enum class IntRegisterValueType
    {
        NotSet,
        Signed,
        Unsigned,
    };

    enum class FloatRegisterValueType
    {
        NotSet,
        Float,
        DoubleLow,
        DoubleHigh,
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

        [[nodiscard]] phi::boolean GetFPSRValue() const noexcept;

        void SetFPSRValue(phi::boolean value) noexcept;

        //

        void ExecuteInstruction(const Instruction& inst) noexcept;

        phi::boolean LoadProgram(ParsedProgram& programm) noexcept;

        [[nodiscard]] phi::observer_ptr<ParsedProgram> GetCurrentProgramm() const noexcept;

        void ExecuteStep() noexcept;

        void ExecuteCurrentProgram() noexcept;

        void Reset() noexcept;

        void ClearRegisters() noexcept;

        void ClearMemory() noexcept;

        void Raise(Exception exception) noexcept;

        [[nodiscard]] Exception GetLastRaisedException() const noexcept;

        [[nodiscard]] phi::boolean IsHalted() const noexcept;

        [[nodiscard]] const MemoryBlock& GetMemory() const noexcept;

        [[nodiscard]] MemoryBlock& GetMemory() noexcept;

        [[nodiscard]] phi::u32 GetProgramCounter() const noexcept;

        void SetProgramCounter(phi::u32 new_pc) noexcept;

        [[nodiscard]] phi::u32 GetNextProgramCounter() const noexcept;

        void SetNextProgramCounter(phi::u32 new_npc) noexcept;

        [[nodiscard]] phi::usize GetCurrentStepCount() const noexcept;

        void SetMaxNumberOfSteps(phi::usize new_max) noexcept;

        // Dumping

        [[nodiscard]] std::string GetRegisterDump() const noexcept;

        [[nodiscard]] std::string GetMemoryDump() const noexcept;

        [[nodiscard]] std::string GetProcessorDump() const noexcept;

        [[nodiscard]] std::string GetCurrentProgrammDump() const noexcept;

    private:
        phi::observer_ptr<ParsedProgram> m_CurrentProgram;

        std::array<IntRegister, 32u>          m_IntRegisters;
        std::array<IntRegisterValueType, 32u> m_IntRegistersValueTypes;

        std::array<FloatRegister, 32u>          m_FloatRegisters;
        std::array<FloatRegisterValueType, 32u> m_FloatRegistersValueTypes;

        StatusRegister m_FPSR;

        MemoryBlock m_MemoryBlock;

        phi::u32   m_ProgramCounter{0u};
        phi::u32   m_NextProgramCounter{0u};
        phi::usize m_CurrentStepCount{0u};
        phi::usize m_MaxNumberOfSteps{10'000u};

        Exception m_LastRaisedException{Exception::None};

        phi::boolean m_Halted{false};

        RegisterAccessType m_CurrentInstructionAccessType{RegisterAccessType::Ignored};
    };
} // namespace dlx

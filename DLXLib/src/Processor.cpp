#include "DLX/Processor.hpp"

#include "DLX/FloatRegister.hpp"
#include "DLX/Instruction.hpp"
#include "DLX/InstructionInfo.hpp"
#include "DLX/IntRegister.hpp"
#include "DLX/Logger.hpp"
#include "DLX/Parser.hpp"
#include "DLX/RegisterNames.hpp"
#include "DLX/StatusRegister.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/types.hpp>
#include <phi/type_traits/to_underlying.hpp>

PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wuninitialized")
PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(5262)

#include <fmt/core.h>
#include <fmt/format.h>

PHI_MSVC_SUPPRESS_WARNING_POP()
PHI_GCC_SUPPRESS_WARNING_POP()

PHI_GCC_SUPPRESS_WARNING("-Wconversion")
// TODO: Fix strict aliasing problems
PHI_GCC_SUPPRESS_WARNING("-Wstrict-aliasing")

namespace dlx
{
    static constexpr phi::boolean RegisterAccessTypeMatches(RegisterAccessType expected_access,
                                                            RegisterAccessType access) noexcept
    {
        PHI_ASSERT(access == RegisterAccessType::Signed || access == RegisterAccessType::Unsigned ||
                   access == RegisterAccessType::Float || access == RegisterAccessType::Double);

        switch (expected_access)
        {
            case RegisterAccessType::Ignored:
                return true;
            case RegisterAccessType::None:
                return false;
            case RegisterAccessType::MixedFloatDouble:
                return access == RegisterAccessType::Float || access == RegisterAccessType::Double;
            case RegisterAccessType::MixedSignedUnsigned:
                return access == RegisterAccessType::Signed ||
                       access == RegisterAccessType::Unsigned;
            case RegisterAccessType::MixedFloatSigned:
                return access == RegisterAccessType::Float || access == RegisterAccessType::Signed;
            case RegisterAccessType::MixedDoubleSigned:
                return access == RegisterAccessType::Double || access == RegisterAccessType::Signed;
            default:
                return expected_access == access;
        }
    }

    Processor::Processor() noexcept
        : m_IntRegistersValueTypes{}
        , m_FloatRegistersValueTypes{}
        , m_MemoryBlock(1000u, 1000u)
    {
        // Mark R0 as ready only
        m_IntRegisters[0].SetReadOnly(true);
    }

    IntRegister& Processor::GetIntRegister(IntRegisterID id) noexcept
    {
        PHI_ASSERT(id != IntRegisterID::None);
        phi::size_t id_value = phi::to_underlying(id);

        PHI_ASSERT(id_value < m_IntRegisters.size());

        return m_IntRegisters[id_value];
    }

    const IntRegister& Processor::GetIntRegister(IntRegisterID id) const noexcept
    {
        PHI_ASSERT(id != IntRegisterID::None);
        phi::size_t id_value = phi::to_underlying(id);

        PHI_ASSERT(id_value < m_IntRegisters.size());

        return m_IntRegisters[id_value];
    }

    phi::i32 Processor::IntRegisterGetSignedValue(IntRegisterID id) const noexcept
    {
        PHI_ASSERT(RegisterAccessTypeMatches(m_CurrentInstructionAccessType,
                                             RegisterAccessType::Signed),
                   "Mismatch for instruction access type");

        const phi::size_t id_value = phi::to_underlying(id);

        PHI_ASSERT(id_value < m_IntRegistersValueTypes.size());
        const IntRegisterValueType register_value_type = m_IntRegistersValueTypes[id_value];
        if (register_value_type != IntRegisterValueType::NotSet &&
            register_value_type != IntRegisterValueType::Signed)
        {
            DLX_WARN("Mismatch for register value type");
        }

        return GetIntRegister(id).GetSignedValue();
    }

    phi::u32 Processor::IntRegisterGetUnsignedValue(IntRegisterID id) const noexcept
    {
        PHI_ASSERT(RegisterAccessTypeMatches(m_CurrentInstructionAccessType,
                                             RegisterAccessType::Unsigned),
                   "Mismatch for instruction access type");

        const phi::size_t id_value = phi::to_underlying(id);

        PHI_ASSERT(id_value < m_IntRegistersValueTypes.size());
        const IntRegisterValueType register_value_type = m_IntRegistersValueTypes[id_value];
        if (register_value_type != IntRegisterValueType::NotSet &&
            register_value_type != IntRegisterValueType::Unsigned)
        {
            DLX_WARN("Mismatch for register value type");
        }

        return GetIntRegister(id).GetUnsignedValue();
    }

    void Processor::IntRegisterSetSignedValue(IntRegisterID id, phi::i32 value) noexcept
    {
        PHI_ASSERT(RegisterAccessTypeMatches(m_CurrentInstructionAccessType,
                                             RegisterAccessType::Signed),
                   "Mismatch for instruction access type");

        IntRegister& reg = GetIntRegister(id);

        if (reg.IsReadOnly())
        {
            return;
        }

        reg.SetSignedValue(value);

        const phi::size_t id_value = phi::to_underlying(id);

        PHI_ASSERT(id_value < m_IntRegistersValueTypes.size());
        m_IntRegistersValueTypes[id_value] = IntRegisterValueType::Signed;
    }

    void Processor::IntRegisterSetUnsignedValue(IntRegisterID id, phi::u32 value) noexcept
    {
        PHI_ASSERT(RegisterAccessTypeMatches(m_CurrentInstructionAccessType,
                                             RegisterAccessType::Unsigned),
                   "Mismatch for instruction access type");

        IntRegister& reg = GetIntRegister(id);

        if (reg.IsReadOnly())
        {
            return;
        }

        reg.SetUnsignedValue(value);

        const phi::size_t id_value = phi::to_underlying(id);

        PHI_ASSERT(id_value < m_IntRegistersValueTypes.size());
        m_IntRegistersValueTypes[id_value] = IntRegisterValueType::Unsigned;
    }

    FloatRegister& Processor::GetFloatRegister(FloatRegisterID id) noexcept
    {
        PHI_ASSERT(id != FloatRegisterID::None);
        const phi::size_t id_value = phi::to_underlying(id);

        PHI_ASSERT(id_value < m_FloatRegisters.size());

        return m_FloatRegisters[id_value];
    }

    const FloatRegister& Processor::GetFloatRegister(FloatRegisterID id) const noexcept
    {
        PHI_ASSERT(id != FloatRegisterID::None);
        const phi::size_t id_value = phi::to_underlying(id);

        PHI_ASSERT(id_value < m_FloatRegisters.size());

        return m_FloatRegisters[id_value];
    }

    [[nodiscard]] phi::f32 Processor::FloatRegisterGetFloatValue(FloatRegisterID id) const noexcept
    {
        PHI_ASSERT(RegisterAccessTypeMatches(m_CurrentInstructionAccessType,
                                             RegisterAccessType::Float),
                   "Mismatch for instruction access type");

        const phi::size_t id_value = phi::to_underlying(id);

        PHI_ASSERT(id_value < m_FloatRegistersValueTypes.size());
        const FloatRegisterValueType register_value_type = m_FloatRegistersValueTypes[id_value];
        if (register_value_type != FloatRegisterValueType::NotSet &&
            register_value_type != FloatRegisterValueType::Float)
        {
            /*
            DLX_WARN("Mismatch for register value type");
            */
        }

        const FloatRegister& reg = GetFloatRegister(id);

        return reg.GetValue();
    }

    [[nodiscard]] phi::f64 Processor::FloatRegisterGetDoubleValue(FloatRegisterID id) noexcept
    {
        PHI_ASSERT(RegisterAccessTypeMatches(m_CurrentInstructionAccessType,
                                             RegisterAccessType::Double),
                   "Mismatch for instruction access type");

        if (phi::to_underlying(id) % 2 == 1)
        {
            Raise(Exception::MisalignedRegisterAccess);
            return {0.0};
        }

        const phi::size_t id_value = phi::to_underlying(id);

        PHI_ASSERT(id_value + 1u < m_FloatRegistersValueTypes.size());
        const FloatRegisterValueType register_value_type_low = m_FloatRegistersValueTypes[id_value];
        if (register_value_type_low != FloatRegisterValueType::NotSet &&
            register_value_type_low != FloatRegisterValueType::DoubleLow)
        {
            DLX_WARN("Mismatch for register value type");
        }

        const FloatRegisterValueType register_value_type_high =
                m_FloatRegistersValueTypes[id_value + 1u];
        if (register_value_type_high != FloatRegisterValueType::NotSet &&
            register_value_type_high != FloatRegisterValueType::DoubleHigh)
        {
            DLX_WARN("Mismatch for register value type");
        }

        const FloatRegister& first_reg = GetFloatRegister(id);
        const FloatRegister& second_reg =
                GetFloatRegister(static_cast<FloatRegisterID>(static_cast<phi::size_t>(id) + 1));

        const float first_value  = first_reg.GetValue().unsafe();
        const float second_value = second_reg.GetValue().unsafe();

        // TODO: Don't invoke undefined behavior this way. Instead use union type punning
        PHI_CLANG_SUPPRESS_WARNING_WITH_PUSH("-Wundefined-reinterpret-cast")

        const phi::uint32_t first_value_bits =
                *reinterpret_cast<const phi::uint32_t*>(&first_value);
        const phi::uint32_t second_value_bits =
                *reinterpret_cast<const phi::uint32_t*>(&second_value);

        phi::uint64_t final_value_bits =
                static_cast<phi::uint64_t>(second_value_bits) << 32u | first_value_bits;

        return *reinterpret_cast<double*>(&final_value_bits);

        PHI_CLANG_SUPPRESS_WARNING_POP()
    }

    void Processor::FloatRegisterSetFloatValue(FloatRegisterID id, phi::f32 value) noexcept
    {
        PHI_ASSERT(RegisterAccessTypeMatches(m_CurrentInstructionAccessType,
                                             RegisterAccessType::Float),
                   "Mismatch for instruction access type");

        FloatRegister& reg = GetFloatRegister(id);

        reg.SetValue(value);

        const phi::size_t id_value = phi::to_underlying(id);

        PHI_ASSERT(id_value < m_FloatRegistersValueTypes.size());
        m_FloatRegistersValueTypes[id_value] = FloatRegisterValueType::Float;
    }

    void Processor::FloatRegisterSetDoubleValue(FloatRegisterID id, phi::f64 value) noexcept
    {
        PHI_ASSERT(RegisterAccessTypeMatches(m_CurrentInstructionAccessType,
                                             RegisterAccessType::Double),
                   "Mismatch for instruction access type");

        if (phi::to_underlying(id) % 2 == 1)
        {
            Raise(Exception::MisalignedRegisterAccess);
            return;
        }

        const constexpr phi::uint64_t first_32_bits  = 0b11111111'11111111'11111111'11111111;
        const constexpr phi::uint64_t second_32_bits = first_32_bits << 32u;

        double              value_raw  = value.unsafe();
        const phi::uint64_t value_bits = *reinterpret_cast<phi::uint64_t*>(&value_raw);

        const phi::uint32_t first_bits  = value_bits & first_32_bits;
        const phi::uint32_t second_bits = (value_bits & second_32_bits) >> 32u;

        const float first_value  = *reinterpret_cast<const float*>(&first_bits);
        const float second_value = *reinterpret_cast<const float*>(&second_bits);

        FloatRegister& first_reg = GetFloatRegister(id);
        FloatRegister& second_reg =
                GetFloatRegister(static_cast<FloatRegisterID>(static_cast<phi::size_t>(id) + 1));

        first_reg.SetValue(first_value);
        second_reg.SetValue(second_value);

        const phi::size_t id_value = phi::to_underlying(id);

        PHI_ASSERT(id_value + 1u < m_FloatRegistersValueTypes.size());
        m_FloatRegistersValueTypes[id_value]      = FloatRegisterValueType::DoubleLow;
        m_FloatRegistersValueTypes[id_value + 1u] = FloatRegisterValueType::DoubleHigh;
    }

    StatusRegister& Processor::GetFPSR() noexcept
    {
        return m_FPSR;
    }

    const StatusRegister& Processor::GetFPSR() const noexcept
    {
        return m_FPSR;
    }

    phi::boolean Processor::GetFPSRValue() const noexcept
    {
        const StatusRegister& status_reg = GetFPSR();

        return status_reg.Get();
    }

    void Processor::SetFPSRValue(phi::boolean value) noexcept
    {
        StatusRegister& status_reg = GetFPSR();

        status_reg.SetStatus(value);
    }

    void Processor::ExecuteInstruction(const Instruction& inst) noexcept
    {
        m_CurrentInstructionAccessType = inst.GetInfo().GetRegisterAccessType();

        inst.Execute(*this);
    }

    phi::boolean Processor::LoadProgram(ParsedProgram& program) noexcept
    {
        if (!program.m_ParseErrors.empty())
        {
            DLX_WARN("Trying to load program with parsing errors");
            return false;
        }

        m_CurrentProgram = &program;

        m_ProgramCounter               = 0u;
        m_Halted                       = false;
        m_CurrentInstructionAccessType = RegisterAccessType::Ignored;
        m_LastRaisedException          = Exception::None;
        m_CurrentStepCount             = 0u;

        return true;
    }

    phi::observer_ptr<ParsedProgram> Processor::GetCurrentProgram() const noexcept
    {
        return m_CurrentProgram;
    }

    void Processor::ExecuteStep() noexcept
    {
        // No nothing when no program is loaded
        if (!m_CurrentProgram)
        {
            return;
        }

        // Halt if there are no instruction to execute
        if (m_CurrentProgram->m_Instructions.empty())
        {
            m_Halted                       = true;
            m_CurrentInstructionAccessType = RegisterAccessType::Ignored;
        }

        // Do nothing when processor is halted
        if (m_Halted)
        {
            return;
        }

        // Increase Next program counter (may be later overwritten by branch instructions)
        m_NextProgramCounter = m_ProgramCounter + 1u;

        // Get current instruction pointed to by the program counter
        const auto& current_instruction =
                m_CurrentProgram->m_Instructions.at(m_ProgramCounter.unsafe());

        // Execute current instruction
        ExecuteInstruction(current_instruction);

        // Stop executing if the last instruction halted the processor
        if (m_Halted)
        {
            m_CurrentInstructionAccessType = RegisterAccessType::Ignored;
            return;
        }

        m_ProgramCounter = m_NextProgramCounter;

        ++m_CurrentStepCount;

        if ((m_MaxNumberOfSteps != 0u && m_CurrentStepCount >= m_MaxNumberOfSteps) ||
            (m_ProgramCounter >= m_CurrentProgram->m_Instructions.size()))
        {
            m_Halted                       = true;
            m_CurrentInstructionAccessType = RegisterAccessType::Ignored;
        }
    }

    void Processor::ExecuteCurrentProgram() noexcept
    {
        // Do nothing when no program is loaded
        if (!m_CurrentProgram)
        {
            return;
        }

        m_ProgramCounter               = 0u;
        m_Halted                       = false;
        m_CurrentInstructionAccessType = RegisterAccessType::Ignored;
        m_LastRaisedException          = Exception::None;
        m_CurrentStepCount             = 0u;

        while (!m_Halted)
        {
            ExecuteStep();
        }

        PHI_ASSERT(m_CurrentInstructionAccessType == RegisterAccessType::Ignored,
                   "RegisterAccessType was not reset correctly");
    }

    void Processor::Reset() noexcept
    {
        ClearMemory();
        ClearMemory();
        m_CurrentProgram.reset();
        m_ProgramCounter               = 0u;
        m_NextProgramCounter           = 0u;
        m_Halted                       = true;
        m_CurrentInstructionAccessType = RegisterAccessType::Ignored;
        m_LastRaisedException          = Exception::None;
        m_CurrentStepCount             = 0u;
    }

    void Processor::ClearRegisters() noexcept
    {
        for (auto& reg : m_IntRegisters)
        {
            reg.SetSignedValue(0);
        }

        for (auto& reg : m_FloatRegisters)
        {
            reg.SetValue(0.0f);
        }

        m_FPSR.SetStatus(false);
    }

    void Processor::ClearMemory() noexcept
    {
        m_MemoryBlock.Clear();
    }

    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_PUSH()
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wswitch")
    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4702) // Unreachable code

    void Processor::Raise(Exception exception) noexcept
    {
        PHI_ASSERT(exception != Exception::None, "Cannot raise None exception");

        m_LastRaisedException = exception;

        switch (exception)
        {
            case Exception::DivideByZero:
                m_Halted = true;
                DLX_ERROR("Division through zero");
                return;
            case Exception::Overflow:
                DLX_WARN("Overflow");
                return;
            case Exception::Underflow:
                DLX_WARN("Underflow");
                return;
            case Exception::Trap:
                m_Halted = true;
                DLX_ERROR("Trapped");
                return;
            case Exception::Halt:
                m_Halted = true;
                return;
            case Exception::UnknownLabel:
                m_Halted = true;
                DLX_ERROR("Unknown label");
                return;
            case Exception::BadShift:
                DLX_ERROR("Bad shift");
                return;
            case Exception::AddressOutOfBounds:
                DLX_ERROR("Address out of bounds");
                m_Halted = true;
                return;
            case Exception::MisalignedRegisterAccess:
                DLX_ERROR("Misaligned register access");
                m_Halted = true;
                return;

#if !defined(DLXEMU_COVERAGE_BUILD)
            default:
                PHI_ASSERT_NOT_REACHED();
#endif
        }

#if !defined(DLXEMU_COVERAGE_BUILD)
        PHI_ASSERT_NOT_REACHED();
#endif
    }

    PHI_MSVC_SUPPRESS_WARNING_POP()
    PHI_CLANG_AND_GCC_SUPPRESS_WARNING_POP()

    Exception Processor::GetLastRaisedException() const noexcept
    {
        return m_LastRaisedException;
    }

    phi::boolean Processor::IsHalted() const noexcept
    {
        return m_Halted;
    }

    const MemoryBlock& Processor::GetMemory() const noexcept
    {
        return m_MemoryBlock;
    }

    MemoryBlock& Processor::GetMemory() noexcept
    {
        return m_MemoryBlock;
    }

    phi::u32 Processor::GetProgramCounter() const noexcept
    {
        return m_ProgramCounter;
    }

    void Processor::SetProgramCounter(phi::u32 new_pc) noexcept
    {
        m_ProgramCounter = new_pc;
    }

    phi::u32 Processor::GetNextProgramCounter() const noexcept
    {
        return m_NextProgramCounter;
    }

    void Processor::SetNextProgramCounter(phi::u32 new_npc) noexcept
    {
        m_NextProgramCounter = new_npc;
    }

    phi::usize Processor::GetCurrentStepCount() const noexcept
    {
        return m_CurrentStepCount;
    }

    void Processor::SetMaxNumberOfSteps(phi::usize new_max) noexcept
    {
        m_MaxNumberOfSteps = new_max;
    }

    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wabi-tag")

    std::string Processor::GetRegisterDump() const noexcept
    {
        std::string text{"Int registers:\n"};

        for (phi::usize i{0u}; i < m_IntRegisters.size(); ++i)
        {
            const IntRegister reg = m_IntRegisters.at(i.unsafe());
            text.append(fmt::format(
                    "R{0}: sdec: {1:d}, udec: {2:d}, hex: 0x{2:08X}, bin: {2:#032b}\n", i.unsafe(),
                    reg.GetSignedValue().unsafe(), reg.GetUnsignedValue().unsafe()));
        }

        text.append("\nFloat registers:\n");

        for (phi::usize i{0u}; i < m_FloatRegisters.size(); ++i)
        {
            const FloatRegister reg        = m_FloatRegisters.at(i.unsafe());
            float               value      = reg.GetValue().unsafe();
            phi::uint32_t       value_uint = *reinterpret_cast<phi::uint32_t*>(&value);
            text.append(fmt::format("F{0}: flt: {1:f}, hex: 0x{2:08X}, bin: {2:#032b}\n",
                                    i.unsafe(), reg.GetValue().unsafe(), value_uint));
        }

        text.append("\nStatus registers:\n");

        text.append(fmt::format("FPSR: {}", m_FPSR.Get() ? "Set" : "Not Set"));

        return text;
    }

    std::string Processor::GetMemoryDump() const noexcept
    {
        std::string text{"Not Implemented"};

        // TODO: Properly implement

        return text;
    }

    std::string Processor::GetProcessorDump() const noexcept
    {
        std::string text;

        text.append(fmt::format("H: {:s}\n", m_Halted ? "True" : "False"));
        text.append(fmt::format("PC: {:d}, NPC: {:d}\n", m_ProgramCounter.unsafe(),
                                m_NextProgramCounter.unsafe()));

        if (m_CurrentProgram)
        {
            if (m_CurrentProgram->m_ParseErrors.empty() &&
                m_ProgramCounter < m_CurrentProgram->m_Instructions.size())
            {
                Instruction instr = m_CurrentProgram->m_Instructions.at(m_ProgramCounter.unsafe());
                text.append(fmt::format("INSTR:\n{}\n", instr.DebugInfo()));
            }
            else
            {
                text.append(fmt::format("INSTR:\nPC >= Instruction count ({:d}))\n",
                                        m_CurrentProgram->m_Instructions.size()));
            }
        }
        else
        {
            text.append("INSTR:\nNo program loaded\n");
        }

        text.append(fmt::format("EX: {}\n", dlx::enum_name(m_LastRaisedException).data()));
        text.append(fmt::format("IAT: {}", dlx::enum_name(m_CurrentInstructionAccessType).data()));

        return text;
    }

    std::string Processor::GetCurrentProgramDump() const noexcept
    {
        if (m_CurrentProgram)
        {
            return m_CurrentProgram->GetDump();
        }

        return "No Program";
    }

    PHI_GCC_SUPPRESS_WARNING_POP()
} // namespace dlx

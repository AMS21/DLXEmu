#include "DLX/Processor.hpp"

#include "DLX/FloatRegister.hpp"
#include "DLX/InstructionInfo.hpp"
#include "DLX/Parser.hpp"
#include "DLX/RegisterNames.hpp"
#include "DLX/StatusRegister.hpp"
#include "Phi/Core/Boolean.hpp"
#include "Phi/Core/Log.hpp"
#include "Phi/Core/Types.hpp"

namespace dlx
{
    static phi::Boolean RegisterAccessTypeMatches(RegisterAccessType expected_access,
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
            default:
                return expected_access == access;
        }
    }

    Processor::Processor() noexcept
        : m_MemoryBlock(1000u, 1000u)
    {
        // Mark R0 as ready only
        m_IntRegisters.at(0).SetReadOnly(true);
    }

    IntRegister& Processor::GetIntRegister(IntRegisterID id) noexcept
    {
        PHI_ASSERT(id != IntRegisterID::None);
        std::underlying_type_t<IntRegisterID> id_value =
                static_cast<std::underlying_type_t<IntRegisterID>>(id);

        PHI_ASSERT(id_value >= 0 && id_value <= 31);

        return m_IntRegisters.at(id_value);
    }

    const IntRegister& Processor::GetIntRegister(IntRegisterID id) const noexcept
    {
        PHI_ASSERT(id != IntRegisterID::None);
        std::underlying_type_t<IntRegisterID> id_value =
                static_cast<std::underlying_type_t<IntRegisterID>>(id);

        PHI_ASSERT(id_value >= 0 && id_value <= 31);

        return m_IntRegisters.at(id_value);
    }

    phi::i32 Processor::IntRegisterGetSignedValue(IntRegisterID id) const noexcept
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType, RegisterAccessType::Signed))
        {
            PHI_LOG_WARN("Mismatch for instruction access type");
        }

        return GetIntRegister(id).GetSignedValue();
    }

    phi::u32 Processor::IntRegisterGetUnsignedValue(IntRegisterID id) const noexcept
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType,
                                       RegisterAccessType::Unsigned))
        {
            PHI_LOG_WARN("Mismatch for instruction access type");
        }

        return GetIntRegister(id).GetUnsignedValue();
    }

    void Processor::IntRegisterSetSignedValue(IntRegisterID id, phi::i32 value) noexcept
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType, RegisterAccessType::Signed))
        {
            PHI_LOG_WARN("Mismatch for instruction access type");
        }

        IntRegister& reg = GetIntRegister(id);

        if (reg.IsReadOnly())
        {
            return;
        }

        reg.SetSignedValue(value);
    }

    void Processor::IntRegisterSetUnsignedValue(IntRegisterID id, phi::u32 value) noexcept
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType,
                                       RegisterAccessType::Unsigned))
        {
            PHI_LOG_WARN("Mismatch for instruction access type");
        }

        IntRegister& reg = GetIntRegister(id);

        if (reg.IsReadOnly())
        {
            return;
        }

        reg.SetUnsignedValue(value);
    }

    FloatRegister& Processor::GetFloatRegister(FloatRegisterID id) noexcept
    {
        PHI_ASSERT(id != FloatRegisterID::None);
        std::underlying_type_t<FloatRegisterID> id_value =
                static_cast<std::underlying_type_t<FloatRegisterID>>(id);

        PHI_ASSERT(id_value >= 0 && id_value <= 31);

        return m_FloatRegisters.at(id_value);
    }

    const FloatRegister& Processor::GetFloatRegister(FloatRegisterID id) const noexcept
    {
        PHI_ASSERT(id != FloatRegisterID::None);
        std::underlying_type_t<FloatRegisterID> id_value =
                static_cast<std::underlying_type_t<FloatRegisterID>>(id);

        PHI_ASSERT(id_value >= 0 && id_value <= 31);

        return m_FloatRegisters.at(id_value);
    }

    [[nodiscard]] phi::f32 Processor::FloatRegisterGetFloatValue(FloatRegisterID id) const noexcept
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType, RegisterAccessType::Float))
        {
            PHI_LOG_WARN("Mismatch for instruction access type");
        }

        const FloatRegister& reg = GetFloatRegister(id);

        return reg.GetValue();
    }

    [[nodiscard]] phi::f64 Processor::FloatRegisterGetDoubleValue(FloatRegisterID id) noexcept
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType, RegisterAccessType::Double))
        {
            PHI_LOG_WARN("Mismatch for instruction access type");
        }

        if (id == FloatRegisterID::F31)
        {
            Raise(Exception::RegisterOutOfBounds);
            return phi::f64(0.0);
        }

        const FloatRegister& first_reg = GetFloatRegister(id);
        const FloatRegister& second_reg =
                GetFloatRegister(static_cast<FloatRegisterID>(static_cast<std::size_t>(id) + 1));

        const float first_value  = first_reg.GetValue().get();
        const float second_value = second_reg.GetValue().get();

        const std::uint32_t first_value_bits =
                *reinterpret_cast<const std::uint32_t*>(&first_value);
        const std::uint32_t second_value_bits =
                *reinterpret_cast<const std::uint32_t*>(&second_value);

        std::uint64_t final_value_bits =
                static_cast<std::uint64_t>(second_value_bits) << 32u | first_value_bits;

        return *reinterpret_cast<double*>(&final_value_bits);
    }

    void Processor::FloatRegisterSetFloatValue(FloatRegisterID id, phi::f32 value) noexcept
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType, RegisterAccessType::Float))
        {
            PHI_LOG_WARN("Mismatch for instruction access type");
        }

        FloatRegister& reg = GetFloatRegister(id);

        reg.SetValue(value);
    }

    void Processor::FloatRegisterSetDoubleValue(FloatRegisterID id, phi::f64 value) noexcept
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType, RegisterAccessType::Double))
        {
            PHI_LOG_WARN("Mismatch for instruction access type");
        }

        if (id == FloatRegisterID::F31)
        {
            Raise(Exception::RegisterOutOfBounds);
            return;
        }

        const constexpr std::uint64_t first_32_bits  = 0b11111111'11111111'11111111'11111111;
        const constexpr std::uint64_t second_32_bits = first_32_bits << 32u;

        double              value_raw  = value.get();
        const std::uint64_t value_bits = *reinterpret_cast<std::uint64_t*>(&value_raw);

        const std::uint32_t first_bits  = value_bits & first_32_bits;
        const std::uint32_t second_bits = (value_bits & second_32_bits) >> 32u;

        const float first_value  = *reinterpret_cast<const float*>(&first_bits);
        const float second_value = *reinterpret_cast<const float*>(&second_bits);

        FloatRegister& first_reg = GetFloatRegister(id);
        FloatRegister& second_reg =
                GetFloatRegister(static_cast<FloatRegisterID>(static_cast<std::size_t>(id) + 1));

        first_reg.SetValue(first_value);
        second_reg.SetValue(second_value);
    }

    StatusRegister& Processor::GetFPSR() noexcept
    {
        return m_FPSR;
    }

    const StatusRegister& Processor::GetFPSR() const noexcept
    {
        return m_FPSR;
    }

    phi::Boolean Processor::GetFPSRValue() const noexcept
    {
        const StatusRegister& status_reg = GetFPSR();

        return status_reg.Get();
    }

    void Processor::SetFPSRValue(phi::Boolean value) noexcept
    {
        StatusRegister& status_reg = GetFPSR();

        status_reg.SetStatus(value);
    }

    void Processor::ExecuteInstruction(const Instruction& inst) noexcept
    {
        m_CurrentInstructionAccessType = inst.GetInfo().GetRegisterAccessType();

        inst.Execute(*this);
    }

    phi::Boolean Processor::LoadProgram(ParsedProgram& program) noexcept
    {
        if (!program.m_ParseErrors.empty())
        {
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

    phi::ObserverPtr<ParsedProgram> Processor::GetCurrentProgramm() const noexcept
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

        // No nothing when processor is halted
        if (m_Halted)
        {
            return;
        }

        // Increase Next program counter (may be later overwritten by branch instructions)
        m_NextProgramCounter = m_ProgramCounter + 1u;

        // Get current instruction pointed to by the program counter
        const auto& current_instruction =
                m_CurrentProgram->m_Instructions.at(m_ProgramCounter.get());

        // Execute current instruction
        ExecuteInstruction(current_instruction);

        m_ProgramCounter = m_NextProgramCounter;

        ++m_CurrentStepCount;

        if ((m_MaxNumberOfSteps != 0u && m_CurrentStepCount >= m_MaxNumberOfSteps) ||
            (m_ProgramCounter >= m_CurrentProgram->m_Instructions.size()))
        {
            m_Halted = true;
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

    void Processor::Raise(Exception exception) noexcept
    {
        PHI_ASSERT(exception != Exception::None, "Cannot raise None exception");

        m_LastRaisedException = exception;

        switch (exception)
        {
            case Exception::None:
                PHI_ASSERT_NOT_REACHED();
                return;
            case Exception::DivideByZero:
                m_Halted = true;
                PHI_LOG_ERROR("Division through zero");
                return;
            case Exception::Overflow:
                PHI_LOG_WARN("Overflow");
                return;
            case Exception::Underflow:
                PHI_LOG_WARN("Underflow");
                return;
            case Exception::Trap:
                m_Halted = true;
                PHI_LOG_ERROR("Trapped");
                return;
            case Exception::Halt:
                m_Halted = true;
                return;
            case Exception::UnknownLabel:
                m_Halted = true;
                PHI_LOG_ERROR("Unknown label");
                return;
            case Exception::BadShift:
                PHI_LOG_ERROR("Bad shift");
                return;
            case Exception::AddressOutOfBounds:
                PHI_LOG_ERROR("Address out of bounds");
                m_Halted = true;
                return;
            case Exception::RegisterOutOfBounds:
                PHI_LOG_ERROR("Register out of bounds");
                m_Halted = true;
                return;
        }

        PHI_ASSERT_NOT_REACHED();
    }

    Exception Processor::GetLastRaisedException() const noexcept
    {
        return m_LastRaisedException;
    }

    phi::Boolean Processor::IsHalted() const noexcept
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

    [[nodiscard]] phi::u32 Processor::GetNextProgramCounter() const noexcept
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
} // namespace dlx

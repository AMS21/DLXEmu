#include "DLX/Processor.hpp"

#include "DLX/Parser.hpp"

namespace dlx
{
    static phi::Boolean RegisterAccessTypeMatches(RegisterAccessType expected_access,
                                                  RegisterAccessType access)
    {
        PHI_ASSERT(access == RegisterAccessType::Signed || access == RegisterAccessType::Unsigned);

        switch (expected_access)
        {
            case RegisterAccessType::Ignored:
                return true;
            case RegisterAccessType::None:
                return false;
            default:
                return expected_access == access;
        }
    }

    Processor::Processor()
        : m_MemoryBlock(1000u, 1000u)
    {
        // Mark R0 as ready only
        m_IntRegisters.at(0).SetReadOnly(true);
    }

    IntRegister& Processor::GetIntRegister(IntRegisterID id)
    {
        PHI_ASSERT(id != IntRegisterID::None);
        std::underlying_type_t<IntRegisterID> id_value =
                static_cast<std::underlying_type_t<IntRegisterID>>(id);

        PHI_ASSERT(id_value >= 0 && id_value <= 31);

        return m_IntRegisters.at(id_value);
    }

    const IntRegister& Processor::GetIntRegister(IntRegisterID id) const
    {
        PHI_ASSERT(id != IntRegisterID::None);
        std::underlying_type_t<IntRegisterID> id_value =
                static_cast<std::underlying_type_t<IntRegisterID>>(id);

        PHI_ASSERT(id_value >= 0 && id_value <= 31);

        return m_IntRegisters.at(id_value);
    }

    phi::i32 Processor::IntRegisterGetSignedValue(IntRegisterID id) const
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType, RegisterAccessType::Signed))
        {
            PHI_LOG_WARN("Mismatch for instruction access type");
        }

        return GetIntRegister(id).GetSignedValue();
    }

    phi::u32 Processor::IntRegisterGetUnsignedValue(IntRegisterID id) const
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType, RegisterAccessType::Unsigned))
        {
            PHI_LOG_WARN("Mismatch for instruction access type");
        }

        return GetIntRegister(id).GetUnsignedValue();
    }

    void Processor::IntRegisterSetSignedValue(IntRegisterID id, phi::i32 value)
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

    void Processor::IntRegisterSetUnsignedValue(IntRegisterID id, phi::u32 value)
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType, RegisterAccessType::Unsigned))
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

    void Processor::ExecuteInstruction(const Instruction& inst)
    {
        m_CurrentInstructionAccessType = inst.m_Info.GetRegisterAccessType();

        inst.Execute(*this);
    }

    void Processor::LoadProgram(ParsedProgram& programm)
    {
        m_CurrentProgram = &programm;

        m_ProgramCounter = 0u;
        m_Halted         = false;
        m_CurrentInstructionAccessType = RegisterAccessType::Ignored;
    }

    phi::ObserverPtr<ParsedProgram> Processor::GetCurrentProgramm() const noexcept
    {
        return m_CurrentProgram;
    }

    void Processor::ExecuteCurrentProgram()
    {
        PHI_ASSERT(m_CurrentProgram);

        // Don't execute a program with parsing errors
        if (!m_CurrentProgram->m_ParseErrors.empty())
        {
            return;
        }

        m_ProgramCounter      = 0u;
        m_Halted              = false;
        m_LastRaisedException = Exception::None;

        phi::usize StepCount{0u};

        while (m_ProgramCounter < m_CurrentProgram->m_Instructions.size() && !m_Halted &&
               (m_MaxNumberOfSteps != 0u && StepCount < m_MaxNumberOfSteps))
        {
            m_NextProgramCounter = m_ProgramCounter + 1u;

            const auto& current_instruction =
                    m_CurrentProgram->m_Instructions.at(m_ProgramCounter.get());
            ExecuteInstruction(current_instruction);

            m_ProgramCounter = m_NextProgramCounter;

            ++StepCount;
        }
    }

    void Processor::ClearRegisters()
    {
        for (auto& reg : m_IntRegisters)
        {
            reg.SetSignedValue(0);
        }
    }

    void Processor::ClearMemory()
    {
        m_MemoryBlock.Clear();
    }

    void Processor::Raise(Exception exception)
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
        }

        PHI_ASSERT_NOT_REACHED();
    }

    Exception Processor::GetLastRaisedException() const noexcept
    {
        return m_LastRaisedException;
    }

    [[nodiscard]] phi::Boolean Processor::IsHalted() const noexcept
    {
        return m_Halted;
    }
} // namespace dlx

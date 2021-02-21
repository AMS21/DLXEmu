#include "DLX/Processor.hpp"

#include "DLX/FloatRegister.hpp"
#include "DLX/InstructionInfo.hpp"
#include "DLX/Parser.hpp"
#include "Phi/Core/Log.hpp"
#include "Phi/Core/Types.hpp"

namespace dlx
{
    static phi::Boolean RegisterAccessTypeMatches(RegisterAccessType expected_access,
                                                  RegisterAccessType access)
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
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType,
                                       RegisterAccessType::Unsigned))
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

    FloatRegister& Processor::GetFloatRegister(FloatRegisterID id)
    {
        PHI_ASSERT(id != FloatRegisterID::None);
        std::underlying_type_t<FloatRegisterID> id_value =
                static_cast<std::underlying_type_t<FloatRegisterID>>(id);

        PHI_ASSERT(id_value >= 0 && id_value <= 31);

        return m_FloatRegisters.at(id_value);
    }

    const FloatRegister& Processor::GetFloatRegister(FloatRegisterID id) const
    {
        PHI_ASSERT(id != FloatRegisterID::None);
        std::underlying_type_t<FloatRegisterID> id_value =
                static_cast<std::underlying_type_t<FloatRegisterID>>(id);

        PHI_ASSERT(id_value >= 0 && id_value <= 31);

        return m_FloatRegisters.at(id_value);
    }

    [[nodiscard]] phi::f32 Processor::FloatRegisterGetFloatValue(FloatRegisterID id) const
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType, RegisterAccessType::Float))
        {
            PHI_LOG_WARN("Mismatch for instruction access type");
        }

        const FloatRegister& reg = GetFloatRegister(id);

        return reg.GetValue();
    }

    [[nodiscard]] phi::f64 Processor::FloatRegisterGetDoubleValue(FloatRegisterID id)
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

    void Processor::FloatRegisterSetFloatValue(FloatRegisterID id, phi::f32 value)
    {
        if (!RegisterAccessTypeMatches(m_CurrentInstructionAccessType, RegisterAccessType::Float))
        {
            PHI_LOG_WARN("Mismatch for instruction access type");
        }

        FloatRegister& reg = GetFloatRegister(id);

        reg.SetValue(value);
    }

    void Processor::FloatRegisterSetDoubleValue(FloatRegisterID id, phi::f64 value)
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

    void Processor::ExecuteInstruction(const Instruction& inst)
    {
        m_CurrentInstructionAccessType = inst.GetInfo().GetRegisterAccessType();

        inst.Execute(*this);
    }

    void Processor::LoadProgram(ParsedProgram& programm)
    {
        m_CurrentProgram = &programm;

        m_ProgramCounter               = 0u;
        m_Halted                       = false;
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

        for (auto& reg : m_FloatRegisters)
        {
            reg.SetValue(0.0f);
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

    [[nodiscard]] phi::Boolean Processor::IsHalted() const noexcept
    {
        return m_Halted;
    }
} // namespace dlx

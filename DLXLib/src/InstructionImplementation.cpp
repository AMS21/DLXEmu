#include "DLX/InstructionImplementation.hpp"

#include "DLX/InstructionArg.hpp"
#include "DLX/InstructionInfo.hpp"
#include "DLX/Parser.hpp"
#include "DLX/Processor.hpp"
#include "DLX/RegisterNames.hpp"
#include "Phi/Core/Assert.hpp"
#include "Phi/Core/Boolean.hpp"
#include "Phi/Core/Types.hpp"
#include <string_view>

namespace dlx
{
    static std::int32_t clear_top_n_bits(std::int32_t value, std::int32_t n)
    {
        PHI_ASSERT(n > 0 && n < 32, "Would invoke undefined behaviour");

        return value & ~(-1 << (32 - n));
    }

    static void JumpToLabel(Processor& processor, std::string_view label_name)
    {
        // Lookup the label
        const phi::ObserverPtr<ParsedProgram> program = processor.GetCurrentProgramm();
        PHI_ASSERT(program);

        std::string label(label_name.data(), label_name.size());
        if (program->m_JumpData.find(label) == program->m_JumpData.end())
        {
            PHI_LOG_ERROR("Unable to find jump label {}", label_name);
            processor.Raise(Exception::UnknownLabel);
            return;
        }

        // Set program counter
        processor.m_NextProgramCounter = program->m_JumpData.at(label);
    }

    static void JumpToRegister(Processor& processor, IntRegisterID reg_id)
    {
        phi::u32 address = processor.IntRegisterGetUnsignedValue(reg_id);

        phi::u32 max_address =
                static_cast<std::uint32_t>(processor.GetCurrentProgramm()->m_Instructions.size());
        if (address >= max_address)
        {
            processor.Raise(Exception::AddressOutOfBounds);
            return;
        }

        processor.m_NextProgramCounter = address.get();
    }

    static std::optional<phi::i32> CalculateDisplacementAddress(
            Processor& processor, const InstructionArg::AddressDisplacement& adr_displacement)
    {
        phi::i32 register_value = processor.IntRegisterGetSignedValue(adr_displacement.register_id);

        phi::i32 address = adr_displacement.displacement + register_value;

        if (address < 0)
        {
            processor.Raise(Exception::AddressOutOfBounds);
            return {};
        }

        return address;
    }

    static std::optional<phi::i32> GetLoadStoreAddress(Processor&           processor,
                                                       const InstructionArg argument)
    {
        if (argument.GetType() == ArgumentType::ImmediateInteger)
        {
            const auto& imm_value = argument.AsImmediateValue();

            if (imm_value.signed_value < 0)
            {
                return {};
            }

            return imm_value.signed_value;
        }

        if (argument.GetType() == ArgumentType::AddressDisplacement)
        {
            const auto& adr_displacement = argument.AsAddressDisplacement();
            return CalculateDisplacementAddress(processor, adr_displacement);
        }

        PHI_ASSERT_NOT_REACHED();
    }

    static void SafeWriteInteger(Processor& processor, IntRegisterID dest_reg, phi::i64 value)
    {
        constexpr phi::i64 min = phi::i32::limits_type::min();
        constexpr phi::i64 max = phi::i32::limits_type::max();

        // Check for underflow
        if (value < min)
        {
            processor.Raise(Exception::Underflow);

            value = max + (value % (min - 1));
        }
        // Check for overflow
        else if (value > max)
        {
            processor.Raise(Exception::Overflow);

            value = min + (value % (max + 1));
        }

        PHI_ASSERT(value >= min);
        PHI_ASSERT(value <= max);

        processor.IntRegisterSetSignedValue(dest_reg, static_cast<std::int32_t>(value.get()));
    }

    static void SafeWriteInteger(Processor& processor, IntRegisterID dest_reg, phi::u64 value)
    {
        constexpr phi::u64 min = phi::u32::limits_type::min();
        constexpr phi::u64 max = phi::u32::limits_type::max();

        // Check for overflow
        if (value > max)
        {
            processor.Raise(Exception::Overflow);

            value %= max + 1u;
        }

        PHI_ASSERT(value <= max);

        processor.IntRegisterSetUnsignedValue(dest_reg, static_cast<std::uint32_t>(value.get()));
    }

    static void Addition(Processor& processor, IntRegisterID dest_reg, phi::i32 lhs, phi::i32 rhs)
    {
        phi::i64 res = phi::i64(lhs) + rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Addition(Processor& processor, IntRegisterID dest_reg, phi::u32 lhs, phi::u32 rhs)
    {
        phi::u64 res = phi::u64(lhs) + rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Subtraction(Processor& processor, IntRegisterID dest_reg, phi::i32 lhs,
                            phi::i32 rhs)
    {
        phi::i64 res = phi::i64(lhs) - rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Subtraction(Processor& processor, IntRegisterID dest_reg, phi::u32 lhs,
                            phi::u32 rhs)
    {
        constexpr phi::u32 max = phi::u32::limits_type::max();

        if (lhs < rhs)
        {
            processor.Raise(Exception::Underflow);

            phi::u64 res = max - rhs + lhs + 1u;
            SafeWriteInteger(processor, dest_reg, res);
            return;
        }

        phi::u64 res = phi::u64(lhs) - rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Multiplication(Processor& processor, IntRegisterID dest_reg, phi::i32 lhs,
                               phi::i32 rhs)
    {
        phi::i64 res = phi::i64(lhs) * rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Multiplication(Processor& processor, IntRegisterID dest_reg, phi::u32 lhs,
                               phi::u32 rhs)
    {
        phi::u64 res = phi::u64(lhs) * rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Division(Processor& processor, IntRegisterID dest_reg, phi::i32 lhs, phi::i32 rhs)
    {
        if (rhs == 0)
        {
            processor.Raise(Exception::DivideByZero);
            return;
        }

        phi::i64 res = phi::i64(lhs) / rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Division(Processor& processor, IntRegisterID dest_reg, phi::u32 lhs, phi::u32 rhs)
    {
        if (rhs == 0u)
        {
            processor.Raise(Exception::DivideByZero);
            return;
        }

        phi::u64 res = phi::u64(lhs) / rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void ShiftRightLogical(Processor& processor, IntRegisterID dest_reg, phi::i32 base,
                                  phi::i32 shift)
    {
        // Prevent undefined behavior by shifting by more than 31
        if (shift > 31)
        {
            processor.Raise(Exception::BadShift);

            // Just set register to 0
            processor.IntRegisterSetSignedValue(dest_reg, 0);
            return;
        }

        // Do nothing when shifting by zero to prevent undefined behavior
        if (shift == 0)
        {
            return;
        }

        // Negative shifts are undefiend behaviour
        if (shift < 0)
        {
            processor.Raise(Exception::BadShift);
            return;
        }

        phi::i32 new_value = base.get() >> shift.get();

        new_value = clear_top_n_bits(new_value.get(), shift.get());

        processor.IntRegisterSetSignedValue(dest_reg, new_value);
    }

    static void ShiftRightArithmetic(Processor& processor, IntRegisterID dest_reg, phi::i32 base,
                                     phi::i32 shift)
    {
        // Prevent undefined behavior by shifting by more than 31
        if (shift > 31)
        {
            processor.Raise(Exception::BadShift);

            // Is negative ie. sign bit is set
            if (base < 0)
            {
                // Set every byte to 1
                processor.IntRegisterSetSignedValue(dest_reg, ~0);
            }
            else
            {
                // Set every byte to 0
                processor.IntRegisterSetSignedValue(dest_reg, 0);
            }
            return;
        }

        // Negative shifts are undefined behaviour
        if (shift < 0)
        {
            processor.Raise(Exception::BadShift);
            return;
        }

        phi::i32 new_value = base.get() >> shift.get();

        processor.IntRegisterSetSignedValue(dest_reg, new_value);
    }

    // Behavior is the same for logical and arithmetic shifts
    static void ShiftLeft(Processor& processor, IntRegisterID dest_reg, phi::i32 base,
                          phi::i32 shift)
    {
        if (shift > 31)
        {
            processor.Raise(Exception::BadShift);

            // Just set register to 0
            processor.IntRegisterSetSignedValue(dest_reg, 0);
            return;
        }

        // Negative shifts are undefined behaviour
        if (shift < 0)
        {
            processor.Raise(Exception::BadShift);
            return;
        }

        phi::i32 new_value = base.get() << shift.get();

        processor.IntRegisterSetSignedValue(dest_reg, new_value);
    }

    namespace impl
    {
        void ADD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            Addition(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void ADDI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            Addition(processor, dest_reg.register_id, src_value, imm_value.signed_value);
        }

        void ADDU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            Addition(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void ADDUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            Addition(processor, dest_reg.register_id, src_value, imm_value.unsigned_value);
        }

        void ADDF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::FloatRegister);

            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::f32 new_value = lhs_value + rhs_value;

            processor.FloatRegisterSetFloatValue(dest_reg, new_value);
        }

        void ADDD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::FloatRegister);

            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::f64 new_value = lhs_value + rhs_value;

            processor.FloatRegisterSetDoubleValue(dest_reg, new_value);
        }

        void SUB(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            Subtraction(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void SUBI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            Subtraction(processor, dest_reg.register_id, src_value, imm_value.signed_value);
        }

        void SUBU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            Subtraction(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void SUBUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            Subtraction(processor, dest_reg.register_id, src_value, imm_value.unsigned_value);
        }

        void SUBF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::FloatRegister);

            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::f32 new_value = lhs_value - rhs_value;

            processor.FloatRegisterSetFloatValue(dest_reg, new_value);
        }

        void SUBD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::FloatRegister);

            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::f64 new_value = lhs_value - rhs_value;

            processor.FloatRegisterSetDoubleValue(dest_reg, new_value);
        }

        void MULT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            Multiplication(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void MULTI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            Multiplication(processor, dest_reg.register_id, src_value, imm_value.signed_value);
        }

        void MULTU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            Multiplication(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void MULTUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                    const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            Multiplication(processor, dest_reg.register_id, src_value, imm_value.unsigned_value);
        }

        void MULTF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::FloatRegister);

            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::f32 new_value = lhs_value * rhs_value;

            processor.FloatRegisterSetFloatValue(dest_reg, new_value);
        }

        void MULTD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::FloatRegister);

            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::f64 new_value = lhs_value * rhs_value;

            processor.FloatRegisterSetDoubleValue(dest_reg, new_value);
        }

        void DIV(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            Division(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void DIVI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            Division(processor, dest_reg.register_id, src_value, imm_value.signed_value);
        }

        void DIVU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            Division(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void DIVUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            Division(processor, dest_reg.register_id, src_value, imm_value.unsigned_value);
        }

        void DIVF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::FloatRegister);

            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            if (rhs_value.get() == 0.0f)
            {
                processor.Raise(Exception::DivideByZero);
                return;
            }

            const phi::f32 new_value = lhs_value / rhs_value;

            processor.FloatRegisterSetFloatValue(dest_reg, new_value);
        }

        void DIVD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::FloatRegister);

            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            if (rhs_value.get() == 0.0)
            {
                processor.Raise(Exception::DivideByZero);
                return;
            }

            const phi::f64 new_value = lhs_value / rhs_value;

            processor.FloatRegisterSetDoubleValue(dest_reg, new_value);
        }

        void SLL(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            ShiftLeft(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void SLLI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value   = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 shift_value = imm_value.signed_value;

            ShiftLeft(processor, dest_reg.register_id, src_value, shift_value);
        }

        void SRL(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            ShiftRightLogical(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void SRLI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value   = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 shift_value = imm_value.signed_value;

            ShiftRightLogical(processor, dest_reg.register_id, src_value, shift_value);
        }

        void SLA(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            ShiftLeft(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void SLAI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value   = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 shift_value = imm_value.signed_value;

            ShiftLeft(processor, dest_reg.register_id, src_value, shift_value);
        }

        void SRA(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            ShiftRightArithmetic(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void SRAI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value   = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 shift_value = imm_value.signed_value;

            ShiftRightArithmetic(processor, dest_reg.register_id, src_value, shift_value);
        }

        void AND(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);
            phi::i32 new_value = lhs_value.get() & rhs_value.get();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void ANDI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 new_value = src_value.get() & imm_value.signed_value.get();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void OR(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);
            phi::i32 new_value = lhs_value.get() | rhs_value.get();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void ORI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 new_value = src_value.get() | imm_value.signed_value.get();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void XOR(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);
            phi::i32 new_value = lhs_value.get() ^ rhs_value.get();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void XORI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 new_value = src_value.get() ^ imm_value.signed_value.get();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SLT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            const phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            const phi::i32 new_value = (lhs_value < rhs_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SLTI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            const phi::i32 new_value = (src_value < imm_value.signed_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SLTU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            const phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            const phi::u32 new_value = (lhs_value < rhs_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void SLTUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            const phi::u32 new_value = (src_value < imm_value.unsigned_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void LTF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::Boolean new_value = (lhs_value < rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void LTD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::Boolean new_value = (lhs_value < rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void SGT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            const phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            const phi::i32 new_value = (lhs_value > rhs_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SGTI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            const phi::i32 new_value = (src_value > imm_value.signed_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SGTU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            const phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            const phi::u32 new_value = (lhs_value > rhs_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void SGTUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            const phi::u32 new_value = (src_value > imm_value.unsigned_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void GTF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::Boolean new_value = (lhs_value > rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void GTD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::Boolean new_value = (lhs_value > rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void SLE(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            const phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            const phi::i32 new_value = (lhs_value <= rhs_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SLEI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            const phi::i32 new_value = (src_value <= imm_value.signed_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SLEU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            const phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            const phi::u32 new_value = (lhs_value <= rhs_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void SLEUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            const phi::u32 new_value = (src_value <= imm_value.unsigned_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void LEF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::Boolean new_value = (lhs_value <= rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void LED(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::Boolean new_value = (lhs_value <= rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void SGE(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            const phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            const phi::i32 new_value = (lhs_value >= rhs_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SGEI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            const phi::i32 new_value = (src_value >= imm_value.signed_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SGEU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            const phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            const phi::u32 new_value = (lhs_value >= rhs_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void SGEUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            const phi::u32 new_value = (src_value >= imm_value.unsigned_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void GEF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::Boolean new_value = (lhs_value >= rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void GED(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::Boolean new_value = (lhs_value >= rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void SEQ(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            const phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            const phi::i32 new_value = (lhs_value == rhs_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SEQI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            const phi::i32 new_value = (src_value == imm_value.signed_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SEQU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            const phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            const phi::u32 new_value = (lhs_value == rhs_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void SEQUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            const phi::u32 new_value = (src_value == imm_value.unsigned_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void EQF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::Boolean new_value = (lhs_value.get() == rhs_value.get());

            processor.SetFPSRValue(new_value);
        }

        void EQD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::Boolean new_value = (lhs_value.get() == rhs_value.get());

            processor.SetFPSRValue(new_value);
        }

        void SNE(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            const phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            const phi::i32 new_value = (lhs_value != rhs_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SNEI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            const phi::i32 new_value = (src_value != imm_value.signed_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SNEU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::IntRegister);

            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            const phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            const phi::u32 new_value = (lhs_value != rhs_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void SNEUI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                   const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::ImmediateInteger);

            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            const phi::u32 new_value = (src_value != imm_value.unsigned_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void NEF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::Boolean new_value = (lhs_value.get() != rhs_value.get());

            processor.SetFPSRValue(new_value);
        }

        void NED(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::Boolean new_value = (lhs_value.get() != rhs_value.get());

            processor.SetFPSRValue(new_value);
        }

        void BEQZ(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::Label);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& test_reg   = arg1.AsRegisterInt();
            const auto& jump_label = arg2.AsLabel();

            phi::i32 test_value = processor.IntRegisterGetSignedValue(test_reg.register_id);

            if (test_value == 0)
            {
                JumpToLabel(processor, jump_label.label_name);
            }
        }

        void BNEZ(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::Label);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& test_reg   = arg1.AsRegisterInt();
            const auto& jump_label = arg2.AsLabel();

            phi::i32 test_value = processor.IntRegisterGetSignedValue(test_reg.register_id);

            if (test_value != 0)
            {
                JumpToLabel(processor, jump_label.label_name);
            }
        }

        void BFPT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::Label);
            PHI_ASSERT(arg2.GetType() == ArgumentType::None);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& jump_label = arg1.AsLabel();

            phi::Boolean test_value = processor.GetFPSRValue();

            if (test_value)
            {
                JumpToLabel(processor, jump_label.label_name);
            }
        }

        void BFPF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::Label);
            PHI_ASSERT(arg2.GetType() == ArgumentType::None);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& jump_label = arg1.AsLabel();

            phi::Boolean test_value = processor.GetFPSRValue();

            if (!test_value)
            {
                JumpToLabel(processor, jump_label.label_name);
            }
        }

        void J(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
               const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::Label);
            PHI_ASSERT(arg2.GetType() == ArgumentType::None);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& jump_label = arg1.AsLabel();

            JumpToLabel(processor, jump_label.label_name);
        }

        void JR(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::None);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& jump_register = arg1.AsRegisterInt();

            JumpToRegister(processor, jump_register.register_id);
        }

        void JAL(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::Label);
            PHI_ASSERT(arg2.GetType() == ArgumentType::None);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& jump_label = arg1.AsLabel();

            processor.IntRegisterSetUnsignedValue(
                    IntRegisterID::R31,
                    static_cast<std::uint32_t>(processor.m_NextProgramCounter.get()));

            JumpToLabel(processor, jump_label.label_name);
        }

        void JALR(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::None);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& jump_register = arg1.AsRegisterInt();

            processor.IntRegisterSetUnsignedValue(
                    IntRegisterID::R31,
                    static_cast<std::uint32_t>(processor.m_NextProgramCounter.get()));

            JumpToRegister(processor, jump_register.register_id);
        }

        void LHI(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const IntRegisterID dest_reg  = arg1.AsRegisterInt().register_id;
            std::int32_t        imm_value = arg2.AsImmediateValue().signed_value.get();

            imm_value = (imm_value << 16) & 0xFFFF0000;

            processor.IntRegisterSetSignedValue(dest_reg, imm_value);
        }

        void LB(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::AddressDisplacement ||
                       arg2.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& dest_reg = arg1.AsRegisterInt();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value =
                    processor.m_MemoryBlock.LoadByte(static_cast<std::size_t>(address.get()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to load byte at address {}", address.get());
                return;
            }

            phi::i32 value = optional_value.value();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, value);
        }

        void LBU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::AddressDisplacement ||
                       arg2.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& dest_reg = arg1.AsRegisterInt();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value = processor.m_MemoryBlock.LoadUnsignedByte(
                    static_cast<std::size_t>(address.get()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to load unsigned byte at address {}", address.get());
                return;
            }

            phi::i32 value = optional_value.value();

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, optional_value.value());
        }

        void LH(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::AddressDisplacement ||
                       arg2.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& dest_reg = arg1.AsRegisterInt();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value =
                    processor.m_MemoryBlock.LoadHalfWord(static_cast<std::size_t>(address.get()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to load half byte at address {}", address.get());
                return;
            }

            phi::i32 value = optional_value.value();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, optional_value.value());
        }

        void LHU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::AddressDisplacement ||
                       arg2.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& dest_reg = arg1.AsRegisterInt();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value = processor.m_MemoryBlock.LoadUnsignedHalfWord(
                    static_cast<std::size_t>(address.get()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to load unsigned half byte at address {}", address.get());
                return;
            }

            phi::i32 value = optional_value.value();

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, optional_value.value());
        }

        void LW(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::AddressDisplacement ||
                       arg2.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& dest_reg = arg1.AsRegisterInt();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value =
                    processor.m_MemoryBlock.LoadWord(static_cast<std::size_t>(address.get()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to load word at address {}", address.get());
                return;
            }

            processor.IntRegisterSetSignedValue(dest_reg.register_id, optional_value.value());
        }

        void LWU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::AddressDisplacement ||
                       arg2.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& dest_reg = arg1.AsRegisterInt();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value = processor.m_MemoryBlock.LoadUnsignedWord(
                    static_cast<std::size_t>(address.get()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to load unsigned word at address {}", address.get());
                return;
            }

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, optional_value.value());
        }

        void LF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::AddressDisplacement ||
                       arg2.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& dest_reg = arg1.AsRegisterFloat();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value =
                    processor.m_MemoryBlock.LoadFloat(static_cast<std::size_t>(address.get()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to load float at address {}", address.get());
                return;
            }

            processor.FloatRegisterSetFloatValue(dest_reg.register_id, optional_value.value());
        }

        void LD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::AddressDisplacement ||
                       arg2.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const auto& dest_reg = arg1.AsRegisterFloat();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value =
                    processor.m_MemoryBlock.LoadDouble(static_cast<std::size_t>(address.get()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to load double at address {}", address.get());
                return;
            }

            processor.FloatRegisterSetDoubleValue(dest_reg.register_id, optional_value.value());
        }

        void SB(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::AddressDisplacement ||
                       arg1.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterInt();

            phi::i32 value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            phi::Boolean success = processor.m_MemoryBlock.StoreByte(
                    static_cast<std::size_t>(address.get()), static_cast<std::int8_t>(value.get()));

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to store byte at address {}", address.get());
            }
        }

        void SBU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::AddressDisplacement ||
                       arg1.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterInt();

            phi::u32 value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            phi::Boolean success = processor.m_MemoryBlock.StoreUnsignedByte(
                    static_cast<std::size_t>(address.get()),
                    static_cast<std::uint8_t>(value.get()));

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to store unsigned byte at address {}", address.get());
            }
        }

        void SH(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::AddressDisplacement ||
                       arg1.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterInt();

            phi::i32 value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            phi::Boolean success =
                    processor.m_MemoryBlock.StoreHalfWord(static_cast<std::size_t>(address.get()),
                                                          static_cast<std::int16_t>(value.get()));

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to store half word at address {}", address.get());
            }
        }

        void SHU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::AddressDisplacement ||
                       arg1.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterInt();

            phi::u32 value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            phi::Boolean success = processor.m_MemoryBlock.StoreUnsignedHalfWord(
                    static_cast<std::size_t>(address.get()),
                    static_cast<std::uint16_t>(value.get()));

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to store unsigned half word at address {}", address.get());
            }
        }

        void SW(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::AddressDisplacement ||
                       arg1.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterInt();

            phi::i32 value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            phi::Boolean success = processor.m_MemoryBlock.StoreWord(
                    static_cast<std::size_t>(address.get()), value);

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to store word at address {}", address.get());
            }
        }

        void SWU(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::AddressDisplacement ||
                       arg1.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterInt();

            phi::u32 value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            phi::Boolean success = processor.m_MemoryBlock.StoreUnsignedWord(
                    static_cast<std::size_t>(address.get()), value);

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to store unsigned word at address {}", address.get());
            }
        }

        void SF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::AddressDisplacement ||
                       arg1.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterFloat();

            phi::f32 value = processor.FloatRegisterGetFloatValue(src_reg.register_id);

            phi::Boolean success = processor.m_MemoryBlock.StoreFloat(
                    static_cast<std::size_t>(address.get()), value);

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to store float at address {}", address.get());
            }
        }

        void SD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::AddressDisplacement ||
                       arg1.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterFloat();

            phi::f64 value = processor.FloatRegisterGetDoubleValue(src_reg.register_id);

            phi::Boolean success = processor.m_MemoryBlock.StoreDouble(
                    static_cast<std::size_t>(address.get()), value);

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                PHI_LOG_ERROR("Failed to store float at address {}", address.get());
            }
        }

        void MOVF(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID dest_reg   = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID source_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 source_value = processor.FloatRegisterGetFloatValue(source_reg);

            processor.FloatRegisterSetFloatValue(dest_reg, source_value);
        }

        void MOVD(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID dest_reg   = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID source_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 source_value = processor.FloatRegisterGetDoubleValue(source_reg);

            processor.FloatRegisterSetDoubleValue(dest_reg, source_value);
        }

        void MOVFP2I(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                     const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const IntRegisterID   dest_reg   = arg1.AsRegisterInt().register_id;
            const FloatRegisterID source_reg = arg2.AsRegisterFloat().register_id;

            const float source_value = processor.FloatRegisterGetFloatValue(source_reg).get();

            const std::uint32_t moved_value =
                    *reinterpret_cast<const std::uint32_t*>(&source_value);

            processor.IntRegisterSetUnsignedValue(dest_reg, moved_value);
        }

        void MOVI2FP(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                     const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::IntRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID dest_reg   = arg1.AsRegisterFloat().register_id;
            const IntRegisterID   source_reg = arg2.AsRegisterInt().register_id;

            const std::uint32_t source_value =
                    processor.IntRegisterGetUnsignedValue(source_reg).get();

            const float moved_value = *reinterpret_cast<const float*>(&source_value);

            processor.FloatRegisterSetFloatValue(dest_reg, moved_value);
        }

        void CVTF2D(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                    const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID dest_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID src_reg  = arg2.AsRegisterFloat().register_id;

            const phi::f32 src_value = processor.FloatRegisterGetFloatValue(src_reg);

            processor.FloatRegisterSetDoubleValue(dest_reg, src_value);
        }

        void CVTF2I(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                    const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID dest_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID src_reg  = arg2.AsRegisterFloat().register_id;

            const float        src_value = processor.FloatRegisterGetFloatValue(src_reg).get();
            const std::int32_t converted_value_int = static_cast<std::int32_t>(src_value);
            const float        converted_value_float =
                    *reinterpret_cast<const float*>(&converted_value_int);

            processor.FloatRegisterSetFloatValue(dest_reg, converted_value_float);
        }

        void CVTD2F(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                    const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID dest_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID src_reg  = arg2.AsRegisterFloat().register_id;

            const double src_value       = processor.FloatRegisterGetDoubleValue(src_reg).get();
            const float  converted_value = static_cast<float>(src_value);

            processor.FloatRegisterSetFloatValue(dest_reg, converted_value);
        }

        void CVTD2I(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                    const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID dest_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID src_reg  = arg2.AsRegisterFloat().register_id;

            const double       src_value = processor.FloatRegisterGetDoubleValue(src_reg).get();
            const std::int32_t converted_value_int = static_cast<std::int32_t>(src_value);
            const float        converted_value_float =
                    *reinterpret_cast<const float*>(&converted_value_int);

            processor.FloatRegisterSetFloatValue(dest_reg, converted_value_float);
        }

        void CVTI2F(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                    const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID dest_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID src_reg  = arg2.AsRegisterFloat().register_id;

            const float        src_value = processor.FloatRegisterGetFloatValue(src_reg).get();
            const std::int32_t converted_value_int =
                    *reinterpret_cast<const std::int32_t*>(&src_value);
            const float converted_value_float = static_cast<float>(converted_value_int);

            processor.FloatRegisterSetFloatValue(dest_reg, converted_value_float);
        }

        void CVTI2D(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                    const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg2.GetType() == ArgumentType::FloatRegister);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            const FloatRegisterID dest_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID src_reg  = arg2.AsRegisterFloat().register_id;

            const float        src_value = processor.FloatRegisterGetFloatValue(src_reg).get();
            const std::int32_t converted_value_int =
                    *reinterpret_cast<const std::int32_t*>(&src_value);
            const double converted_value_double = static_cast<double>(converted_value_int);

            processor.FloatRegisterSetDoubleValue(dest_reg, converted_value_double);
        }

        void TRAP(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::ImmediateInteger);
            PHI_ASSERT(arg2.GetType() == ArgumentType::None);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            processor.Raise(Exception::Trap);
        }

        void HALT(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                  const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::None);
            PHI_ASSERT(arg2.GetType() == ArgumentType::None);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            processor.Raise(Exception::Halt);
        }

        void NOP(Processor& processor, const InstructionArg& arg1, const InstructionArg& arg2,
                 const InstructionArg& arg3)
        {
            PHI_ASSERT(arg1.GetType() == ArgumentType::None);
            PHI_ASSERT(arg2.GetType() == ArgumentType::None);
            PHI_ASSERT(arg3.GetType() == ArgumentType::None);

            /* Do nothing */
        }
    } // namespace impl
} // namespace dlx

#include "DLX/InstructionImplementation.hpp"

#include "DLX/InstructionArgument.hpp"
#include "DLX/InstructionInfo.hpp"
#include "DLX/Logger.hpp"
#include "DLX/Parser.hpp"
#include "DLX/Processor.hpp"
#include "DLX/RegisterNames.hpp"
#include <phi/compiler_support/warning.hpp>
#include <phi/core/assert.hpp>
#include <phi/core/boolean.hpp>
#include <phi/core/types.hpp>

// TODO: Fix warnings
PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wunused-parameter")
PHI_CLANG_AND_GCC_SUPPRESS_WARNING("-Wfloat-equal")
PHI_CLANG_SUPPRESS_WARNING("-Wundefined-reinterpret-cast")
PHI_GCC_SUPPRESS_WARNING("-Wstrict-aliasing")
PHI_GCC_SUPPRESS_WARNING("-Wsign-conversion")
PHI_MSVC_SUPPRESS_WARNING(4100)

namespace dlx
{
    PHI_GCC_SUPPRESS_WARNING_PUSH()
    PHI_GCC_SUPPRESS_WARNING("-Wshift-negative-value")

    static phi::int32_t clear_top_n_bits(phi::int32_t value, phi::int32_t n) noexcept
    {
        PHI_ASSERT(n > 0 && n < 32, "Would invoke undefined behavior");

        return value & ~(-1 << (32 - n));
    }

    PHI_GCC_SUPPRESS_WARNING_POP()

    static void JumpToLabel(Processor& processor, phi::string_view label_name) noexcept
    {
        // Lookup the label
        const phi::observer_ptr<ParsedProgram> program = processor.GetCurrentProgram();
        PHI_ASSERT(program != nullptr);
        PHI_ASSERT(!label_name.is_empty(), "Can't jump to empty label");

        if (program->m_JumpData.find(label_name) == program->m_JumpData.end())
        {
            DLX_ERROR("Unable to find jump label {}", label_name);
            processor.Raise(Exception::UnknownLabel);
            return;
        }

        const phi::uint32_t jump_point = program->m_JumpData.at(label_name);
        PHI_ASSERT(jump_point < program->m_Instructions.size(), "Jump point out of bounds");

        // Set program counter
        processor.SetNextProgramCounter(jump_point);
    }

    static void JumpToRegister(Processor& processor, IntRegisterID reg_id) noexcept
    {
        phi::u32 address = processor.IntRegisterGetUnsignedValue(reg_id);

        phi::u32 max_address =
                static_cast<phi::uint32_t>(processor.GetCurrentProgram()->m_Instructions.size());
        if (address >= max_address)
        {
            processor.Raise(Exception::AddressOutOfBounds);
            return;
        }

        processor.SetNextProgramCounter(address.unsafe());
    }

    PHI_GCC_SUPPRESS_WARNING_WITH_PUSH("-Wmaybe-uninitialized")

    static phi::optional<phi::i32> CalculateDisplacementAddress(
            Processor&                                      processor,
            const InstructionArgument::AddressDisplacement& adr_displacement) noexcept
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

    PHI_GCC_SUPPRESS_WARNING_POP()

    PHI_GCC_SUPPRESS_WARNING_PUSH()
    PHI_GCC_SUPPRESS_WARNING("-Wreturn-type")
    PHI_MSVC_SUPPRESS_WARNING_WITH_PUSH(4702) // Unreachable code

    static phi::optional<phi::i32> GetLoadStoreAddress(Processor&                processor,
                                                       const InstructionArgument argument) noexcept
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

        PHI_ASSERT(argument.GetType() == ArgumentType::AddressDisplacement);

        const auto& adr_displacement = argument.AsAddressDisplacement();
        return CalculateDisplacementAddress(processor, adr_displacement);
    }

    PHI_MSVC_SUPPRESS_WARNING_POP()
    PHI_GCC_SUPPRESS_WARNING_POP()

    static void SafeWriteInteger(Processor& processor, IntRegisterID dest_reg,
                                 phi::i64 value) noexcept
    {
        static const constexpr phi::i64 min = phi::i32::limits_type::min();
        static const constexpr phi::i64 max = phi::i32::limits_type::max();

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

        processor.IntRegisterSetSignedValue(dest_reg, static_cast<phi::int32_t>(value.unsafe()));
    }

    static void SafeWriteInteger(Processor& processor, IntRegisterID dest_reg,
                                 phi::u64 value) noexcept
    {
        static constexpr const phi::u64 max = phi::u32::limits_type::max();

        // Check for overflow
        if (value > max)
        {
            processor.Raise(Exception::Overflow);

            value %= max + 1u;
        }

        PHI_ASSERT(value <= max);

        processor.IntRegisterSetUnsignedValue(dest_reg, static_cast<phi::uint32_t>(value.unsafe()));
    }

    static void Addition(Processor& processor, IntRegisterID dest_reg, phi::i32 lhs,
                         phi::i32 rhs) noexcept
    {
        phi::i64 res = phi::i64(lhs) + rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Addition(Processor& processor, IntRegisterID dest_reg, phi::u32 lhs,
                         phi::u32 rhs) noexcept
    {
        phi::u64 res = phi::u64(lhs) + rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Subtraction(Processor& processor, IntRegisterID dest_reg, phi::i32 lhs,
                            phi::i32 rhs) noexcept
    {
        phi::i64 res = phi::i64(lhs) - rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Subtraction(Processor& processor, IntRegisterID dest_reg, phi::u32 lhs,
                            phi::u32 rhs) noexcept
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
                               phi::i32 rhs) noexcept
    {
        phi::i64 res = phi::i64(lhs) * rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Multiplication(Processor& processor, IntRegisterID dest_reg, phi::u32 lhs,
                               phi::u32 rhs) noexcept
    {
        phi::u64 res = phi::u64(lhs) * rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Division(Processor& processor, IntRegisterID dest_reg, phi::i32 lhs,
                         phi::i32 rhs) noexcept
    {
        if (rhs == 0)
        {
            processor.Raise(Exception::DivideByZero);
            return;
        }

        phi::i64 res = phi::i64(lhs) / rhs;

        SafeWriteInteger(processor, dest_reg, res);
    }

    static void Division(Processor& processor, IntRegisterID dest_reg, phi::u32 lhs,
                         phi::u32 rhs) noexcept
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
                                  phi::i32 shift) noexcept
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
            processor.IntRegisterSetSignedValue(dest_reg, base);
            return;
        }

        // Negative shifts are undefiend behavior
        if (shift < 0)
        {
            processor.Raise(Exception::BadShift);
            return;
        }

        phi::i32 new_value = base.unsafe() >> shift.unsafe();

        new_value = clear_top_n_bits(new_value.unsafe(), shift.unsafe());

        processor.IntRegisterSetSignedValue(dest_reg, new_value);
    }

    static void ShiftRightArithmetic(Processor& processor, IntRegisterID dest_reg, phi::i32 base,
                                     phi::i32 shift) noexcept
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

        // Negative shifts are undefined behavior
        if (shift < 0)
        {
            processor.Raise(Exception::BadShift);
            return;
        }

        phi::i32 new_value = base.unsafe() >> shift.unsafe();

        processor.IntRegisterSetSignedValue(dest_reg, new_value);
    }

    // Behavior is the same for logical and arithmetic shifts
    static void ShiftLeft(Processor& processor, IntRegisterID dest_reg, phi::i32 base,
                          phi::i32 shift) noexcept
    {
        if (shift > 31)
        {
            processor.Raise(Exception::BadShift);

            // Just set register to 0
            processor.IntRegisterSetSignedValue(dest_reg, 0);
            return;
        }

        // Negative shifts are undefined behavior
        if (shift < 0)
        {
            processor.Raise(Exception::BadShift);
            return;
        }

        phi::i32 new_value = base.unsafe() << shift.unsafe();

        processor.IntRegisterSetSignedValue(dest_reg, new_value);
    }

    namespace impl
    {
        void ADD(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            Addition(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void ADDI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            Addition(processor, dest_reg.register_id, src_value, imm_value.signed_value);
        }

        void ADDU(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            Addition(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void ADDUI(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            Addition(processor, dest_reg.register_id, src_value, imm_value.unsigned_value);
        }

        void ADDF(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::f32 new_value = lhs_value + rhs_value;

            processor.FloatRegisterSetFloatValue(dest_reg, new_value);
        }

        void ADDD(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::f64 new_value = lhs_value + rhs_value;

            processor.FloatRegisterSetDoubleValue(dest_reg, new_value);
        }

        void SUB(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            Subtraction(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void SUBI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            Subtraction(processor, dest_reg.register_id, src_value, imm_value.signed_value);
        }

        void SUBU(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            Subtraction(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void SUBUI(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            Subtraction(processor, dest_reg.register_id, src_value, imm_value.unsigned_value);
        }

        void SUBF(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::f32 new_value = lhs_value - rhs_value;

            processor.FloatRegisterSetFloatValue(dest_reg, new_value);
        }

        void SUBD(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::f64 new_value = lhs_value - rhs_value;

            processor.FloatRegisterSetDoubleValue(dest_reg, new_value);
        }

        void MULT(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            Multiplication(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void MULTI(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            Multiplication(processor, dest_reg.register_id, src_value, imm_value.signed_value);
        }

        void MULTU(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            Multiplication(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void MULTUI(Processor& processor, const InstructionArgument& arg1,
                    const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            Multiplication(processor, dest_reg.register_id, src_value, imm_value.unsigned_value);
        }

        void MULTF(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::f32 new_value = lhs_value * rhs_value;

            processor.FloatRegisterSetFloatValue(dest_reg, new_value);
        }

        void MULTD(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::f64 new_value = lhs_value * rhs_value;

            processor.FloatRegisterSetDoubleValue(dest_reg, new_value);
        }

        void DIV(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            Division(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void DIVI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            Division(processor, dest_reg.register_id, src_value, imm_value.signed_value);
        }

        void DIVU(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            Division(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void DIVUI(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            Division(processor, dest_reg.register_id, src_value, imm_value.unsigned_value);
        }

        void DIVF(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            if (rhs_value.unsafe() == 0.0f)
            {
                processor.Raise(Exception::DivideByZero);
                return;
            }

            const phi::f32 new_value = lhs_value / rhs_value;

            processor.FloatRegisterSetFloatValue(dest_reg, new_value);
        }

        void DIVD(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterFloat().register_id;
            const auto& lhs_reg  = arg2.AsRegisterFloat().register_id;
            const auto& rhs_reg  = arg3.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            if (rhs_value.unsafe() == 0.0)
            {
                processor.Raise(Exception::DivideByZero);
                return;
            }

            const phi::f64 new_value = lhs_value / rhs_value;

            processor.FloatRegisterSetDoubleValue(dest_reg, new_value);
        }

        void SLL(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            ShiftLeft(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void SLLI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value   = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 shift_value = imm_value.signed_value;

            ShiftLeft(processor, dest_reg.register_id, src_value, shift_value);
        }

        void SRL(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            ShiftRightLogical(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void SRLI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value   = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 shift_value = imm_value.signed_value;

            ShiftRightLogical(processor, dest_reg.register_id, src_value, shift_value);
        }

        void SLA(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            ShiftLeft(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void SLAI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value   = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 shift_value = imm_value.signed_value;

            ShiftLeft(processor, dest_reg.register_id, src_value, shift_value);
        }

        void SRA(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            ShiftRightArithmetic(processor, dest_reg.register_id, lhs_value, rhs_value);
        }

        void SRAI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value   = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 shift_value = imm_value.signed_value;

            ShiftRightArithmetic(processor, dest_reg.register_id, src_value, shift_value);
        }

        void AND(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);
            phi::i32 new_value = lhs_value.unsafe() & rhs_value.unsafe();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void ANDI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 new_value = src_value.unsafe() & imm_value.signed_value.unsafe();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void OR(Processor& processor, const InstructionArgument& arg1,
                const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);
            phi::i32 new_value = lhs_value.unsafe() | rhs_value.unsafe();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void ORI(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 new_value = src_value.unsafe() | imm_value.signed_value.unsafe();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void XOR(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);
            phi::i32 new_value = lhs_value.unsafe() ^ rhs_value.unsafe();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void XORI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);
            phi::i32 new_value = src_value.unsafe() ^ imm_value.signed_value.unsafe();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SLT(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            const phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            const phi::i32 new_value = (lhs_value < rhs_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SLTI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            const phi::i32 new_value = (src_value < imm_value.signed_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SLTU(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            const phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            const phi::u32 new_value = (lhs_value < rhs_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void SLTUI(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            const phi::u32 new_value = (src_value < imm_value.unsigned_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void LTF(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::boolean new_value = (lhs_value < rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void LTD(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::boolean new_value = (lhs_value < rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void SGT(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            const phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            const phi::i32 new_value = (lhs_value > rhs_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SGTI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            const phi::i32 new_value = (src_value > imm_value.signed_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SGTU(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            const phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            const phi::u32 new_value = (lhs_value > rhs_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void SGTUI(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            const phi::u32 new_value = (src_value > imm_value.unsigned_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void GTF(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::boolean new_value = (lhs_value > rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void GTD(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::boolean new_value = (lhs_value > rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void SLE(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            const phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            const phi::i32 new_value = (lhs_value <= rhs_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SLEI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            const phi::i32 new_value = (src_value <= imm_value.signed_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SLEU(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            const phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            const phi::u32 new_value = (lhs_value <= rhs_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void SLEUI(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            const phi::u32 new_value = (src_value <= imm_value.unsigned_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void LEF(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::boolean new_value = (lhs_value <= rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void LED(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::boolean new_value = (lhs_value <= rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void SGE(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            const phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            const phi::i32 new_value = (lhs_value >= rhs_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SGEI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            const phi::i32 new_value = (src_value >= imm_value.signed_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SGEU(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            const phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            const phi::u32 new_value = (lhs_value >= rhs_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void SGEUI(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            const phi::u32 new_value = (src_value >= imm_value.unsigned_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void GEF(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::boolean new_value = (lhs_value >= rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void GED(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::boolean new_value = (lhs_value >= rhs_value);

            processor.SetFPSRValue(new_value);
        }

        void SEQ(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            const phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            const phi::i32 new_value = (lhs_value == rhs_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SEQI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            const phi::i32 new_value = (src_value == imm_value.signed_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SEQU(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            const phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            const phi::u32 new_value = (lhs_value == rhs_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void SEQUI(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            const phi::u32 new_value = (src_value == imm_value.unsigned_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void EQF(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::boolean new_value = (lhs_value.unsafe() == rhs_value.unsafe());

            processor.SetFPSRValue(new_value);
        }

        void EQD(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::boolean new_value = (lhs_value.unsafe() == rhs_value.unsafe());

            processor.SetFPSRValue(new_value);
        }

        void SNE(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::i32 lhs_value = processor.IntRegisterGetSignedValue(lhs_reg.register_id);
            const phi::i32 rhs_value = processor.IntRegisterGetSignedValue(rhs_reg.register_id);

            const phi::i32 new_value = (lhs_value != rhs_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SNEI(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::i32 src_value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            const phi::i32 new_value = (src_value != imm_value.signed_value ? 1 : 0);

            processor.IntRegisterSetSignedValue(dest_reg.register_id, new_value);
        }

        void SNEU(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();
            const auto& lhs_reg  = arg2.AsRegisterInt();
            const auto& rhs_reg  = arg3.AsRegisterInt();

            const phi::u32 lhs_value = processor.IntRegisterGetUnsignedValue(lhs_reg.register_id);
            const phi::u32 rhs_value = processor.IntRegisterGetUnsignedValue(rhs_reg.register_id);

            const phi::u32 new_value = (lhs_value != rhs_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void SNEUI(Processor& processor, const InstructionArgument& arg1,
                   const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg  = arg1.AsRegisterInt();
            const auto& src_reg   = arg2.AsRegisterInt();
            const auto& imm_value = arg3.AsImmediateValue();

            const phi::u32 src_value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            const phi::u32 new_value = (src_value != imm_value.unsigned_value ? 1u : 0u);

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, new_value);
        }

        void NEF(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 lhs_value = processor.FloatRegisterGetFloatValue(lhs_reg);
            const phi::f32 rhs_value = processor.FloatRegisterGetFloatValue(rhs_reg);

            const phi::boolean new_value = (lhs_value.unsafe() != rhs_value.unsafe());

            processor.SetFPSRValue(new_value);
        }

        void NED(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID lhs_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID rhs_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 lhs_value = processor.FloatRegisterGetDoubleValue(lhs_reg);
            const phi::f64 rhs_value = processor.FloatRegisterGetDoubleValue(rhs_reg);

            const phi::boolean new_value = (lhs_value.unsafe() != rhs_value.unsafe());

            processor.SetFPSRValue(new_value);
        }

        void BEQZ(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& test_reg   = arg1.AsRegisterInt();
            const auto& jump_label = arg2.AsLabel();

            phi::i32 test_value = processor.IntRegisterGetSignedValue(test_reg.register_id);

            if (test_value == 0)
            {
                JumpToLabel(processor, jump_label.label_name);
            }
        }

        void BNEZ(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& test_reg   = arg1.AsRegisterInt();
            const auto& jump_label = arg2.AsLabel();

            phi::i32 test_value = processor.IntRegisterGetSignedValue(test_reg.register_id);

            if (test_value != 0)
            {
                JumpToLabel(processor, jump_label.label_name);
            }
        }

        void BFPT(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& jump_label = arg1.AsLabel();

            phi::boolean test_value = processor.GetFPSRValue();

            if (test_value)
            {
                JumpToLabel(processor, jump_label.label_name);
            }
        }

        void BFPF(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& jump_label = arg1.AsLabel();

            phi::boolean test_value = processor.GetFPSRValue();

            if (!test_value)
            {
                JumpToLabel(processor, jump_label.label_name);
            }
        }

        void J(Processor& processor, const InstructionArgument& arg1,
               const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& jump_label = arg1.AsLabel();

            JumpToLabel(processor, jump_label.label_name);
        }

        void JR(Processor& processor, const InstructionArgument& arg1,
                const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& jump_register = arg1.AsRegisterInt();

            JumpToRegister(processor, jump_register.register_id);
        }

        void JAL(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& jump_label = arg1.AsLabel();

            processor.IntRegisterSetUnsignedValue(IntRegisterID::R31,
                                                  processor.GetNextProgramCounter());

            JumpToLabel(processor, jump_label.label_name);
        }

        void JALR(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& jump_register = arg1.AsRegisterInt();

            processor.IntRegisterSetUnsignedValue(IntRegisterID::R31,
                                                  processor.GetNextProgramCounter());

            JumpToRegister(processor, jump_register.register_id);
        }

        void LHI(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const IntRegisterID dest_reg  = arg1.AsRegisterInt().register_id;
            phi::int32_t        imm_value = arg2.AsImmediateValue().signed_value.unsafe();

            imm_value = static_cast<phi::int32_t>((imm_value << 16) & 0xFFFF0000);

            processor.IntRegisterSetSignedValue(dest_reg, imm_value);
        }

        void LB(Processor& processor, const InstructionArgument& arg1,
                const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value =
                    processor.GetMemory().LoadByte(static_cast<phi::size_t>(address.unsafe()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to load byte at address {}", address.unsafe());
                return;
            }

            phi::i32 value = optional_value.value();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, value);
        }

        void LBU(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value = processor.GetMemory().LoadUnsignedByte(
                    static_cast<phi::size_t>(address.unsafe()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to load unsigned byte at address {}", address.unsafe());
                return;
            }

            phi::u32 value = optional_value.value();

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, value);
        }

        void LH(Processor& processor, const InstructionArgument& arg1,
                const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value =
                    processor.GetMemory().LoadHalfWord(static_cast<phi::size_t>(address.unsafe()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to load half byte at address {}", address.unsafe());
                return;
            }

            phi::i32 value = optional_value.value();

            processor.IntRegisterSetSignedValue(dest_reg.register_id, value);
        }

        void LHU(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value = processor.GetMemory().LoadUnsignedHalfWord(
                    static_cast<phi::size_t>(address.unsafe()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to load unsigned half byte at address {}", address.unsafe());
                return;
            }

            phi::u32 value = optional_value.value();

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, value);
        }

        void LW(Processor& processor, const InstructionArgument& arg1,
                const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value =
                    processor.GetMemory().LoadWord(static_cast<phi::size_t>(address.unsafe()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to load word at address {}", address.unsafe());
                return;
            }

            processor.IntRegisterSetSignedValue(dest_reg.register_id, optional_value.value());
        }

        void LWU(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterInt();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value = processor.GetMemory().LoadUnsignedWord(
                    static_cast<phi::size_t>(address.unsafe()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to load unsigned word at address {}", address.unsafe());
                return;
            }

            processor.IntRegisterSetUnsignedValue(dest_reg.register_id, optional_value.value());
        }

        void LF(Processor& processor, const InstructionArgument& arg1,
                const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterFloat();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value =
                    processor.GetMemory().LoadFloat(static_cast<phi::size_t>(address.unsafe()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to load float at address {}", address.unsafe());
                return;
            }

            processor.FloatRegisterSetFloatValue(dest_reg.register_id, optional_value.value());
        }

        void LD(Processor& processor, const InstructionArgument& arg1,
                const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const auto& dest_reg = arg1.AsRegisterFloat();

            auto optional_address = GetLoadStoreAddress(processor, arg2);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            auto optional_value =
                    processor.GetMemory().LoadDouble(static_cast<phi::size_t>(address.unsafe()));

            if (!optional_value.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to load double at address {}", address.unsafe());
                return;
            }

            processor.FloatRegisterSetDoubleValue(dest_reg.register_id, optional_value.value());
        }

        void SB(Processor& processor, const InstructionArgument& arg1,
                const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterInt();

            phi::i32 value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            phi::boolean success =
                    processor.GetMemory().StoreByte(static_cast<phi::size_t>(address.unsafe()),
                                                    static_cast<phi::int8_t>(value.unsafe()));

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to store byte at address {}", address.unsafe());
            }
        }

        void SBU(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterInt();

            phi::u32 value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            phi::boolean success = processor.GetMemory().StoreUnsignedByte(
                    static_cast<phi::size_t>(address.unsafe()),
                    static_cast<phi::uint8_t>(value.unsafe()));

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to store unsigned byte at address {}", address.unsafe());
            }
        }

        void SH(Processor& processor, const InstructionArgument& arg1,
                const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterInt();

            phi::i32 value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            phi::boolean success =
                    processor.GetMemory().StoreHalfWord(static_cast<phi::size_t>(address.unsafe()),
                                                        static_cast<phi::int16_t>(value.unsafe()));

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to store half word at address {}", address.unsafe());
            }
        }

        void SHU(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterInt();

            phi::u32 value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            phi::boolean success = processor.GetMemory().StoreUnsignedHalfWord(
                    static_cast<phi::size_t>(address.unsafe()),
                    static_cast<phi::uint16_t>(value.unsafe()));

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to store unsigned half word at address {}", address.unsafe());
            }
        }

        void SW(Processor& processor, const InstructionArgument& arg1,
                const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterInt();

            phi::i32 value = processor.IntRegisterGetSignedValue(src_reg.register_id);

            phi::boolean success = processor.GetMemory().StoreWord(
                    static_cast<phi::size_t>(address.unsafe()), value);

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to store word at address {}", address.unsafe());
            }
        }

        void SWU(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterInt();

            phi::u32 value = processor.IntRegisterGetUnsignedValue(src_reg.register_id);

            phi::boolean success = processor.GetMemory().StoreUnsignedWord(
                    static_cast<phi::size_t>(address.unsafe()), value);

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to store unsigned word at address {}", address.unsafe());
            }
        }

        void SF(Processor& processor, const InstructionArgument& arg1,
                const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterFloat();

            phi::f32 value = processor.FloatRegisterGetFloatValue(src_reg.register_id);

            phi::boolean success = processor.GetMemory().StoreFloat(
                    static_cast<phi::size_t>(address.unsafe()), value);

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to store float at address {}", address.unsafe());
            }
        }

        void SD(Processor& processor, const InstructionArgument& arg1,
                const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            auto optional_address = GetLoadStoreAddress(processor, arg1);

            if (!optional_address.has_value())
            {
                processor.Raise(Exception::AddressOutOfBounds);
                return;
            }

            phi::i32 address = optional_address.value();

            const auto& src_reg = arg2.AsRegisterFloat();

            phi::f64 value = processor.FloatRegisterGetDoubleValue(src_reg.register_id);

            phi::boolean success = processor.GetMemory().StoreDouble(
                    static_cast<phi::size_t>(address.unsafe()), value);

            if (!success)
            {
                processor.Raise(Exception::AddressOutOfBounds);
                DLX_ERROR("Failed to store float at address {}", address.unsafe());
            }
        }

        void MOVF(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID dest_reg   = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID source_reg = arg2.AsRegisterFloat().register_id;

            const phi::f32 source_value = processor.FloatRegisterGetFloatValue(source_reg);

            processor.FloatRegisterSetFloatValue(dest_reg, source_value);
        }

        void MOVD(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID dest_reg   = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID source_reg = arg2.AsRegisterFloat().register_id;

            const phi::f64 source_value = processor.FloatRegisterGetDoubleValue(source_reg);

            processor.FloatRegisterSetDoubleValue(dest_reg, source_value);
        }

        void MOVFP2I(Processor& processor, const InstructionArgument& arg1,
                     const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const IntRegisterID   dest_reg   = arg1.AsRegisterInt().register_id;
            const FloatRegisterID source_reg = arg2.AsRegisterFloat().register_id;

            const float source_value = processor.FloatRegisterGetFloatValue(source_reg).unsafe();

            const phi::int32_t moved_value = *reinterpret_cast<const phi::int32_t*>(&source_value);

            processor.IntRegisterSetSignedValue(dest_reg, moved_value);
        }

        void MOVI2FP(Processor& processor, const InstructionArgument& arg1,
                     const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID dest_reg   = arg1.AsRegisterFloat().register_id;
            const IntRegisterID   source_reg = arg2.AsRegisterInt().register_id;

            const phi::int32_t source_value =
                    processor.IntRegisterGetSignedValue(source_reg).unsafe();

            const float moved_value = *reinterpret_cast<const float*>(&source_value);

            processor.FloatRegisterSetFloatValue(dest_reg, moved_value);
        }

        void CVTF2D(Processor& processor, const InstructionArgument& arg1,
                    const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID dest_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID src_reg  = arg2.AsRegisterFloat().register_id;

            const phi::f32 src_value = processor.FloatRegisterGetFloatValue(src_reg);

            processor.FloatRegisterSetDoubleValue(dest_reg, src_value);
        }

        void CVTF2I(Processor& processor, const InstructionArgument& arg1,
                    const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID dest_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID src_reg  = arg2.AsRegisterFloat().register_id;

            const float        src_value = processor.FloatRegisterGetFloatValue(src_reg).unsafe();
            const phi::int32_t converted_value_int = static_cast<phi::int32_t>(src_value);
            const float        converted_value_float =
                    *reinterpret_cast<const float*>(&converted_value_int);

            processor.FloatRegisterSetFloatValue(dest_reg, converted_value_float);
        }

        void CVTD2F(Processor& processor, const InstructionArgument& arg1,
                    const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID dest_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID src_reg  = arg2.AsRegisterFloat().register_id;

            const double src_value       = processor.FloatRegisterGetDoubleValue(src_reg).unsafe();
            const float  converted_value = static_cast<float>(src_value);

            processor.FloatRegisterSetFloatValue(dest_reg, converted_value);
        }

        void CVTD2I(Processor& processor, const InstructionArgument& arg1,
                    const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID dest_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID src_reg  = arg2.AsRegisterFloat().register_id;

            const double       src_value = processor.FloatRegisterGetDoubleValue(src_reg).unsafe();
            const phi::int32_t converted_value_int = static_cast<phi::int32_t>(src_value);
            const float        converted_value_float =
                    *reinterpret_cast<const float*>(&converted_value_int);

            processor.FloatRegisterSetFloatValue(dest_reg, converted_value_float);
        }

        void CVTI2F(Processor& processor, const InstructionArgument& arg1,
                    const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID dest_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID src_reg  = arg2.AsRegisterFloat().register_id;

            const float        src_value = processor.FloatRegisterGetFloatValue(src_reg).unsafe();
            const phi::int32_t converted_value_int =
                    *reinterpret_cast<const phi::int32_t*>(&src_value);
            const float converted_value_float = static_cast<float>(converted_value_int);

            processor.FloatRegisterSetFloatValue(dest_reg, converted_value_float);
        }

        void CVTI2D(Processor& processor, const InstructionArgument& arg1,
                    const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            const FloatRegisterID dest_reg = arg1.AsRegisterFloat().register_id;
            const FloatRegisterID src_reg  = arg2.AsRegisterFloat().register_id;

            const float        src_value = processor.FloatRegisterGetFloatValue(src_reg).unsafe();
            const phi::int32_t converted_value_int =
                    *reinterpret_cast<const phi::int32_t*>(&src_value);
            const double converted_value_double = static_cast<double>(converted_value_int);

            processor.FloatRegisterSetDoubleValue(dest_reg, converted_value_double);
        }

        void TRAP(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            processor.Raise(Exception::Trap);
        }

        void HALT(Processor& processor, const InstructionArgument& arg1,
                  const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            processor.Raise(Exception::Halt);
        }

        void NOP(Processor& processor, const InstructionArgument& arg1,
                 const InstructionArgument& arg2, const InstructionArgument& arg3) noexcept
        {
            /* Do nothing */
        }
    } // namespace impl
} // namespace dlx

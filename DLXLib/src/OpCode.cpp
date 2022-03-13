#include "DLX/OpCode.hpp"

#include <magic_enum.hpp>
#include <phi/compiler_support/inline.hpp>
#include <algorithm>

namespace dlx
{
    // Test if char c is equal to uppercase t or lowercase t
    [[nodiscard]] PHI_ALWAYS_INLINE constexpr bool ice(char c, char t) noexcept
    {
        constexpr const char diff = 'a' - 'A';
        return (c == t) || (c == t + diff);
    }

    OpCode StringToOpCode(std::string_view token) noexcept
    {
        switch (token.length())
        {
            // 1 character OpCodes
            case 1: {
                char c1 = token[0];
                if (ice(c1, 'J'))
                {
                    return OpCode::J;
                }
                break;
            }

            // 2 character OpCodes
            case 2: {
                char c1 = token[0];
                char c2 = token[1];

                switch (c1)
                {
                    case 'J':
                    case 'j':
                        if (ice(c2, 'R'))
                        {
                            return OpCode::JR;
                        }
                        break;

                    case 'L':
                    case 'l':
                        switch (c2)
                        {
                            case 'B':
                            case 'b':
                                return OpCode::LB;
                            case 'D':
                            case 'd':
                                return OpCode::LD;
                            case 'F':
                            case 'f':
                                return OpCode::LF;
                            case 'H':
                            case 'h':
                                return OpCode::LH;
                            case 'W':
                            case 'w':
                                return OpCode::LW;
                        }
                        break;
                    case 'O':
                    case 'o':
                        if (ice(c2, 'R'))
                        {
                            return OpCode::OR;
                        }
                        break;
                    case 'S':
                    case 's':
                        switch (c2)
                        {
                            case 'B':
                            case 'b':
                                return OpCode::SB;
                            case 'D':
                            case 'd':
                                return OpCode::SD;
                            case 'F':
                            case 'f':
                                return OpCode::SF;
                            case 'H':
                            case 'h':
                                return OpCode::SH;
                            case 'w':
                            case 'W':
                                return OpCode::SW;
                        }
                        break;
                }
                break;
            }

            // 3 character OpCodes
            case 3: {
                char c1 = token[0];
                char c2 = token[1];
                char c3 = token[2];

                switch (c1)
                {
                    case 'A':
                    case 'a':
                        if (ice(c2, 'D') && ice(c3, 'D'))
                        {
                            return OpCode::ADD;
                        }
                        else if (ice(c2, 'N') && ice(c3, 'D'))
                        {
                            return OpCode::AND;
                        }
                        break;

                    case 'D':
                    case 'd':
                        if (ice(c2, 'I') && ice(c3, 'V'))
                        {
                            return OpCode::DIV;
                        }
                        break;

                    case 'E':
                    case 'e':
                        if (ice(c2, 'Q'))
                        {
                            if (ice(c3, 'D'))
                            {
                                return OpCode::EQD;
                            }
                            else if (ice(c3, 'F'))
                            {
                                return OpCode::EQF;
                            }
                        }
                        break;

                    case 'G':
                    case 'g':
                        if (ice(c2, 'E'))
                        {
                            if (ice(c3, 'D'))
                            {
                                return OpCode::GED;
                            }
                            else if (ice(c3, 'F'))
                            {
                                return OpCode::GEF;
                            }
                        }
                        else if (ice(c2, 'T'))
                        {
                            if (ice(c3, 'D'))
                            {
                                return OpCode::GTD;
                            }
                            if (ice(c3, 'F'))
                            {
                                return OpCode::GTF;
                            }
                        }
                        break;

                    case 'J':
                    case 'j':
                        if (ice(c2, 'A') && ice(c3, 'L'))
                        {
                            return OpCode::JAL;
                        }
                        break;

                    case 'L':
                    case 'l':
                        switch (c2)
                        {
                            case 'B':
                            case 'b':
                                if (ice(c3, 'U'))
                                {
                                    return OpCode::LBU;
                                }
                                break;

                            case 'E':
                            case 'e':
                                if (ice(c3, 'D'))
                                {
                                    return OpCode::LED;
                                }
                                else if (ice(c3, 'F'))
                                {
                                    return OpCode::LEF;
                                }
                                break;

                            case 'H':
                            case 'h':
                                if (ice(c3, 'I'))
                                {
                                    return OpCode::LHI;
                                }
                                else if (ice(c3, 'U'))
                                {
                                    return OpCode::LHU;
                                }
                                break;

                            case 'T':
                            case 't':
                                if (ice(c3, 'D'))
                                {
                                    return OpCode::LTD;
                                }
                                else if (ice(c3, 'F'))
                                {
                                    return OpCode::LTF;
                                }
                                break;

                            case 'W':
                            case 'w':
                                if (ice(c3, 'U'))
                                {
                                    return OpCode::LWU;
                                }
                                break;
                        }
                        break;

                    case 'N':
                    case 'n':
                        if (ice(c2, 'E'))
                        {
                            if (ice(c3, 'D'))
                            {
                                return OpCode::NED;
                            }
                            else if (ice(c3, 'F'))
                            {
                                return OpCode::NEF;
                            }
                        }
                        else if (ice(c2, 'O') && ice(c3, 'P'))
                        {
                            return OpCode::NOP;
                        }

                        break;

                    case 'O':
                    case 'o':
                        if (ice(c2, 'R') && ice(c3, 'I'))
                        {
                            return OpCode::ORI;
                        }
                        break;

                    case 'S':
                    case 's':
                        switch (c2)
                        {
                            case 'B':
                            case 'b':
                                if (ice(c3, 'U'))
                                {
                                    return OpCode::SBU;
                                }
                                break;

                            case 'E':
                            case 'e':
                                if (ice(c3, 'Q'))
                                {
                                    return OpCode::SEQ;
                                }
                                break;

                            case 'G':
                            case 'g':
                                if (ice(c3, 'E'))
                                {
                                    return OpCode::SGE;
                                }
                                else if (ice(c3, 'T'))
                                {
                                    return OpCode::SGT;
                                }
                                break;

                            case 'H':
                            case 'h':
                                if (ice(c3, 'U'))
                                {
                                    return OpCode::SHU;
                                }
                                break;

                            case 'L':
                            case 'l':
                                switch (c3)
                                {
                                    case 'A':
                                    case 'a':
                                        return OpCode::SLA;

                                    case 'E':
                                    case 'e':
                                        return OpCode::SLE;

                                    case 'L':
                                    case 'l':
                                        return OpCode::SLL;

                                    case 'T':
                                    case 't':
                                        return OpCode::SLT;
                                }
                                break;

                            case 'N':
                            case 'n':
                                if (ice(c3, 'E'))
                                {
                                    return OpCode::SNE;
                                }
                                break;

                            case 'R':
                            case 'r':
                                if (ice(c3, 'A'))
                                {
                                    return OpCode::SRA;
                                }
                                else if (ice(c3, 'L'))
                                {
                                    return OpCode::SRL;
                                }
                                break;

                            case 'U':
                            case 'u':
                                if (ice(c3, 'B'))
                                {
                                    return OpCode::SUB;
                                }
                                break;

                            case 'W':
                            case 'w':
                                if (ice(c3, 'U'))
                                {
                                    return OpCode::SWU;
                                }
                                break;
                        }

                    case 'X':
                    case 'x':
                        if (ice(c2, 'O') && ice(c3, 'R'))
                        {
                            return OpCode::XOR;
                        }
                        break;
                }
                break;
            }

            // 4 character OpCodes
            case 4: {
                char c1 = token[0];
                char c2 = token[1];
                char c3 = token[2];
                char c4 = token[3];

                switch (c1)
                {
                    case 'A':
                    case 'a':
                        switch (c2)
                        {
                            case 'D':
                            case 'd':
                                if (ice(c3, 'D'))
                                {
                                    switch (c4)
                                    {
                                        case 'D':
                                        case 'd':
                                            return OpCode::ADDD;

                                        case 'F':
                                        case 'f':
                                            return OpCode::ADDF;

                                        case 'I':
                                        case 'i':
                                            return OpCode::ADDI;

                                        case 'U':
                                        case 'u':
                                            return OpCode::ADDU;
                                    }
                                }
                                break;

                            case 'N':
                            case 'n':
                                if (ice(c3, 'D') && ice(c4, 'I'))
                                {
                                    return OpCode::ANDI;
                                }

                                break;
                        }
                        break;

                    case 'B':
                    case 'b':
                        switch (c2)
                        {
                            case 'E':
                            case 'e':
                                if (ice(c3, 'Q') && ice(c4, 'Z'))
                                {
                                    return OpCode::BEQZ;
                                }
                                break;

                            case 'F':
                            case 'f':
                                if (ice(c3, 'P'))
                                {
                                    if (ice(c4, 'F'))
                                    {
                                        return OpCode::BFPF;
                                    }
                                    else if (ice(c4, 'T'))
                                    {
                                        return OpCode::BFPT;
                                    }
                                }
                                break;

                            case 'N':
                            case 'n':
                                if (ice(c3, 'E') && ice(c4, 'Z'))
                                {
                                    return OpCode::BNEZ;
                                }
                        }
                        break;

                    case 'D':
                    case 'd':
                        if (ice(c2, 'I') && ice(c3, 'V'))
                        {
                            switch (c4)
                            {
                                case 'D':
                                case 'd':
                                    return OpCode::DIVD;

                                case 'F':
                                case 'f':
                                    return OpCode::DIVF;

                                case 'I':
                                case 'i':
                                    return OpCode::DIVI;

                                case 'U':
                                case 'u':
                                    return OpCode::DIVU;
                            }
                        }
                        break;

                    case 'H':
                    case 'h':
                        if (ice(c2, 'A') && ice(c3, 'L') && ice(c4, 'T'))
                        {
                            return OpCode::HALT;
                        }
                        break;

                    case 'J':
                    case 'j':
                        if (ice(c2, 'A') && ice(c3, 'L') && ice(c4, 'R'))
                        {
                            return OpCode::JALR;
                        }
                        break;

                    case 'M':
                    case 'm':
                        if (ice(c2, 'O') && ice(c3, 'V'))
                        {
                            if (ice(c4, 'D'))
                            {
                                return OpCode::MOVD;
                            }
                            else if (ice(c4, 'F'))
                            {
                                return OpCode::MOVF;
                            }
                        }
                        else if (ice(c2, 'U') && ice(c3, 'L') && ice(c4, 'T'))
                        {
                            return OpCode::MULT;
                        }
                        break;

                    case 'S':
                    case 's':
                        switch (c2)
                        {
                            case 'E':
                            case 'e':
                                if (ice(c3, 'Q'))
                                {
                                    if (ice(c4, 'I'))
                                    {
                                        return OpCode::SEQI;
                                    }
                                    else if (ice(c4, 'U'))
                                    {
                                        return OpCode::SEQU;
                                    }
                                }
                                break;

                            case 'G':
                            case 'g':
                                if (ice(c3, 'E'))
                                {
                                    if (ice(c4, 'I'))
                                    {
                                        return OpCode::SGEI;
                                    }
                                    else if (ice(c4, 'U'))
                                    {
                                        return OpCode::SGEU;
                                    }
                                }
                                else if (ice(c3, 'T'))
                                {
                                    if (ice(c4, 'I'))
                                    {
                                        return OpCode::SGTI;
                                    }
                                    else if (ice(c4, 'U'))
                                    {
                                        return OpCode::SGTU;
                                    }
                                }
                                break;

                            case 'L':
                            case 'l':
                                switch (c3)
                                {
                                    case 'A':
                                    case 'a':
                                        if (ice(c4, 'I'))
                                        {
                                            return OpCode::SLAI;
                                        }
                                        break;

                                    case 'E':
                                    case 'e':
                                        if (ice(c4, 'I'))
                                        {
                                            return OpCode::SLEI;
                                        }
                                        else if (ice(c4, 'U'))
                                        {
                                            return OpCode::SLEU;
                                        }
                                        break;

                                    case 'L':
                                    case 'l':
                                        if (ice(c4, 'I'))
                                        {
                                            return OpCode::SLLI;
                                        }
                                        break;

                                    case 'T':
                                    case 't':
                                        if (ice(c4, 'I'))
                                        {
                                            return OpCode::SLTI;
                                        }
                                        else if (ice(c4, 'U'))
                                        {
                                            return OpCode::SLTU;
                                        }
                                        break;
                                }

                            case 'N':
                            case 'n':
                                if (ice(c3, 'E'))
                                {
                                    if (ice(c4, 'I'))
                                    {
                                        return OpCode::SNEI;
                                    }
                                    else if (ice(c4, 'U'))
                                    {
                                        return OpCode::SNEU;
                                    }
                                }
                                break;

                            case 'R':
                            case 'r':
                                if (ice(c3, 'A') && ice(c4, 'I'))
                                {
                                    return OpCode::SRAI;
                                }
                                else if (ice(c3, 'L') && ice(c4, 'I'))
                                {
                                    return OpCode::SRLI;
                                }
                                break;

                            case 'U':
                            case 'u':
                                if (ice(c3, 'B'))
                                {
                                    switch (c4)
                                    {
                                        case 'D':
                                        case 'd':
                                            return OpCode::SUBD;

                                        case 'F':
                                        case 'f':
                                            return OpCode::SUBF;

                                        case 'I':
                                        case 'i':
                                            return OpCode::SUBI;

                                        case 'U':
                                        case 'u':
                                            return OpCode::SUBU;
                                    }
                                }
                                break;
                        }
                        break;

                    case 'T':
                    case 't':
                        if (ice(c2, 'R') && ice(c3, 'A') && ice(c4, 'P'))
                        {
                            return OpCode::TRAP;
                        }
                        break;
                    case 'X':
                    case 'x':
                        if (ice(c2, 'O') && ice(c3, 'R') && ice(c4, 'I'))
                        {
                            return OpCode::XORI;
                        }
                        break;
                }
                break;
            }

            // 5 character OpCodes
            case 5: {
                char c1 = token[0];
                char c2 = token[1];
                char c3 = token[2];
                char c4 = token[3];
                char c5 = token[4];

                switch (c1)
                {
                    case 'A':
                    case 'a':
                        if (ice(c2, 'D') && ice(c3, 'D') && ice(c4, 'U') && ice(c5, 'I'))
                        {
                            return OpCode::ADDUI;
                        }
                        break;

                    case 'D':
                    case 'd':
                        if (ice(c2, 'I') && ice(c3, 'V') && ice(c4, 'U') && ice(c5, 'I'))
                        {
                            return OpCode::DIVUI;
                        }
                        break;

                    case 'M':
                    case 'm':
                        if (ice(c2, 'U') && ice(c3, 'L') && ice(c4, 'T'))
                        {
                            switch (c5)
                            {
                                case 'D':
                                case 'd':
                                    return OpCode::MULTD;

                                case 'F':
                                case 'f':
                                    return OpCode::MULTF;

                                case 'I':
                                case 'i':
                                    return OpCode::MULTI;

                                case 'U':
                                case 'u':
                                    return OpCode::MULTU;
                            }
                        }
                        break;

                    case 'S':
                    case 's':
                        switch (c2)
                        {
                            case 'E':
                            case 'e':
                                if (ice(c3, 'Q') && ice(c4, 'U') && ice(c5, 'I'))
                                {
                                    return OpCode::SEQUI;
                                }
                                break;

                            case 'G':
                            case 'g':
                                if (ice(c3, 'E') && ice(c4, 'U') && ice(c5, 'I'))
                                {
                                    return OpCode::SGEUI;
                                }
                                else if (ice(c3, 'T') && ice(c4, 'U') && ice(c5, 'I'))
                                {
                                    return OpCode::SGTUI;
                                }
                                break;

                            case 'L':
                            case 'l':
                                if (ice(c3, 'E') && ice(c4, 'U') && ice(c5, 'I'))
                                {
                                    return OpCode::SLEUI;
                                }
                                else if (ice(c3, 'T') && ice(c4, 'U') && ice(c5, 'I'))
                                {
                                    return OpCode::SLTUI;
                                }
                                break;

                            case 'N':
                            case 'n':
                                if (ice(c3, 'E') && ice(c4, 'U') && ice(c5, 'I'))
                                {
                                    return OpCode::SNEUI;
                                }
                                break;

                            case 'U':
                            case 'u':
                                if (ice(c3, 'B') && ice(c4, 'U') && ice(c5, 'I'))
                                {
                                    return OpCode::SUBUI;
                                }
                                break;
                        }

                        break;
                }

                break;
            }

            // 6 character OpCodes
            case 6: {
                char c1 = token[0];
                char c2 = token[1];
                char c3 = token[2];
                char c4 = token[3];
                char c5 = token[4];
                char c6 = token[5];

                if (ice(c1, 'C') && ice(c2, 'V') && ice(c3, 'T'))
                {
                    switch (c4)
                    {
                        case 'D':
                        case 'd':
                            if (c5 == '2')
                            {
                                if (ice(c6, 'F'))
                                {
                                    return OpCode::CVTD2F;
                                }
                                else if (ice(c6, 'I'))
                                {
                                    return OpCode::CVTD2I;
                                }
                            }
                            break;

                        case 'F':
                        case 'f':
                            if (c5 == '2')
                            {
                                if (ice(c6, 'D'))
                                {
                                    return OpCode::CVTF2D;
                                }
                                else if (ice(c6, 'I'))
                                {
                                    return OpCode::CVTF2I;
                                }
                            }
                            break;

                        case 'I':
                        case 'i':
                            if (c5 == '2')
                            {
                                if (ice(c6, 'D'))
                                {
                                    return OpCode::CVTI2D;
                                }
                                else if (ice(c6, 'F'))
                                {
                                    return OpCode::CVTI2F;
                                }
                            }
                    }
                }
                else if (ice(c1, 'M') && ice(c2, 'U') && ice(c3, 'L') && ice(c4, 'T') &&
                         ice(c5, 'U') && ice(c6, 'I'))
                {
                    return OpCode::MULTUI;
                }
                break;
            }

            // 7 character OpCodes
            case 7: {
                char c1 = token[0];
                char c2 = token[1];
                char c3 = token[2];
                char c4 = token[3];
                char c5 = token[4];
                char c6 = token[5];
                char c7 = token[6];

                if (ice(c1, 'M') && ice(c2, 'O') && ice(c3, 'V'))
                {
                    if (ice(c4, 'I') && c5 == '2' && ice(c6, 'F') && ice(c7, 'P'))
                    {
                        return OpCode::MOVI2FP;
                    }
                    else if (ice(c4, 'F') && ice(c5, 'P') && c6 == '2' && ice(c7, 'I'))
                    {
                        return OpCode::MOVFP2I;
                    }
                }
                break;
            }

            default: {
                break;
            }
        }

        // None found
        return OpCode::NONE;
    }
} // namespace dlx

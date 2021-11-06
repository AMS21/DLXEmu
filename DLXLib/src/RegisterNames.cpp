#include "DLX/RegisterNames.hpp"

#include <string_view>

namespace dlx
{
    IntRegisterID StringToIntRegister(std::string_view token) noexcept
    {
        if (token.length() == 2)
        {
            char first_char = token[0];
            if (first_char == 'R' || first_char == 'r')
            {
                char second_char = token[1];
                if (second_char >= '0' && second_char <= '9')
                {
                    return static_cast<IntRegisterID>(second_char - '0');
                }
            }
        }
        else if (token.length() == 3)
        {
            char first_char = token[0];
            if (first_char == 'R' || first_char == 'r')
            {
                char second_char = token[1];
                char third_char  = token[2];

                switch (second_char)
                {
                    case '1': {
                        if (third_char >= '0' && third_char <= '9')
                        {
                            return static_cast<IntRegisterID>(third_char - '0' + 10);
                        }
                        break;
                    }
                    case '2': {
                        if (third_char >= '0' && third_char <= '9')
                        {
                            return static_cast<IntRegisterID>(third_char - '0' + 20);
                        }
                        break;
                    }
                    case '3': {
                        if (third_char == '0' || third_char == '1')
                        {
                            return static_cast<IntRegisterID>(third_char - '0' + 30);
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }

        return IntRegisterID::None;
    }

    FloatRegisterID StringToFloatRegister(std::string_view token) noexcept
    {
        if (token.length() == 2)
        {
            char first_char = token[0];
            if (first_char == 'F' || first_char == 'f')
            {
                char second_char = token[1];
                if (second_char >= '0' && second_char <= '9')
                {
                    return static_cast<FloatRegisterID>(second_char - '0');
                }
            }
        }
        else if (token.length() == 3)
        {
            char first_char = token[0];
            if (first_char == 'F' || first_char == 'f')
            {
                char second_char = token[1];
                char third_char  = token[2];

                switch (second_char)
                {
                    case '1': {
                        if (third_char >= '0' && third_char <= '9')
                        {
                            return static_cast<FloatRegisterID>(third_char - '0' + 10);
                        }
                        break;
                    }
                    case '2': {
                        if (third_char >= '0' && third_char <= '9')
                        {
                            return static_cast<FloatRegisterID>(third_char - '0' + 20);
                        }
                        break;
                    }
                    case '3': {
                        if (third_char == '0' || third_char == '1')
                        {
                            return static_cast<FloatRegisterID>(third_char - '0' + 30);
                        }
                        break;
                    }
                    default: {
                        break;
                    }
                }
            }
        }

        return FloatRegisterID::None;
    }

    bool IsFPSR(std::string_view token) noexcept
    {
        if (token.length() == 4)
        {
            char c1 = token[0];
            char c2 = token[1];
            char c3 = token[2];
            char c4 = token[3];

            return (c1 == 'F' || c1 == 'f') && (c2 == 'P' || c2 == 'p') &&
                   (c3 == 'S' || c3 == 's') && (c4 == 'R' || c4 == 'r');
        }
    }
} // namespace dlx

#include "DLX/RegisterNames.hpp"

#include <phi/compiler_support/extended_attributes.hpp>
#include <phi/compiler_support/warning.hpp>
#include <phi/core/boolean.hpp>

namespace dlx
{
    IntRegisterID StringToIntRegister(phi::string_view token) noexcept
    {
        if (token.length() == 2u)
        {
            const char first_char = token[0u];
            if (first_char == 'R' || first_char == 'r')
            {
                const char second_char = token[1u];
                if (second_char >= '0' && second_char <= '9')
                {
                    return static_cast<IntRegisterID>(second_char - '0');
                }
            }
        }
        else if (token.length() == 3u)
        {
            const char first_char = token[0u];
            if (first_char == 'R' || first_char == 'r')
            {
                const char second_char = token[1u];
                const char third_char  = token[2u];

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

    FloatRegisterID StringToFloatRegister(phi::string_view token) noexcept
    {
        if (token.length() == 2u)
        {
            const char first_char = token[0u];
            if (first_char == 'F' || first_char == 'f')
            {
                const char second_char = token[1u];
                if (second_char >= '0' && second_char <= '9')
                {
                    return static_cast<FloatRegisterID>(second_char - '0');
                }
            }
        }
        else if (token.length() == 3u)
        {
            const char first_char = token[0u];
            if (first_char == 'F' || first_char == 'f')
            {
                const char second_char = token[1u];
                const char third_char  = token[2u];

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

    phi::boolean IsFPSR(phi::string_view token) noexcept
    {
        if (token.length() == 4u)
        {
            const char c1 = token[0u];
            const char c2 = token[1u];
            const char c3 = token[2u];
            const char c4 = token[3u];

            return (c1 == 'F' || c1 == 'f') && (c2 == 'P' || c2 == 'p') &&
                   (c3 == 'S' || c3 == 's') && (c4 == 'R' || c4 == 'r');
        }

        return false;
    }
} // namespace dlx

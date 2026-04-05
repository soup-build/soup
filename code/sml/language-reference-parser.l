
%top{
// #define SHOW_TOKENS

#pragma warning(disable:4102)
#pragma warning(disable:4189)
#pragma warning(disable:4702)

#include <optional>
#include <iostream>
#include <string>
#include <chrono>

module Soup.SML;
import reflex;
import Opal;

enum class LanguageReferenceToken : int
{
    EndOfFile = 0,
    Name,
    Divider,
    Integer,
    Decimal,
    Unknown,
};

}

%option fast unicode noline nodefault namespace=Soup::SML::LR

name            [A-Za-z][\w#+.]*
divider         \|
integer         \d+
decimal         \.\d+

%%
{name}          return (int)LanguageReferenceToken::Name;
{divider}       return (int)LanguageReferenceToken::Divider;
{integer}       return (int)LanguageReferenceToken::Integer;
{decimal}       return (int)LanguageReferenceToken::Decimal;
.               return (int)LanguageReferenceToken::Unknown;
%%

namespace Soup::SML
{

class LanguageReferenceParser : public LR::Lexer
{
private:
    LanguageReferenceToken currentToken;
    std::string name;
    Opal::SemanticVersion version;

public:
    LanguageReferenceParser(const reflex::Input& input) :
        Lexer(input),
        name(),
        version(0, 1, 0) // TODO: Have a default for now
    {
    }

    bool TryParse()
    {
        // Parse the name
        if (!TryMoveNext())
          return false;
        switch (currentToken)
        {
            case LanguageReferenceToken::Name:
                name = text();
                break;
            default:
                return false;
        }

        // Check for optional version
        if (!TryMoveNext())
          return false;
        switch (currentToken)
        {
            case LanguageReferenceToken::Divider:
                return TryParseVersion();
            case LanguageReferenceToken::EndOfFile:
                return true;
            default:
                return false;
        }
    }

    LanguageReference GetResult()
    {
        return LanguageReference(std::move(name), std::move(version));
    }

  private:
    bool TryParseVersion()
    {
        int major;
        std::optional<int> minor;
        std::optional<int> patch;

        // Parse the major version
        if (!TryMoveNext())
          return false;
        switch (currentToken)
        {
            case LanguageReferenceToken::Integer:
                major = std::stoi(text());
                break;
            default:
                return false;
        }

        // Check for optional minor version
        if (!TryMoveNext())
          return false;
        switch (currentToken)
        {
            case LanguageReferenceToken::Decimal:
                // Skip the period
                minor = std::stoi(text() + 1);
                break;
            case LanguageReferenceToken::EndOfFile:
                version = Opal::SemanticVersion(major, minor, patch);
                return true;
            default:
                return false;
        }

        // Check for optional patch version
        if (!TryMoveNext())
          return false;
        switch (currentToken)
        {
            case LanguageReferenceToken::Decimal:
                // Skip the period
                patch = std::stoi(text() + 1);
                break;
            case LanguageReferenceToken::EndOfFile:
                version = Opal::SemanticVersion(major, minor, patch);
                return true;
            default:
                return false;
        }

        // Verify end of file
        if (!TryMoveNext())
          return false;
        switch (currentToken)
        {
            case LanguageReferenceToken::EndOfFile:
                version = Opal::SemanticVersion(major, minor, patch);
                return true;
            default:
                return false;
        }
    }

    bool TryMoveNext()
    {
        currentToken = (LanguageReferenceToken)lex();

        #ifdef SHOW_TOKENS
            switch (currentToken)
            {
                case LanguageReferenceToken::EndOfFile:
                    std::cout << "Token: " << "EndOfFile" << '\n';
                    break;
                case LanguageReferenceToken::Name:
                    std::cout << "Token: " << "Name" << '\n';
                    break;
                case LanguageReferenceToken::Divider:
                    std::cout << "Token: " << "Divider" << '\n';
                    break;
                case LanguageReferenceToken::Integer:
                    std::cout << "Token: " << "Integer" << '\n';
                    break;
                case LanguageReferenceToken::Decimal:
                    std::cout << "Token: " << "Decimal" << '\n';
                    break;
                case LanguageReferenceToken::Unknown:
                    std::cout << "Token: " << "Unknown" << '\n';
                    break;
                default:
                    std::cout << "Token: " << "UNKNOWN" << '\n';
                    break;
            }
        #endif

        return true;
    }
};

/*static*/ bool LanguageReference::TryParse(const std::string& value, LanguageReference& result)
{
    auto parser = LanguageReferenceParser(value);
    if (parser.TryParse())
    {
      result = parser.GetResult();
      return true;
    }
    else
    {
      return false;
    }
}

}

%top{
module;

// #define SHOW_TOKENS

# include <iostream>
# include <chrono>
# include <optional>
# include <unordered_map>
# include <vector>

module Soup.ModuleParser;
import reflex;

enum class SimplifiedCppToken : int
{
    EndOfFile = 0,
    Newline,
    Colon,
    Semicolon,
    Comma,
    Module,
    Import,
    Export,
    Error,
};

}

%option fast unicode noline nodefault namespace=Soup::ModuleParser

// BasicCharacter: 
//      Tab Character, Letter, Number, Marks, Punctuation, Symbols, Separators
//      except "Quotation Mark", "Reverse Solidus"
//      (Everything except controls)

// KeyCharacter:
//      Letter, Numbers
// TODO: Remove extra symbols

%x QUOTE
basic_char           [\t\p{L}\p{N}\p{M}\p{P}\p{S}\p{Z}]{-}['\\]
whitespace           [ \t]+
newline              \r?\n
preprocessor         "#".*?\r?\n
comment              "//".*?\r?\n

%%
{whitespace}         { /* ignore whitespace */ }
{preprocessor}       { /* ignore preprocessor statements */ }
{comment}            { /* ignore comments */ }
{newline}            return (int)SimplifiedCppToken::Newline;
"import"             return (int)SimplifiedCppToken::Import;
"export"             return (int)SMLToken::Export;
"module"             return (int)SMLToken::Module;
":"                  return (int)SMLToken::Colon;
";"                  return (int)SMLToken::Semicolon;
.                    return (int)SMLToken::Error;
%%

namespace Soup::Core
{

/// <summary>
/// Gradient decent Module parser
/// </summary>
class ModuleParser : public ModuleParser::Lexer
{
private:
    SMLToken _currentToken;
    SMLTable _root;

public:
    ModuleParser(const reflex::Input& input) :
        Lexer(input),
        _currentToken(),
        _root()
    {
    }

    bool TryParse()
    {
        std::vector<std::string> imports;
        if (TryParseTranslationUnit(imports))
        {
            // Verify we are at the end of the content
            if (_currentToken != SMLToken::EndOfFile)
                return false;

            _root = SMLTable(std::move(table));
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    bool TryParseTranslationUnit(std::vector<std::string>& imports)
    {
        // Verify match language name
        MoveNext();
        if (_currentToken != SMLToken::AlphaLiteral &&
            _currentToken != SMLToken::AlphaExt3Literal)
            return false;

        // Key token already matched
        auto languageName = str();

        // Verify the separator
        MoveNext();
        if (_currentToken != SMLToken::AtSign)
            return false;

        // Check version type
        SemanticVersion version;
        MoveNext();
        switch (_currentToken)
        {
            case SMLToken::Integer:
            {
                int64_t majorVersion = std::stoll(text());
                version = SemanticVersion(majorVersion);
                break;
            }
            case SMLToken::Decimal:
            {
                auto versionText = std::string_view(text());
                version = SemanticVersion::Parse(versionText);
                break;
            }
            default:
            {
                return false;
            }
        }

        // Verify we are at the end of the content
        MoveNext();
        if (_currentToken != SMLToken::CloseParenthesis)
            return false;

        languageReference = LanguageReference(
            std::move(languageName),
            version);

        return true;
    }

    SMLToken MoveNext()
    {
        _currentToken = (SMLToken)lex();

        #ifdef SHOW_TOKENS
            switch (_currentToken)
            {
                case SMLToken::EndOfFile:
                    std::cout << "Token: " << "EndOfFile" << '\n';
                    break;
                case SMLToken::Newline:
                    std::cout << "Token: " << "Newline" << '\n';
                    break;
                case SMLToken::KeyLiteral:
                    std::cout << "Token: " << "KeyLiteral" << '\n';
                    break;
                case SMLToken::Integer:
                    std::cout << "Token: " << "Integer" << '\n';
                    break;
                case SMLToken::Decimal:
                    std::cout << "Token: " << "Decimal" << '\n';
                    break;
                case SMLToken::Colon:
                    std::cout << "Token: " << "Colon" << '\n';
                    break;
                case SMLToken::Comma:
                    std::cout << "Token: " << "Comma" << '\n';
                    break;
                case SMLToken::OpenBracket:
                    std::cout << "Token: " << "OpenBracket" << '\n';
                    break;
                case SMLToken::CloseBracket:
                    std::cout << "Token: " << "CloseBracket" << '\n';
                    break;
                case SMLToken::OpenBrace:
                    std::cout << "Token: " << "OpenBrace" << '\n';
                    break;
                case SMLToken::CloseBrace:
                    std::cout << "Token: " << "CloseBrace" << '\n';
                    break;
                case SMLToken::StringLiteral:
                    std::cout << "Token: " << "StringLiteral" << '\n';
                    break;
                case SMLToken::True:
                    std::cout << "Token: " << "True" << '\n';
                    break;
                case SMLToken::False:
                    std::cout << "Token: " << "False" << '\n';
                    break;
                case SMLToken::Error:
                    std::cout << "Token: " << "Error" << '\n';
                    break;
                default:
                    std::cout << "Token: " << "UNKNOWN" << '\n';
                    break;
            }
        #endif

        return _currentToken;
    }
};

/*static*/ SMLDocument SMLDocument::Parse(std::istream& stream)
{
    auto input = reflex::Input(stream);
    auto parser = SMLParser(stream);
    if (parser.TryParse())
    {
        return parser.GetResult();
    }
    else
    {
        auto line = parser.lineno();
        auto column = parser.columno();
        auto text = parser.text();

        std::stringstream message;
        message << "Failed to parse at " << line << ":" << column << " " << text;
        throw std::runtime_error(message.str());
    }
}

/*static*/ SMLDocument SMLDocument::Parse(const char* data, size_t size)
{
    auto input = reflex::Input(data, size);
    auto parser = SMLParser(input);
    if (parser.TryParse())
    {
        return parser.GetResult();
    }
    else
    {
        auto line = parser.lineno();
        auto column = parser.columno();
        auto text = parser.text();

        std::stringstream message;
        message << "Failed to parse at " << line << ":" << column << " " << text;
        throw std::runtime_error(message.str());
    }
}

}

%top{
module;

# include <iostream>
# include <chrono>
# include <optional>
# include <unordered_map>
# include <vector>

export module Soup.ParseModules:ModuleParser;
import reflex;

enum class SimplifiedCppToken : int
{
    EndOfFile = 0,
    Newline,
    Colon,
    Semicolon,
    Module,
    Import,
    Export,
};

}

%option fast unicode noline nodefault namespace=Soup::ParseModules

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
"export"             return (int)SimplifiedCppToken::Export;
"module"             return (int)SimplifiedCppToken::Module;
":"                  return (int)SimplifiedCppToken::Colon;
";"                  return (int)SimplifiedCppToken::Semicolon;
%%

namespace Soup::ParseModules
{

/// <summary>
/// Gradient decent Module parser
/// </summary>
export class ModuleParser : public Lexer
{
private:
    SimplifiedCppToken _currentToken;

public:
    ModuleParser(const reflex::Input& input) :
        Lexer(input),
        _currentToken()
    {
    }

    bool TryParse()
    {
        std::vector<std::string> imports;
        if (TryParseTranslationUnit(imports))
        {
            // Verify we are at the end of the content
            if (_currentToken != SimplifiedCppToken::EndOfFile)
                return false;

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
        // Verify first token is module
        MoveNext();
        if (_currentToken != SimplifiedCppToken::Module)
            return false;

        // Verify semicolon
        MoveNext();
        if (_currentToken != SimplifiedCppToken::Semicolon)
            return false;

        return true;
    }

    SimplifiedCppToken MoveNext()
    {
        _currentToken = (SimplifiedCppToken)lex();

        #ifdef SHOW_TOKENS
            switch (_currentToken)
            {
                case SimplifiedCppToken::EndOfFile:
                    std::cout << "Token: " << "EndOfFile" << '\n';
                    break;
                case SimplifiedCppToken::Module:
                    std::cout << "Token: " << "Module" << '\n';
                    break;
                case SimplifiedCppToken::Export:
                    std::cout << "Token: " << "Export" << '\n';
                    break;
                case SimplifiedCppToken::Import:
                    std::cout << "Token: " << "Import" << '\n';
                    break;
                case SimplifiedCppToken::Newline:
                    std::cout << "Token: " << "Newline" << '\n';
                    break;
                case SimplifiedCppToken::Colon:
                    std::cout << "Token: " << "Colon" << '\n';
                    break;
                case SimplifiedCppToken::Semicolon:
                    std::cout << "Token: " << "Semicolon" << '\n';
                    break;
                default:
                    std::cout << "Token: " << "UNKNOWN" << '\n';
                    break;
            }
        #endif

        return _currentToken;
    }
};

}
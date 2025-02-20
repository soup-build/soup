
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
    Identifier,
    Period,
    Colon,
    Semicolon,
    Module,
    Import,
    Export,
};

}

%option fast unicode noline nodefault namespace=Soup::ParseModules

whitespace           [ \t]+
newline              \r?\n
preprocessor         "#".*?\r?\n
comment              "//".*?\r?\n
identifier           [A-Za-z_]+

%%
{whitespace}         { /* ignore whitespace */ }
{preprocessor}       { /* ignore preprocessor statements */ }
{comment}            { /* ignore comments */ }
{newline}            { /* ignore newlines */ }
"import"             return (int)SimplifiedCppToken::Import;
"export"             return (int)SimplifiedCppToken::Export;
"module"             return (int)SimplifiedCppToken::Module;
"."                  return (int)SimplifiedCppToken::Period;
":"                  return (int)SimplifiedCppToken::Colon;
";"                  return (int)SimplifiedCppToken::Semicolon;
{identifier}         return (int)SimplifiedCppToken::Identifier;
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

        // Verify first token is export
        MoveNext();
        if (_currentToken != SimplifiedCppToken::Export)
            return false;

        // Verify first token is module
        MoveNext();
        if (_currentToken != SimplifiedCppToken::Module)
            return false;

        std::string moduleName;
        if (!TryParseModuleName(moduleName))
            return false;

        // Verify semicolon after module name
        if (_currentToken != SimplifiedCppToken::Semicolon)
            return false;

        return true;
    }

    bool TryParseModuleName(std::string& name)
    {
        auto result = std::string();

        // Verify first token is an identifier
        MoveNext();
        if (_currentToken != SimplifiedCppToken::Identifier)
            return false;

        // Check for optional period separators
        MoveNext();
        while (_currentToken == SimplifiedCppToken::Period)
        {
            // Verify required identifier
            MoveNext();
            if (_currentToken != SimplifiedCppToken::Identifier)
                return false;

            MoveNext();
        }

        result = std::move(result);
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
                case SimplifiedCppToken::Colon:
                    std::cout << "Token: " << "Colon" << '\n';
                    break;
                case SimplifiedCppToken::Semicolon:
                    std::cout << "Token: " << "Semicolon" << '\n';
                    break;
                case SimplifiedCppToken::Period:
                    std::cout << "Token: " << "Period" << '\n';
                    break;
                case SimplifiedCppToken::Identifier:
                    std::cout << "Token: " << "Identifier" << '\n';
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
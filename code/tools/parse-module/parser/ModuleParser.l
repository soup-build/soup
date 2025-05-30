
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
    NumberSign,
    LessThan,
    GreaterThan,
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
identifier           [A-Za-z_][A-Za-z_1-9]*

%%
{whitespace}         { /* ignore whitespace */ }
{preprocessor}       { /* ignore preprocessor statements */ }
{comment}            { /* ignore comments */ }
{newline}            { /* ignore newlines */ }
"import"             return (int)SimplifiedCppToken::Import;
"export"             return (int)SimplifiedCppToken::Export;
"module"             return (int)SimplifiedCppToken::Module;
"#"                  return (int)SimplifiedCppToken::NumberSign;
"<"                  return (int)SimplifiedCppToken::LessThan;
">"                  return (int)SimplifiedCppToken::GreaterThan;
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
    SimplifiedCppToken _nextToken;
    std::vector<std::string> _result;

public:
    ModuleParser(const reflex::Input& input) :
        Lexer(input),
        _currentToken(),
        _nextToken((SimplifiedCppToken)-1),
        _result()
    {
    }

    bool TryParse()
    {
        MoveNext();

        // Check if there is an optional global module declaration
        if (TryParseOptionalGlobalModuleFragment())
        {
            // TODO: Parse global module fragment

            // Must have a module declaration
            std::string moduleName;
            if (!TryParseModuleDeclaration(moduleName))
                throw std::runtime_error("TU with global module fragment must have a module declaration");

            _result.push_back("module " + moduleName);
        }
        else
        {
            // Check for optional module declarationn
            std::string moduleName;
            if (TryParseModuleDeclaration(moduleName))
            {
                _result.push_back("module " + moduleName);
            }
        }

        std::string importName;
        while (TryParseImportModule(importName))
        {
            _result.push_back("import " + importName);
        }

        return true;
    }

    std::vector<std::string> GetResult()
    {
      return std::move(_result);
    }

private:
    bool TryParseOptionalGlobalModuleFragment()
    {
        #ifdef SHOW_TOKENS
          std::cout << "TryParseOptionalGlobalModuleFragment" << std::endl;
        #endif

        // Verify first token is module
        if (_currentToken == SimplifiedCppToken::Module &&
          PeekNext() == SimplifiedCppToken::Semicolon)
        {
            // Move past the semicolon
            MoveNext();
            MoveNext();

            // There is a global module fragment
            return true;
        }

        return false;
    }

    bool TryParseModuleDeclaration(std::string& moduleName)
    {
        #ifdef SHOW_TOKENS
          std::cout << "TryParseModuleDeclaration" << std::endl;
        #endif

        // Check for optional first export token
        if (_currentToken == SimplifiedCppToken::Export)
        {
            MoveNext();
        }

        // Verify required module token
        if (_currentToken != SimplifiedCppToken::Module)
        {
            // Only return false here since there is not tokens that indicate a module declaration
            return false;
        }

        MoveNext();

        std::string result;
        if (!TryParseModuleName(result))
            throw std::runtime_error("Malformed module name in module declaration");

        // Check for optional partition
        std::string partitionName;
        if (TryParseModulePartition(partitionName))
        {
            result += partitionName;
        }

        // Verify semicolon after module name
        if (_currentToken != SimplifiedCppToken::Semicolon)
            throw std::runtime_error("Malformed module declaration");

        moduleName = std::move(result);
        return true;
    }

    bool TryParseModuleName(std::string& name)
    {
        #ifdef SHOW_TOKENS
          std::cout << "TryParseModuleName" << std::endl;
        #endif

        auto result = std::string();

        // Verify first token is an identifier
        if (_currentToken == SimplifiedCppToken::Identifier)
            result += str();
        else
            return false;

        // Check for optional period separators
        MoveNext();
        while (_currentToken == SimplifiedCppToken::Period)
        {
            result += str();

            // Verify required identifier
            MoveNext();
            if (_currentToken == SimplifiedCppToken::Identifier)
                result += str();
            else
                return false;

            MoveNext();
        }

        name = std::move(result);
        return true;
    }

    bool TryParseModulePartition(std::string& name)
    {
        #ifdef SHOW_TOKENS
          std::cout << "TryParseModulePartition" << std::endl;
        #endif

        // Verify first token is a colon
        if (_currentToken != SimplifiedCppToken::Colon)
            return false;

        MoveNext();
        auto moduleName = std::string();
        if (!TryParseModuleName(moduleName))
          return false;

        auto result = std::string(":");
        result += moduleName;
        name = std::move(result);
        return true;
    }

    bool TryParseHeaderName(std::string& name)
    {
        #ifdef SHOW_TOKENS
          std::cout << "TryParseHeaderName" << std::endl;
        #endif

        // TODO: Implement header imports
        name = "TODO";
        return false;
    }

    bool TryParseImportModule(std::string& name)
    {
        #ifdef SHOW_TOKENS
          std::cout << "TryParseImportModule" << std::endl;
        #endif

        // Check for optional first export
        if (_currentToken == SimplifiedCppToken::Export)
        {
            // Has Export
            MoveNext();
        }

        // Verify required import
        if (_currentToken != SimplifiedCppToken::Import)
            return false;

        MoveNext();

        auto result = std::string();
        if (TryParseModuleName(result))
        {
            name = std::move(result);
        }
        else if (TryParseModulePartition(result))
        {
            name = std::move(result);
        }
        else if (TryParseHeaderName(result))
        {
            name = std::move(result);
        }
        else
        {
            return false;
        }

        // Verify semicolon at end
        if (_currentToken != SimplifiedCppToken::Semicolon)
            return false;

        return true;
    }

    SimplifiedCppToken PeekNext()
    {
        if (_nextToken == (SimplifiedCppToken)-1)
        {
            _nextToken = (SimplifiedCppToken)lex();
        }

        return _nextToken;
    }

    SimplifiedCppToken MoveNext()
    {
        if (_nextToken != (SimplifiedCppToken)-1)
        {
            _currentToken = _nextToken;
            _nextToken = (SimplifiedCppToken)-1;
        }
        else
        {
            _currentToken = (SimplifiedCppToken)lex();
        }

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
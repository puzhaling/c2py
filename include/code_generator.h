#pragma once

#include "ast.h"
#include "semantic.h"

#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <unordered_set>

/**
 * CodeGenerator - генератор Python кода из аннотированного AST.
 * 
 * Трансляция C конструкций в Python:
 * - Управляющие конструкции: if, for, while, do-while -> Python эквиваленты
 * - Функции: main() -> if __name__ == "__main__"
 * - Типы данных: C типы игнорируются (Python динамическая типизация)
 * - Операции: унарные, бинарные, составные
 */

class CodeGenerator {
private:
    const SemanticAnalyzer* semanticAnalyzer;  // Для доступа к аннотациям
    std::ostringstream output;
    int indentLevel;
    const std::string indentStr = "    ";  // 4 пробела
    
    // Флаги состояния
    bool inLoop = false;
    bool inFunction = false;
    bool hasReturn = false;
    std::string currentFunctionName = "";
    
    // Импорты, необходимые для программы
    std::unordered_set<std::string> requiredImports;
    
    // Вспомогательные методы
    std::string getIndent() const;
    void increaseIndent();
    void decreaseIndent();
    void emitLine(const std::string& line);
    
    // Генерация выражений
    std::string generateExpression(Expression* expr);
    std::string generateNumber(NumberExpr* expr);
    std::string generateIdentifier(IdentifierExpr* expr);
    std::string generateUnary(UnaryExpr* expr);
    std::string generateBinary(BinaryExpr* expr);
    
    // Трансляция операторов
    std::string translateUnaryOp(const std::string& op);
    std::string translateBinaryOp(const std::string& op);
    
    // Генерация операторов
    void generateStatement(Statement* stmt);
    void generateVarDecl(VarDecl* decl);
    void generateExpressionStmt(ExpressionStmt* stmt);
    void generateBlock(BlockStmt* block);
    void generateIf(IfStmt* stmt);
    void generateWhile(WhileStmt* stmt);
    void generateDoWhile(DoWhileStmt* stmt);
    void generateFor(ForStmt* stmt);
    void generateReturn(ReturnStmt* stmt);
    void generateBreak(BreakStmt* stmt);
    void generateContinue(ContinueStmt* stmt);
    
    // Генерация функций
    void generateFunctionDecl(FunctionDecl* func);
    void generateMainFunction();
    
    // Утилиты
    std::string pythonifyVarName(const std::string& name);
    bool isMainFunction(const std::string& name);
    std::string getPythonType(const std::string& cType);

public:
    CodeGenerator(const SemanticAnalyzer* analyzer = nullptr);
    
    /**
     * Главный метод генерации кода.
     * @param program AST программы
     * @return Строка с Python кодом
     */
    std::string generate(const Program* program);
    
    /**
     * Получить сгенерированный код
     */
    std::string getOutput() const { return output.str(); }
    
    /**
     * Очистить состояние генератора
     */
    void reset();
};

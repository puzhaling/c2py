#ifndef SEMANTIC_H
# define SEMANTIC_H


#include "ast.h"
#include "symbol_table.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <iostream>


// Информация о типах для аннотации
struct TypeInfo {
    enum Kind {
        Void, Int, Float, Double, Char, Bool, 
        Unknown, Error, String, Pointer, Array
    };
    
    Kind kind = Unknown;
    bool isConst = false;
    bool isArray = false;
    int arraySize = -1; // -1 для неизвестного размера
    TypeInfo* elementType = nullptr; // для массивов/указателей
    
    TypeInfo() = default;
    TypeInfo(Kind k) : kind(k) {}
    
    std::string toString() const {
        static const char* names[] = {
            "void", "int", "float", "double", "char", "bool",
            "unknown", "error", "string", "pointer", "array"
        };
        return names[kind];
    }
    
    bool isNumeric() const {
        return kind == Int || kind == Float || kind == Double || kind == Char;
    }
    
    bool isIntegral() const {
        return kind == Int || kind == Char || kind == Bool;
    }
    
    bool operator==(const TypeInfo& other) const {
        return kind == other.kind && isArray == other.isArray;
    }
    
    bool operator!=(const TypeInfo& other) const { return !(*this == other); }
};

// Семантическая аннотация для узла AST
struct SemanticAnnotation {
    TypeInfo type;
    bool isLValue = false;        // Может быть слева от присваивания
    bool isConstant = false;      // Константное выражение
    bool isUsed = false;          // Переменная использована
    bool isInitialized = false;   // Переменная инициализирована
    bool hasSideEffects = false;  // Выражение имеет побочные эффекты
    
    // Для функций
    TypeInfo returnType;
    std::vector<TypeInfo> paramTypes;
    bool returnsValue = false;
    
    // Связь с объявлением (уже есть в IdentifierExpr)
    ASTNode* resolvedDecl = nullptr;
};

// Главный класс семантического анализатора
class SemanticAnalyzer {
private:
    SymbolTable& symbolTable;  // Используем вашу существующую таблицу символов
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    // Контекст анализа
    TypeInfo currentReturnType;
    bool inLoop = false;
    bool inFunction = false;
    std::string currentFunctionName;
    
    // Карта аннотаций: ASTNode* -> SemanticAnnotation
    std::unordered_map<ASTNode*, SemanticAnnotation> annotations;
    
    // Вспомогательные методы
    void error(const std::string& msg, ASTNode* node);
    void warning(const std::string& msg, ASTNode* node);
    
    // Преобразование строки типа в TypeInfo
    TypeInfo typeFromString(const std::string& typeStr);
    
    // Работа с аннотациями
    SemanticAnnotation& annotate(ASTNode* node);
    SemanticAnnotation* getAnnotation(ASTNode* node);
    
    // Основные методы обхода AST
    void analyzeProgram(Program* program);
    void analyzeFunction(FunctionDecl* func);
    TypeInfo analyzeExpression(Expression* expr);
    void analyzeStatement(Statement* stmt);
    
    // Конкретные анализаторы
    void analyzeVarDecl(VarDecl* decl);
    void analyzeBlock(BlockStmt* block);
    void analyzeIf(IfStmt* stmt);
    void analyzeWhile(WhileStmt* stmt);
    void analyzeFor(ForStmt* stmt);
    void analyzeDoWhile(DoWhileStmt* stmt);
    void analyzeContinue(ContinueStmt* stmt);
    void analyzeBreak(BreakStmt* stmt);
    void analyzeReturn(ReturnStmt* stmt);
    void analyzeExpressionStmt(ExpressionStmt* stmt);
    
    TypeInfo analyzeBinaryExpr(BinaryExpr* expr);
    TypeInfo analyzeUnaryExpr(UnaryExpr* expr);
    TypeInfo analyzeIdentifier(IdentifierExpr* expr);
    TypeInfo analyzeNumber(NumberExpr* expr);
    
    // Проверки типов
    bool checkTypeCompatibility(const TypeInfo& expected, const TypeInfo& actual, 
                               ASTNode* node, const std::string& context);
    TypeInfo getCommonType(const TypeInfo& t1, const TypeInfo& t2);
    
public:
    SemanticAnalyzer(SymbolTable& symTab);
    
    // Основной публичный метод
    bool analyze(ProgramPtr& program);
    
    // Доступ к результатам
    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    bool hasErrors() const { return !errors.empty(); }
    
    // Получение аннотации для узла (для генератора кода)
    const SemanticAnnotation* getAnnotationForNode(ASTNode* node) const;
    
    // Отладочный вывод
    void printAnnotations(std::ostream& os = std::cout) const;
};


#endif  /*  ! SEMANTIC_H  */
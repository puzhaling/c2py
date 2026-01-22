#include "semantic.h"

#include <sstream>
#include <cctype>
#include <algorithm>


SemanticAnalyzer::SemanticAnalyzer (SymbolTable& symTab) 
    : symbolTable(symTab)
{}

void
SemanticAnalyzer::error (const std::string& msg, ASTNode* node)
{
    std::ostringstream oss;
    oss << "Semantic error at line " << node->line << ":" << node->column 
        << " - " << msg;
    errors.push_back(oss.str());
}

void
SemanticAnalyzer::warning (const std::string& msg, ASTNode* node)
{
    std::ostringstream oss;
    oss << "Warning at line " << node->line << ":" << node->column 
        << " - " << msg;
    warnings.push_back(oss.str());
}

TypeInfo
SemanticAnalyzer::typeFromString (const std::string& typeStr)
{
    if (typeStr == "int") return TypeInfo(TypeInfo::Int);
    if (typeStr == "float") return TypeInfo(TypeInfo::Float);
    if (typeStr == "double") return TypeInfo(TypeInfo::Double);
    if (typeStr == "char") return TypeInfo(TypeInfo::Char);
    if (typeStr == "bool") return TypeInfo(TypeInfo::Bool);
    if (typeStr == "void") return TypeInfo(TypeInfo::Void);
    return TypeInfo(TypeInfo::Unknown);
}

SemanticAnnotation&
SemanticAnalyzer::annotate (ASTNode* node)
{
    return annotations[node];
}

SemanticAnnotation*
SemanticAnalyzer::getAnnotation (ASTNode* node)
{
    auto it = annotations.find(node);
    return (it != annotations.end()) ? &it->second : nullptr;
}

const SemanticAnnotation*
SemanticAnalyzer::getAnnotationForNode (ASTNode* node) const
{
    auto it = annotations.find(node);
    return (it != annotations.end()) ? &it->second : nullptr;
}

// Основной метод анализа
bool
SemanticAnalyzer::analyze(ProgramPtr& program)
{
    errors.clear();
    warnings.clear();
    annotations.clear();
    
    try {
        analyzeProgram(program.get());
    } catch (const std::exception& e) {
        error(std::string("Fatal: ") + e.what(), program.get());
    }
    
    return errors.empty();
}

void
SemanticAnalyzer::analyzeProgram (Program* program)
{
    // Первый проход: объявляем все функции
<<<<<<< HEAD
    for (auto& func : program->functions) {
=======

    for (auto& func : program->functions) {

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
        // Создаем аннотацию для функции
        SemanticAnnotation& ann = annotate(func.get());
        ann.returnType = typeFromString(func->returnType);
        ann.type = ann.returnType;
        
        // Собираем типы параметров
        for (const auto& param : func->params) {
            ann.paramTypes.push_back(typeFromString(param.first));
        }
        
        // Регистрируем функцию в таблице символов
        symbolTable.declare(func->name, func.get());
    }
    
    // Второй проход: анализируем тела функций
<<<<<<< HEAD
    for (auto& func : program->functions) {
        analyzeFunction(func.get());
    }
=======

    for (auto& func : program->functions) {

        analyzeFunction(func.get());
    }

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
}

void
SemanticAnalyzer::analyzeFunction (FunctionDecl* func)
{
<<<<<<< HEAD
=======

    
    // Очищаем старые параметры
    parameterDecls.clear();
    
>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
    // Устанавливаем контекст
    inFunction = true;
    currentFunctionName = func->name;
    
    SemanticAnnotation* funcAnn = getAnnotation(func);
    if (funcAnn) {
        currentReturnType = funcAnn->returnType;
    }
    
    // Область видимости параметров
    symbolTable.pushScope();
    
    // Объявляем параметры
    for (const auto& param : func->params) {
<<<<<<< HEAD
        TypeInfo paramType = typeFromString(param.first);
        
        // Создаем временный узел для параметра
=======

        TypeInfo paramType = typeFromString(param.first);
        
        // Создаем узел для параметра и сохраняем его в хранилище
>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
        auto paramDecl = std::make_unique<VarDecl>(param.first, param.second);
        paramDecl->line = func->line;
        paramDecl->column = func->column;
        
        // Аннотируем
        SemanticAnnotation& ann = annotate(paramDecl.get());
        ann.type = paramType;
        ann.isLValue = true;
        
<<<<<<< HEAD
        symbolTable.declare(param.second, paramDecl.get());
    }
    
    // Анализируем тело функции
    if (func->body) {
        analyzeBlock(func->body.get());
    }
=======
        // Объявляем в таблице символов, используя адрес из хранилища
        parameterDecls.push_back(std::move(paramDecl));
        symbolTable.declare(param.second, parameterDecls.back().get());
    }
    
    // Анализируем тело функции

    if (func->body) {
        analyzeBlock(func->body.get());
    }

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
    
    // Проверяем, что не-void функция возвращает значение
    if (currentReturnType != TypeInfo::Void && !funcAnn->returnsValue) {
        warning("Function '" + func->name + "' may not return a value", func);
    }
    
    symbolTable.popScope();
    inFunction = false;
    currentFunctionName.clear();
<<<<<<< HEAD
=======

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
}

void
SemanticAnalyzer::analyzeBlock (BlockStmt* block)
{
<<<<<<< HEAD
    symbolTable.pushScope();
    
    for (auto& stmt : block->statements) {
        analyzeStatement(stmt.get());
    }
    
    symbolTable.popScope();
}

void SemanticAnalyzer::analyzeStatement(Statement* stmt) {
    if (auto varDecl = dynamic_cast<VarDecl*>(stmt)) {
        analyzeVarDecl(varDecl);
    }
    else if (auto block = dynamic_cast<BlockStmt*>(stmt)) {
        analyzeBlock(block);
    }
    else if (auto ifStmt = dynamic_cast<IfStmt*>(stmt)) {
        analyzeIf(ifStmt);
    }
    else if (auto whileStmt = dynamic_cast<WhileStmt*>(stmt)) {
        analyzeWhile(whileStmt);
    }
    else if (auto doWhileStmt = dynamic_cast<DoWhileStmt*>(stmt)) {
        analyzeDoWhile(doWhileStmt);
    }
    else if (auto forStmt = dynamic_cast<ForStmt*>(stmt)) {
        analyzeFor(forStmt);
    }
    else if (auto returnStmt = dynamic_cast<ReturnStmt*>(stmt)) {
        analyzeReturn(returnStmt);
    }
    else if (auto breakStmt = dynamic_cast<BreakStmt*>(stmt)) {
        analyzeBreak(breakStmt);
    }
    else if (auto continueStmt = dynamic_cast<ContinueStmt*>(stmt)) {
        analyzeContinue(continueStmt);
    }
    else if (auto exprStmt = dynamic_cast<ExpressionStmt*>(stmt)) {
=======

    symbolTable.pushScope();
    
    for (size_t i = 0; i < block->statements.size(); ++i) {

        analyzeStatement(block->statements[i].get());
    }
    
    symbolTable.popScope();

}

void SemanticAnalyzer::analyzeStatement(Statement* stmt) {

    if (auto varDecl = dynamic_cast<VarDecl*>(stmt)) {

        analyzeVarDecl(varDecl);
    }
    else if (auto block = dynamic_cast<BlockStmt*>(stmt)) {

        analyzeBlock(block);
    }
    else if (auto ifStmt = dynamic_cast<IfStmt*>(stmt)) {

        analyzeIf(ifStmt);
    }
    else if (auto whileStmt = dynamic_cast<WhileStmt*>(stmt)) {

        analyzeWhile(whileStmt);
    }
    else if (auto doWhileStmt = dynamic_cast<DoWhileStmt*>(stmt)) {

        analyzeDoWhile(doWhileStmt);
    }
    else if (auto forStmt = dynamic_cast<ForStmt*>(stmt)) {

        analyzeFor(forStmt);
    }
    else if (auto returnStmt = dynamic_cast<ReturnStmt*>(stmt)) {

        analyzeReturn(returnStmt);
    }
    else if (auto breakStmt = dynamic_cast<BreakStmt*>(stmt)) {

        analyzeBreak(breakStmt);
    }
    else if (auto continueStmt = dynamic_cast<ContinueStmt*>(stmt)) {

        analyzeContinue(continueStmt);
    }
    else if (auto exprStmt = dynamic_cast<ExpressionStmt*>(stmt)) {

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
        analyzeExpressionStmt(exprStmt);
    }
    else {
        // Это должно быть невозможно, если парсер корректен
<<<<<<< HEAD
        error("Unknown statement type", stmt);
    }
=======

        error("Unknown statement type", stmt);
    }

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
}

void
SemanticAnalyzer::analyzeVarDecl (VarDecl* decl)
{
    // Определяем тип переменной
    TypeInfo varType = typeFromString(decl->type);
    
    // Аннотируем объявление
    SemanticAnnotation& ann = annotate(decl);
    ann.type = varType;
    ann.isLValue = true;
    ann.isInitialized = (decl->init != nullptr);
    
    // Регистрируем в таблице символов
    symbolTable.declare(decl->name, decl);
    
    // Анализируем инициализатор, если есть
    if (decl->init) {
        TypeInfo initType = analyzeExpression(decl->init.get());
        
        // Проверяем совместимость типов
        if (!checkTypeCompatibility(varType, initType, decl, 
                                   "variable initialization")) {
            // Пытаемся найти общий тип
            TypeInfo common = getCommonType(varType, initType);
            if (common.kind != TypeInfo::Error) {
                ann.type = common; // Обновляем аннотированный тип
            }
        }
        
        // Аннотируем выражение инициализации
        SemanticAnnotation& initAnn = annotate(decl->init.get());
        initAnn.type = initType;
    }
}

TypeInfo
SemanticAnalyzer::analyzeExpression (Expression* expr) 
{
<<<<<<< HEAD
    TypeInfo resultType(TypeInfo::Unknown);
    
    if (auto num = dynamic_cast<NumberExpr*>(expr)) {
        resultType = analyzeNumber(num);
    }
    else if (auto id = dynamic_cast<IdentifierExpr*>(expr)) {
        resultType = analyzeIdentifier(id);
    }
    else if (auto binary = dynamic_cast<BinaryExpr*>(expr)) {
        resultType = analyzeBinaryExpr(binary);
    }
    else if (auto unary = dynamic_cast<UnaryExpr*>(expr)) {
        resultType = analyzeUnaryExpr(unary);
    }
=======

    TypeInfo resultType(TypeInfo::Unknown);
    
    if (auto num = dynamic_cast<NumberExpr*>(expr)) {

        resultType = analyzeNumber(num);
    }
    else if (auto id = dynamic_cast<IdentifierExpr*>(expr)) {

        resultType = analyzeIdentifier(id);
    }
    else if (auto binary = dynamic_cast<BinaryExpr*>(expr)) {

        resultType = analyzeBinaryExpr(binary);
    }
    else if (auto unary = dynamic_cast<UnaryExpr*>(expr)) {

        resultType = analyzeUnaryExpr(unary);
    }
    else if (auto call = dynamic_cast<CallExpr*>(expr)) {

        resultType = analyzeCall(call);
    }
    else {

    }
>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
    
    // Сохраняем тип в аннотации
    if (expr) {
        SemanticAnnotation& ann = annotate(expr);
        ann.type = resultType;
    }
    
<<<<<<< HEAD
=======

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
    return resultType;
}

TypeInfo
SemanticAnalyzer::analyzeIdentifier (IdentifierExpr* expr)
{
<<<<<<< HEAD
    // Ищем в таблице символов
    ASTNode* decl = symbolTable.lookup(expr->name);
=======

    // Ищем в таблице символов
    ASTNode* decl = symbolTable.lookup(expr->name);

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
    if (!decl) {
        error("Undeclared identifier: '" + expr->name + "'", expr);
        return TypeInfo(TypeInfo::Error);
    }
    
    // Связываем идентификатор с объявлением
    expr->declaration = decl;
    
    // Аннотируем идентификатор
<<<<<<< HEAD
=======

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
    SemanticAnnotation& ann = annotate(expr);
    ann.isLValue = true;
    ann.resolvedDecl = decl;
    
    // Получаем тип из объявления
<<<<<<< HEAD
    if (auto varDecl = dynamic_cast<VarDecl*>(decl)) {
        SemanticAnnotation* declAnn = getAnnotation(varDecl);
        if (declAnn) {
            ann.type = declAnn->type;
=======

    if (auto varDecl = dynamic_cast<VarDecl*>(decl)) {

        SemanticAnnotation* declAnn = getAnnotation(varDecl);

        if (declAnn) {
            ann.type = declAnn->type;

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
            return declAnn->type;
        }
    }
    
<<<<<<< HEAD
=======

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
    return TypeInfo(TypeInfo::Unknown);
}

TypeInfo
SemanticAnalyzer::analyzeNumber (NumberExpr* expr)
{
    SemanticAnnotation& ann = annotate(expr);
    ann.isConstant = true;
    
    // Простой анализ типа числа
    std::string val = expr->value;
    bool hasDot = val.find('.') != std::string::npos;
    bool hasExp = val.find('e') != std::string::npos || val.find('E') != std::string::npos;
    bool hasFloatSuffix = val.back() == 'f' || val.back() == 'F';
    
    if (hasFloatSuffix) {
        ann.type = TypeInfo(TypeInfo::Float);
        return TypeInfo(TypeInfo::Float);
    }
    else if (hasDot || hasExp) {
        ann.type = TypeInfo(TypeInfo::Double);
        return TypeInfo(TypeInfo::Double);
    }
    else {
        ann.type = TypeInfo(TypeInfo::Int);
        return TypeInfo(TypeInfo::Int);
    }
}

<<<<<<< HEAD
=======
TypeInfo
SemanticAnalyzer::analyzeCall (CallExpr* expr)
{
    // Ищем функцию в таблице символов
    ASTNode* decl = symbolTable.lookup(expr->name);
    if (!decl) {
        error("Undefined function: '" + expr->name + "'", expr);
        return TypeInfo(TypeInfo::Error);
    }
    
    // Проверяем, что это функция
    if (auto funcDecl = dynamic_cast<FunctionDecl*>(decl)) {
        // Анализируем аргументы
        for (auto& arg : expr->args) {
            analyzeExpression(arg.get());
        }
        
        // Получаем тип возврата функции
        SemanticAnnotation& ann = annotate(expr);
        
        // Преобразуем C тип в TypeInfo
        TypeInfo retType = TypeInfo(TypeInfo::Unknown);
        if (funcDecl->returnType == "int") {
            retType = TypeInfo(TypeInfo::Int);
        } else if (funcDecl->returnType == "float") {
            retType = TypeInfo(TypeInfo::Float);
        } else if (funcDecl->returnType == "double") {
            retType = TypeInfo(TypeInfo::Double);
        } else if (funcDecl->returnType == "void") {
            retType = TypeInfo(TypeInfo::Void);
        }
        
        ann.type = retType;
        return retType;
    }
    
    error("'" + expr->name + "' is not a function", expr);
    return TypeInfo(TypeInfo::Error);
}

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
void
SemanticAnalyzer::analyzeIf (IfStmt* stmt)
{
    // Анализируем условие
    TypeInfo condType = analyzeExpression(stmt->condition.get());
    
    // Проверяем, что условие - boolean
    if (condType.kind != TypeInfo::Bool && condType.kind != TypeInfo::Unknown) {
        error("Condition must be boolean", stmt->condition.get());
    }
    
    // Аннотируем условие
    SemanticAnnotation& condAnn = annotate(stmt->condition.get());
    condAnn.type = condType;
    
    // Анализируем ветки
    if (stmt->thenBranch) {
        analyzeStatement(stmt->thenBranch.get());
    }
    
    if (stmt->elseBranch) {
        analyzeStatement(stmt->elseBranch.get());
    }
    
    // Аннотируем весь if
    SemanticAnnotation& stmtAnn = annotate(stmt);
    stmtAnn.type = TypeInfo(TypeInfo::Void);
}

void
SemanticAnalyzer::analyzeWhile (WhileStmt* stmt)
{
    // Сохраняем предыдущее состояние
    bool wasInLoop = inLoop;
    inLoop = true;
    
    // Анализируем условие
    TypeInfo condType = analyzeExpression(stmt->condition.get());
    
    if (condType.kind != TypeInfo::Bool && condType.kind != TypeInfo::Unknown) {
        error("While condition must be boolean", stmt->condition.get());
    }
    
    // Аннотируем условие
    SemanticAnnotation& condAnn = annotate(stmt->condition.get());
    condAnn.type = condType;
    
    // Анализируем тело
    if (stmt->body) {
        analyzeStatement(stmt->body.get());
    }
    
    // Аннотируем весь while
    SemanticAnnotation& stmtAnn = annotate(stmt);
    stmtAnn.type = TypeInfo(TypeInfo::Void);
    
    // Восстанавливаем состояние
    inLoop = wasInLoop;
}

void
SemanticAnalyzer::analyzeFor (ForStmt* stmt)
{
    bool wasInLoop = inLoop;
    inLoop = true;
    
    // Анализируем инициализатор
    if (stmt->init) {
        analyzeStatement(stmt->init.get());
    }
    
    // Анализируем условие (может быть nullptr - бесконечный цикл)
    if (stmt->condition) {
        TypeInfo condType = analyzeExpression(stmt->condition.get());
        
        if (condType.kind != TypeInfo::Bool && condType.kind != TypeInfo::Unknown) {
            error("For condition must be boolean", stmt->condition.get());
        }
        
        SemanticAnnotation& condAnn = annotate(stmt->condition.get());
        condAnn.type = condType;
    }
    
    // Анализируем инкремент
    if (stmt->update) {
        TypeInfo updateType = analyzeExpression(stmt->update.get());
        SemanticAnnotation& updateAnn = annotate(stmt->update.get());
        updateAnn.type = updateType;
        updateAnn.hasSideEffects = true; // Инкремент имеет побочные эффекты
    }
    
    // Анализируем тело
    if (stmt->body) {
        analyzeStatement(stmt->body.get());
    }
    
    // Аннотируем весь for
    SemanticAnnotation& stmtAnn = annotate(stmt);
    stmtAnn.type = TypeInfo(TypeInfo::Void);
    
    inLoop = wasInLoop;
}

void
SemanticAnalyzer::analyzeExpressionStmt (ExpressionStmt* stmt)
{
    if (stmt->expr) {
        TypeInfo exprType = analyzeExpression(stmt->expr.get());
        
        // Аннотируем выражение
        SemanticAnnotation& exprAnn = annotate(stmt->expr.get());
        exprAnn.type = exprType;
        
        // Проверяем, не является ли выражение бессмысленным (только для предупреждений)
        if (!exprAnn.hasSideEffects) {
            // warning("Expression statement has no effect", stmt->expr.get());
        }
    }
    
    // Аннотируем сам statement
    SemanticAnnotation& stmtAnn = annotate(stmt);
    stmtAnn.type = TypeInfo(TypeInfo::Void);
}

TypeInfo
SemanticAnalyzer::analyzeUnaryExpr (UnaryExpr* expr)
{
    TypeInfo operandType = analyzeExpression(expr->expr.get());
    
    // Аннотируем операнд
    SemanticAnnotation& operandAnn = annotate(expr->expr.get());
    operandAnn.type = operandType;
    
    // Аннотируем всё выражение
    SemanticAnnotation& exprAnn = annotate(expr);
    
    // Определяем тип в зависимости от операции
    if (expr->op == "!" || expr->op == "!=") {
        // Логическое отрицание
        if (operandType.kind != TypeInfo::Bool && operandType != TypeInfo::Unknown) {
            error("Operand of '!' must be boolean", expr->expr.get());
        }
        exprAnn.type = TypeInfo(TypeInfo::Bool);
        return TypeInfo(TypeInfo::Bool);
    }
    else if (expr->op == "-") {
        // Арифметическое отрицание
        if (!operandType.isNumeric() && operandType != TypeInfo::Unknown) {
            error("Operand of unary '-' must be numeric", expr->expr.get());
        }
        exprAnn.type = operandType;
        return operandType;
    }
    else if (expr->op == "++" || expr->op == "--") {
        // Инкремент/декремент
        SemanticAnnotation* operandAnnPtr = getAnnotation(expr->expr.get());
        if (operandAnnPtr && !operandAnnPtr->isLValue) {
            error("Operand of '++'/'--' must be an lvalue", expr->expr.get());
        }
        if (!operandType.isNumeric() && operandType != TypeInfo::Unknown) {
            error("Operand of '++'/'--' must be numeric", expr->expr.get());
        }
        exprAnn.type = operandType;
        exprAnn.hasSideEffects = true;
        return operandType;
    }
    
    // Неизвестная операция
    exprAnn.type = TypeInfo(TypeInfo::Error);
    return TypeInfo(TypeInfo::Error);
}

// Также добавьте анализаторы для DoWhile, Break, Continue если нужно:

void
SemanticAnalyzer::analyzeDoWhile (DoWhileStmt* stmt)
{
    bool wasInLoop = inLoop;
    inLoop = true;
    
    // Анализируем тело
    if (stmt->body) {
        analyzeStatement(stmt->body.get());
    }
    
    // Анализируем условие
    TypeInfo condType = analyzeExpression(stmt->condition.get());
    
    if (condType.kind != TypeInfo::Bool && condType.kind != TypeInfo::Unknown) {
        error("Do-while condition must be boolean", stmt->condition.get());
    }
    
    SemanticAnnotation& condAnn = annotate(stmt->condition.get());
    condAnn.type = condType;
    
    SemanticAnnotation& stmtAnn = annotate(stmt);
    stmtAnn.type = TypeInfo(TypeInfo::Void);
    
    inLoop = wasInLoop;
}

void
SemanticAnalyzer::analyzeBreak (BreakStmt* stmt)
{
    if (!inLoop) {
        error("Break statement outside loop", stmt);
    }
    
    SemanticAnnotation& ann = annotate(stmt);
    ann.type = TypeInfo(TypeInfo::Void);
}

void
SemanticAnalyzer::analyzeContinue (ContinueStmt* stmt)
{
    if (!inLoop) {
        error("Continue statement outside loop", stmt);
    }
    
    SemanticAnnotation& ann = annotate(stmt);
    ann.type = TypeInfo(TypeInfo::Void);
}

TypeInfo
SemanticAnalyzer::analyzeBinaryExpr (BinaryExpr* expr)
{
    TypeInfo leftType = analyzeExpression(expr->lhs.get());
    TypeInfo rightType = analyzeExpression(expr->rhs.get());
    
    SemanticAnnotation& ann = annotate(expr);
    
    // Определяем тип операции
    if (expr->op == "=") {
        // Присваивание
        // Проверяем, что левая часть - lvalue
        SemanticAnnotation* leftAnn = getAnnotation(expr->lhs.get());
        if (!leftAnn || !leftAnn->isLValue) {
            error("Left side of assignment must be an lvalue", expr);
        }
        
        checkTypeCompatibility(leftType, rightType, expr, "assignment");
        ann.type = leftType;
        return leftType;
    }
    else if (expr->op == "==" || expr->op == "!=" || 
             expr->op == "<" || expr->op == ">" ||
             expr->op == "<=" || expr->op == ">=") {
        // Операции сравнения
        checkTypeCompatibility(leftType, rightType, expr, "comparison");
        ann.type = TypeInfo(TypeInfo::Bool);
        return TypeInfo(TypeInfo::Bool);
    }
    else if (expr->op == "&&" || expr->op == "||") {
        // Логические операции
        if (leftType != TypeInfo::Bool) {
            error("Left operand of '" + expr->op + "' must be boolean", expr);
        }
        if (rightType != TypeInfo::Bool) {
            error("Right operand of '" + expr->op + "' must be boolean", expr);
        }
        ann.type = TypeInfo(TypeInfo::Bool);
        return TypeInfo(TypeInfo::Bool);
    }
    else {
        // Арифметические операции: +, -, *, /, %
        if (!leftType.isNumeric()) {
            error("Left operand of '" + expr->op + "' must be numeric", expr);
        }
        if (!rightType.isNumeric()) {
            error("Right operand of '" + expr->op + "' must be numeric", expr);
        }
        
        // Определяем общий тип
        TypeInfo commonType = getCommonType(leftType, rightType);
        ann.type = commonType;
        return commonType;
    }
}

void
SemanticAnalyzer::analyzeReturn (ReturnStmt* stmt)
{
<<<<<<< HEAD
    // Отмечаем, что функция возвращает значение
    if (inFunction) {
=======

    // Отмечаем, что функция возвращает значение
    if (inFunction) {

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
        SemanticAnnotation* funcAnn = getAnnotation(
            symbolTable.lookup(currentFunctionName)
        );
        if (funcAnn) {
            funcAnn->returnsValue = true;
        }
    }
    
    if (stmt->value) {
<<<<<<< HEAD
        TypeInfo exprType = analyzeExpression(stmt->value.get());
=======

        TypeInfo exprType = analyzeExpression(stmt->value.get());

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
        
        // Проверяем совместимость с типом возвращаемого значения
        checkTypeCompatibility(currentReturnType, exprType, stmt, "return statement");
        
        // Аннотируем
        SemanticAnnotation& ann = annotate(stmt);
        ann.type = exprType;
    } else {
        // Пустой return
        if (currentReturnType != TypeInfo::Void) {
            error("Function must return a value", stmt);
        }
    }
<<<<<<< HEAD
=======

>>>>>>> d354e8b (Add complete C to Python translator implementation with function call support and updated examples)
}

// Другие методы (analyzeIf, analyzeWhile, analyzeFor) аналогичны...

bool
SemanticAnalyzer::checkTypeCompatibility (const TypeInfo& expected, 
                                        const TypeInfo& actual,
                                        ASTNode* node,
                                        const std::string& context)
{
    if (expected == actual) return true;
    
    // Разрешаем некоторые неявные преобразования
    if (expected.isNumeric() && actual.isNumeric()) {
        return true; // Числовые преобразования разрешены
    }
    
    if (expected.kind == TypeInfo::Bool && actual.isIntegral()) {
        return true; // Целое -> bool разрешено
    }
    
    std::ostringstream oss;
    oss << context << ": type mismatch. Expected: " << expected.toString()
        << ", got: " << actual.toString();
    error(oss.str(), node);
    
    return false;
}

TypeInfo
SemanticAnalyzer::getCommonType (const TypeInfo& t1, const TypeInfo& t2)
{
    if (t1 == t2) return t1;
    
    // Иерархия типов для арифметики: double > float > int > char
    if (t1.kind == TypeInfo::Double || t2.kind == TypeInfo::Double)
        return TypeInfo(TypeInfo::Double);
    if (t1.kind == TypeInfo::Float || t2.kind == TypeInfo::Float)
        return TypeInfo(TypeInfo::Float);
    if (t1.kind == TypeInfo::Int || t2.kind == TypeInfo::Int)
        return TypeInfo(TypeInfo::Int);
    if (t1.kind == TypeInfo::Char || t2.kind == TypeInfo::Char)
        return TypeInfo(TypeInfo::Char);
    
    return TypeInfo(TypeInfo::Error);
}

void
SemanticAnalyzer::printAnnotations (std::ostream& os) const
{
    for (const auto& pair : annotations) {
        ASTNode* node = pair.first;
        const SemanticAnnotation& ann = pair.second;
        
        os << "Node @" << node << " [line " << node->line 
           << "]: type=" << ann.type.toString();
        
        if (ann.isLValue) os << " LValue";
        if (ann.isConstant) os << " Const";
        if (ann.resolvedDecl) os << " -> Decl@" << ann.resolvedDecl;
        
        os << std::endl;
    }
}

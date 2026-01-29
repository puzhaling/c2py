#include "code_generator.h"
#include <algorithm>
#include <cctype>
#include <cassert>

CodeGenerator::CodeGenerator(const SemanticAnalyzer* analyzer)
    : semanticAnalyzer(analyzer), indentLevel(0) {}

std::string CodeGenerator::getIndent() const {
    std::string indent;
    for (int i = 0; i < indentLevel; ++i) {
        indent += indentStr;
    }
    return indent;
}

void CodeGenerator::increaseIndent() {
    ++indentLevel;
}

void CodeGenerator::decreaseIndent() {
    if (indentLevel > 0) --indentLevel;
}

void CodeGenerator::emitLine(const std::string& line) {
    if (!line.empty()) {
        output << getIndent() << line << "\n";
    } else {
        output << "\n";
    }
}

void CodeGenerator::reset() {
    output.str("");
    output.clear();
    indentLevel = 0;
    inLoop = false;
    inFunction = false;
    hasReturn = false;
    currentFunctionName = "";
    requiredImports.clear();
}

// ===== Трансляция операторов =====

std::string CodeGenerator::translateUnaryOp(const std::string& op) {
    if (op == "++") return "+= 1";  // Преобразуется в составное присваивание
    if (op == "--") return "-= 1";
    if (op == "!") return "not ";
    if (op == "-") return "-";
    if (op == "+") return "+";
    if (op == "~") return "~";
    return op;
}

std::string CodeGenerator::translateBinaryOp(const std::string& op) {
    if (op == "==") return "==";
    if (op == "!=") return "!=";
    if (op == "<") return "<";
    if (op == ">") return ">";
    if (op == "<=") return "<=";
    if (op == ">=") return ">=";
    if (op == "&&") return "and";
    if (op == "||") return "or";
    if (op == "&") return "&";
    if (op == "|") return "|";
    if (op == "^") return "^";
    if (op == "<<") return "<<";
    if (op == ">>") return ">>";
    if (op == "+") return "+";
    if (op == "-") return "-";
    if (op == "*") return "*";
    if (op == "/") return "//";  // Целочисленное деление в Python
    if (op == "%") return "%";
    if (op == "+=") return "+=";
    if (op == "-=") return "-=";
    if (op == "*=") return "*=";
    if (op == "/=") return "//=";
    if (op == "%=") return "%=";
    if (op == "&=") return "&=";
    if (op == "|=") return "|=";
    if (op == "^=") return "^=";
    if (op == "<<=") return "<<=";
    if (op == ">>=") return ">>=";
    if (op == "=") return "=";
    return op;
}

// ===== Генерация выражений =====

std::string CodeGenerator::generateNumber(NumberExpr* expr) {
    return expr->value;
}

std::string CodeGenerator::generateIdentifier(IdentifierExpr* expr) {
    return pythonifyVarName(expr->name);
}

std::string CodeGenerator::generateUnary(UnaryExpr* expr) {
    std::string operand = generateExpression(expr->expr.get());
    std::string op = translateUnaryOp(expr->op);
    
    // Специальная обработка для инкремента/декремента
    if (expr->op == "++" || expr->op == "--") {
        // Унарный постфикс/префикс инкремент - преобразуется в составное присваивание
        // В контексте выражения это обычно используется как statement
        IdentifierExpr* id = dynamic_cast<IdentifierExpr*>(expr->expr.get());
        if (id) {
            std::string pyOp = (expr->op == "++") ? "+=" : "-=";
            return pythonifyVarName(id->name) + " " + pyOp + " 1";
        }
    }
    
    // Логический NOT
    if (expr->op == "!") {
        return "not " + operand;
    }
    
    // Унарные + и -
    return op + operand;
}

std::string CodeGenerator::generateBinary(BinaryExpr* expr) {
    std::string lhs = generateExpression(expr->lhs.get());
    std::string rhs = generateExpression(expr->rhs.get());
    std::string op = translateBinaryOp(expr->op);
    
    return lhs + " " + op + " " + rhs;
}

std::string CodeGenerator::generateCall(CallExpr* expr) {
    std::string funcName = pythonifyVarName(expr->name);
    std::string args;
    
    for (size_t i = 0; i < expr->args.size(); ++i) {
        if (i > 0) args += ", ";
        args += generateExpression(expr->args[i].get());
    }
    
    return funcName + "(" + args + ")";
}

std::string CodeGenerator::generateExpression(Expression* expr) {
    if (!expr) return "";
    
    if (auto* num = dynamic_cast<NumberExpr*>(expr)) {
        return generateNumber(num);
    }
    if (auto* id = dynamic_cast<IdentifierExpr*>(expr)) {
        return generateIdentifier(id);
    }
    if (auto* unary = dynamic_cast<UnaryExpr*>(expr)) {
        return generateUnary(unary);
    }
    if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
        return generateBinary(binary);
    }
    if (auto* call = dynamic_cast<CallExpr*>(expr)) {
        return generateCall(call);
    }
    
    return "";
}

// ===== Генерация операторов =====

void CodeGenerator::generateVarDecl(VarDecl* decl) {
    std::string varName = pythonifyVarName(decl->name);
    
    if (decl->init) {
        std::string initValue = generateExpression(decl->init.get());
        emitLine(varName + " = " + initValue);
    } else {
        // Инициализация по умолчанию в зависимости от типа
        std::string defaultValue = "0";  // Для числовых типов
        if (decl->type == "float" || decl->type == "double") {
            defaultValue = "0.0";
        } else if (decl->type == "char") {
            defaultValue = "'\\0'";
        } else if (decl->type == "bool") {
            defaultValue = "False";
        }
        emitLine(varName + " = " + defaultValue);
    }
}

void CodeGenerator::generateExpressionStmt(ExpressionStmt* stmt) {
    std::string expr = generateExpression(stmt->expr.get());
    if (!expr.empty()) {
        emitLine(expr);
    }
}

void CodeGenerator::generateBlock(BlockStmt* block) {
    if (!block) return;
    
    for (auto& stmt : block->statements) {
        generateStatement(stmt.get());
    }
}

void CodeGenerator::generateIf(IfStmt* stmt) {
    std::string condition = generateExpression(stmt->condition.get());
    emitLine("if " + condition + ":");
    
    increaseIndent();
    generateStatement(stmt->thenBranch.get());
    decreaseIndent();
    
    if (stmt->elseBranch) {
        emitLine("else:");
        increaseIndent();
        generateStatement(stmt->elseBranch.get());
        decreaseIndent();
    }
}

void CodeGenerator::generateWhile(WhileStmt* stmt) {
    std::string condition = generateExpression(stmt->condition.get());
    emitLine("while " + condition + ":");
    
    bool wasInLoop = inLoop;
    inLoop = true;
    
    increaseIndent();
    generateStatement(stmt->body.get());
    decreaseIndent();
    
    inLoop = wasInLoop;
}

void CodeGenerator::generateDoWhile(DoWhileStmt* stmt) {
    // do-while в Python эмулируется как:
    // while True:
    //     ...body...
    //     if not condition: break
    
    emitLine("while True:");
    
    bool wasInLoop = inLoop;
    inLoop = true;
    
    increaseIndent();
    generateStatement(stmt->body.get());
    
    std::string condition = generateExpression(stmt->condition.get());
    emitLine("if not (" + condition + "): break");
    
    decreaseIndent();
    inLoop = wasInLoop;
}

void CodeGenerator::generateFor(ForStmt* stmt) {
    // Инициализация
    if (stmt->init) {
        generateStatement(stmt->init.get());
    }
    
    // Условие по умолчанию True (бесконечный цикл)
    std::string condition = "True";
    if (stmt->condition) {
        condition = generateExpression(stmt->condition.get());
    }
    
    // Проверка: if это простой инкремент (i++), то используем range()
    if (auto* exprStmt = dynamic_cast<ExpressionStmt*>(stmt->init.get())) {
        if (auto* binExpr = dynamic_cast<BinaryExpr*>(exprStmt->expr.get())) {
            if (binExpr->op == "=" && dynamic_cast<IdentifierExpr*>(binExpr->lhs.get())) {
                // Это присваивание, может быть начало цикла
                // Попытаемся создать Python range() цикл для простых случаев
                
                // Проверяем условие и update
                if (auto* condBin = dynamic_cast<BinaryExpr*>(stmt->condition.get())) {
                    if ((condBin->op == "<" || condBin->op == "<=") && 
                        dynamic_cast<IdentifierExpr*>(condBin->lhs.get())) {
                        
                        if (auto* updateBin = dynamic_cast<BinaryExpr*>(stmt->update.get())) {
                            if ((updateBin->op == "++" || updateBin->op == "+=")) {
                                // Это для цикла for (i = X; i < Y; i++)
                                std::string varName = generateExpression(dynamic_cast<IdentifierExpr*>(condBin->lhs.get()));
                                std::string endVal = generateExpression(condBin->rhs.get());
                                std::string startVal = generateExpression(binExpr->rhs.get());
                                
                                emitLine("for " + varName + " in range(" + startVal + ", " + endVal + "):");
                                
                                bool wasInLoop = inLoop;
                                inLoop = true;
                                
                                increaseIndent();
                                generateStatement(stmt->body.get());
                                decreaseIndent();
                                
                                inLoop = wasInLoop;
                                return;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Общий случай: while цикл с условием
    emitLine("while " + condition + ":");
    
    bool wasInLoop = inLoop;
    inLoop = true;
    
    increaseIndent();
    generateStatement(stmt->body.get());
    
    // Обновление
    if (stmt->update) {
        std::string updateExpr = generateExpression(stmt->update.get());
        if (!updateExpr.empty()) {
            emitLine(updateExpr);
        }
    }
    
    decreaseIndent();
    inLoop = wasInLoop;
}

void CodeGenerator::generateReturn(ReturnStmt* stmt) {
    // Если мы в main, то return должен быть заменен на exit() или sys.exit()
    if (inFunction && currentFunctionName == "main") {
        if (stmt->value) {
            std::string value = generateExpression(stmt->value.get());
            requiredImports.insert("import sys");
            emitLine("sys.exit(" + value + ")");
        } else {
            requiredImports.insert("import sys");
            emitLine("sys.exit(0)");
        }
    } else {
        if (stmt->value) {
            std::string value = generateExpression(stmt->value.get());
            emitLine("return " + value);
        } else {
            emitLine("return");
        }
    }
    hasReturn = true;
}

void CodeGenerator::generateBreak(BreakStmt*) {
    if (!inLoop) {
        // Ошибка: break вне цикла (должна была быть отловлена семантическим анализатором)
        emitLine("# break outside loop");
        return;
    }
    emitLine("break");
}

void CodeGenerator::generateContinue(ContinueStmt*) {
    if (!inLoop) {
        // Ошибка: continue вне цикла
        emitLine("# continue outside loop");
        return;
    }
    emitLine("continue");
}

void CodeGenerator::generateStatement(Statement* stmt) {
    if (!stmt) return;
    
    if (auto* varDecl = dynamic_cast<VarDecl*>(stmt)) {
        generateVarDecl(varDecl);
    } else if (auto* exprStmt = dynamic_cast<ExpressionStmt*>(stmt)) {
        generateExpressionStmt(exprStmt);
    } else if (auto* block = dynamic_cast<BlockStmt*>(stmt)) {
        generateBlock(block);
    } else if (auto* ifStmt = dynamic_cast<IfStmt*>(stmt)) {
        generateIf(ifStmt);
    } else if (auto* whileStmt = dynamic_cast<WhileStmt*>(stmt)) {
        generateWhile(whileStmt);
    } else if (auto* doWhileStmt = dynamic_cast<DoWhileStmt*>(stmt)) {
        generateDoWhile(doWhileStmt);
    } else if (auto* forStmt = dynamic_cast<ForStmt*>(stmt)) {
        generateFor(forStmt);
    } else if (auto* returnStmt = dynamic_cast<ReturnStmt*>(stmt)) {
        generateReturn(returnStmt);
    } else if (auto* breakStmt = dynamic_cast<BreakStmt*>(stmt)) {
        generateBreak(breakStmt);
    } else if (auto* continueStmt = dynamic_cast<ContinueStmt*>(stmt)) {
        generateContinue(continueStmt);
    }
}

// ===== Генерация функций =====

bool CodeGenerator::isMainFunction(const std::string& name) {
    return name == "main";
}

void CodeGenerator::generateFunctionDecl(FunctionDecl* func) {
    if (isMainFunction(func->name)) {
        generateMainFunction();
    } else {
        // Обычная функция
        std::string funcName = pythonifyVarName(func->name);
        
        std::string params;
        for (size_t i = 0; i < func->params.size(); ++i) {
            if (i > 0) params += ", ";
            params += pythonifyVarName(func->params[i].second);
        }
        
        emitLine("def " + funcName + "(" + params + "):");
        
        bool wasInFunc = inFunction;
        std::string prevFuncName = currentFunctionName;
        inFunction = true;
        currentFunctionName = func->name;
        hasReturn = false;
        
        increaseIndent();
        if (func->body) {
            generateBlock(func->body.get());
        }
        
        // Если функция не имеет явного return, добавляем return None
        if (!hasReturn) {
            emitLine("return None");
        }
        
        decreaseIndent();
        inFunction = wasInFunc;
        currentFunctionName = prevFuncName;
        
        // Пустая строка между функциями
        emitLine("");
    }
}

void CodeGenerator::generateMainFunction() {
    // main() -> if __name__ == "__main__":
    emitLine("if __name__ == \"__main__\":");
    
    // Если есть тело функции, выполняем его
    bool wasInFunc = inFunction;
    inFunction = true;
    
    increaseIndent();
    // Тело main будет генерировано в методе generate()
    decreaseIndent();
    inFunction = wasInFunc;
}

std::string CodeGenerator::pythonifyVarName(const std::string& name) {
    // Преобразование переменных C в Python style (может потребоваться)
    // Пока просто возвращаем имя как есть
    return name;
}

std::string CodeGenerator::getPythonType(const std::string& cType) {
    // В Python нет явных типов, но можно использовать комментарии или type hints
    if (cType == "int") return "int";
    if (cType == "float") return "float";
    if (cType == "double") return "float";
    if (cType == "char") return "str";
    if (cType == "bool") return "bool";
    if (cType == "void") return "None";
    return "Any";
}

// ===== Главный метод генерации =====

std::string CodeGenerator::generate(const Program* program) {
    reset();
    
    if (!program) return "";
    
    // Сначала проходим по всем функциям, чтобы собрать все необходимые импорты
    // Это требует полного анализа, поэтому делаем в два прохода
    
    // Создаем временный output для первого прохода
    std::ostringstream tempOutput = std::move(output);
    output.str("");
    output.clear();
    indentLevel = 0;
    requiredImports.clear();
    
    // Проходим по всем функциям и собираем импорты
    for (auto& func : program->functions) {
        if (isMainFunction(func->name)) {
            // Обработаем main в конце
            continue;
        }
        generateFunctionDecl(func.get());
    }
    
    // Теперь генерируем main, если он есть
    for (auto& func : program->functions) {
        if (isMainFunction(func->name)) {
            emitLine("if __name__ == \"__main__\":");
            increaseIndent();
            
            // Установим флаги для main функции
            bool wasInFunc = inFunction;
            std::string prevFuncName = currentFunctionName;
            inFunction = true;
            currentFunctionName = "main";
            
            if (func->body) {
                for (auto& stmt : func->body->statements) {
                    generateStatement(stmt.get());
                }
            }
            
            decreaseIndent();
            
            // Восстановим состояние
            inFunction = wasInFunc;
            currentFunctionName = prevFuncName;
            break;
        }
    }
    
    // Теперь создаем финальный output с импортами в начале
    std::ostringstream result;
    
    // Выводим импорты в начало
    for (const auto& imp : requiredImports) {
        result << imp << "\n";
    }
    
    // Если были импорты, добавляем пустую строку
    if (!requiredImports.empty()) {
        result << "\n";
    }
    
    // Выводим остальной код
    result << output.str();
    
    return result.str();
}

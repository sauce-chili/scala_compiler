#ifndef SCALA_LEXER_METHODCODEGENERATOR_H
#define SCALA_LEXER_METHODCODEGENERATOR_H

#include <cstdint>
#include <string>
#include <vector>

#include "BytecodeBuffer.h"
#include "Instructions.h"
#include "semantic/tables/tables.hpp"
#include "semantic/scopes/Scope.h"

class ExprNode;
class InfixExprNode;
class SimpleExprNode;
class SimpleExpr1Node;
class AssignmentNode;
class ArgumentExprsNode;
class EnumeratorsNode;
class BlockStatsNode;
class ConstantPoolBuilder;

/**
 * MethodCodeGenerator - generates JVM bytecode from AST expressions
 *
 * Handles:
 * - Literals (int, char, double, string, bool, null)
 * - Local variable access
 * - Field access
 * - Method calls
 * - Arithmetic and bitwise operations
 * - Control flow (if/else, while, for)
 * - Object creation
 * - Array operations
 */
class MethodCodeGenerator {
private:
    MethodMetaInfo* method;
    ClassMetaInfo* currentClass;
    ConstantPoolBuilder* constantPool;
    BytecodeBuffer code;

    // Current scope for variable resolution
    Scope* currentScope = nullptr;

    // Expression generation
    void generateExprNode(ExprNode* expr);
    void generateInfixExpr(InfixExprNode* infix);
    void generateSimpleExpr(SimpleExprNode* simpleExpr);
    void generateSimpleExpr1(SimpleExpr1Node* expr1);
    void generateAssignment(AssignmentNode* assign);
    void generateBlockStats(BlockStatsNode* block);

    // Specific expression types
    void generateLiteral(SimpleExpr1Node* literal);
    void generateIdentifier(SimpleExpr1Node* idNode);
    void generateMethodCall(SimpleExpr1Node* call);
    void generateFieldAccess(SimpleExpr1Node* access);
    void generateNewInstance(SimpleExprNode* newExpr);
    void generateArrayCreation(SimpleExpr1Node* arrNode);
    void generateArrayAccess(SimpleExpr1Node* access, ArgumentExprsNode* index);

    // Control flow
    void generateIfElse(ExprNode* cond, ExprNode* trueBranch, ExprNode* falseBranch);
    void generateIf(ExprNode* cond, ExprNode* body);
    void generateWhile(ExprNode* cond, ExprNode* body);
    void generateDoWhile(ExprNode* cond, ExprNode* body);
    void generateFor(EnumeratorsNode* enums, ExprNode* body);
    void generateReturn(ExprNode* value);

    // Operators via RTL
    void generateBinaryOp(const std::string& op, const DataType& leftType, const DataType& rightType);
    void generateUnaryOp(const std::string& op, const DataType& type);

    // RTL type handling
    void generateRtlMethodCall(const std::string& rtlClass, const std::string& methodName,
                               const std::string& descriptor, bool isStatic);
    void boxToRtlDouble(ExprNode* expr);
    void unboxFromRtlDouble();

    // Helpers
    bool isStaticMethod() const;
    uint16_t localSlot(uint16_t number) const;
    uint16_t getLocalSlot(const std::string& varName);
    void loadThis();
    void loadLocal(uint16_t slot, const DataType& type);
    void storeLocal(uint16_t slot, const DataType& type);

    // Method call helpers
    void generateArgumentList(ArgumentExprsNode* args);
    void invokeMethod(ClassMetaInfo* targetClass, MethodMetaInfo* targetMethod, CallInfo callType);
    void invokeConstructor(ClassMetaInfo* targetClass, const std::vector<DataType>& argTypes);

public:
    MethodCodeGenerator(MethodMetaInfo* method, ClassMetaInfo* currentClass, ConstantPoolBuilder* pool);

    /**
     * Generate bytecode for method body
     */
    void generate();

    /**
     * Generate default constructor (calls super.<init>())
     */
    void generateDefaultConstructor();

    /**
     * Generate primary constructor with field initializations
     */
    void generatePrimaryConstructor();

    /**
     * Get the bytecode buffer
     */
    BytecodeBuffer& getCode() { return code; }

    /**
     * Get max stack size used
     */
    uint16_t getMaxStack() const { return code.getMaxStack(); }

    /**
     * Get max locals (this + args + local vars)
     */
    uint16_t getMaxLocals() const;

    /**
     * Generate Code attribute bytes (max_stack, max_locals, code, exception_table, attributes)
     */
    std::vector<uint8_t> generateCodeAttribute();
};

#endif // SCALA_LEXER_METHODCODEGENERATOR_H

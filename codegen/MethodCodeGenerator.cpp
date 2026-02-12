#include "MethodCodeGenerator.h"
#include "semantic/constants/ConstantPoolBuilder.h"
#include "semantic/SemanticContext.h"
#include "semantic/tools/NameTransformer.h"
#include "nodes/exprs/ExprNode.h"
#include "nodes/exprs/InfixExprNode.h"
#include "nodes/exprs/SimpleExprNode.h"
#include "nodes/exprs/SimpleExpr1Node.h"
#include "nodes/exprs/AssignmentNode.h"
#include "nodes/exprs/PrefixExprNode.h"
#include "nodes/exprs/ArgumentExprsNode.h"
#include "nodes/exprs/ExprsNode.h"
#include "nodes/stats/BlockStatsNode.h"
#include "nodes/stats/BlockStatNode.h"
#include "nodes/generator/EnumeratorsNode.h"
#include "nodes/generator/GeneratorNode.h"
#include "nodes/id/IdNode.h"
#include "nodes/var/VarDefsNode.h"

#include <stdexcept>

// JVM Constructor name
extern const std::string JVM_CONSTRUCTOR_NAME;

MethodCodeGenerator::MethodCodeGenerator(MethodMetaInfo* method, ClassMetaInfo* currentClass, ConstantPoolBuilder* pool)
    : method(method), currentClass(currentClass), constantPool(pool) {
}

void MethodCodeGenerator::generate() {
    if (method->body == nullptr) {
        // Abstract method - no code
        return;
    }

    // Generate body
    generateExprNode(method->body);

    // Add implicit return
    code.emitReturn(method->returnType);

    code.resolveLabels();
}

void MethodCodeGenerator::generateDefaultConstructor() {
    // ALOAD_0
    code.emit(Instruction::aload_0);

    // INVOKESPECIAL super/<init>()V
    std::string superClass = currentClass->parent ? currentClass->parent->jvmName : "java/lang/Object";
    auto* methodRef = constantPool->addMethodRef(superClass, "<init>", "()V");
    code.emit(Instruction::invokespecial, methodRef->index);
    code.adjustStack(-1);  // pops this

    // RETURN
    code.emit(Instruction::return_);
    code.resolveLabels();
}

void MethodCodeGenerator::generatePrimaryConstructor() {
    // Call super constructor first
    code.emit(Instruction::aload_0);
    std::string superClass = currentClass->parent ? currentClass->parent->jvmName : "java/lang/Object";

    // TODO: If parent has constructor with args, call that instead
    auto* methodRef = constantPool->addMethodRef(superClass, "<init>", "()V");
    code.emit(Instruction::invokespecial, methodRef->index);
    code.adjustStack(-1);

    // Initialize fields from constructor parameters
    // Fields that have initializers will be set
    for (auto& [name, field] : currentClass->fields) {
        if (field->value != nullptr) {
            // this.field = value
            code.emit(Instruction::aload_0);  // this
            generateExprNode(field->value);

            auto* fieldRef = constantPool->addFieldRef(currentClass, field);
            code.emit(Instruction::putfield, fieldRef->index);
            code.adjustStack(-2);  // pops this and value
        }
    }

    // Generate constructor body for secondary constructors only.
    // Primary constructors already have super.<init>() and field inits above;
    // their body AST node contains a duplicate super call that must be skipped.
    if (method != nullptr && method->body != nullptr && !method->isPrimaryConstructor) {
        generateExprNode(method->body);
    }

    code.emit(Instruction::return_);
    code.resolveLabels();
}

uint16_t MethodCodeGenerator::getMaxLocals() const {
    // this (for non-static) + args + local vars
    // Static methods (e.g., main) don't have 'this'
    bool isStatic = (method != nullptr && method->name == "main" && currentClass == ctx().mainClass);
    uint16_t count = isStatic ? 0 : 1;

    if (method != nullptr) {
        for (auto* arg : method->args) {
            count++;
        }

        // Count all local variables
        for (auto& [name, scopeMap] : method->localVars) {
            count += scopeMap.size();
        }
    }

    return count;
}

std::vector<uint8_t> MethodCodeGenerator::generateCodeAttribute() {
    std::vector<uint8_t> attr;

    uint16_t maxStack = code.getMaxStack();
    if (maxStack < 2) maxStack = 2;  // Minimum for safety

    uint16_t maxLocals = getMaxLocals();
    if (maxLocals < 1) maxLocals = 1;

    std::vector<uint8_t> bytecode = code.getBytes();

    // max_stack (u2)
    attr.push_back(static_cast<uint8_t>((maxStack >> 8) & 0xFF));
    attr.push_back(static_cast<uint8_t>(maxStack & 0xFF));

    // max_locals (u2)
    attr.push_back(static_cast<uint8_t>((maxLocals >> 8) & 0xFF));
    attr.push_back(static_cast<uint8_t>(maxLocals & 0xFF));

    // code_length (u4)
    uint32_t codeLen = bytecode.size();
    attr.push_back(static_cast<uint8_t>((codeLen >> 24) & 0xFF));
    attr.push_back(static_cast<uint8_t>((codeLen >> 16) & 0xFF));
    attr.push_back(static_cast<uint8_t>((codeLen >> 8) & 0xFF));
    attr.push_back(static_cast<uint8_t>(codeLen & 0xFF));

    // code[]
    attr.insert(attr.end(), bytecode.begin(), bytecode.end());

    // exception_table_length (u2) = 0
    attr.push_back(0);
    attr.push_back(0);

    // attributes_count (u2) = 0
    attr.push_back(0);
    attr.push_back(0);

    return attr;
}

// ==================== Expression Generation ====================

void MethodCodeGenerator::generateExprNode(ExprNode* expr) {
    if (expr == nullptr) return;

    switch (expr->type) {
        case _IF_ELSE:
            if (expr->exprs && expr->exprs->size() >= 3) {
                auto it = expr->exprs->begin();
                ExprNode* cond = *it++;
                ExprNode* trueBranch = *it++;
                ExprNode* falseBranch = *it;
                generateIfElse(cond, trueBranch, falseBranch);
            }
            break;

        case _IF:
            if (expr->exprs && expr->exprs->size() >= 2) {
                auto it = expr->exprs->begin();
                ExprNode* cond = *it++;
                ExprNode* body = *it;
                generateIf(cond, body);
            }
            break;

        case _WHILE:
            if (expr->exprs && expr->exprs->size() >= 2) {
                auto it = expr->exprs->begin();
                ExprNode* cond = *it++;
                ExprNode* body = *it;
                generateWhile(cond, body);
            }
            break;

        case _DO_WHILE:
            if (expr->exprs && expr->exprs->size() >= 2) {
                auto it = expr->exprs->begin();
                ExprNode* cond = *it++;
                ExprNode* body = *it;
                generateDoWhile(cond, body);
            }
            break;

        case _FOR:
            if (expr->enumerators && expr->exprs && !expr->exprs->empty()) {
                generateFor(expr->enumerators, expr->exprs->front());
            }
            break;

        case _RETURN_EMPTY:
            code.emit(Instruction::return_);
            break;

        case _RETURN_EXPR:
            if (expr->exprs && !expr->exprs->empty()) {
                generateReturn(expr->exprs->front());
            }
            break;

        case _INFIX:
            if (expr->infixExpr) {
                generateInfixExpr(expr->infixExpr);
            }
            break;

        case _ASSIGNMENT:
            if (expr->assignment) {
                generateAssignment(expr->assignment);
            }
            break;

        default:
            throw std::runtime_error("Unsupported expression type in codegen");
    }
}

void MethodCodeGenerator::generateInfixExpr(InfixExprNode* infix) {
    if (infix == nullptr) return;

    // If this is a leaf (just prefix expr)
    if (infix->prefixExpr != nullptr && infix->left == nullptr) {
        generateSimpleExpr(infix->prefixExpr->simpleExpr);

        // Handle prefix operator via RTL
        if (infix->prefixExpr->type != _NO_UNARY_OPERATOR) {
            DataType type = infix->prefixExpr->inferType(currentClass, method, currentScope);
            generateUnaryOp(prefixExprToString(infix->prefixExpr->type), type);
        }
        return;
    }

    // Binary infix expression
    if (infix->left != nullptr && infix->right != nullptr && infix->fullId != nullptr) {
        std::string op = infix->fullId->name;

        // Get types for operator selection
        DataType leftType = infix->left->inferType(currentClass, method, currentScope);
        DataType rightType = infix->right->inferType(currentClass, method, currentScope);

        // Generate left operand
        generateInfixExpr(infix->left);

        // Generate right operand
        generateInfixExpr(infix->right);

        // Generate operator via RTL
        generateBinaryOp(op, leftType, rightType);
    }
}

void MethodCodeGenerator::generateSimpleExpr(SimpleExprNode* simpleExpr) {
    if (simpleExpr == nullptr) return;

    switch (simpleExpr->type) {
        case _INSTANCE_CREATING:
            generateNewInstance(simpleExpr);
            break;

        case _ARRAY_CREATING:
            // new Array[T](size)
            {
                // Get element type
                DataType elemType = DataType::createFromNode(simpleExpr->simpleType);

                // Generate size expression
                if (simpleExpr->arguments) {
                    generateArgumentList(simpleExpr->arguments);
                }

                // Create array via RTL Array.ofSize
                std::string rtlArrayClass = "rtl/Array";
                std::string elemDescriptor = elemType.toJvmDescriptor();

                auto* methodRef = constantPool->addMethodRef(
                    rtlArrayClass,
                    "ofSize",
                    "(I)Lrtl/Array;"
                );
                code.emit(Instruction::invokestatic, methodRef->index);
            }
            break;

        case _BLOCK_STATS:
            if (simpleExpr->blockStats) {
                generateBlockStats(simpleExpr->blockStats);
            }
            break;

        case _SIMPLE_EXPR_1:
            if (simpleExpr->simpleExpr1) {
                generateSimpleExpr1(simpleExpr->simpleExpr1);
            }
            break;
    }
}

void MethodCodeGenerator::generateSimpleExpr1(SimpleExpr1Node* expr1) {
    if (expr1 == nullptr) return;

    switch (expr1->type) {
        case _INTEGER_LITERAL:
        case _CHAR_LITERAL:
        case _DOUBLE_LITERAL:
        case _STRING_LITERAL:
        case _BOOL_LITERAL:
        case _NULL_LITERAL:
        case _UNIT_LITERAL:
            generateLiteral(expr1);
            break;

        case _IDENTIFIER:
            generateIdentifier(expr1);
            break;

        case _METHOD_CALL:
            generateMethodCall(expr1);
            break;

        case _THIS_FIELD_ACCESS:
        case _SUPER_FIELD_ACCESS:
        case _EXPRESSION_FIELD_ACCESS:
            generateFieldAccess(expr1);
            break;

        case _PARENTHESIZED_EXPR:
            if (expr1->expr) {
                generateExprNode(expr1->expr);
            }
            break;

        case _PLAIN_THIS:
            code.emit(Instruction::aload_0);
            break;

        case _ARRAY_BUILDER:
        case _ARRAY_WITH_TYPE_BUILDER:
            generateArrayCreation(expr1);
            break;
    }
}

void MethodCodeGenerator::generateLiteral(SimpleExpr1Node* literal) {
    switch (literal->type) {
        case _INTEGER_LITERAL:
            // Raw int value - transformLiterals wraps in new Int(literal)
            code.emitIconst(literal->intValue, constantPool);
            break;

        case _CHAR_LITERAL:
            // Raw char value
            code.emitIconst(static_cast<int32_t>(literal->charValue), constantPool);
            break;

        case _DOUBLE_LITERAL:
            // Raw float value
            {
                auto* floatConst = constantPool->addFloat(static_cast<float>(literal->doubleValue));
                if (floatConst->index <= 255) {
                    code.emit(Instruction::ldc, static_cast<uint8_t>(floatConst->index));
                } else {
                    code.emit(Instruction::ldc_w, floatConst->index);
                }
                code.adjustStack(1);
            }
            break;

        case _STRING_LITERAL:
            // Raw java.lang.String from constant pool
            {
                auto* strConst = constantPool->addString(literal->stringValue);
                if (strConst->index <= 255) {
                    code.emit(Instruction::ldc, static_cast<uint8_t>(strConst->index));
                } else {
                    code.emit(Instruction::ldc_w, strConst->index);
                }
                code.adjustStack(1);
            }
            break;

        case _BOOL_LITERAL:
            // Raw boolean (0 or 1)
            code.emit(literal->boolValue ? Instruction::iconst_1 : Instruction::iconst_0);
            break;

        case _NULL_LITERAL:
            code.emit(Instruction::aconst_null);
            break;

        case _UNIT_LITERAL:
            // Unit - push Unit singleton
            {
                auto* methodRef = constantPool->addMethodRef(
                    "rtl/Unit",
                    "getInstance",
                    "()Lrtl/Unit;"
                );
                code.emit(Instruction::invokestatic, methodRef->index);
                code.adjustStack(1);
            }
            break;

        default:
            break;
    }
}

void MethodCodeGenerator::generateIdentifier(SimpleExpr1Node* idNode) {
    std::string name = idNode->identifier->name;

    // Handle special identifiers
    if (name == "super") {
        // super is effectively 'this' at JVM level
        // The actual super method dispatch happens via invokespecial
        code.emit(Instruction::aload_0);
        return;
    }

    if (name == "this") {
        code.emit(Instruction::aload_0);
        return;
    }

    // Try to find as local variable
    if (method != nullptr) {
        auto localOpt = method->resolveLocal(name, currentScope);
        if (localOpt.has_value()) {
            MethodVarMetaInfo* var = localOpt.value();
            code.emitLoad(var->dataType, localSlot(var->number));
            return;
        }
    }

    // Try to find as field
    auto fieldOpt = currentClass->resolveField(name, currentClass);
    if (fieldOpt.has_value()) {
        FieldMetaInfo* field = fieldOpt.value();
        code.emit(Instruction::aload_0);  // this
        auto* fieldRef = constantPool->addFieldRef(currentClass, field);
        code.emit(Instruction::getfield, fieldRef->index);
        code.adjustStack(0);  // -1 (this) +1 (value)
        return;
    }

    // Check if it's a known class name (for static calls like Predef.println)
    if (ctx().classes.count(name) > 0) {
        // For static access, we don't push anything - the static call will handle it
        // This is a placeholder - actual static method calls need different handling
        return;
    }

    // Could be a class name (for static access) - not fully supported
    throw std::runtime_error("Unknown identifier: " + name);
}

void MethodCodeGenerator::generateMethodCall(SimpleExpr1Node* call) {
    // Method call: receiver.methodName(args)
    // After AST transformation, infix ops become method calls
    // Structure: call->simpleExpr1 (type=_EXPRESSION_FIELD_ACCESS) contains:
    //   - identifier: method name (e.g., "+")
    //   - simpleExpr: receiver expression

    if (call->simpleExpr1 == nullptr) {
        throw std::runtime_error("Method call without receiver");
    }

    // Extract method name and receiver from the field access node
    std::string methodName;
    SimpleExprNode* receiverExpr = nullptr;
    bool isSuperCall = false;
    bool isSuperConstructorCall = false;

    if (call->simpleExpr1->type == _EXPRESSION_FIELD_ACCESS && call->simpleExpr1->identifier) {
        methodName = call->simpleExpr1->identifier->name;
        receiverExpr = call->simpleExpr1->simpleExpr;
    } else if (call->simpleExpr1->type == _THIS_FIELD_ACCESS && call->simpleExpr1->identifier) {
        methodName = call->simpleExpr1->identifier->name;
        // receiver is 'this'
    } else if (call->simpleExpr1->type == _IDENTIFIER && call->simpleExpr1->identifier) {
        // Simple method call: methodName(args) - implicit 'this' as receiver
        methodName = call->simpleExpr1->identifier->name;
        // Check if this is a super constructor call: super(args)
        if (methodName == "super") {
            isSuperConstructorCall = true;
        }
        // receiver is 'this'
    } else if (call->simpleExpr1->type == _SUPER_FIELD_ACCESS && call->simpleExpr1->identifier) {
        // Super method call: super.methodName(args)
        methodName = call->simpleExpr1->identifier->name;
        isSuperCall = true;
        // receiver is 'this', but method resolved from parent class
    } else {
        throw std::runtime_error("Unsupported method call structure");
    }

    // Generate receiver
    if (receiverExpr) {
        generateSimpleExpr(receiverExpr);
    } else {
        // 'this' is the receiver (for this., super., or implicit this)
        code.emit(Instruction::aload_0);
    }

    // Collect argument types for method resolution
    std::vector<DataType*> argTypes;
    if (call->argumentExprs && call->argumentExprs->exprs && call->argumentExprs->exprs->exprs) {
        for (auto* expr : *(call->argumentExprs->exprs->exprs)) {
            if (expr) {
                DataType exprType = expr->inferType(currentClass, method, currentScope);
                argTypes.push_back(new DataType(exprType));
            }
        }
    }

    // Generate arguments
    if (call->argumentExprs) {
        generateArgumentList(call->argumentExprs);
    }

    // Handle super constructor call: super(args) -> parent.<init>(args)
    if (isSuperConstructorCall) {
        if (currentClass->parent == nullptr) {
            for (auto* dt : argTypes) delete dt;
            throw std::runtime_error("Super constructor call in class without parent: " + currentClass->name);
        }

        // Build descriptor for parent constructor
        std::string descriptor = "(";
        for (auto* dt : argTypes) {
            descriptor += dt->toJvmDescriptor();
        }
        descriptor += ")V";

        auto* methodRef = constantPool->addMethodRef(
            currentClass->parent->jvmName,
            "<init>",
            descriptor
        );
        code.emit(Instruction::invokespecial, methodRef->index);

        for (auto* dt : argTypes) delete dt;
        return;
    }

    // Determine receiver type for method resolution
    DataType receiverType;
    ClassMetaInfo* receiverClass = nullptr;

    if (isSuperCall) {
        // For super calls, resolve method in parent class
        if (currentClass->parent == nullptr) {
            for (auto* dt : argTypes) delete dt;
            throw std::runtime_error("Super call in class without parent: " + currentClass->name);
        }
        receiverClass = currentClass->parent;
    } else if (receiverExpr) {
        receiverType = receiverExpr->inferType(currentClass, method, currentScope);
        std::string className = receiverType.getClassName();
        receiverClass = ctx().classes[className];
        if (receiverClass == nullptr) {
            for (auto* dt : argTypes) delete dt;
            throw std::runtime_error("Unknown class: " + className);
        }
    } else {
        receiverClass = currentClass;
    }

    auto methodOpt = receiverClass->resolveMethod(methodName, argTypes, currentClass);
    if (!methodOpt.has_value()) {
        for (auto* dt : argTypes) delete dt;
        throw std::runtime_error("Method not found: " + methodName + " in class " + receiverClass->name);
    }

    MethodMetaInfo* resolvedMethod = methodOpt.value();

    // Check if receiver is an RTL primitive type (Int, Double, Char, Bool)
    // These map to JVM primitives (I, F, C, Z), so invokevirtual won't work.
    // Use invokestatic with receiver type prepended to descriptor.
    bool isRtlPrimitive = false;
    if (auto* rtl = dynamic_cast<RtlClassMetaInfo*>(receiverClass)) {
        DataType::Kind k = receiverType.kind;
        isRtlPrimitive = (k == DataType::Kind::Int || k == DataType::Kind::Double ||
                          k == DataType::Kind::Char || k == DataType::Kind::Bool);
    }

    if (isSuperCall) {
        auto* methodRef = constantPool->addMethodRef(receiverClass, resolvedMethod);
        code.emit(Instruction::invokespecial, methodRef->index);
    } else if (isRtlPrimitive) {
        // invokestatic: receiver is first arg, so prepend its type to descriptor
        std::string origDesc = resolvedMethod->jvmDescriptor();
        std::string receiverDesc = receiverType.toJvmDescriptor();
        std::string staticDesc = "(" + receiverDesc + origDesc.substr(1);
        auto* methodRef = constantPool->addMethodRef(
            receiverClass->jvmName, resolvedMethod->jvmName, staticDesc);
        code.emit(Instruction::invokestatic, methodRef->index);
    } else {
        auto* methodRef = constantPool->addMethodRef(receiverClass, resolvedMethod);
        code.emit(Instruction::invokevirtual, methodRef->index);
    }

    // Cleanup allocated DataType pointers
    for (auto* dt : argTypes) {
        delete dt;
    }
}

void MethodCodeGenerator::generateFieldAccess(SimpleExpr1Node* access) {
    std::string fieldName = access->identifier->name;

    switch (access->type) {
        case _THIS_FIELD_ACCESS:
            {
                code.emit(Instruction::aload_0);  // this
                auto fieldOpt = currentClass->resolveField(fieldName, currentClass);
                if (fieldOpt.has_value()) {
                    auto* fieldRef = constantPool->addFieldRef(currentClass, fieldOpt.value());
                    code.emit(Instruction::getfield, fieldRef->index);
                    code.adjustStack(0);
                } else {
                    // Try as no-arg method call
                    std::vector<DataType*> emptyArgs;
                    auto methodOpt = currentClass->resolveMethod(fieldName, emptyArgs, currentClass);
                    if (methodOpt.has_value()) {
                        auto* methodRef = constantPool->addMethodRef(currentClass, methodOpt.value());
                        code.emit(Instruction::invokevirtual, methodRef->index);
                        code.adjustStack(0);  // -1 (this) +1 (result)
                    } else {
                        throw std::runtime_error("Unknown member: " + fieldName);
                    }
                }
            }
            break;

        case _SUPER_FIELD_ACCESS:
            {
                code.emit(Instruction::aload_0);  // this
                if (currentClass->parent == nullptr) {
                    // Treat as if parent is java/lang/Object
                    throw std::runtime_error("No parent class for super access to: " + fieldName);
                }
                auto fieldOpt = currentClass->parent->resolveField(fieldName, currentClass, false);
                if (fieldOpt.has_value()) {
                    auto* fieldRef = constantPool->addFieldRef(currentClass->parent, fieldOpt.value());
                    code.emit(Instruction::getfield, fieldRef->index);
                    code.adjustStack(0);
                } else {
                    // Try as no-arg method call (super.method() without explicit parens)
                    std::vector<DataType*> emptyArgs;
                    auto methodOpt = currentClass->parent->resolveMethod(fieldName, emptyArgs, currentClass);
                    if (methodOpt.has_value()) {
                        // Use invokespecial for super method calls
                        auto* methodRef = constantPool->addMethodRef(currentClass->parent, methodOpt.value());
                        code.emit(Instruction::invokespecial, methodRef->index);
                        code.adjustStack(0);  // -1 (this) +1 (result)
                    } else {
                        throw std::runtime_error("Unknown super member: " + fieldName);
                    }
                }
            }
            break;

        case _EXPRESSION_FIELD_ACCESS:
            {
                // Generate receiver expression
                generateSimpleExpr(access->simpleExpr);

                DataType receiverType = access->simpleExpr->inferType(currentClass, method, currentScope);

                // Resolve field in receiver's class
                std::string className = receiverType.getClassName();
                auto* receiverClass = ctx().classes[className];
                if (receiverClass == nullptr) {
                    throw std::runtime_error("Unknown class: " + className);
                }

                // Encode operator names (e.g., "*" -> "$times")
                std::string encodedFieldName = NameTransformer::encode(fieldName);

                auto fieldOpt = receiverClass->resolveField(fieldName, currentClass, false);
                if (fieldOpt.has_value()) {
                    auto* fieldRef = constantPool->addFieldRef(receiverClass, fieldOpt.value());
                    code.emit(Instruction::getfield, fieldRef->index);
                    code.adjustStack(0);
                } else {
                    // Try as no-arg method call
                    std::vector<DataType*> emptyArgs;

                    // Try original name first
                    auto methodOpt = receiverClass->resolveMethod(fieldName, emptyArgs, currentClass);

                    // If not found and encoded name is different, try encoded name
                    if (!methodOpt.has_value() && encodedFieldName != fieldName) {
                        methodOpt = receiverClass->resolveMethod(encodedFieldName, emptyArgs, currentClass);
                    }

                    if (methodOpt.has_value()) {
                        auto* methodRef = constantPool->addMethodRef(receiverClass, methodOpt.value());
                        code.emit(Instruction::invokevirtual, methodRef->index);
                        code.adjustStack(0);  // -1 (receiver) +1 (result)
                    } else {
                        throw std::runtime_error("Unknown member: " + fieldName + " in class " + className);
                    }
                }
            }
            break;

        default:
            break;
    }
}

std::vector<DataType> copyPointersToValues(const std::vector<DataType*>& src) {
    std::vector<DataType> dst;
    dst.reserve(src.size());
    for (auto* p : src) {
        if (p) dst.push_back(*p);
        else    dst.emplace_back();
    }
    return dst;
}

void MethodCodeGenerator::generateNewInstance(SimpleExprNode* newExpr) {
    std::string className = newExpr->fullId->name;

    // Resolve jvmName through class metadata
    auto classIt = ctx().classes.find(className);
    std::string jvmClassName = (classIt != ctx().classes.end())
        ? classIt->second->jvmName : className;

    // NEW className
    auto* classRef = constantPool->addClass(jvmClassName);
    code.emit(Instruction::new_, classRef->index);
    code.adjustStack(1);

    // DUP
    code.emit(Instruction::dup);

    // Generate constructor arguments
    std::vector<DataType> argTypes;
    if (newExpr->arguments) {
        // TODO: Collect argument types
        argTypes = copyPointersToValues(newExpr->arguments->getArgsTypes(currentClass, method, currentScope));
        generateArgumentList(newExpr->arguments);
    }

    // INVOKESPECIAL <init>
    invokeConstructor(ctx().classes[className], argTypes);
}

void MethodCodeGenerator::generateArrayCreation(SimpleExpr1Node* arrNode) {
    if (arrNode->type == _ARRAY_WITH_TYPE_BUILDER) {
        // Array[Type](elements...)
        DataType elemType = DataType::createFromNode(arrNode->simpleType);

        // Count elements
        int count = 0;
        if (arrNode->argumentExprs && arrNode->argumentExprs->exprs && arrNode->argumentExprs->exprs->exprs) {
            count = arrNode->argumentExprs->exprs->exprs->size();
        }

        // Create array with size via RTL
        code.emitIconst(count, constantPool);
        auto* methodRef = constantPool->addMethodRef(
            "rtl/Array",
            "ofSize",
            "(I)Lrtl/Array;"
        );
        code.emit(Instruction::invokestatic, methodRef->index);

        // Fill array with elements via RTL Array.update
        if (arrNode->argumentExprs && arrNode->argumentExprs->exprs && arrNode->argumentExprs->exprs->exprs) {
            int idx = 0;
            for (ExprNode* elem : *arrNode->argumentExprs->exprs->exprs) {
                code.emit(Instruction::dup);        // array ref

                // Box index to RTL Int via new rtl/Int(idx)
                auto* intClassRef = constantPool->addClass("rtl/Int");
                code.emit(Instruction::new_, intClassRef->index);
                code.adjustStack(1);
                code.emit(Instruction::dup);
                code.emitIconst(idx, constantPool);
                auto* intInitRef = constantPool->addMethodRef("rtl/Int", "<init>", "(I)V");
                code.emit(Instruction::invokespecial, intInitRef->index);
                code.adjustStack(-2);

                generateExprNode(elem);              // value

                // Call Array.update(Int, Any)
                auto* updateMethodRef = constantPool->addMethodRef(
                    "rtl/Array",
                    "update",
                    "(Lrtl/Int;Ljava/lang/Object;)V"
                );
                code.emit(Instruction::invokevirtual, updateMethodRef->index);
                code.adjustStack(-3);
                idx++;
            }
        }
    } else if (arrNode->type == _ARRAY_BUILDER) {
        // Array(elements...) - infer type from elements
        int count = 0;
        if (arrNode->argumentExprs && arrNode->argumentExprs->exprs && arrNode->argumentExprs->exprs->exprs) {
            count = arrNode->argumentExprs->exprs->exprs->size();
        }

        // Create array via RTL
        code.emitIconst(count, constantPool);
        auto* methodRef = constantPool->addMethodRef(
            "rtl/Array",
            "ofSize",
            "(I)Lrtl/Array;"
        );
        code.emit(Instruction::invokestatic, methodRef->index);

        // Fill array
        if (arrNode->argumentExprs && arrNode->argumentExprs->exprs && arrNode->argumentExprs->exprs->exprs) {
            int idx = 0;
            for (ExprNode* elem : *arrNode->argumentExprs->exprs->exprs) {
                code.emit(Instruction::dup);

                // Box index to RTL Int via new rtl/Int(idx)
                auto* intClassRef = constantPool->addClass("rtl/Int");
                code.emit(Instruction::new_, intClassRef->index);
                code.adjustStack(1);
                code.emit(Instruction::dup);
                code.emitIconst(idx, constantPool);
                auto* intInitRef = constantPool->addMethodRef("rtl/Int", "<init>", "(I)V");
                code.emit(Instruction::invokespecial, intInitRef->index);
                code.adjustStack(-2);

                generateExprNode(elem);

                // Call Array.update(Int, Any)
                auto* updateMethodRef = constantPool->addMethodRef(
                    "rtl/Array",
                    "update",
                    "(Lrtl/Int;Ljava/lang/Object;)V"
                );
                code.emit(Instruction::invokevirtual, updateMethodRef->index);
                code.adjustStack(-3);
                idx++;
            }
        }
    }
}

void MethodCodeGenerator::generateAssignment(AssignmentNode* assign) {
    if (assign->fullId != nullptr && assign->simpleExpr == nullptr && assign->simpleExpr1 == nullptr) {
        // Simple variable assignment: x = expr
        std::string name = assign->fullId->name;

        // Generate value
        generateExprNode(assign->expr);

        // Store to local or field
        auto localOpt = method->resolveLocal(name, currentScope);
        if (localOpt.has_value()) {
            MethodVarMetaInfo* var = localOpt.value();
            code.emitStore(var->dataType, localSlot(var->number));
        } else {
            // Field assignment
            auto fieldOpt = currentClass->resolveField(name, currentClass);
            if (!fieldOpt.has_value()) {
                throw std::runtime_error("Unknown variable: " + name);
            }

            // Need to reorganize stack: value is on top, need this, value
            // Store to temp, load this, load temp, putfield
            // For simplicity, regenerate:
            code.emit(Instruction::pop);  // discard generated value
            code.emit(Instruction::aload_0);  // this
            generateExprNode(assign->expr);  // value again

            auto* fieldRef = constantPool->addFieldRef(currentClass, fieldOpt.value());
            code.emit(Instruction::putfield, fieldRef->index);
            code.adjustStack(-2);
        }
    } else if (assign->simpleExpr != nullptr) {
        // Field assignment: obj.field = expr
        generateSimpleExpr(assign->simpleExpr);  // receiver

        generateExprNode(assign->expr);  // value

        std::string fieldName = assign->fullId->name;
        DataType receiverType = assign->simpleExpr->inferType(currentClass, method, currentScope);

        auto* receiverClass = ctx().classes[receiverType.className];
        auto fieldOpt = receiverClass->resolveField(fieldName, currentClass, false);

        auto* fieldRef = constantPool->addFieldRef(receiverClass, fieldOpt.value());
        code.emit(Instruction::putfield, fieldRef->index);
        code.adjustStack(-2);
    } else if (assign->simpleExpr1 != nullptr && assign->argumentExprs != nullptr) {
        // Array assignment: arr(index) = expr via RTL Array.update
        generateSimpleExpr1(assign->simpleExpr1);  // array

        // Generate index
        if (assign->argumentExprs->exprs && assign->argumentExprs->exprs->exprs &&
            !assign->argumentExprs->exprs->exprs->empty()) {
            generateExprNode(assign->argumentExprs->exprs->exprs->front());
        }

        generateExprNode(assign->expr);  // value

        // Call RTL Array.update(index, value)
        auto* methodRef = constantPool->addMethodRef(
            "rtl/Array",
            "update",
            "(Lrtl/Int;Ljava/lang/Object;)V"
        );
        code.emit(Instruction::invokevirtual, methodRef->index);
        code.adjustStack(-3);
    }
}

void MethodCodeGenerator::generateBlockStats(BlockStatsNode* block) {
    if (block == nullptr || block->blockStats == nullptr) return;

    // Set current scope from block for variable resolution
    Scope* prevScope = currentScope;
    if (block->blockScope) {
        currentScope = block->blockScope;
    }

    for (auto* stat : *block->blockStats) {
        if (stat == nullptr) continue;

        // Handle variable definitions
        if (stat->varDefs != nullptr) {
            // Variable definition - just generate initializer and store
            VarDefsNode* varDefs = stat->varDefs;
            if (varDefs && varDefs->expr) {
                generateExprNode(varDefs->expr);

                // Store to local variable
                std::string varName = varDefs->fullId->name;
                auto localOpt = method->resolveLocal(varName, currentScope);
                if (localOpt.has_value()) {
                    MethodVarMetaInfo* var = localOpt.value();
                    code.emitStore(var->dataType, localSlot(var->number));
                }
            }
        } else if (stat->expr != nullptr) {
            generateExprNode(stat->expr);
            // Pop result if not last statement
            // (Simplified - in real impl, track if value is used)
        }
    }

    // Restore previous scope
    currentScope = prevScope;
}

// ==================== Control Flow ====================

void MethodCodeGenerator::generateIfElse(ExprNode* cond, ExprNode* trueBranch, ExprNode* falseBranch) {
    std::string elseLabel = code.generateLabel("else");
    std::string endLabel = code.generateLabel("endif");

    // Generate condition
    generateExprNode(cond);

    // Unbox RTL Boolean to JVM int for comparison
    auto* unboxMethodRef = constantPool->addMethodRef(
        "rtl/Boolean",
        "booleanValue",
        "()Z"
    );
    code.emit(Instruction::invokevirtual, unboxMethodRef->index);

    // If false, jump to else
    code.emitBranch(Instruction::ifeq, elseLabel);

    // True branch
    generateExprNode(trueBranch);
    code.emitBranch(Instruction::goto_, endLabel);

    // Else branch
    code.emitLabel(elseLabel);
    generateExprNode(falseBranch);

    code.emitLabel(endLabel);
}

void MethodCodeGenerator::generateIf(ExprNode* cond, ExprNode* body) {
    std::string endLabel = code.generateLabel("endif");

    generateExprNode(cond);

    // Unbox RTL Boolean
    auto* unboxMethodRef = constantPool->addMethodRef(
        "rtl/Boolean",
        "booleanValue",
        "()Z"
    );
    code.emit(Instruction::invokevirtual, unboxMethodRef->index);

    code.emitBranch(Instruction::ifeq, endLabel);

    generateExprNode(body);
    code.emit(Instruction::pop);  // Discard result

    code.emitLabel(endLabel);
}

void MethodCodeGenerator::generateWhile(ExprNode* cond, ExprNode* body) {
    std::string loopStart = code.generateLabel("while_start");
    std::string loopEnd = code.generateLabel("while_end");

    code.emitLabel(loopStart);

    generateExprNode(cond);

    // Unbox RTL Boolean
    auto* unboxMethodRef = constantPool->addMethodRef(
        "rtl/Boolean",
        "booleanValue",
        "()Z"
    );
    code.emit(Instruction::invokevirtual, unboxMethodRef->index);

    code.emitBranch(Instruction::ifeq, loopEnd);

    generateExprNode(body);
    code.emit(Instruction::pop);  // Discard body result

    code.emitBranch(Instruction::goto_, loopStart);

    code.emitLabel(loopEnd);
}

void MethodCodeGenerator::generateDoWhile(ExprNode* cond, ExprNode* body) {
    std::string loopStart = code.generateLabel("do_start");

    code.emitLabel(loopStart);

    generateExprNode(body);
    code.emit(Instruction::pop);

    generateExprNode(cond);

    // Unbox RTL Boolean
    auto* unboxMethodRef = constantPool->addMethodRef(
        "rtl/Boolean",
        "booleanValue",
        "()Z"
    );
    code.emit(Instruction::invokevirtual, unboxMethodRef->index);

    code.emitBranch(Instruction::ifne, loopStart);  // If true, continue
}

void MethodCodeGenerator::generateFor(EnumeratorsNode* enums, ExprNode* body) {
    if (enums == nullptr || enums->generator == nullptr) return;

    // Set for-loop scope for variable resolution
    Scope* prevScope = currentScope;
    if (enums->scope) {
        currentScope = enums->scope;
    }

    GeneratorNode* gen = enums->generator;

    // for (x: Type <- arr) body
    // Compiles to RTL iteration:
    // val iter = arr.iterator()
    // while (iter.hasNext()) {
    //   val x = iter.next()
    //   body
    // }

    std::string loopStart = code.generateLabel("for_start");
    std::string loopEnd = code.generateLabel("for_end");

    // Generate array expression and get iterator
    generateExprNode(gen->expr);

    // Call Array.iterator()
    auto* iteratorMethodRef = constantPool->addMethodRef(
        "rtl/Array",
        "iterator",
        "()Lrtl/Iterator;"
    );
    code.emit(Instruction::invokevirtual, iteratorMethodRef->index);

    // Store iterator to temp slot
    uint16_t iterSlot = getMaxLocals();
    code.emit(Instruction::astore, static_cast<uint8_t>(iterSlot));

    code.emitLabel(loopStart);

    // Check hasNext
    code.emit(Instruction::aload, static_cast<uint8_t>(iterSlot));
    auto* hasNextMethodRef = constantPool->addMethodRef(
        "rtl/Iterator",
        "hasNext",
        "()Lrtl/Boolean;"
    );
    code.emit(Instruction::invokevirtual, hasNextMethodRef->index);

    // Unbox Boolean for comparison
    auto* unboxMethodRef = constantPool->addMethodRef(
        "rtl/Boolean",
        "booleanValue",
        "()Z"
    );
    code.emit(Instruction::invokevirtual, unboxMethodRef->index);

    code.emitBranch(Instruction::ifeq, loopEnd);

    // Get next element
    code.emit(Instruction::aload, static_cast<uint8_t>(iterSlot));
    auto* nextMethodRef = constantPool->addMethodRef(
        "rtl/Iterator",
        "next",
        "()Ljava/lang/Object;"
    );
    code.emit(Instruction::invokevirtual, nextMethodRef->index);

    // Store to loop variable
    std::string varName = gen->fullId->name;
    auto localOpt = method->resolveLocal(varName, currentScope);
    if (localOpt.has_value()) {
        code.emitStore(localOpt.value()->dataType, localSlot(localOpt.value()->number));
    } else {
        // Variable not registered - use temp slot
        uint16_t varSlot = iterSlot + 1;
        code.emit(Instruction::astore, static_cast<uint8_t>(varSlot));
    }

    // Generate body
    generateExprNode(body);
    code.emit(Instruction::pop);

    code.emitBranch(Instruction::goto_, loopStart);

    code.emitLabel(loopEnd);

    // Restore previous scope
    currentScope = prevScope;
}

void MethodCodeGenerator::generateReturn(ExprNode* value) {
    if (value != nullptr) {
        generateExprNode(value);
    }
    code.emitReturn(method->returnType);
}

// ==================== Operators via RTL ====================

void MethodCodeGenerator::generateBinaryOp(const std::string& op, const DataType& leftType, const DataType& rightType) {
    // Stack has: [leftOperand, rightOperand]
    // All binary operations go through RTL method calls: leftOperand.op(rightOperand)

    // Get RTL class for left operand type
    RtlClassMetaInfo* rtlClass = RtlClassMetaInfo::getRtlClassInfo(leftType.toString());
    if (rtlClass == nullptr) {
        throw std::runtime_error("No RTL class for type: " + leftType.toString());
    }

    // Resolve operator method in RTL class
    std::vector<DataType*> argTypes = { const_cast<DataType*>(&rightType) };
    auto methodOpt = rtlClass->resolveMethod(op, argTypes, rtlClass);
    if (!methodOpt.has_value()) {
        throw std::runtime_error("RTL method not found: " + leftType.toString() + "." + op);
    }

    MethodMetaInfo* rtlMethod = methodOpt.value();

    // Use invokestatic: receiver becomes first argument
    // Original descriptor (I)I -> static descriptor (II)I
    std::string origDescriptor = rtlMethod->jvmDescriptor();
    std::string receiverDescriptor = leftType.toJvmDescriptor();
    // Insert receiver type after '('
    std::string staticDescriptor = "(" + receiverDescriptor + origDescriptor.substr(1);

    auto* methodRef = constantPool->addMethodRef(rtlClass->jvmName, rtlMethod->jvmName, staticDescriptor);
    code.emit(Instruction::invokestatic, methodRef->index);
    code.adjustStack(-1);  // pops both operands, pushes result
}

void MethodCodeGenerator::generateUnaryOp(const std::string& op, const DataType& type) {
    // Unary plus is a no-op
    if (op == "_UNARY_PLUS" || op == "unary_$plus") return;

    // All unary operations go through RTL method calls
    RtlClassMetaInfo* rtlClass = RtlClassMetaInfo::getRtlClassInfo(type.toString());
    if (rtlClass == nullptr) return;

    // Map internal op names to Scala method names
    std::string methodName;
    if (op == "_UNARY_MINUS" || op == "unary_$minus") methodName = "unary_-";
    else if (op == "_NOT" || op == "unary_$bang") methodName = "unary_!";
    else if (op == "_BIT_NOT" || op == "unary_$tilde") methodName = "unary_~";
    else return;

    std::vector<DataType*> noArgs;
    auto methodOpt = rtlClass->resolveMethod(methodName, noArgs, rtlClass);
    if (!methodOpt.has_value()) return;

    MethodMetaInfo* rtlMethod = methodOpt.value();

    // Use invokestatic: operand becomes first argument
    // Original descriptor ()I -> static descriptor (I)I
    std::string origDescriptor = rtlMethod->jvmDescriptor();
    std::string operandDescriptor = type.toJvmDescriptor();
    std::string staticDescriptor = "(" + operandDescriptor + origDescriptor.substr(1);

    auto* methodRef = constantPool->addMethodRef(rtlClass->jvmName, rtlMethod->jvmName, staticDescriptor);
    code.emit(Instruction::invokestatic, methodRef->index);
}

// ==================== Helpers ====================

bool MethodCodeGenerator::isStaticMethod() const {
    return method != nullptr && method->name == "main" && currentClass == ctx().mainClass;
}

uint16_t MethodCodeGenerator::localSlot(uint16_t number) const {
    // For instance methods, slot 0 = this, so args/locals shift by +1
    // For static methods, args start at slot 0
    return isStaticMethod() ? number : number + 1;
}

uint16_t MethodCodeGenerator::getLocalSlot(const std::string& varName) {
    auto localOpt = method->resolveLocal(varName, currentScope);
    if (localOpt.has_value()) {
        return localSlot(localOpt.value()->number);
    }
    throw std::runtime_error("Unknown local variable: " + varName);
}

void MethodCodeGenerator::loadThis() {
    code.emit(Instruction::aload_0);
}

void MethodCodeGenerator::loadLocal(uint16_t slot, const DataType& type) {
    code.emitLoad(type, slot);
}

void MethodCodeGenerator::storeLocal(uint16_t slot, const DataType& type) {
    code.emitStore(type, slot);
}

void MethodCodeGenerator::generateArgumentList(ArgumentExprsNode* args) {
    if (args == nullptr || args->exprs == nullptr || args->exprs->exprs == nullptr) return;

    for (ExprNode* arg : *args->exprs->exprs) {
        generateExprNode(arg);
    }
}

void MethodCodeGenerator::invokeMethod(ClassMetaInfo* targetClass, MethodMetaInfo* targetMethod, CallInfo callType) {
    auto* methodRef = constantPool->addMethodRef(targetClass, targetMethod);

    switch (callType) {
        case CallInfo::VIRTUAL:
            code.emit(Instruction::invokevirtual, methodRef->index);
            break;
        case CallInfo::SPECIAL:
            code.emit(Instruction::invokespecial, methodRef->index);
            break;
        case CallInfo::STATIC:
            code.emit(Instruction::invokestatic, methodRef->index);
            break;
        case CallInfo::INTERFACE:
            // invokeinterface has extra bytes
            code.emit(Instruction::invokeinterface, methodRef->index);
            code.emitByte(static_cast<uint8_t>(targetMethod->args.size() + 1));  // count
            code.emitByte(0);  // must be 0
            break;
    }

    // Adjust stack based on method signature
    int stackChange = -1;  // pops this
    stackChange -= targetMethod->args.size();  // pops args
    if (targetMethod->returnType.kind != DataType::Kind::Unit) {
        stackChange += 1;  // pushes result
    }
    code.adjustStack(stackChange);
}

void MethodCodeGenerator::invokeConstructor(ClassMetaInfo* targetClass, const std::vector<DataType>& argTypes) {
    // Look for constructors in multiple places:
    // 1. Under name "this" (secondary constructors)
    // 2. Under class name (primary constructor)
    // 3. Methods with isPrimaryConstructor flag

    std::vector<MethodMetaInfo*> constructors;

    // Check for "this" constructors
    auto thisIt = targetClass->methods.find("this");
    if (thisIt != targetClass->methods.end()) {
        for (MethodMetaInfo* ctor : thisIt->second) {
            constructors.push_back(ctor);
        }
    }

    // Check for primary constructor under class name
    auto classNameIt = targetClass->methods.find(targetClass->name);
    if (classNameIt != targetClass->methods.end()) {
        for (MethodMetaInfo* ctor : classNameIt->second) {
            if (ctor->isPrimaryConstructor) {
                constructors.push_back(ctor);
            }
        }
    }

    // Also scan all methods for isPrimaryConstructor flag
    for (auto& [name, methodList] : targetClass->methods) {
        for (MethodMetaInfo* m : methodList) {
            if (m->isPrimaryConstructor) {
                // Avoid duplicates
                bool found = false;
                for (auto* c : constructors) {
                    if (c == m) { found = true; break; }
                }
                if (!found) constructors.push_back(m);
            }
        }
    }

    std::string descriptor = "(";
    for (const DataType& dt : argTypes) {
        descriptor += dt.toJvmDescriptor();
    }
    descriptor += ")V";

    // If no constructors found, use default ()V
    if (constructors.empty()) {
        auto* methodRef = constantPool->addMethodRef(targetClass->jvmName, "<init>", descriptor);
        code.emit(Instruction::invokespecial, methodRef->index);
        code.adjustStack(-1);
        return;
    }

    // Find constructor with matching args
    for (MethodMetaInfo* ctor : constructors) {
        if (ctor->args.size() == argTypes.size()) {
            // TODO: Check types match more precisely
            auto* methodRef = constantPool->addMethodRef(targetClass->jvmName, "<init>", descriptor);
            code.emit(Instruction::invokespecial, methodRef->index);
            code.adjustStack(-1 - static_cast<int>(argTypes.size()));
            return;
        }
    }

    // If we have a no-arg call and any constructor exists, try default
    if (argTypes.empty()) {
        auto* methodRef = constantPool->addMethodRef(targetClass->jvmName, "<init>", descriptor);
        code.emit(Instruction::invokespecial, methodRef->index);
        code.adjustStack(-1);
        return;
    }

    throw std::runtime_error("No matching constructor found for " + targetClass->name +
                             " with " + std::to_string(argTypes.size()) + " arguments");
}

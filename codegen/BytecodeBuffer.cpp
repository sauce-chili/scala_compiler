#include "BytecodeBuffer.h"
#include "semantic/constants/ConstantPoolBuilder.h"
#include <stdexcept>

// ==================== Stack Effect Helpers ====================

// Get stack effect of an instruction (how much it changes stack depth)
static int getStackEffect(Instruction op) {
    switch (op) {
        // Push 1 value
        case Instruction::aconst_null:
        case Instruction::iconst_m1:
        case Instruction::iconst_0:
        case Instruction::iconst_1:
        case Instruction::iconst_2:
        case Instruction::iconst_3:
        case Instruction::iconst_4:
        case Instruction::iconst_5:
        case Instruction::fconst_0:
        case Instruction::fconst_1:
        case Instruction::fconst_2:
        case Instruction::bipush:
        case Instruction::sipush:
        case Instruction::ldc:
        case Instruction::ldc_w:
        case Instruction::iload:
        case Instruction::fload:
        case Instruction::aload:
        case Instruction::iload_0:
        case Instruction::iload_1:
        case Instruction::iload_2:
        case Instruction::iload_3:
        case Instruction::fload_0:
        case Instruction::fload_1:
        case Instruction::fload_2:
        case Instruction::fload_3:
        case Instruction::aload_0:
        case Instruction::aload_1:
        case Instruction::aload_2:
        case Instruction::aload_3:
        case Instruction::dup:
            return 1;

        // Push 2 values (long/double)
        case Instruction::lconst_0:
        case Instruction::lconst_1:
        case Instruction::dconst_0:
        case Instruction::dconst_1:
        case Instruction::ldc2_w:
        case Instruction::lload:
        case Instruction::dload:
        case Instruction::lload_0:
        case Instruction::lload_1:
        case Instruction::lload_2:
        case Instruction::lload_3:
        case Instruction::dload_0:
        case Instruction::dload_1:
        case Instruction::dload_2:
        case Instruction::dload_3:
        case Instruction::dup2:
            return 2;

        // Pop 1 value
        case Instruction::pop:
        case Instruction::istore:
        case Instruction::fstore:
        case Instruction::astore:
        case Instruction::istore_0:
        case Instruction::istore_1:
        case Instruction::istore_2:
        case Instruction::istore_3:
        case Instruction::fstore_0:
        case Instruction::fstore_1:
        case Instruction::fstore_2:
        case Instruction::fstore_3:
        case Instruction::astore_0:
        case Instruction::astore_1:
        case Instruction::astore_2:
        case Instruction::astore_3:
        case Instruction::ifeq:
        case Instruction::ifne:
        case Instruction::iflt:
        case Instruction::ifge:
        case Instruction::ifgt:
        case Instruction::ifle:
        case Instruction::ifnull:
        case Instruction::ifnonnull:
        case Instruction::ireturn:
        case Instruction::freturn:
        case Instruction::areturn:
        case Instruction::athrow:
        case Instruction::monitorenter:
        case Instruction::monitorexit:
            return -1;

        // Pop 2 values
        case Instruction::pop2:
        case Instruction::lstore:
        case Instruction::dstore:
        case Instruction::lstore_0:
        case Instruction::lstore_1:
        case Instruction::lstore_2:
        case Instruction::lstore_3:
        case Instruction::dstore_0:
        case Instruction::dstore_1:
        case Instruction::dstore_2:
        case Instruction::dstore_3:
        case Instruction::if_icmpeq:
        case Instruction::if_icmpne:
        case Instruction::if_icmplt:
        case Instruction::if_icmpge:
        case Instruction::if_icmpgt:
        case Instruction::if_icmple:
        case Instruction::if_acmpeq:
        case Instruction::if_acmpne:
        case Instruction::lreturn:
        case Instruction::dreturn:
            return -2;

        // Binary operations: pop 2, push 1 = -1
        case Instruction::iadd:
        case Instruction::isub:
        case Instruction::imul:
        case Instruction::idiv:
        case Instruction::irem:
        case Instruction::ishl:
        case Instruction::ishr:
        case Instruction::iushr:
        case Instruction::iand:
        case Instruction::ior:
        case Instruction::ixor:
        case Instruction::fadd:
        case Instruction::fsub:
        case Instruction::fmul:
        case Instruction::fdiv:
        case Instruction::frem:
        case Instruction::fcmpl:
        case Instruction::fcmpg:
            return -1;

        // Long/double binary: pop 4, push 2 = -2
        case Instruction::ladd:
        case Instruction::lsub:
        case Instruction::lmul:
        case Instruction::ldiv_:
        case Instruction::lrem:
        case Instruction::land:
        case Instruction::lor:
        case Instruction::lxor:
        case Instruction::dadd:
        case Instruction::dsub:
        case Instruction::dmul:
        case Instruction::ddiv:
        case Instruction::drem:
            return -2;

        // lcmp, dcmpl, dcmpg: pop 4, push 1 = -3
        case Instruction::lcmp:
        case Instruction::dcmpl:
        case Instruction::dcmpg:
            return -3;

        // Unary operations: no change
        case Instruction::ineg:
        case Instruction::fneg:
        case Instruction::i2f:
        case Instruction::f2i:
        case Instruction::i2b:
        case Instruction::i2c:
        case Instruction::i2s:
        case Instruction::checkcast:
        case Instruction::instanceof_:
            return 0;

        case Instruction::lneg:
        case Instruction::dneg:
        case Instruction::l2d:
        case Instruction::d2l:
            return 0;

        // Widening conversions: +1 (int to long/double)
        case Instruction::i2l:
        case Instruction::i2d:
        case Instruction::f2l:
        case Instruction::f2d:
            return 1;

        // Narrowing conversions: -1 (long/double to int/float)
        case Instruction::l2i:
        case Instruction::l2f:
        case Instruction::d2i:
        case Instruction::d2f:
            return -1;

        // Array load: pop 2 (array, index), push 1 = -1
        case Instruction::iaload:
        case Instruction::faload:
        case Instruction::aaload:
        case Instruction::baload:
        case Instruction::caload:
        case Instruction::saload:
            return -1;

        // Array load long/double: pop 2, push 2 = 0
        case Instruction::laload:
        case Instruction::daload:
            return 0;

        // Array store: pop 3 (array, index, value) = -3
        case Instruction::iastore:
        case Instruction::fastore:
        case Instruction::aastore:
        case Instruction::bastore:
        case Instruction::castore:
        case Instruction::sastore:
            return -3;

        // Array store long/double: pop 4 = -4
        case Instruction::lastore:
        case Instruction::dastore:
            return -4;

        // arraylength: pop 1, push 1 = 0
        case Instruction::arraylength:
            return 0;

        // new: push 1
        case Instruction::new_:
            return 1;

        // newarray, anewarray: pop 1 (count), push 1 (array) = 0
        case Instruction::newarray:
        case Instruction::anewarray:
            return 0;

        // No stack change
        case Instruction::nop:
        case Instruction::goto_:
        case Instruction::goto_w:
        case Instruction::return_:
        case Instruction::iinc:
        case Instruction::wide:
            return 0;

        // Field/method ops have variable effects - handled separately
        case Instruction::getstatic:
        case Instruction::putstatic:
        case Instruction::getfield:
        case Instruction::putfield:
        case Instruction::invokevirtual:
        case Instruction::invokespecial:
        case Instruction::invokestatic:
        case Instruction::invokeinterface:
        case Instruction::invokedynamic:
            return 0;  // Must be tracked manually

        default:
            return 0;
    }
}

void BytecodeBuffer::updateMaxStack(int16_t delta) {
    currentStack += delta;
    if (currentStack > maxStack) {
        maxStack = currentStack;
    }
    if (currentStack < 0) {
        currentStack = 0;  // Shouldn't happen, but be safe
    }
}

// ==================== Basic Emit ====================

void BytecodeBuffer::emit(Instruction op) {
    buffer.push_back(static_cast<uint8_t>(op));
    updateMaxStack(getStackEffect(op));
}

void BytecodeBuffer::emit(Instruction op, uint8_t operand) {
    buffer.push_back(static_cast<uint8_t>(op));
    buffer.push_back(operand);
    updateMaxStack(getStackEffect(op));
}

void BytecodeBuffer::emit(Instruction op, uint16_t operand) {
    buffer.push_back(static_cast<uint8_t>(op));
    buffer.push_back(static_cast<uint8_t>((operand >> 8) & 0xFF));
    buffer.push_back(static_cast<uint8_t>(operand & 0xFF));
    updateMaxStack(getStackEffect(op));
}

void BytecodeBuffer::emit(Instruction op, uint8_t op1, uint8_t op2) {
    buffer.push_back(static_cast<uint8_t>(op));
    buffer.push_back(op1);
    buffer.push_back(op2);
    updateMaxStack(getStackEffect(op));
}

void BytecodeBuffer::emitByte(uint8_t b) {
    buffer.push_back(b);
}

void BytecodeBuffer::emitShort(uint16_t value) {
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
}

void BytecodeBuffer::emitInt(uint32_t value) {
    buffer.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    buffer.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    buffer.push_back(static_cast<uint8_t>(value & 0xFF));
}

// ==================== Branch Instructions ====================

void BytecodeBuffer::emitBranch(Instruction op, const std::string& label) {
    buffer.push_back(static_cast<uint8_t>(op));

    // Check if label already defined (backward jump)
    auto it = labels.find(label);
    if (it != labels.end()) {
        // Backward jump - calculate offset from instruction start
        int32_t offset = it->second - static_cast<int32_t>(buffer.size() - 1);
        emitShort(static_cast<uint16_t>(offset));
    } else {
        // Forward jump - record position and emit placeholder
        pendingJumps.push_back({static_cast<int32_t>(buffer.size() - 1), label});
        emitShort(0);  // Placeholder for offset
    }

    updateMaxStack(getStackEffect(op));
}

void BytecodeBuffer::emitLabel(const std::string& label) {
    if (labels.count(label) > 0) {
        throw std::runtime_error("Duplicate label: " + label);
    }
    labels[label] = static_cast<int32_t>(buffer.size());
}

void BytecodeBuffer::resolveLabels() {
    for (const auto& [pos, label] : pendingJumps) {
        auto it = labels.find(label);
        if (it == labels.end()) {
            throw std::runtime_error("Undefined label: " + label);
        }

        // Calculate offset from instruction start (pos) to label
        int32_t offset = it->second - pos;

        // Write offset at position + 1 (after opcode)
        size_t offsetPos = pos + 1;
        buffer[offsetPos] = static_cast<uint8_t>((offset >> 8) & 0xFF);
        buffer[offsetPos + 1] = static_cast<uint8_t>(offset & 0xFF);
    }
    pendingJumps.clear();
}

// ==================== Smart Emit Methods ====================

void BytecodeBuffer::emitIconst(int32_t value, ConstantPoolBuilder* pool) {
    if (value >= -1 && value <= 5) {
        // Use ICONST_x
        emit(static_cast<Instruction>(static_cast<uint8_t>(Instruction::iconst_0) + value));
    } else if (value >= -128 && value <= 127) {
        // Use BIPUSH
        emit(Instruction::bipush, static_cast<uint8_t>(value));
    } else if (value >= -32768 && value <= 32767) {
        // Use SIPUSH
        emit(Instruction::sipush, static_cast<uint16_t>(value));
    } else {
        // Use LDC - need constant pool
        if (pool == nullptr) {
            throw std::runtime_error("Constant pool required for LDC with value: " + std::to_string(value));
        }
        auto* constant = pool->addInt(value);
        if (constant->index <= 255) {
            emit(Instruction::ldc, static_cast<uint8_t>(constant->index));
        } else {
            emit(Instruction::ldc_w, constant->index);
        }
    }
}

void BytecodeBuffer::emitLoad(const DataType& type, uint16_t slot) {
    // Determine base instruction based on type
    Instruction base;
    bool isCategory2 = false;  // long/double take 2 slots

    switch (type.kind) {
        case DataType::Kind::Bool:
        case DataType::Kind::Double:
        case DataType::Kind::Int:
        case DataType::Kind::Char:
            base = Instruction::aload;
            break;
        case DataType::Kind::String:
        case DataType::Kind::Class:
        case DataType::Kind::Array:
        case DataType::Kind::Any:
        case DataType::Kind::Null:
            base = Instruction::aload;
            break;
        case DataType::Kind::Unit:
        case DataType::Kind::Undefined:
        default:
            throw std::runtime_error("Cannot load value of type: " + type.toString());
    }

    // Use short form if slot <= 3
    if (slot <= 3) {
        Instruction shortForm;
        if (base == Instruction::iload) {
            shortForm = static_cast<Instruction>(static_cast<uint8_t>(Instruction::iload_0) + slot);
        } else if (base == Instruction::aload) {
            shortForm = static_cast<Instruction>(static_cast<uint8_t>(Instruction::aload_0) + slot);
        } else if (base == Instruction::fload) {
            shortForm = static_cast<Instruction>(static_cast<uint8_t>(Instruction::fload_0) + slot);
        } else if (base == Instruction::dload) {
            shortForm = static_cast<Instruction>(static_cast<uint8_t>(Instruction::dload_0) + slot);
        } else if (base == Instruction::lload) {
            shortForm = static_cast<Instruction>(static_cast<uint8_t>(Instruction::lload_0) + slot);
        } else {
            emit(base, static_cast<uint8_t>(slot));
            return;
        }
        emit(shortForm);
    } else if (slot <= 255) {
        emit(base, static_cast<uint8_t>(slot));
    } else {
        // Need wide prefix
        emit(Instruction::wide);
        emit(base, slot);
    }
}

void BytecodeBuffer::emitStore(const DataType& type, uint16_t slot) {
    Instruction base;

    switch (type.kind) {
        case DataType::Kind::Bool:
        case DataType::Kind::Double:
        case DataType::Kind::Int:
        case DataType::Kind::Char:
            base = Instruction::astore;
            break;
        case DataType::Kind::Class:
        case DataType::Kind::String:
        case DataType::Kind::Array:
        case DataType::Kind::Any:
        case DataType::Kind::Null:
            base = Instruction::astore;
            break;
        case DataType::Kind::Unit:
        case DataType::Kind::Undefined:
        default:
            throw std::runtime_error("Cannot store value of type: " + type.toString());
    }

    // Use short form if slot <= 3
    if (slot <= 3) {
        Instruction shortForm;
        if (base == Instruction::istore) {
            shortForm = static_cast<Instruction>(static_cast<uint8_t>(Instruction::istore_0) + slot);
        } else if (base == Instruction::astore) {
            shortForm = static_cast<Instruction>(static_cast<uint8_t>(Instruction::astore_0) + slot);
        } else if (base == Instruction::fstore) {
            shortForm = static_cast<Instruction>(static_cast<uint8_t>(Instruction::fstore_0) + slot);
        } else if (base == Instruction::dstore) {
            shortForm = static_cast<Instruction>(static_cast<uint8_t>(Instruction::dstore_0) + slot);
        } else if (base == Instruction::lstore) {
            shortForm = static_cast<Instruction>(static_cast<uint8_t>(Instruction::lstore_0) + slot);
        } else {
            emit(base, static_cast<uint8_t>(slot));
            return;
        }
        emit(shortForm);
    } else if (slot <= 255) {
        emit(base, static_cast<uint8_t>(slot));
    } else {
        // Need wide prefix
        emit(Instruction::wide);
        emit(base, slot);
    }
}

void BytecodeBuffer::emitReturn(const DataType& type) {
    switch (type.kind) {
        case DataType::Kind::Int:
        case DataType::Kind::Bool:
        case DataType::Kind::Double:
        case DataType::Kind::Char:
            emit(Instruction::areturn);
            break;
        case DataType::Kind::String:
        case DataType::Kind::Class:
        case DataType::Kind::Array:
        case DataType::Kind::Any:
            emit(Instruction::areturn);
            break;
        case DataType::Kind::Unit:
        case DataType::Kind::Undefined:
        case DataType::Kind::Null:
            emit(Instruction::return_);
            break;
    }
}

void BytecodeBuffer::emitArrayLoad(const DataType& elementType) {
    switch (elementType.kind) {
        case DataType::Kind::Int:
        case DataType::Kind::Bool:
        case DataType::Kind::Char:
            emit(Instruction::caload);
            break;
        case DataType::Kind::Double:
        case DataType::Kind::String:
        case DataType::Kind::Class:
        case DataType::Kind::Array:
        case DataType::Kind::Any:
            emit(Instruction::aaload);
            break;
        default:
            throw std::runtime_error("Cannot load array element of type: " + elementType.toString());
    }
}

void BytecodeBuffer::emitArrayStore(const DataType& elementType) {
    switch (elementType.kind) {
        case DataType::Kind::Int:
        case DataType::Kind::Bool:
        case DataType::Kind::Char:
            emit(Instruction::castore);
            break;
        case DataType::Kind::Double:
        case DataType::Kind::String:
        case DataType::Kind::Class:
        case DataType::Kind::Array:
        case DataType::Kind::Any:
            emit(Instruction::aastore);
            break;
        default:
            throw std::runtime_error("Cannot store array element of type: " + elementType.toString());
    }
}

// ==================== Stack Tracking ====================

void BytecodeBuffer::adjustStack(int16_t delta) {
    updateMaxStack(delta);
}

// ==================== Utilities ====================

bool BytecodeBuffer::hasLabel(const std::string& label) const {
    return labels.count(label) > 0;
}

std::string BytecodeBuffer::generateLabel(const std::string& prefix) {
    return prefix + std::to_string(labelCounter++);
}

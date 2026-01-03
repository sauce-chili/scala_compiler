//
// Created by Roman on 1/3/2026.
//

#include "Types.h"

std::string modifierToString(ModifierType m) {
    switch (m) {
        case PRIVATE:   return "PRIVATE";
        case PROTECTED: return "PROTECTED";
        case OPEN:      return "OPEN";
        case FINAL:     return "FINAL";
        case OVERRIDE:  return "OVERRIDE";
    }
}
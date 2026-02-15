//
// Created by Dmitry Smirnov on 22.01.2026.
//
#pragma once

#ifndef SCALA_LEXER_LOGGER_H
#define SCALA_LEXER_LOGGER_H

#include <iostream>

#ifndef DEBUG_ENABLE
    #define DEBUG_ENABLE 0 // 1 - включено, 0 - выключено
#endif

#if DEBUG_ENABLE
    #define debug(x) std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << " > " << x << std::endl
#else
    // затычка, удалиться оптимизатором
    #define debug(x) do {} while (0)
#endif

#endif //SCALA_LEXER_LOGGER_H
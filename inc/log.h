// Copyright (c) 2023, Marvin Borner <dev@marvinborner.de>
// SPDX-License-Identifier: MIT

#ifndef SHARING_LOG_H
#define SHARING_LOG_H

void debug(const char *format, ...);
void debug_enable(int enable);
void fatal(const char *format, ...) __attribute__((noreturn));

#endif

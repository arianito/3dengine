#pragma once


typedef void (*FunctionPtr)();

typedef struct {
    FunctionPtr load;
    FunctionPtr update;
    FunctionPtr unload;
} Level;



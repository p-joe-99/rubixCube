//
// Created by Copper on 2019/10/23.
//

#ifndef CPPPLAYGROUND06_RECORD_H
#define CPPPLAYGROUND06_RECORD_H

#include "util.h"

typedef struct
{
    double time = 0;
    int rotType = -1;
} Record;

static std::vector<Record> records{};
static int recordPointer = 0;

void _pushRecord(double time, int rotType, CubeState state)
{
    if (recordPointer != records.size())
        records.resize(recordPointer);

    Record record;
    record.time = time;
    record.rotType = rotType;

    recordPointer++;
    records.push_back(record);
}

void _goBack()
{
    recordPointer--;
}

void _goForward()
{
    recordPointer++;
}


#endif //CPPPLAYGROUND06_RECORD_H

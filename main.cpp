#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

//TODO реализовать Dump
//TODO реализовать stackError
//TODO пределать free под канарейки                          // DONE
//TODO minimumSz не ноль                                     //DONE
//#include "IsE_Stack.h"

#define asserted(x) || !printf("[Error] on line number: %s\n" "return code: %s", __LINE__, x)
#define ARGNAME(x) #x

typedef double stackType;
const stackType poison = NAN;

const double ENLARGE_BIG_COEFFICIENT = 2;
const double ENLARGE_SMALL_COEFFICIENT = 1.5;
const double REDUCE_COEFFICIENT = 0.4;
const unsigned long long CANARY_VALUE = 0xBADADEEEBADADAAA;

struct Stack
{
    long long sufferer1;
    size_t capacity;
    size_t sz;
    size_t minimumSz;
    int errCode;
    void* data;
    long long stackHash;
    long long sufferer2;
};

enum StackErrorCode
{
    NO_STACK_ERRORS = 0,
    BAD_SIZE = 1,
    UNACCEPTABLE_Member = 2,
    BAD_MINIMUM_SIZE = 3
};

enum FunctionErrorCode
{
    NO_FUNCTION_ERRORS = 0,
    NO_FREE_MEMORY = -1,
    NULL_POINTER_IN_ARGUMENT = -2,
    BAD_ARGUMENT = -3
};

FILE* logs = fopen("logs_stack.txt","a");

int stackConstructor(struct Stack* thou, size_t capacity);

void stackDestructor(struct Stack* thou);

int pushIntoStack(struct Stack* thou, stackType newMember);

int enlargeStack(struct Stack* thou);

int reduceStack(struct Stack* thou);

stackType pop(struct Stack* thou);

stackType front(struct Stack* thou);

int stackError(struct Stack* thou);

int stackDump(struct Stack* thou, char* reason, int line);

int stackResize(struct Stack* thou, size_t newCapacity);

void fillStackWithPoison(struct Stack* thou, size_t beginInd, size_t endInd);

long long makeHash(struct Stack* thou);

long long ROL(long long value);

long long ROR(long long value);

int main() {

    fseek(logs, 0L, SEEK_SET);
    Stack stk;
    stackConstructor(&stk, 2);
    pushIntoStack(&stk, 10.0);
    stackType a = pop(&stk);
    printf("%lg\n", a);
    a = pop(&stk);
    printf("%lg\n", a);
    printf("%zu %zu\n", stk.sz, stk.capacity);
    pushIntoStack(&stk, 30.0);
    pushIntoStack(&stk, 40.0);
    pushIntoStack(&stk, 50.0);
    a = pop(&stk);
    printf("%lg\n", a);
    a = pop(&stk);
    printf("%lg\n", a);
    a = pop(&stk);
    printf("%lg\n", a);
    printf("%zu %zu\n", stk.sz, stk.capacity);

    fclose(logs);
    return 0;
}

//--------------------------------------------------------

int stackConstructor(struct Stack* thou, size_t capacity)
{
    assert(thou);

    //thou = (Stack*)calloc(1, sizeof(*thou) + sizeof(thou->data) * capacity) ;
    thou->data = (stackType*)calloc(1, (capacity) * sizeof(stackType) + 2 * sizeof(unsigned long long) );

    *((unsigned long long *)thou->data) = 0xBADADEEEBADADAAA;
    thou->data = ((unsigned long long *)thou->data + 1);
    *((unsigned long long *)((stackType*)thou->data + capacity)) = 0xBADADEEEBADADAAA;

    if(!thou->data)
        return NO_FREE_MEMORY;

    thou->minimumSz = (capacity < 10) ? capacity : 10;
    thou->capacity = capacity;
    thou->sz = 0;
    thou->errCode = NO_STACK_ERRORS;
    thou->sufferer1 = 0xBADADEEEBADADAAA;
    thou->sufferer2 = 0xBADADEEEBADADAAA;
    thou->stackHash = makeHash(thou);

    fillStackWithPoison(thou, 0,capacity - 1);
    return 0;
}

//------------------------------------------------------------

void stackDestructor(struct Stack* thou)
{
    assert(thou);

    free((unsigned long long*)thou->data - 1);

    thou->data = NULL;
}

//------------------------------------------------------------

int pushIntoStack(struct Stack* thou, stackType newMember)
{
    assert(thou);

    if (thou->sz == thou->capacity)
        if(enlargeStack(thou))
        {
            stackDump(thou,ARGNAME(NO_FREE_MEMORY), __LINE__);
            return NO_FREE_MEMORY;
        }


    *((stackType*)thou->data + (thou->sz++)) = newMember;
    printf("%lg\n",*((stackType*)thou->data + (thou->sz-1)));
    stackDump(thou,ARGNAME(OK), __LINE__);
    return NO_FUNCTION_ERRORS;
}

//-------------------------------------------------------
int enlargeStack(struct Stack* thou)
{
    size_t oldCapacity = thou->capacity;
    if(!stackResize(thou, (size_t)(thou->capacity * ENLARGE_BIG_COEFFICIENT)))
    {
        fillStackWithPoison(thou, oldCapacity, thou->capacity - 1);
        return  NO_FUNCTION_ERRORS;
    }

    if(!stackResize(thou, (size_t)(thou->capacity * ENLARGE_SMALL_COEFFICIENT)))
    {
        fillStackWithPoison(thou, oldCapacity, thou->capacity - 1);
        return  NO_FUNCTION_ERRORS;
    }

    if(!stackResize(thou, thou->capacity + 100))
    {
        fillStackWithPoison(thou, oldCapacity, thou->capacity - 1);
        return  NO_FUNCTION_ERRORS;
    }

    if(!stackResize(thou, thou->capacity + 1))
    {
        fillStackWithPoison(thou, oldCapacity, thou->capacity - 1);
        return  NO_FUNCTION_ERRORS;
    }

    return NO_FREE_MEMORY;
}

//----------------------------------------------------------------

int reduceStack(struct Stack* thou)
{
    if(!stackResize(thou, thou->capacity * REDUCE_COEFFICIENT))
        return  NO_FUNCTION_ERRORS;

    return NO_FREE_MEMORY;
}

//----------------------------------------------------------------

stackType pop(struct  Stack* thou)
{
    if(stackError(thou))
    {
        printf("catched, %d\n", stackError(thou));
        return poison;
    }


    if(thou->sz > 0){
        stackType returnMember;
        thou->sz--;
        returnMember = *((stackType*)thou->data + thou->sz);
        fillStackWithPoison(thou, thou->sz, thou->sz);

        if (thou->sz <= thou->capacity * REDUCE_COEFFICIENT && thou->sz > thou->minimumSz)
            reduceStack(thou);

        return returnMember;
    }

    else
        return poison;
}

//-----------------------------------------------------------------------

int stackError(struct Stack* thou)
{
    if(!thou)
        return NULL_POINTER_IN_ARGUMENT;

    if(thou->sz > thou->capacity)
    {
        thou->errCode = BAD_SIZE;
        return BAD_ARGUMENT;
    }

    for(size_t ind = thou->sz; ind < thou->capacity; ++ind )
        if(isfinite(*((stackType*)thou->data + ind)))
        {
            thou->errCode = UNACCEPTABLE_Member;
            return BAD_ARGUMENT;
        }

    if(thou->minimumSz == 0 || thou->minimumSz > thou->capacity)
        return BAD_MINIMUM_SIZE;

    return  NO_STACK_ERRORS;
}

//-------------------------------------------------------------------------

int stackResize(struct  Stack* thou, size_t newCapacity)
{
    //size_t sz = thou->sz;
    //int errcode = thou->errCode;
    //struct Stack* temp =(struct Stack*)realloc(thou, sizeof(*thou) + sizeof(thou->data) * newCapacity);
    stackType* temp = (stackType*)realloc((unsigned long long*)thou->data - 1, sizeof(*temp) + 2 * sizeof(unsigned long long));
    if(!temp)
        return NO_FREE_MEMORY;

    else
    {
        *(unsigned long long*)temp = 0xBADADEEEBADADAAA;
        thou->data = (unsigned long long*)temp + 1;
        *(unsigned long long*)((stackType*)thou->data + newCapacity) = 0xBADADEEEBADADAAA;
        thou->capacity = newCapacity;
        return NO_FUNCTION_ERRORS;
    }
}

//-----------------------------------------------------------------------------

void fillStackWithPoison(struct Stack* thou, size_t beginInd, size_t endInd)
{
    for(size_t ind = beginInd; ind <= endInd; ++ind)
        *((stackType*)thou->data + ind) = poison;
}

//--------------------------------------------------------------------------

int stackDump(struct Stack* thou, char* problem, int line)
{
    if(!thou)
    {
        fprintf(logs, "ERROR_Stack(NULL_PTR");
        return NULL_POINTER_IN_ARGUMENT;
    }
    char marker = '*';
    int sz = thou->sz;
    fprintf(logs, "executed on line %d\n", line);
    fprintf(logs, "Stack(%s) [%p] \n { \n", problem, thou);
    fprintf(logs, "\t" "sz       = %zu\n", thou->sz);
    fprintf(logs, "\t" "capacity = %zu\n", thou->capacity);
    fprintf(logs, "\t" "data [%p]\n", thou->data);
    fprintf(logs, "\t\t" "{\n");
    for(int ind = 0; ind < thou->capacity; ++ind)
    {
        if(ind >= thou->sz)
            marker = ' ';
        fprintf(logs, "\t\t" "%c[%d] %lg\n",marker, ind, ((stackType*)thou->data)[ind]);
    }
    fprintf(logs, "\t\t" "}\n");
    fprintf(logs, "\t" "}\n");
}

//------------------------------------------------------------------------

long long makeHash(struct Stack* thou)
{
    long long oldHash = thou->stackHash;
    thou->stackHash = 0;
    long long newHash = 0;

    char* stackBuff = (char*)calloc(1,sizeof(*thou));
    for(int ind = 0; ind < sizeof(*thou); ++ind)
        newHash = ROR(newHash) + stackBuff[ind];

    char* dataBuff = (char*)calloc(1,sizeof(*(stackType*)thou->data));
    for(int ind = 0; ind < sizeof(*thou); ++ind)
        newHash = ROR(newHash) + dataBuff[ind];

    thou->stackHash = oldHash;
    return newHash;
}

//-----------------------------------------------------------

long long ROL(long long value)
{
    return (value << 1 | value >> 63);
}

//--------------------------------------------------

long long ROR(long long value)
{
    return (value >> 1 | value << 63);
}


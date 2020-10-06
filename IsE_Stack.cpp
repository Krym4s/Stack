//
// Created by egolg on 05.10.2020.
//

//--------------------------------------------------------
#include "IsE_Stack.h"
#include "string.h"

#define ARGNAME(x) #x
#define NCAN(a,b,c,d)  a != CANARY_VALUE || b != CANARY_VALUE || c != CANARY_VALUE || d != CANARY_VALUE

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
    long long stackHash;
    void* data;
    long long sufferer2;
};

enum StackErrorCode
{
    NO_STACK_ERRORS = 0,
    BAD_SIZE = 1,
    UNACCEPTABLE_Member = 2,
    BAD_MINIMUM_SIZE = 3,
    CANARY_VALUE_CHANGED = 4,
    BAD_HASH = 5
};

enum FunctionErrorCode
{
    NO_FUNCTION_ERRORS = 0,
    NO_FREE_MEMORY = -1,
    NULL_POINTER_IN_ARGUMENT = -2,
    BAD_ARGUMENT = -3
};

FILE* logs = fopen("logs_stack.txt","w");

int stackConstructor(struct Stack* thou, size_t capacity)
{
    assert(thou);

    thou->data = (stackType*)calloc(1, (capacity) * sizeof(stackType) + 2 * sizeof(unsigned long long) );

    if (!thou->data)
        return NO_FREE_MEMORY;

    *((unsigned long long *)thou->data) = CANARY_VALUE;
    thou->data = ((unsigned long long *)thou->data + 1);
    *((unsigned long long *)((stackType*)thou->data + capacity)) = CANARY_VALUE;

    if(!thou->data)
        return NO_FREE_MEMORY;

    thou->minimumSz = (capacity < 10) ? capacity : 10;
    thou->capacity = capacity;
    thou->sz = 0;
    thou->errCode = NO_STACK_ERRORS;
    thou->sufferer1 = CANARY_VALUE;
    thou->sufferer2 = CANARY_VALUE;


    fillStackWithPoison(thou, 0,capacity - 1);

    thou->stackHash = makeHash(thou);
    return 0;
}

//------------------------------------------------------------

void stackDestructor(struct Stack* thou)
{
    assert(thou);

    free((stackType*)((unsigned long long*)thou->data - 1));

    free(thou);
}

//------------------------------------------------------------

int pushIntoStack(struct Stack* thou, stackType newMember)
{
    assert(thou);
    stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );

    if (thou->sz == thou->capacity)
        if(enlargeStack(thou))
        {
            stackDump(thou,ARGNAME(NO_FREE_MEMORY), __LINE__);
            return NO_FREE_MEMORY;
        }


    *((stackType*)thou->data + (thou->sz++)) = newMember;
    //printf("%lg\n",*((stackType*)thou->data + (thou->sz-1)));
    thou->stackHash = makeHash(thou);

    stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
    return NO_FUNCTION_ERRORS;
}

//-------------------------------------------------------
int enlargeStack(struct Stack* thou)
{
    stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );

    size_t oldCapacity = thou->capacity;
    if(!stackResize(thou, (size_t)(thou->capacity * ENLARGE_BIG_COEFFICIENT)))
    {
        fillStackWithPoison(thou, oldCapacity, thou->capacity - 1);
        thou->stackHash = makeHash(thou);
        stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
        return  NO_FUNCTION_ERRORS;
    }

    if(!stackResize(thou, (size_t)(thou->capacity * ENLARGE_SMALL_COEFFICIENT)))
    {
        fillStackWithPoison(thou, oldCapacity, thou->capacity - 1);
        thou->stackHash = makeHash(thou);
        stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
        return  NO_FUNCTION_ERRORS;
    }

    if(!stackResize(thou, thou->capacity + 100))
    {
        fillStackWithPoison(thou, oldCapacity, thou->capacity - 1);
        thou->stackHash = makeHash(thou);
        stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
        return  NO_FUNCTION_ERRORS;
    }

    if(!stackResize(thou, thou->capacity + 1))
    {
        fillStackWithPoison(thou, oldCapacity, thou->capacity - 1);
        thou->stackHash = makeHash(thou);
        stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
        return  NO_FUNCTION_ERRORS;
    }

    stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
    return NO_FREE_MEMORY;
}

//----------------------------------------------------------------

int reduceStack(struct Stack* thou)
{
    stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );

    if(!stackResize(thou, thou->capacity * REDUCE_COEFFICIENT))
    {
        thou->stackHash = makeHash(thou);
        stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
        return  NO_FUNCTION_ERRORS;
    }

    stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
    return NO_FREE_MEMORY;
}

//----------------------------------------------------------------

stackType pop(struct  Stack* thou)
{
    stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );

    if(stackError(thou))
    {
        stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
        return poison;
    }


    if(thou->sz > 0){
        stackType returnMember;
        thou->sz--;
        returnMember = *((stackType*)thou->data + thou->sz);
        fillStackWithPoison(thou, thou->sz, thou->sz);

        if (thou->sz <= thou->capacity * REDUCE_COEFFICIENT && thou->capacity * REDUCE_COEFFICIENT > thou->minimumSz)
            reduceStack(thou);

        thou->stackHash = makeHash(thou);

        stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
        return returnMember;
    }

    stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
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

    if(NCAN(thou->sufferer1,thou->sufferer2, *((unsigned long long*)thou->data - 1),*((unsigned long long*)thou->data + thou->capacity)))
        return CANARY_VALUE_CHANGED;

    if(thou->stackHash != makeHash(thou))
        return BAD_HASH;

    return  NO_STACK_ERRORS;
}

//-------------------------------------------------------------------------

int stackResize(struct  Stack* thou, size_t newCapacity)
{
    stackType* temp = (stackType*)realloc((unsigned long long*)thou->data - 1, sizeof(*temp) + 2 * sizeof(unsigned long long));

    if(!temp)
    {
        stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
        return NO_FREE_MEMORY;
    }


    else
    {
        *(unsigned long long*)temp = CANARY_VALUE;
        thou->data = (unsigned long long*)temp + 1;
        *(unsigned long long*)((stackType*)thou->data + newCapacity) = CANARY_VALUE;

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
    fprintf(logs, "\t" "sufferer1 = %llx\n", thou->sufferer1);
    fprintf(logs, "\t" "minimumSz = %zu\n", thou->minimumSz);
    fprintf(logs, "\t" "errcode   = %d\n", thou->errCode);
    fprintf(logs, "\t" "stackHash = %llu\n", thou->stackHash);
    fprintf(logs, "\t" "sz        = %zu\n", thou->sz);
    fprintf(logs, "\t" "capacity  = %zu\n", thou->capacity);
    fprintf(logs, "\t" "data [%p]\n", thou->data);
    fprintf(logs, "\t\t" "{\n");
    fprintf(logs, "\t\t" "canary1 = %llx\n", ((unsigned long long*)thou->data)[-1]);
    for(int ind = 0; ind < thou->capacity; ++ind)
    {
        if(ind >= thou->sz)
            marker = ' ';
        fprintf(logs, "\t\t" "%c[%d] %lg\n",marker, ind, ((stackType*)thou->data)[ind]);
    }
    fprintf(logs, "\t\t" "canary2 = %llx\n", ((unsigned long long*)thou->data)[thou->capacity]);
    fprintf(logs, "\t\t" "}\n");
    fprintf(logs, "\t" "sufferer2 = %llx\n", thou->sufferer2);
    fprintf(logs, "}\n");
    fflush(logs);

    return NO_FUNCTION_ERRORS;
}

//------------------------------------------------------------------------

long long makeHash(struct Stack* thou)
{
    long long oldHash = thou->stackHash;
    thou->stackHash = 0;
    long long newHash = 0;

    char* stackBuff =(char*) thou;
    for(int ind = 0; ind < sizeof(*thou); ++ind)
        newHash = ROR(newHash) + stackBuff[ind];

    char* dataBuff = (char*) thou->data;
    for(int ind = 0; ind < sizeof(*(stackType*)thou->data); ++ind)
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

//-------------------------------------------------

char* numOfErrorCode(int errorCode)
{
    switch (errorCode)
    {
        case NO_STACK_ERRORS: return ARGNAME(NO_STACK_ERRORS);
        case BAD_SIZE: return ARGNAME(BAD_SIZE);
        case UNACCEPTABLE_Member: return ARGNAME(UNACCEPTABLE_Member);
        case BAD_MINIMUM_SIZE: return  ARGNAME(BAD_MINIMUM_SIZE);
        case CANARY_VALUE_CHANGED: return  ARGNAME(CANARY_VALUE_CHANGED);
        case BAD_HASH: return ARGNAME(BAD_HASH);
        default:
            char* str;
            sprintf(str, "%d", errorCode);
            str = strcpy(str,"stack can`t have this status" );
            return  str;
    }
}

//-------------------------------------------------

struct Stack* newStack()
{
    struct Stack* temp = (struct  Stack*)calloc(1, sizeof(*temp));
    if(!temp)
        return NULL;

    return temp;
}

//--------------------------------------------------

stackType top(struct Stack* thou)
{
    stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );

    if(stackError(thou))
    {
        stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
        return poison;
    }


    if(thou->sz > 0){
        stackType returnMember;
        thou->sz;
        returnMember = *((stackType*)thou->data + thou->sz - 1);

        stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
        return returnMember;
    }

    stackDump(thou, numOfErrorCode(stackError(thou)), __LINE__ );
    return poison;
}

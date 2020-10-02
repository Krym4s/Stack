#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
//#include "IsE_Stack.h"

#define  asserted(x) || printf("[Error] on line number: %s\n" "return code: %s", __LINE__, x);
typedef double stackType;
const stackType poison = NAN;
struct  Stack
{
    size_t capacity;
    size_t sz;
    stackType data[0];
};

enum ErrorCode
{
    NO_ERRORS = 0,
    NO_FREE_MEMORY = -1
};

int stackConstructor(struct Stack* thou, size_t capacity);

void stackDestructor(struct Stack* thou);

int push(struct Stack* thou, stackType newMember);

int enlargeStack(struct Stack* thou);

int reduceStack(struct Stack* thou);

stackType pop(struct Stack* thou);

stackType front(struct Stack* thou);

int main() {

    return 0;
}

//--------------------------------------------------------

int stackConstructor(struct Stack* thou, size_t capacity)
{
    assert(thou);

    thou = (Stack*) calloc(1, sizeof(*thou) + sizeof(thou->data) * capacity) ;
    thou asserted(NO_FREE_MEMORY);

    thou->capacity = capacity;
    thou->sz = 0;

    for (size_t i = 0; i < capacity; i++)
        (thou->data)[i] = poison;

    return 0;
}

//------------------------------------------------------------

void stackDestructor(struct Stack* thou)
{
    assert(thou);

    free(thou);

    thou = NULL;
}

//------------------------------------------------------------

int push(struct Stack* thou, stackType newMember)
{
    assert(thou);

    if (thou->sz == thou->capacity)
        enlargeStack(thou) asserted(NO_FREE_MEMORY);

    (thou->data)[thou->sz++] = newMember;
    return NO_ERRORS;
}

//-------------------------------------------------------

int enlargeStack(struct Stack* thou)
{
    size_t newCapacity = thou->capacity;
    size_t oldCapacity = thou->capacity;
    size_t sz = thou->sz;

    struct Stack* tempStack = (struct Stack*)realloc(thou, sizeof(*thou) + sizeof(thou->data) * thou->capacity * 2);
    newCapacity = oldCapacity * 2;

    if(tempStack == NULL)
    {
        tempStack = (struct Stack*)realloc(thou, sizeof(*thou) + sizeof(thou->data) * thou->capacity * 1.5);
        newCapacity = oldCapacity * 1.5;
    }


    if(tempStack == NULL)
    {
        tempStack = (struct Stack*)realloc(thou, sizeof(*thou) + sizeof(thou->data) * (thou->capacity + 100) );
        newCapacity = oldCapacity + 100;
    }

    if(tempStack == NULL)
    {
        tempStack = (struct Stack*)realloc(thou, sizeof(*thou) + sizeof(thou->data) * (thou->capacity + 1) );
        newCapacity = oldCapacity + 1;
    }


    if(tempStack == NULL)
        return NO_FREE_MEMORY;

    thou = tempStack;
    thou->capacity = newCapacity;

    for (int ind = oldCapacity +1; ind <= newCapacity; ++ind)
    {
        (thou->data)[ind] = poison;
    }

    return NO_ERRORS;
}

//----------------------------------------------------------------

int reduceStack(struct Stack* thou)
{

    struct Stack* tempStack = (struct Stack*)realloc(thou, sizeof(*thou) + sizeof(thou->data) * thou->capacity / 2);
    if(tempStack == NULL)
        return NO_FREE_MEMORY;

    thou = tempStack;
    return NO_ERRORS;
}

//----------------------------------------------------------------



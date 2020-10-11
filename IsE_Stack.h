// --------------------------------------------------------
// Created by Egor Dolgodvorov (IsE_Katin, Krym4s) on 05.10.2020.
//
// This header contains declaration of functions, which work with stack for standart types
//

#ifndef STACK_ISE_STACK_H
#define STACK_ISE_STACK_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

typedef double stackType;
const stackType poison = NAN;

struct Stack;

//!----------------------------------------------------------
//!         This function constructs structure Stack
//!
//! \param [out] thou - pointer to current Stack
//! \param [in] capacity - beginning capacity of stack structure
//!
//! \return code of function or stack error
//!
//! \note every stack have it`s own minimum size that always is not more than capacity
//!--------------------------------------------------------

int stackConstructor (struct Stack* thou, size_t capacity);

//!---------------------------------------------------------
//!         Destructs current Stack
//! \param [in] thou - pointer to current Stack
//!
//! \note frees memory and makes pointers to adress to NULL
//!-------------------------------------------

void stackDestructor (struct Stack* thou);

//!-------------------------------------------------
//!         Pushes new member to stack
//! \param [in] thou - pointer to current Stack
//! \param [in] newMember - member, which we push to stack
//!
//! \return code of function or stack error
//!-------------------------------------------------

int pushIntoStack (struct Stack* thou, stackType newMember);

//!---------------------------------------------------
//!         Tries to enlarge stack capacity and fill new stack volume with poison
//! \param [in] thou - pointer to current stack
//!
//! \return code of function or stack error
//!
//! \note function tries to enlarge stack 4 times and after that return error code
//!---------------------------------------------------

int enlargeStack (struct Stack* thou);

//!---------------------------------------------------
//!         Tries to reduce stack capacity
//! \param [in] thou - pointer to current stack
//!
//! \return code of function or stack error
//!---------------------------------------------------

int reduceStack (struct Stack* thou);

//!----------------------------------------------------
//!         Put top member out of stack
//! \param [in] thou - pointer to current stack
//!
//! \return top member
//!
//! \note returns poison if stack buffer is empty
//!-----------------------------------------------------

stackType popFromStack (struct Stack* thou);

//!---------------------------------------------------
//!        shows top member of stack
//! \param [in] thou - pointer to current stack
//!
//! \return top member
//!
//! \note returns poison if stack buffer is empty
//!-----------------------------------------------------

stackType topOfStack (struct Stack* thou);

//!----------------------------------------------------
//!         checks if stack is correct or incorrect
//! \param [in] thou - pointer to current stack
//!
//! \return code of function or stack error
//!---------------------------------------------------

int stackError (struct Stack* thou);

//!---------------------------------------------------
//!         Make logs
//! \param [in] thou - pointer to a current stack
//! \param [in] reason - name of error
//! \param [in] line - line in code, where function was executed
//! \return  code of error of function
//!---------------------------------------------------

int stackDump (struct Stack* thou, char* reason, int line);

//!---------------------------------------------------
//!         resizes data and change capacity of stack
//! \param [in] thou - pointer to a current stack
//! \param [in] newCapacity - capacity of changed stack
//! \return code of function or stack error
//!--------------------------------------------------

int stackResize (struct Stack* thou, size_t newCapacity);

//!---------------------------------------------
//!         fill data members of stack from beginInd to endInd with poison
//! \param [in] thou - pointer to a current stack
//! \param [in] beginInd - index of first member
//! \param [in] endInd - index of last member
//!---------------------------------------------

void fillStackWithPoison (struct Stack* thou, int beginInd, int endInd);

//!----------------------------------------------
//!         makes hash to identify stack
//! \param [in] thou - pointer to a current stack
//! \return hash
//!----------------------------------------------

long long makeHash (struct Stack* thou);

//!-------------------------------------
//!         makes left cycle of bits
//! \param [in] value - input value
//! \return left cycle of bits
//!------------------------------------

long long ROL (long long value);

//!-------------------------------------
//!         makes right cycle of bits
//! \param [in] value - input value
//! \return right cycle of bits
//!------------------------------------

long long ROR (long long value);

//!-----------------------------------
//!         translates code of error to name of error
//! \param [in] errorCode - integer associated with error code
//! \return name of error
//!-----------------------------------

char* numOfErrorCode (int errorCode);

//!----------------------------------
//!         Returns pointer to Stack with properate memory
//! \return pointer to Stack with properate memory
//!------------------------------------

struct Stack* newStack();

void fLogsClose();

#endif //STACK_ISE_STACK_H

//TODO define определение типов
//TODO define(функция) пределения яда
//TODO функция транзакции
//TODO реализовать Dump                                      //DONE
//TODO реализовать stackError hash                           //DONE
//TODO пределать free под канарейки                          //DONE
//TODO minimumSz не ноль                                     //DONE
//TODO TOP                                                   //DONE

#include "IsE_Stack.h"

int main() {

    Stack* stk;
    stk = newStack();
    stackConstructor (stk, 2);

    stackResize(stk, 4);
/*
    pushIntoStack (stk, 10.0);
    stackType a = popFromStack (stk);
    printf ("%lg\n", a);
    a = popFromStack (stk);
    printf ("%lg\n", a);
    pushIntoStack (stk, 30.0);
    pushIntoStack (stk, 40.0);
    pushIntoStack (stk, 50.0);
    pushIntoStack (stk, 60.0);
    pushIntoStack (stk, 70.0);
    a = popFromStack (stk);
    printf ("%lg\n", a);
    a = popFromStack (stk);
    printf ("%lg\n", a);
    a = popFromStack (stk);
    printf ("%lg\n", a);

    stackDestructor (stk);

    fLogsClose();*/
    return 0;
}




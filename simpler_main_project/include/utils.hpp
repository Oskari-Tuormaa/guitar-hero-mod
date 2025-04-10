#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

// Bit number to flag macros
#define BIT(n)  (1 << n)
#define IBIT(n) (~(1 << n))

#define T_US(x)        (x)
#define T_MS_TO_US(x)  (1000 * T_US(x))
#define T_SEC_TO_US(x) (1000 * T_MS_TO_US(x))

#define T_MS(x)        (x)
#define T_SEC_TO_MS(x) (1000 * T_MS(x))
#define T_MIN_TO_MS(x) (60 * T_SEC_TO_MS(x))

#endif // ifndef UTILS_H_INCLUDED

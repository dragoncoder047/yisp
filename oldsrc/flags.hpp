#define setflag(x)         (Flags |= 1 << x)
#define clrflag(x)         (Flags &= ~(1 << x))
#define tstflag(x)         (Flags & 1 << x)

volatile uint16_t Flags = 0;

enum flag {
    PRINTREADABLY,
    DISABLEGC,
    ESCAPE,
    NOESC,
}
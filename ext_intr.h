#ifndef EXT_INTR_
#define EXT_INTR_ 1

void
eintr_init(void);

typedef void (*eintr_handler_t)(void);

void
eintr_set_operators_pult_handler(eintr_handler_t handler);

#endif

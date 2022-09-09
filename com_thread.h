#ifndef COM_THREAD
#define COM_THREAD

/* wątek komunikacyjny: odbieranie wiadomości i reagowanie na nie poprzez zmiany stanu */
void *comLoop(void *ptr);

#endif

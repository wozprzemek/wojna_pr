#include "main.h"
#include "watek_glowny.h"

void mainLoop()
{
    srandom(rank);
    while (true)
    {
        int perc = random() % 100;

        if (perc < STATE_CHANGE_PROB)
        {
            if (state = FIGHTING)
            {
                debug("Wysyłam %d do %d", perc, (rank + 1) % size);
                // changeState( InSend );
                packet_t *pkt = malloc(sizeof(packet_t));
                pkt->data = perc;
                changeMoney(-perc);
                sleep(SEC_IN_STATE); // to nam zasymuluje, że wiadomość trochę leci w kanale
                                     // bez tego algorytm formalnie błędny za każdym razem dawałby poprawny wynik
                debug("Lamport: %d", pkt->ts)
                    debug("Data: %d", pkt->data)
                        debug("Src: %d", pkt->src)
                            sendPacket(pkt, (rank + 1) % size, MONEYTRANSFER);
                // changeState( InRun );
                debug("Skończyłem wysyłać");
            }
        }
        else
        {
            debug("Nie moge wyslac: %d", perc)
        }
        sleep(SEC_IN_STATE);
    }
}

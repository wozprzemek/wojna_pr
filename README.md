# wojna_pr

## Opis

W odległej przyszłości wybucha wojna federacji terrańskiej z puchatymi misiami, których słitaśny wygląd kompletnie nie pasuje do ich mrocznej i sadystycznej natury. Wojna ma charakter podjazdowy, federacja wysyła pojedyncze okręty do walki, po których wracają zniszczone w różnym stopniu.

**N** okrętów co pewien czas kończy walkę i wraca do bazy, gdzie ubiega się o jeden z **K** doków. Dodatkowo, dowództwo okrętów ubiega się o mechaników. Liczba mechaników wymaganych do naprawy jest zależna od zniszczeń. Łącznie jest **M** mechaników, i są oni nierozróżnialni. 

## Kompilacja
```
mpiCC main.cpp main_thread.cpp com_thread.cpp -o main
 ```
 
## Uruchamianie
```
mpirun -np <N> main
 ```
 Gdzie **N** oznacza liczbę uruchamianych procesów (max to liczba rdzeni procesora). 

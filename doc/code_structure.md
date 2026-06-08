Všechna dokumentace je WIP (stejně jako kód).
# Struktura kódu

= jak to celé funguje dohromady. Bližší popisy co dělají individuální funkce jsou v headrech.

## Tasky

Na čtečce běží (nepočítaje nějaké WiFi daemony) tyto tasky:

* device\_task (main.c) - Zde je řešená všechna logika a spojuje se zde vyčítání nfc eventů a input eventů. Tento task má také nejvyšší prioritu 2 (je potřeba dát si pozor aby moc neblokoval).
* nfc\_control\_task (nfc.c) - Pokud čtečka zaznamená tag, tento task to zpracuje a uloží do nfcEventQueue. Priorita 1.
* ui\_update\_task (ui.h) - Čeká na zprávy v mailboxu uiQueue, a podle toho updatuje co se vykresluje.
* app\_main (main.c) - Spustí inicializaci všeho, a pak exitne.


### device_task

## Stavové proměnné
### dev_mode
Proměnná dev_mode řídí device_task a tím pádem určuje co vyčítá výstupy ostatních tasků. Má rezervovanou hodnotu DEV_BOOT, kdy se neděje nic (a vše se inicializuje). Momentálně existuje pouze jeden další stav - DEV_IDLE, kdy čtečka normálně funguje v "soutěžním módu".

### ui_state
Toto není jedna stavová proměnná, ale passuje se do mailboxu uiQueue. Určuje, co se bude zobrazovat na displeji. Všechno zobrazování by mělo být řešeno pomocí těchto stavů, protože jejich updatování má vlastní task a tudíž nehrozí block někde jinde.

Jediná výjimka je ui_invoke_menu, které **blokuje**, protože pak se dá jednoduše použít jako callback. Vykreslování také tato funkce řeší sama, protože pak by bylo potřeba nějak passovat do ui_update_task tuny dat.
# Quick start

Tato sekce dokumentace obsahuje návod jak s kompletně čistou čtečkou udělat setup před soutěží. Jednotlivé kroky:
0. (udělat git clone)

1. setup ESP-IDF (nastavení projektu, co repozitář neobsahuje)
2. donastavení Kconfigu
3. build a flash

Pokud máte zkušenosti s Arduinem, kód pro ESP vypadá dost podobně, ale využívá FreeRTOS, což může na první pohled vypadat chaoticky. Do detailu je to popsáno v sekci struktura.md.



Pro build tohoto projektu je potřeba:

* ESP-IDF (předpokládám CLI variantu) (všechno jsem testoval na verzi 6.0.1). Zde je na to dost polopatický návod: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html



## 1\. Setup ESP-IDF

Jediná věc, co je (v tuto chvíli) potřeba udělat je ve složce spustit příkaz:

```bash
idf.py set-target esp32
```

pak si IDF nestěžuje že má problém s kompilací, protože nezná s jakou deskou pracuje.

## 2\. Setup Kconfigu

Zde je už potřeba nastavit samotné údaje co čtečka potřebuje během soutěže. V hlanví složce projektu spustím příkaz:

```bash
idf.py menuconfig
```

Ten otevře menu kde můžete upravovat nastavení, se kterými se zkompilují jednotlivé knihovny, které projekt využívá. Před soutěží je potřeba nastavit akorát věci v kategorii "Strela-Vlna system config".

Tam jsou kategorie:

* Software version 		(není potřeba upravovat)
* Device ID 			(ideálně aby každá čtečka měla unikátní ID aby šlo rozlišit jaká posílá jaký request)
* Strela-Vlna server URL	(DULEZITE: URL, kde běží endpoint hlavního serveru (duh))
* WiFi SSID			(sem je potřeba napsat alespoň jednu wifi (na první v pořadí se čtečka automaticky připojuje po startupu))
* WiFi password			(analogicky)

## 3\. Build and flash

Teď přijde čas na úžasné ESP-IDF, které po napsání tohoto příkazu postahuje všechny knihovny a vyřeší všechno za vás. Na začátek příkaz pusťte s parametrem "monitor" a zkontrolujte, že vše naběhlo v pořádku. Pak máte hotovo.

```bash
idf.py build flash monitor
```


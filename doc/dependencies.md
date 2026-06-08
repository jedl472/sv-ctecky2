Všechna dokumentace je WIP (stejně jako kód).
# Dependencies
Tento projekt závisí na několika knihovnách, zde je popsané co přesně a jak dělají (a jak moc speciální přístup vyžadují).
- u8g2
- u8g2-hal-esp-edf
- esp-idf-pn532

## u8g2
Kromě toho, že je tato knihovna mistrovské dílo tak se i ve čtečce stará o komunikaci s displejem. Snažil jsem se stáhnout nějaký HAL (hardware abstraction layer - de facto port knihovny na nějaké MCU), co by nějak chytře využíval integrovaného spi hardwaru, problém je že tento displej specificky používá nějaký kontrolér co komunikuje po nějaké obskurní variantě SPI. Tak jsem udělal tento HAL: https://github.com/jedl472/u8g2-hal-esp-idf.git, který data prostě probitbanguje ven. Chtělo by to udělat lépe, ale toto funguje. 

## pn532
Samotná NTAG čtečka prostě občas přestane komunikovat (je potřeba restartovat čtečku dokud se nechytne - to se ale děje pořád a ovlivňuje to pouze inicializaci), o tom ale těžko říct, co za to může. Tato knihovna používá starou I2C knihovnu (na což si IDF stále stěžuje), pro to je do toho locklý zbytek programu. Také by chtělo opravit, protože nové IDF to pravděpodobně prostě nezkompilují.
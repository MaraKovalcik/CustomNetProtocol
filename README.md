# VUT - FIT : IPK 2018
### xkoval14@stud.fit.vutbr.cz

Klient-server aplikace pro získání informace o uživatelích

## O projektu

Navržení vlastního aplikačního protokolho realizující přenos informací o uživatelích na straně serveru.
Klientská a serverová aplikace v C realizující zprostředkování informace o uživatelích na serveru.
### Spuštění serveru

Server běží na portu PORT. Pracuje se souborem /etc/passwd.
```
./ipk-server -p PORT
```

### Spuštění klienta

Klient se připoje na server HOST na port PORT. 
host (IP adresa nebo fully-qualified DNS name) identifikace serveru jakožto koncového bodu komunikace klienta;
port (číslo) cílové číslo portu;
+ -n značí, že bude vráceno plné jméno uživatele včetně případných dalších informací pro uvedený login (User ID Info);
+ -f značí, že bude vrácena informace o domácím adresáři uživatele pro uvedený login (Home directory);
+ -l značí, že bude vrácen seznam všech uživatelů, tento bude vypsán tak, že každé uživatelské jméno bude na zvláštním řádku; v tomto případě je login nepovinný. Je-li však uveden bude použit jako prefix pro výběr uživatelů.
+ login určuje přihlašovací jméno uživatele pro výše uvedené operace.

```
./ipk-client -h HOST -p PORT [-n|-f|-l] LOGIN
```

## Funkcionalita (a problémy)

+ Na serveru **merlin.fit.vubtr.cz** funguje všechno jak má, včetně spuštění ipk-server na merlin.fit.vutbrcz a ipk-client na lokálním počítači

+ Na serveru **eva.fit.vubtr.cz** nefuguje připojení na server :(




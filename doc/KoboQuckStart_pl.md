Skrócona instrukcja obsługi Kobo - TopHat
=========================================

Ostrzeżenie
-----------

Włącznie
--------
* Urządzenia
* Programu

Wyłącznie
---------
* Programu
* Urządzenia

Konfiguracja GPS 
----------------
Czytnik Kobo posiada wbudowany port szeregoway pozwalający na podłączenie GPS lub elektroniczengo wairometru. Operacja taka wymaga
otwarcia obudowy oraz przylutowania wyproadzeń w przygotowanym przez producenta urządzenia miejscu. Należy pamietać iż wbudowany port
szeregowy działa przy napięciu 3.3V tak wiec wymagany jest konwerter.

Dodatkowo możliwe jest podłączenie jest GPS poprzez port USB za pomocą konwertera post szeregowy <-> USB. Kobo potrafi funkcjonować
jako USB host jednak nie potrafi dostarczyć napięia zasilającego do zewnetrznych urządzeń. Oznacza to konieczność podłączenia
zasilania poprzez kabel rozgałęziający (nazwywany UBS Y cable). Zasialnie podłączone poprzez taki kabel pozwala zasialać zarówno GSP
jak i samo urządzenie Kobo.

Do wyboru źródła można dojść po trzykrotym (3/4) naciśnieciu przycisku "M" a następnei wybraniu symbolu zębatki. Do konfiguracji urządzeń
wejściowych wchodzi się poprzdez wybranie **Device**. TopHat/XCSoar pozwala na konfigurację jednocześnie kilu urządzeń wejściowych.
Lista wszystich urządzeń widoczna jest na ekranie. Po wybraniu urządzenia możliwa jest jego konfiguracja po naciścnieciu przycisku 
**Edycja**. 

_TopHat/XCSoar przyjmuje i przetwarza dane GPS tylko w trybie lotu (Fly). Żadne dane nie są wyświetlane gdy program działa w trybie
sumulacji. Warto o tym pamietać podczas konfiguracji portów wejściowych._


### Wewnętrzny Port szeregowy

W przypadku Kobo Mini wbudowany port szeregowy port widoczny jest w konfiguracji urządzania jako `ttymxc0`. Prędkość należy wybrać
w zależności od typu podłączonego odbiornika GPS. Standardową prędkością GPS jest 4800 bodów, w nowszych typach można spotkać się również
z większymi prędkościami.

### Port szeregowy USB 

`ttyUSB0`

Synchronizacja danych
---------------------

Uaktualnienie oprogramownia
---------------------------




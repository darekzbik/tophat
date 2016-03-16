Skrócona instrukcja obsługi Kobo - TopHat
=========================================

Ostrzeżenie
-----------

Użytkownik programu TopHat jest całkowicie odpowiedzialny za jego użycie. Oprogramowanie jest
jedynie pomocą nawigacyjną i nie zwalnia pilota w żadnym stopniu z odpowiedzialności za
przestrzeganie przepisów lotniczych.


Włączenie urządzenia
--------------------

Urządzenie Kobo z zainstalowanym oprogramowaniem TopHat włącza się tak jak oprogramowaniem czytnika książek. Pierwsza metoda
polega na przesunięciu przełącznika znajdującego od góry obudowy. Przełącznik należy przesunąć na kila sekund w lewą stronę
a następnie puścić. Drugą metodą włączenia urządzenia jest podłączenie ładowarki poprzez złącze USB. Niezależnie od sposobu włączenia proces uruchamiania oprogramowania sygnalizowany jest poprzez świecenie diody obok włącznika. Po uruchomieniu na
ekranie urządzenia pojawią się ekran startowy zawierający wyrysowane przyciski sterujące dalszą pracą programu.

 
Powitalny ekran startowy pozwala wybrać tryb pracy urządzenia. Nieco poniżej środka ekranu znajdują się przyciski `FLY` oraz `SIM`. Pierwszy tryb przeznaczony jest do faktycznego lotu. W tym trybie odczytywana jest pozycja z odbiornika GPS oraz wykonywane są faktyczne obliczenia nawigacyjne. Drugi tryb oznaczony jako `SIM` przeznaczony jest do nauki posługiwania
się nawigacją. W tym trybie program zamiast wczytywać faktyczne położenie z odbiornika GPS symuluje lot szybowca.

Wyłączenie urządzenia
---------------------

Wyłącznie programu nawigacyjnego następuje poprzez czterokrotne naciśnięcie przycisku `M` (menu) a następnie przyciśnięcie
przycisku `Exit program`. Przed wyłączeniem program dodatkowo poprosi o potwierdzenie czy na pewno chcemy przerwać
nawigację.

Druga metoda polega na użyciu mechanicznego przycisku na górnej ramce obudowy. Przycisk należy na chwilę przesunąć w lewo.
W tym wypadku również program poprosi o potwierdzenie czy istotnie zamierzamy zakończyć działanie. 


Całkowite wyłączenia urządzenia możliwe jest jedynie w chwili gdy wyświetlany jest główny ekran (powitalny). Istnieją
dwie możliwości wyłączenia urządzenia. Pierwsza polega na przyciśnięciu przycisku `Poweroff` w prawym dolnym rogu ekranu.
Drugi sposób wyłączenia polega na użyciu mechanicznego przycisku włącznika na górnej ramce urządzenia. Należy go na chwilę
przesunąć w lewo.

Nawigacja przed wyłączniem wyświetla listę ostatnio wykonanych lotów z datą i godziną wykonania lotu, na samym dole
ekranu po prawej stronie widoczny jest napis _powered off_. Ekran ten pozostaje widoczny również po całkowitym wyłączeniu
się urządzenia. Wyświetlacz typu elektroniczny papier nie zużywa energii elektrycznej w tym stanie.

Ładownie urządzenia
-------------------
Urządzenie posiada wewnętrzną baterię pozwalającą na pracę w trybie nawigacji przez kilka godzin. Ładownie odbywa się poprzez
złącze USB podobnie jak większość dzisiejszych telefonów. Podczas długiego lotu należy zapewnić ładowanie z instalacji szybowca
poprzez ładowarkę z napięciem wyjściowym 5V lub z innego źródła np. przenośnej baterii _Power Bank_.

Warto wiedzieć iż kobo przy silnie rozładowanej baterii nie pozwala się włączyć. Nie jest to również możliwe zaraz po
podłączeniu zasilania. Trzeba cierpliwie zaczekać aż wewnętrzna bateria naładuje się do poziomu pozwalającego uruchomienie
urządzenia, może to zająć około 1h.


Konfiguracja GPS 
----------------
Czytnik Kobo posiada wbudowany port szeregowy pozwalający na podłączenie odbiornika GPS lub elektronicznego wairometru. Operacja taka wymaga otwarcia obudowy oraz przylutowania wyprowadze w przygotowanym przez producenta urządzenia miejscu.
Należy pamietać iż wbudowany port szeregowy działa przy napięciu 3.3V tak wiec wymagany jest konwerter poziomów napięć.

Dodatkowo możliwe jest podłączenie GPS poprzez port USB za pomocą konwertera post szeregowy <-> USB. Kobo potrafi
funkcjonować jako USB host jednak nie potrafi dostarczyć napięcia zasilającego do zewnętrznych urządzeń. Oznacza to
konieczność podłączenia zasilania poprzez kabel rozgałęziający (nazywany UBS Y cable). Zasilanie podłączone poprzez
taki kabel pozwala jednocześnie zasilać urządzenie podłączone do USB jak i samo urządzenie Kobo.

Do wyboru źródła można dojść po trzykrotnym naciśnięciu przycisku `M` a następnie wybraniu symbolu zębatki.

Do konfiguracji urządzeń wejściowych wchodzi się poprzez wybranie `Device`. TopHat/XCSoar pozwala na pobieranie danych jednocześnie kilu urządzeń wejściowych.Lista wszystkich kanałów wejściowych widoczna jest na ekranie. Po wybraniu urządzenia możliwa jest jego konfiguracja po naciśnięciu przycisku `Edycja`. 

_TopHat/XCSoar przyjmuje i przetwarza dane GPS tylko w trybie lotu (Fly). Żadne dane nie są wyświetlane gdy program działa
w trybie sumulacji. Warto o tym pamietać podczas konfiguracji portów wejściowych._


### Wewnętrzny Port szeregowy

W przypadku Kobo Mini wbudowany port szeregowy port widoczny jest w konfiguracji urządzania jako `ttymxc0`. Prędkość należy
wybrać w zależności od typu podłączonego odbiornika GPS. Standardową prędkością GPS jest 4800 bodów, w nowszych typach
odbiorników GPS można spotkać się również z większymi prędkościami (często 9600). Po wybraniu poprawnej prędkości urządzenia
możliwe stan urządzanie powinien zmienić się na _Połączony_. 

### Port szeregowy USB 

Urządzenia szeregowe podłączone poprzez konwerter USB <-> port szeregowy widoczne są jako `ttyUSB0`. Obowiązujące zasady
dotyczące prędkości są takie same jak w przypadku wbudowanego portu szeregowego.


Synchronizacja danych
---------------------

Kobo TopHat umożliwia synchronizację danych z zewnętrznym komputerem. Istnieją dwie metody synchronizacji danych. Za pomocą pamięci USB lub poprzez bezpośrednie podłączenie do komputera. 

### Synchronizacja poprzez pamięć USB  

Podłączenie pamięci USB możliwe jest poprzez kabel USB Y który pozwala na podłączenie zewnętrznego zasilania. Jeśli nawigacja
znajduje się na stronie startowej (tak jak zaraz po włączeniu) to chwilę po podłączeniu pamięci USB powinno pojawić się menu
składające się z kilku przycisków pozwalających na wybranie kierunku przesłania danych.

Dla potrzeb synchronizacji używany jest katalog o nazwie XCSoarData znajdujący się na podłączonym dysku USB.

*Przesłanie lotów na pamięć USB* _Download flights to USB card_ - powoduje skopiowanie wszystkich zapisanych przez nawigację lotów do do podkatalogu `XCSaorData/logs`

*Przesłanie zadań do Kobo* _Upload tasks_ - powoduje skopiowania zadań z katalogu `XCSoarData/tasks`
do nawigacji TopHat.
  
*Skopiowanie wszystkiego na pamięć USB* _Download everything to USB card_ - powoduje skopiowanie
całej zawartości katalogów używanych przez TopHat Kobo do podłączonej pamięci USB. Obejmuje to
również kopię plików konfiguracyjnych.

*Wyczyść wewnętrzne katalogi urządzenia i skopiuj wszystko z pamięci USB* _Clean Kobo data directory and then upload everything to Kobo_ - czyści urządzenie i kopiuje pamięć USB do TopHat.


### Podłączenie do komputera

Nawigację TopHat należy podłączyć do urządzenia poprzez kabel USB. Następnie należy wybrać z
opcję `PC connect`. Stosowny przycisk znajduje się na po lewej stronie na dole. Po zatwierdzeniu
ostrzeżeń urządzenie restartuje się i rozpoczyna działanie jako czytnik książek. Jeśli nie jest zarejestrowany wystarczy wybrać opcję `Don’t have WiFi network`. Po chwili w komputerze do którego jest podłączone jest Kobo powinien być widoczny dysk o nazwie `KOBOeReader`. Na dysku powinien być
widoczny katalog XCSoarData zawierający wewnętrzne pliki nawigacyjne.


Uaktualnienie oprogramowania
----------------------------

_Upgrade Top Hat_


# Projekat iz predmata Operativni sistemi
Projekat je namijenjen za Linux okru\enje.
## Kompajliranje:
gcc projekat.c -o projekat -pthread
## Pokretanje:
./projekat

Tekst zadatka:
Implementirati mehanizam
producent/konzument (producer/consumer) koji koristi zajednički bafer (FIFO red) ograničene
veličine, ali uz obavezno poštovanje prioriteta pri uzimanju podataka iz bafera.
Komponente sistema
Zajednički bafer
• Fiksne veličine MAX_SIZE.
Producent (producer)
• Nit koja generiše cele brojeve (podatke) i smešta ih u bafer. Mora da čeka ako je bafer
pun.
Regularni konzument (regular consumer)
• Nit/niti koja preuzima podatke. Mora da čeka ako je bafer prazan.
Prioritetni konzument (priority consumer)
• Nit/niti koja preuzima podatke.
Pravilo prioriteta
• Ako je bafer popunjen i na raspolaganju je više od jedne vrste konzumenta, prioritetni
konzument mora uvek da preuzme podatak pre bilo kog regularnog konzumenta.
Scenario 1
Pokazati da će, uvek kada su oba konzumenta spremna za rad i bafer ima podatke, prioritetni
konzument dobiti podatak prvi.
Osnovni tok i provera prioriteta
• Pokrenite program sa
o 1. producentom, 1. regularnim konzumentom i 1. prioritetnim konzumentom.
• Producent treba da proizvede 10 stavki.
Scenario 2
Pokazati da prioritetni konzument ne može biti blokiran (izgladnjen) od strane velikog broja
regularnih konzumenata. Regularni konzumenti smeju uzeti stavku iz bafera samo ako nijedan
prioritetni konzument nije trenutno na čekanju.
Testiranje izgladnjivanja (starvation)
• Pokrenite program sa
• 1. producentom, 5 regularnih konzumenata i 1. prioritetnim konzumentom.
Producent treba da proizvede 50 stavki

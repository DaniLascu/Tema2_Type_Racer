# Descrierea Task-urilor
# Scopul Proiectului
Această temă presupune construirea unui joc de tip "TypeRacer" utilizând Arduino, în care jucătorul va tasta rapid cuvinte afișate într-un terminal pentru a acumula un scor într-un timp limitat. Jocul folosește un LED RGB și două butoane pentru a semnaliza starea și a permite configurarea jocului.

# Funcționalități detaliate:


## Indicatorul de stare – LED RGB:

LED-ul RGB funcționează ca indicator de stare pentru joc.
În modul de repaus, LED-ul are culoarea albă, semnalizând că jocul nu a început.
La apăsarea butonului de Start, LED-ul clipește timp de 3 secunde (numărătoarea inversă), pregătind utilizatorul pentru începerea rundei.
În timpul jocului, LED-ul este verde dacă utilizatorul introduce corect cuvântul afișat. Dacă se introduce greșit cuvântul, LED-ul devine roșu.


## Butonul Start/Stop:

Modul de repaus: Dacă jocul este oprit, apăsarea butonului Start/Stop inițiază o nouă rundă după o numărătoare inversă de 3 secunde.
În timpul rundei: Dacă jocul este în desfășurare, apăsarea butonului îl va opri imediat, revenind la modul de repaus.


## Butonul de selectare a dificultății:

În modul de repaus, utilizatorul poate selecta dificultatea jocului folosind acest buton. Fiecare apăsare schimbă dificultatea, ciclând între Easy, Medium și Hard, controlând intervalul de timp pentru afișarea noilor cuvinte.
La fiecare schimbare de dificultate, se afișează un mesaj specific în terminal, de exemplu: “Easy mode on!”.


## Generarea cuvintelor și afișarea lor:

Jocul utilizează un dicționar de cuvinte, din care se selectează aleatoriu cuvintele unul câte unul pentru afișare.
În timpul rundei, cuvintele sunt afișate în terminal într-o ordine aleatorie.
Dacă utilizatorul introduce corect cuvântul afișat, un nou cuvânt apare imediat pe ecran. Dacă introducerea este incorectă, LED-ul devine roșu, iar următorul cuvânt apare după un interval de timp corespunzător dificultății selectate, dacă nu se introduce un cuvânt corect între timp.


## Măsurarea scorului:

Jocul durează 30 de secunde per rundă.
La finalul rundei, numărul de cuvinte scrise corect de utilizator este afișat în terminal, reprezentând scorul acestuia.
Jocul poate fi oprit în orice moment folosind butonul de Start/Stop.

## Observații:
Pentru gestionarea apăsărilor butoanelor, utilizăm debouncing și întreruperi. Timerele sunt folosite pentru a seta frecvența apariției cuvintelor.

## Flow-ul jocului:
Starea de repaus: LED-ul este alb, iar jocul așteaptă selectarea dificultății sau inițierea unei runde.
Selectarea dificultății: Utilizatorul poate apăsa butonul de dificultate pentru a selecta între Easy, Medium și Hard, iar mesajul corespunzător apare în terminal.
Inițierea rundei: La apăsarea butonului Start, LED-ul RGB clipește timp de 3 secunde pentru numărătoarea inversă, iar în terminal se afișează: 3, 2, 1.
Desfășurarea rundei: LED-ul devine verde, iar utilizatorul începe să introducă cuvintele afișate în terminal.
Dacă introducerea este corectă, un nou cuvânt apare imediat.
Dacă introducerea este incorectă, LED-ul devine roșu, iar utilizatorul poate corecta cu ajutorul tastei BackSpace.
Finalul rundei: După 30 de secunde, jocul se încheie, iar în terminal este afișat numărul total de cuvinte scrise corect.
Oprirea jocului: Jocul poate fi oprit în orice moment prin apăsarea butonului Start/Stop, revenind la starea de repaus.

# Componentele utilizate
- Arduino UNO (ATmega328P microcontroller)
- 1x LED RGB (pentru a semnaliza dacă cuvântul corect e scris greșit sau nu)
- 2x Butoane (pentru start/stop rundă și pentru selectarea dificultății)
- 5x Rezistoare (3x 220 ohm, 2x 1000 ohm)
- Breadbord
- Fire de legătură

# Poze ale setup-ului fizic
![poza_setup_0](https://github.com/user-attachments/assets/a16ddc89-009b-4adc-9fe9-4a87595546bb)
![poza_setup_3](https://github.com/user-attachments/assets/0c442569-246e-4d01-b886-6e7ffe131d2c)
![poza_setup_2](https://github.com/user-attachments/assets/4822bfe2-aa4d-49a9-9237-ec2a0dd68b68)
![poza_setup_1](https://github.com/user-attachments/assets/92a3f39d-5ae0-44e5-a30e-48794db20969)
![poza_setup_7](https://github.com/user-attachments/assets/2182d72a-1c24-414f-97bf-c7c4f5c192db)
![poza_setup_6](https://github.com/user-attachments/assets/a706103a-8de3-40d6-a679-576d31d51657)
![poza_setup_5](https://github.com/user-attachments/assets/66fc1682-fb0a-4629-a8df-0cca1bf4dd58)
![poza_setup_4](https://github.com/user-attachments/assets/dde18fb1-b7af-42bf-b659-e9f5d9c4e336)

# Schema electrică (TinkerCAD)
![schema_electrica_tinkercad](https://github.com/user-attachments/assets/4fafbe96-075b-48c8-8ee4-a5dcbfe8afe7)

# Video cu funcționalitatea
https://www.youtube.com/watch?v=oINjeMm-TiQ

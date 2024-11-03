#include <Arduino.h>
// C++ code

#define MAX_MESSAGE 30 //lungimea maxima a unui cuvant din dictionar

#define EASY_FREQUENCY 7 //perioada pentru afisarea cuvintelor easy mode 7s
#define MEDIUM_FREQUENCY 5 //perioada pentru afisarea cuvintelor in medium mode este de 5 s
#define HARD_FREQUENCY 3 //perioada pentru afisarea cuvintelor in hard mode este de 3 s

//valoare care va fi in compare match register pt timer1
//prescaler de 1024 si 15624 => 1Hz frecv pentru intreruperi
const long int timer1_compare_match = 15624;

//declarare pini LED rgb
unsigned const int red_led = 6;
unsigned const int green_led = 4;
unsigned const int blue_led = 5;

//declarare pini butoane
unsigned const int btn_start_stop = 3;
unsigned const int btn_difficulty = 2;

//starile LED-urilor
int red_led_state = HIGH;
int green_led_state = HIGH;
int blue_led_state = HIGH;

//variabile pentru debouncing butoane in intreruperi
volatile unsigned long last_interrupt_time_start_btn = 0;
volatile unsigned long interrupt_time_start_btn;

volatile unsigned long last_interrupt_time_difficulty_btn = 0;
volatile unsigned long interrupt_time_difficulty_btn;


//variabila care verifica daca jocul se desfasoara sau nu
//ISR-ul asociat butonului de start stop modifica variabila
//on atunci cand e apasat butonul,(on face toggle cand butonul e apasat)
// in functie de valoarea pe
//care o va avea variabila on, se apeleaza functia de joc
//daca on = 1 se apeleaza functia care incepe runda
volatile int on = 0;


unsigned const long blinking_time = 3000;//3 secunde clipire
unsigned const long round_time = 30000;//30 secunde darata runda

unsigned long blinking_timer; //timer pentru animatia de clipire
unsigned long round_timer;//timer pentru cronometrarea unei runde de joc


unsigned long toggle_time = 500;//ms timp intre toggel-urile LED-urilor
unsigned long toggle_timer;

//----------------------selectia dificultatii------------------

const String dificultati[3] = {"Easy", "Medium", "Hard"};

volatile int difficulty = 0;//la apasarea butonului de dificultate
//variabila difficulty este incrementata si dupa %3 pentru a alege
//intre cele 3 tipuri de dificultate

volatile int change_difficulty = 0;//o variabila care devine 1
//atunci can se schimba dificultatea, verific valoarea acesteia
//pentru a afisa o singura data in interfata seriala ca s-a
//schimbat dificultatea

//--------------selectia si aparitia cuvintelor------------------

//dictionar de cuvinte
const String words[50] = {
        "apple", "banana", "cherry", "date", "elderberry",
        "fig", "grape", "honeydew", "kiwi", "lemon",
        "mango", "nectarine", "orange", "papaya", "quince",
        "raspberry", "strawberry", "tangerine", "ugli", "watermelon",
        "xigua", "yam", "zucchini", "avocado", "blueberry",
        "cantaloupe", "dragonfruit", "eggplant", "grapefruit", "hazelnut",
        "jackfruit", "kumquat", "lime", "mulberry", "olive",
        "pineapple", "pomegranate", "radish", "squash", "tomato",
        "turnip", "pumpkin", "blackberry", "celery", "leek",
        "parsley", "rhubarb", "spinach", "zest", "mandarin"
    };

String selected_word; //mentine cuvantul curent extras din dictionarul de cuvinte

const String fetch_word(){//functie care returneaza un cuvant random din dictionarul de cuvinte
 	 return words[random(0,50)];
}

volatile int word_display_frequency_counter ;//counter pentru frecventa aparitiei unui cuvant nou
//counter-ul este incrementat de timer 1 la fiecare secunda(timer 1 trimite o intrerupere care incrementeaza counter-ul)
//in functie de dificultate vreau sa afisez cuvinte noi la 3, 5 si 7 secunde
//timer 1 are maxim frecventa de 1Hz, deci pentru a numara mai mult de 1sec, folosesc acest counter

int display_frequency;//variabila cu care il compar pe word_display_frequency_counter
//stabileste frecventa de apariti a cuvintelor in functie de dificultatea aleasa

int frequency_generator(){//genereaza perioada de afisare in functie de dificultate
  if(difficulty == 0){//daca dificultatea e easy, i.e. 0, atunci perioada e 7s
   	return EASY_FREQUENCY; 
  }
  else if(difficulty == 1){//daca dificultatea e medium, i.e. 1, atunci perioada e 5s
   	return MEDIUM_FREQUENCY; 
  }
  else{//daca dificultatea e hard, i.e. 2, atunci perioada e 3s
   	return HARD_FREQUENCY; 
  }
}
//de fapt e perioada nu frecventa, dar se intelege



//----------------------Scor si verificare typing-----------------

unsigned int scor;//variabila care memoreaza scorul. este incrementata cu 1 la fecare cuvant scris corect

bool starts_with(const char *a, const char *b)//functie care verifica daca un string incepe cu alt string
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}

void serial_flush(){ //finctie care curata buffer-ul portului serial pentru input
  //atunci cand introduc de la tastatura un cuvant este posibil sa existe reziduuri in buffer-ul portului serial
  //iar atunci cand fac comparatia dintre cuv scris de la tastatura si cel generat random e posibil sa am erori, chiar daca
  //l-am scris corect, din cauza acelor reziduuri. Asa ca golesc buffer-ul inainte de scrierea in portul serial
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}

char input_char;//caracterul primit ca input prin interfata seriala, de la user
char input_word[MAX_MESSAGE];//cuvantul primit ca input prin interfata seriala, de la user
unsigned char index = 0;

void typing_check(String word){//verifica daca input-ul dat prin interfata seriala este = cu cuvantul selectat random si
  //incrementeaza scorul daca acest lucru se intampla
  
  const char *c = word.c_str();//salvez in variabila c adresa de inceput a cuvantului curent
  
  while (Serial.available() > 0) {//astept ca interfata seriala sa fie available
    input_char = Serial.read(); //citesc litera cu litra de pe intrfata seriala

    // Ignora caracterul '\n'
    if (input_char == '\n') { //cand apas enter se trimite \r si \n. de aceea ignor endln
      continue;
    }

    //Daca s-a citit backspace se sterge ultimul caracter din input_word
    if(input_char == '\b'){
      if (index > 0) { // daca exista ceva in input_word
        index--; // Decrementez index
        input_word[index] = 0; // Null terminate the string

        //Serial.println("\n");
        //Serial.println(input_word);
         
        // Șterge ultimul caracter din interfața serială
        Serial.print(" \b");  // Trimite spatiu si sterge ultimul caracter, backspace pt a revenii pe acea pozitie
      }
      continue;
    }
    
    if(input_char == '\r'){//daca s-a citit enter inseamna ca s-a terminat de introdus cuvantul
      if(word == input_word){ //daca cuvantul introdus = cuvantul generat aleator
        scor++; //incrementez scorul cu 1
        Serial.println("Correct word! Incrementing score.");

        word_display_frequency_counter = display_frequency; //pentru a se genera un cuvant nou imediat dupa ce un cuvant a fost introdus
      }
      else{
        Serial.println("Incorrect");
        //Serial.print(input_word);
      }

      input_word[0] = 0; //curata primul caracter din buffer, pentru a fi gata din nou de scriere
      index = 0;
    }
    else{
      if (index < MAX_MESSAGE-1) {//daca mai e loc in input_word
        input_word[index++] = input_char;//adaug ultimul caracter citit
        input_word[index] = 0;//pun 0 pe urmatorul ccaracter pt a arata ca momentan acolo se sfarseste cuvantul
      }
    }

    //verific daca exista greseli in cuvantul scris de user
    if(input_word[0] != 0){ //daca input_word nu e gol, i.e. s-a scris cel putin un caracter

      if(starts_with(c, input_word)){ //daca cuvantul generat incepe cu ceea ce este scris in input_word
        red_led_state = LOW; //inchid LED-ul rosu
        green_led_state = HIGH; //aprind LED-ul verde
      }
      else{//daca a aparut o greseala in ce a scris user-ul
        red_led_state = HIGH; //LED-ul rosu de aprinde
        green_led_state = LOW; //LED-ul verde se stinge
      }

      //aplic aceste scimbari
      digitalWrite(red_led, red_led_state);
      digitalWrite(green_led, green_led_state);
    }

  }
}


//---------------------- Rutine de intrerupere--------------------

//ISR buton dificultate
void check_btn_difficulty() {
  // Check status of switch

  interrupt_time_difficulty_btn = millis();//retine momemtul la care s-a produs intreruperea curenta

  //Daca intreruperile vin mai repede de 200 de ms inseamna ca a fost zgomot electric
  if(interrupt_time_difficulty_btn - last_interrupt_time_difficulty_btn > 200){
    //daca butonul a fost apasat
    if (digitalRead(btn_difficulty) == LOW && on == 0) {//se poate seta dificultatea doar
      // Switch was pressed                               in starea de repaus
      // schimba starea variabilei difficulty
      difficulty++;
      difficulty = difficulty%3;
      
      change_difficulty = 1;//marchez faptul ca s-a schimbat dificultatea
    }
  }

  last_interrupt_time_difficulty_btn = interrupt_time_difficulty_btn;
}


//ISR buton start_stop
void check_btn_start_stop() {
  // Check status of switch
  // Toggle on variable if button pressed
  
  interrupt_time_start_btn = millis();//retine momentul la care s-a produs intreruperea curenta

  //Daca intreruperile vin mai repede de 200 de ms inseamna ca a fost zgomot electric
  if(interrupt_time_start_btn - last_interrupt_time_start_btn > 200){
    //daca butonul a fost apasat
    if (digitalRead(btn_start_stop) == LOW) {
      // Switch was pressed
      // schimba starea variabilei on
      on = !on;
    }
  }

  last_interrupt_time_start_btn = interrupt_time_start_btn;
}


//ISR pentru intreruperile generate de Timer1
ISR(TIMER1_COMPA_vect)
// Interrupt Service Routine pentru compare mode
{  
  word_display_frequency_counter ++;//incrementez counterul pentru frecventa aparitiei cuvintelor noi
  //la fiecare 1 sec
}


//------------------Functia de pornire si rulare a jocului--------------------
void start_game_function(){
  blinking_timer = millis(); //retin momentul in care a inceput animatia de clipire
  toggle_timer = millis();
  
  unsigned int serial_counter = 3;//sicronizeaza blink-ul cu numaratoarea inversa din
  //portul serial
  
  //animatia de clipire trebuie sa dureze 3 secunde
  //verific ca LED-ul RGB sa faca toggle la 500 de ms
  while((millis() - blinking_timer) <= blinking_time){
    //toggle pinilor din LED-ul RGB
    if( (millis() - toggle_timer) > toggle_time){
    	red_led_state = !red_led_state;
    	green_led_state = !green_led_state;
    	blue_led_state = !blue_led_state;
    
    	digitalWrite(red_led, red_led_state);
      digitalWrite(green_led, green_led_state);
      digitalWrite(blue_led, blue_led_state);
    	toggle_timer = millis();
      
      if(red_led_state == LOW){//afisez numaratoare inversa cand LED-ul RGB este stin
			  Serial.println(serial_counter);
        serial_counter --;
      }
    }
  }
  
  //LED-ul RGB se face verde
  red_led_state = LOW;
  blue_led_state = LOW;
  green_led_state = HIGH;
  digitalWrite(red_led, red_led_state);
  digitalWrite(green_led, green_led_state);
  digitalWrite(blue_led, blue_led_state);
  
  on = 1;//butonul de stop se poate apasa doar in timpul rundei
  //daca l-am apasat in timpul animatiei de 3 secunde, on devine 0, iar runda nu mai incepe
  //pentru a evita asta, eu il fac din nou 1 inainte de inceperea rundei pentru a putea incepe jocul
  
  scor = 0;//scorul e readus la 0 inainte de fiecare runda

  serial_flush(); //golesc buffer-ul portului serial inaiinte de inceperea rundei. Daca scriu ceva intre runde in portul serial
                  //acele caractere raman in buffer-ul interfetei seriale, deoarece nu sunt citite(transmise). 
                  //In acest mod pot aparea erori la compararea cuvintelor
  
  display_frequency = frequency_generator();//generez frecventa de afisare in functie de dificultate
  
  word_display_frequency_counter = display_frequency; // resetăm contorul la începutul rundei
  round_timer = millis();//retin momentul in care incepe runda jocului
  
  while((millis() - round_timer) <= round_time && on == 1){//runda dureaza 30sec
	  //daca apas butonul de start-stop in timpul rundei atunci on = 0
    //deci runda se incheie, ies din while si fac LED-ul RGB inapoi in alb
    
    if(word_display_frequency_counter >= display_frequency){//afisez un cuv nou in functie de perioada display_frequency
      word_display_frequency_counter = 0;//readuc counter-ul la 0 
      selected_word = fetch_word();//se alege un cuvant aleator
      Serial.println('\n');
      Serial.println(selected_word);//este printat in interfata seriala
      serial_flush();

      input_word[0] = 0; //curata primul caracter din buffer, pentru a fi gata din nou de scriere
      index = 0;
      //la scrierea cuvantului anterior este posibil sa nu am timp sa dau enter pentru a se compara
      //cuvantul scris de mine cu cel cerut si deci functia typing_cehck sa nu goleasca vectorul 
      //input_word, in care scrie cuvantul. Astfel la scrierea cuvantului curent este posibil 
      //sa am in vector si litere din cuvantul anterior. De aceea curat input_word la gererarea unui cuv nou


      //de fiecare data cand afisez un cuv nou, LED-ul RGB devine verde
      red_led_state = LOW;
      green_led_state = HIGH;
      digitalWrite(red_led, red_led_state);
      digitalWrite(green_led, green_led_state);
    }
    typing_check(selected_word);//functia care verifica daca am scris cuvantul corect
  }
  
  Serial.println('\n');
  Serial.println("Scor: ");//afisam scorul la sfarsitul rundei
  Serial.println(scor);

  
  //LED-ul RGB se face alb
  red_led_state = HIGH;
  blue_led_state = HIGH;
  green_led_state = HIGH;
  digitalWrite(red_led, red_led_state);
  digitalWrite(green_led, green_led_state);
  digitalWrite(blue_led, blue_led_state);
  
  on = 0;//on devine 0, semn ca runda jocului s-a terminat
}


void setup()
{
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  
  pinMode(btn_start_stop, INPUT_PULLUP);
  pinMode(btn_difficulty, INPUT_PULLUP);
  
  //am aprins toate culorile din LED-ul RGB, pentru a 
  //genera, initial, culoare alba 
  digitalWrite(red_led, red_led_state);
  digitalWrite(blue_led, blue_led_state);
  digitalWrite(green_led, green_led_state);
  
  Serial.begin(9600);
  Serial.println("in function setup");
  
  // Attach Interrupt to Interrupt Service Routine
  //pentru butonul de start
  attachInterrupt(digitalPinToInterrupt(btn_start_stop),check_btn_start_stop, FALLING);
  
  //pentru butonul de dificultate
  attachInterrupt(digitalPinToInterrupt(btn_difficulty),check_btn_difficulty, FALLING);
  
  // Initializez Timer1
  TCCR1A = 0; // Timer normal, fără mod de ieșire PWM
  TCCR1B = 0;
  
  OCR1A = timer1_compare_match;  // Setăm valoarea de comparare la 15624 pentru 1Hz
  
  // Setez prescaler la 1024
  TCCR1B |= (1 << WGM12);  // Mod CTC (Clear Timer on Compare Match)
  TCCR1B |= (1 << CS12) | (1 << CS10);// Prescaler 1024
 
  // Enable timer interrupt pentru compare mode
  TIMSK1 |= (1 << OCIE1A);
 
  //permitem toate intreruperile
  interrupts();
}



void loop()
{
  //Serial.println(on);
  //Serial.println(dificultati[difficulty]);
  if(on == 1){//daca butonul start_stop a fost apasat, incep jocul
   	 start_game_function();
  }
  else{
    if(change_difficulty == 1){//daca s-a schimbat dificultatea
     Serial.println(dificultati[difficulty] + " mode on");//afisez
      change_difficulty = 0;//devine  0 pentru a afisa o singura data schimbarea in interfata seriala
    }
  }
}
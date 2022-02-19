// deklarering av konstanter
const int LED1 = 6; // Blå led
const int LED2 = 7; // Gul led
const int SW1 = 3;
const int SW2 = 4;
const int buzzerPin = 11;
const int greenLED = 9;
const int redLED = 10;
const int blueLED = 8;


int offOn;
char serialInput;
int state;

#define play 0
#define endgame 1
#define reset 2
#define nothing 3

// Spillknapper
int buttonState1;
int buttonState2;
int winner = 0;
int winnerBeep = 750; // Droppet loserBeep, bruker heler winnerbeep bare med negativ endrende pitch
int score1 = 0; 
int score2 = 0;

int randVal;
int setLEDColour;

unsigned long wait = 0;
unsigned long now = 0;
unsigned long prevTime = 0; // Trenger previousTime for å kunne lage gjentakende tilfeldige intervaller

unsigned long greenTime = 0;

void setup() {
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(blueLED, OUTPUT);
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  Serial.println("Velkommen til ButtonBuzzerBeep!");
  Serial.println("SPILLREGLER: ");
  Serial.println("Trykk FØRST når grønn lampe lyser.");
  Serial.println("Om du trykker FØR grønn lampe lyser, blir det minuspoeng.");
  Serial.println("Om det lyser blått og du trykker, blir det minuspoeng!");
  Serial.println("Førstemann til 30! Lykke til!");
  Serial.println("Skriv inn:");
  Serial.println("s - start");
  Serial.println("q - avslutt spill");
  Serial.println("r - start på nytt");
  setLEDColour = LEDColour();
  state = nothing;
}

// FUNKSJONER

// Funksjon som gir en av spillerene poeng
int win(int spiller, int greenTime) {
  for(int i=0;i<4; i++) { // For løkken kjører 3 ganger
    digitalWrite(greenLED, HIGH); // LED til spiller blinker og RGB lyser grønn
    digitalWrite(spiller, LOW);
    delay(200);
    tone(buzzerPin, winnerBeep + (i *100), 300); // Vinnerlyd økende pitch
    digitalWrite(greenLED, LOW);
    digitalWrite(spiller, HIGH);
    delay(200);
    digitalWrite(spiller, LOW); // Avslutter med begge lamper AV.
  }
  if(spiller == LED1) { // Her kommer poeng utdeling til LED1 (om den vant):
    if (greenTime <= 150) {score1 += 10;}
    else if (greenTime <= 250) {score1 += 7;}
    else if (greenTime <= 400) {score1 += 4;}
    else {score1 += 1;}
    if(score1 >= 30) { 
      Serial.println("BLÅ VANT"); 
      Serial.print("STILLING: ");
      Serial.print("BLÅ ");
      Serial.print(score1);
      Serial.print(" | ");
      Serial.print(score2);
      Serial.println(" GUL");
      state = nothing;
      victory(LED1); // Funksjon for å vise at noen har vunnet hele spillet.
    }
  }
  if(spiller == LED2) { // Poeng utdeling til LED2
    if (greenTime <= 150) {score2 += 10;}
    else if (greenTime <= 250) {score2 += 7;}
    else if (greenTime <= 400) {score2 += 4;}
    else {score2 += 1;}
    if(score2 >= 30) {
      Serial.println("GUL VANT");
      Serial.print("STILLING: ");
      Serial.print("BLÅ ");
      Serial.print(score1);
      Serial.print(" | ");
      Serial.print(score2);
      Serial.println(" GUL");
      state = nothing;
      victory(LED2);
    }
  }
}

// Funksjon om du taper et poeng.
int lose1(int spiller) {
  for(int i=0;i<4; i++) {
    digitalWrite(redLED, HIGH); // Nå blinker RGB lampe rødt for å indikere feil.
    digitalWrite(spiller, LOW);
    delay(200);
    tone(buzzerPin, winnerBeep - (i *100), 300); // Synkende pitch gir negativ stemning. 
    digitalWrite(redLED, LOW);
    digitalWrite(spiller, HIGH);
    delay(200);
    digitalWrite(spiller, LOW);
  }
  if(spiller == LED1) { // Minus poeng for å trykke for tidlig, til den det gjelder (spiller).
    score1 -= 2;
  }
  if(spiller == LED2){
    score2 -= 2;
  }
}

int lose2(int spiller) { // Samme funksjon, men når du taper med blått lys.
  for(int i=0;i<4; i++) {
    digitalWrite(redLED, HIGH); 
    digitalWrite(spiller, LOW);
    delay(200);
    tone(buzzerPin, winnerBeep - (i *100), 300); 
    digitalWrite(redLED, LOW);
    digitalWrite(spiller, HIGH);
    delay(200);
    digitalWrite(spiller, LOW);
  }
  if(spiller == LED1) {  // Her mister du 5 poeng.
    score1 -= 5;
  }
  if(spiller == LED2){
    score2 -= 5;
  }
}

// Vinnerfunksjon etter endt spill
int victory(int spiller) { // Funksjonen tar inn variabel som forteller hvem som har vunnet.
  score1 = 0; // Resetter poengsummene
  score2 = 0;
  for( int freq = 750; freq < 1500 ; freq += 30) {
    tone(buzzerPin, freq); // Skrur av buzzeren.
    if((spiller == LED1) && ( offOn == 0)) { // Hvis vinneren er player 1 og lyset er av.
    digitalWrite(greenLED, HIGH); // Skrur lysene til 1 og grønn LED på.
    digitalWrite(spiller, HIGH);
    offOn = 1;
    }
    else {
      digitalWrite(greenLED, LOW);
      digitalWrite(spiller, LOW);
      offOn = 0;
    }
    delay(100); // For å få showet til å vare lengre.
  }
  noTone(buzzerPin); // Skrur av buzzeren.
  for( int freq = 1500; freq > 750 ; freq -= 30) { // Samme men ned igjen.
    tone(buzzerPin, freq); 
    if((spiller == LED2) && ( offOn == 0)) { 
    digitalWrite(greenLED, HIGH);
    digitalWrite(spiller, HIGH);
    offOn = 1;
    }
    else {
      digitalWrite(greenLED, LOW);
      digitalWrite(spiller, LOW);
      offOn = 0;
    }
    delay(100);  
  }
  noTone(buzzerPin);
}

int LEDColour() { // Funksjon som gir en verdi 0 eller 1, hvor 0 kommer 70% av gangene. 
  randVal = random(0, 11);
  if(randVal <= 7) {
    return 0;
  }
  else if(randVal > 7) {
    return 1;
  }
}

// Spillfunksjonen
long firstPress() {
  wait = random(3000, 6000+1); // Henter et tilfeldig tall som er mellom 3 og 6 sekunder, tiden vi skal vente med før vi slår på grønt lys
  now = millis(); // Sjekker hva den virkelige tiden er.
  while(now - prevTime < wait) { 
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, LOW); // Forsikrer oss om at lampene som skal være av er av.
    digitalWrite(redLED, HIGH);
    buttonState1 = !digitalRead(SW1); // knappene er pull-down
    buttonState2 = !digitalRead(SW2);
    if(buttonState1) { // Om knapp trykt inn: 
      lose1(LED1); // Kjør taperfunksjon
      break; // Hopp ut av while løkke, da kjører spillet en ny runde.
    }
    if(buttonState2) { // Samme som over, bare at nå har LED2 tapt et poeng
      lose1(LED2);
      break;
    } 
    
    now = millis(); // Sjekker hva den nye virkelige tiden er.
  } 
  while(wait <= now - prevTime) { // Nå er altså tiden vi fikk i wait gått. Vi venter i denne while løkken til en knapp trykkes
    digitalWrite(redLED, LOW); 
    digitalWrite(blueLED, LOW);
    digitalWrite(greenLED, LOW);
    if(setLEDColour == 0) {
      digitalWrite(greenLED, HIGH);
    }
    else if(setLEDColour == 1){
      digitalWrite(blueLED, HIGH);
    }
    buttonState1 = !digitalRead(SW1); // Sjekker knappene
    buttonState2 = !digitalRead(SW2);
    if((buttonState1) && (setLEDColour == 0)) { // Om knapp 1 er trykt og lampen er grønn.
      greenTime = millis()-now; // Sjekker hvor lang tid grønn LED har lyst, for å tildele poeng. 
      digitalWrite(greenLED, LOW); // Slå av grønn lampe
      win(LED1,greenTime); // Kjør vinner funksjon som vi har hvert igjennom.
      setLEDColour = LEDColour(); // Genererer en ny farge. 
      break; // Hopp ut av løkke og start nytt spill
    }
    if((buttonState2) && (setLEDColour == 0)) { // Samme som over bare at her er det knapp 2 som sjekkes.
      greenTime = millis()-now; 
      digitalWrite(greenLED, LOW);
      win(LED2, greenTime);
      setLEDColour = LEDColour();
      break;
    }
    if((buttonState1) && (setLEDColour == 1))  { // Om knapp 1 er trykt og lampen er blå.
      digitalWrite(blueLED, LOW); // Slå av blå lampe
      lose2(LED1); 
      setLEDColour = LEDColour();
      break; 
    }
    if((buttonState2) && (setLEDColour == 1)) { // Samme som over bare at her er det knapp 2 som sjekkes.
      digitalWrite(blueLED, LOW);
      lose2(LED2);
      setLEDColour = LEDColour();
      break;
    }
    if((setLEDColour == 1) && (millis()>= (2000 + now ))) { // Hvis den er blå og det har gått 2 sekund, vil den gå til rød og begynne på nytt. 
      digitalWrite(blueLED, LOW);
      setLEDColour = LEDColour();
      break;
    }
    if(Serial.available() > 0) {
      serialInput = Serial.read();
      if(serialInput == 'q') {
        state = endgame;
        break;
      }
      if(serialInput == 'r') {
        state = reset;
        break;
      }
    }
  }
  prevTime = millis(); // resetter tiden
}

int undecided() { // Funksjon i tilfelle det blir uavgjort.
  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, HIGH);
  delay(3000);
  digitalWrite(greenLED, LOW);
  digitalWrite(redLED, LOW);
}

void loop() { // Loopen som kjører om og om igjen for alltid
  if(Serial.available() > 0) {
    serialInput = Serial.read();
    if(serialInput == 's') {
      state = play;
    }
    if(serialInput == 'q') {
      state = endgame;
    }
    if(serialInput == 'r') {
      state = reset;
    }
  }
  // Serial.println(state);
  switch (state) {
      case play:
      firstPress();
      Serial.print(score1);
      Serial.print(" - ");
      Serial.println(score2);
      break;

      case endgame:
      if(score1 == score2) {
        Serial.println("UAVGJORT!");
        Serial.print("STILLING: ");
        Serial.print("BLÅ ");
        Serial.print(score1);
        Serial.print(" | ");
        Serial.print(score2);
        Serial.println(" GUL");
        undecided();
      }
      if(score1 > score2) {
        Serial.println("BLÅ VANT"); 
        Serial.print("STILLING: ");
        Serial.print("BLÅ ");
        Serial.print(score1);
        Serial.print(" | ");
        Serial.print(score2);
        Serial.println(" GUL");
        victory(LED1);
      }
      if(score1 < score2) {
        Serial.println("GUL VANT");
        Serial.print("STILLING: ");
        Serial.print("BLÅ ");
        Serial.print(score1);
        Serial.print(" | ");
        Serial.print(score2);
        Serial.println(" GUL");
        victory(LED2);
      }
      state = nothing;
      break;

      case reset:
      score1 = 0;
      score2 = 0;
      state = play;
      break;

      case nothing:
      prevTime = millis();
      digitalWrite(greenLED, LOW);
      digitalWrite(redLED, LOW);
      break;
    }
}

#include <Arduino.h>

const unsigned int round_time = 30 * 1000; // timpul unei runde
unsigned int score = 0;

// timpii in functie de nivelul de dificultate
const unsigned int word_display_easy = 10500;  
const unsigned int word_display_medium = 6000; 
const unsigned int word_display_hard = 5000;   

// dictionarul de cuvinte
const char* cuvinte[] = {
  "hipopotam", "alergie", "intelectual", "Bucuresti", "electronica",
  "carte de istorie", "fibra optica", "cavalerism", "stadion",
  "furculita", "padure", "lemn", "parfum", "piper", "Marasesti",
  "hackatlon", "sport olimpic", "pondere", "vrabioara", "friptura",
  "dinamometru", "marime fizica", "electricitate", "nuclear",
  "America", "Mexic", "Delta Dunarii", "pix", "parasutist",
  "avion de vanatoare", "aeroport", "pescar", "automobil",
  "unitate de masura", "ananas", "castravete", "targ de Craciun",
  "fierar", "fotbalist", "cunoscator", "francez", "Madrid", "palarie"
};

const int n = 41;
const int rgb_r = 6;
const int rgb_g = 5;
const int rgb_b = 4;
const int start_stop_btn = 3;
const int dif_btn = 2;

size_t dificultate = 1; // 1: Easy, 2: Medium, 3: Hard
bool game_state = false; 
bool countdown_active = false; // numaratoarea inversa e activa
bool countdown_finished = false; //numaratoarea inversa s a terminat
// timpul de la ultima apasare a butoanelor
unsigned long lastStartStopPress = 0;
unsigned long lastDifPress = 0;
const unsigned int debounceDelay = 50; // timp de 



unsigned long countdown_start_time = 0;
int countdown_value = 3; 
unsigned long blink_time = 0; 
const unsigned long blink_interval = 500; 
unsigned long game_start_time = 0; 

// Word display timer
unsigned long last_word_display_time = 0; // ultima data cand a fost printat un cuvant
unsigned int word_display_delay = word_display_easy; // dificultate default easy
const char* current_word = ""; // aici stochez cuvantul curent printat

void selecteaza_dificultatea() {
  if (!game_state && millis() - lastDifPress > debounceDelay) { // daca apasam butonul cand jocul nu e activ si timpul de la ultima apasare e mai mic decat 50
    dificultate = (dificultate % 3) + 1; //cilam nivelul de dif
    switch (dificultate) {
      // la setarea dificultatii setam timpul de display al cuv 
      case 1: 
        Serial.println("Easy mode ON!"); 
        word_display_delay = word_display_easy; 
        break;
      case 2: 
        Serial.println("Medium mode ON!"); 
        word_display_delay = word_display_medium; // Set display interval for medium
        break;
      case 3: 
        Serial.println("Hard mode ON!"); 
        word_display_delay = word_display_hard; // Set display interval for hard
        break;
    }
    lastDifPress = millis(); // actualizam timpul de apasare al butonului
  }
}


// functie ce controleeaza rgb-ul , in mod analog 
void turn_rgb_colors(int red, int green, int blue) {
  analogWrite(rgb_r, red);
  analogWrite(rgb_g, green);
  analogWrite(rgb_b, blue);
}

void start_game() {
  if (millis() - lastStartStopPress > debounceDelay) {
    lastStartStopPress = millis(); 
    //daca am apasat inseamna ca starea jocului s a schimbat
    game_state = !game_state; 
    //daca apas cand jocul e pe rest state
    if (game_state) {
      Serial.println("A inceput jocul!");
      score = 0;
      countdown_value = 3; 
      countdown_active = true; // Activez numaratoarea
      countdown_finished = false; // numaratoarea nu  s a terminat
      countdown_start_time = millis(); // timpul pentru num
      game_start_time = millis(); // runda incepe , stochez timpul de incepere
      blink_time = countdown_start_time; 
      last_word_display_time = millis(); 
    } else { // daca apas butonul in timpul rundei opresc fortat runda
      Serial.println("Runda s-a incheiat!");
      turn_rgb_colors(255, 255, 255); // ledul devine alb
      countdown_active = false; // dezactivez numaratoare
      Serial.print("Scor final: "); //printez scorul
      Serial.println(score);
    }
  }
}

// Printeaza random un cuvant
void display_word() {
  int random_index = random(n); 
  current_word = cuvinte[random_index]; 
  Serial.println(current_word); 
  last_word_display_time = millis(); 
}

void check_input() {
  if (Serial.available() > 0) {
    String user_input = Serial.readStringUntil('\n'); 
    user_input.trim(); 

    if (user_input.equals(current_word)) {
      turn_rgb_colors(0, 255, 0); //cuvant corect ledul verde
      score++; 
      display_word(); //urm cuvant apare imediat
    } else {
      turn_rgb_colors(255, 0, 0); // Ledul devine rosu in cazul introducerii eronate
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(rgb_r, OUTPUT);
  pinMode(rgb_g, OUTPUT);
  pinMode(rgb_b, OUTPUT);
  pinMode(start_stop_btn, INPUT); 
  pinMode(dif_btn, INPUT); 

  attachInterrupt(digitalPinToInterrupt(start_stop_btn), start_game, LOW); 
  attachInterrupt(digitalPinToInterrupt(dif_btn), selecteaza_dificultatea, LOW); 

  turn_rgb_colors(255, 255, 255);

  randomSeed(analogRead(0)); // Seed generator
}

void loop() {
  // Secțiunea de numărătoare inversă (countdown)
  if (countdown_active) {
    unsigned long currentMillis = millis(); // Înregistrează timpul curent

    // Verifică dacă a trecut un interval de o secundă pentru a decremanta countdown-ul
    if (currentMillis - countdown_start_time >= 1000) {
      countdown_start_time = currentMillis; // Resetează timpul de start pentru următoarea secundă
      Serial.println(countdown_value); // Afișează valoarea curentă a countdown-ului
      countdown_value--; // Decrementează valoarea countdown-ului
      
      // Setează LED-ul să fie alb pentru a indica numărătoarea inversă
      turn_rgb_colors(255, 255, 255); 
      blink_time = currentMillis; // Setează timpul pentru următoarea clipire a LED-ului
    }

    // Asigură clipirea LED-ului la fiecare interval de timp specificat
    if (currentMillis - blink_time >= blink_interval) {
      blink_time = currentMillis; // Resetează timpul de clipire
      if (countdown_value >= 0) {
        turn_rgb_colors(0, 0, 0); // Oprește LED-ul (LED off)
      }
    }

    // Dacă countdown-ul a ajuns la zero, inițiază jocul
    if (countdown_value < 0) {
      Serial.println("GO!"); // Afișează mesajul "GO!"
      turn_rgb_colors(0, 255, 0); // Setează LED-ul pe verde (semnal pentru startul jocului)
      countdown_active = false; // Dezactivează countdown-ul
      countdown_finished = true; // Marchează că countdown-ul s-a terminat
    }
  }

  // Verifică dacă a trecut timpul de rundă de 30 de secunde
  if (game_state && (millis() - game_start_time >= round_time)) {
    Serial.println("S-a terminat timpul! Runda s-a încheiat!"); 
    game_state = false; // Setează starea jocului ca inactiv
    turn_rgb_colors(255, 255, 255); // Resetează LED-ul la alb
    countdown_active = false; // Dezactivează countdown-ul
    countdown_finished = false; // Resetează indicatorul că countdown-ul s-a terminat
    Serial.print("Final Score: "); // Afișează scorul final
    Serial.println(score);
  }

  // Verifică dacă este timpul să afișeze un nou cuvânt
  if (game_state && countdown_finished && (millis() - last_word_display_time >= word_display_delay)) {
    display_word(); // Afișează un nou cuvânt
  }

  // poti sa introduci cuvinte doar daca jocul e activ
  if (game_state) {
    check_input(); // Verifică dacă utilizatorul a tastat un cuvânt, doar dacă jocul este activ
  }
}

#include <Arduino.h>

const int B_led1 = 10;
const int B_led2 = 9;
const int B_led3 = 8;
const int B_led4 = 7;

const int RGB_led_r = 6;
const int RGB_led_g = 5;
const int RGB_led_b = 4;

const int Start_btn = 3;
const int Stop_btn = 2;

int start_btn_state = HIGH;
int stop_btn_state = HIGH;
bool is_charging = false;//starea statiei
int i = 0; //al catelea led se incarca








void RGB_on(int r, int g, int b) {
    digitalWrite(RGB_led_r, r);
    digitalWrite(RGB_led_g, g);
    digitalWrite(RGB_led_b, b);
}


void clipeste(int led, unsigned long duration) {
    unsigned long start_time = millis();
    while (millis() - start_time < duration) {
        digitalWrite(led, LOW);
        delay(500); 
        digitalWrite(led, HIGH);
        delay(500);
    }
}

void incarcarea_finalizata(int led1, int led2, int led3, int led4) {
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
    digitalWrite(led3, LOW);
    digitalWrite(led4, LOW);

    int i = 3; //toate becurile clipesc simultan (3 ms on 1,5 ms off) pentru animatia finala
    while (i > 0) {
        digitalWrite(led1, HIGH);
        digitalWrite(led2, HIGH);
        digitalWrite(led3, HIGH);
        digitalWrite(led4, HIGH);
        delay(300);
        digitalWrite(led1, LOW);
        digitalWrite(led2, LOW);
        digitalWrite(led3, LOW);
        digitalWrite(led4, LOW);
        delay(150);
        i--;
    }

    RGB_on(LOW, HIGH, LOW); // Procesul de incarcare se termina deci rgb se face verde
}

//incepe procesul de incarcare
void incarca() {
    is_charging = true; 
    i = 1; 

    
    digitalWrite(B_led1, LOW);
    digitalWrite(B_led2, LOW);
    digitalWrite(B_led3, LOW);
    digitalWrite(B_led4, LOW);

    RGB_on(HIGH, LOW, LOW); //RGB se face rosu
}

void setup() {
    pinMode(B_led1, OUTPUT);
    pinMode(B_led2, OUTPUT);
    pinMode(B_led3, OUTPUT);
    pinMode(B_led4, OUTPUT);

    pinMode(RGB_led_r, OUTPUT);
    pinMode(RGB_led_g, OUTPUT);
    pinMode(RGB_led_b, OUTPUT);

    RGB_on(LOW, HIGH, LOW); 

    pinMode(Start_btn, INPUT);
    pinMode(Stop_btn, INPUT);

    Serial.begin(9600);
}

void loop() {
    start_btn_state = digitalRead(Start_btn);
    stop_btn_state = digitalRead(Stop_btn);

    //La apasarea butonului stop(facand debouncing) oprim fortat incarcarea prin animatia de final
    if (stop_btn_state == LOW) {
        delay(10);
        stop_btn_state = digitalRead(Stop_btn);
        if (stop_btn_state == LOW && is_charging) {
            is_charging = false; 
            incarcarea_finalizata(B_led1, B_led2, B_led3, B_led4); 
            return; 
        }
    }

    //Incep incarcarea la pasarea butonului start
    if (start_btn_state == LOW) {
        delay(20); 
        start_btn_state = digitalRead(Start_btn);
        if (start_btn_state == LOW && !is_charging)//pornesc incarcarea doar daca nu incarc deja 
        {
            incarca(); 
        }
    }

    
    if (is_charging) {
        
        static unsigned long last_step_time = 0;
        unsigned long current_time = millis();

        if (i == 1 && current_time - last_step_time >= 1000) {
            clipeste(B_led1, 3000); 
            digitalWrite(B_led1, HIGH); 
            i++;
            last_step_time = current_time; 
        } 
        else if (i == 2 && current_time - last_step_time >= 1000) {
            clipeste(B_led2, 3000); 
            digitalWrite(B_led2, HIGH); 
            i++;
            last_step_time = current_time; 
        } 
        else if (i == 3 && current_time - last_step_time >= 1000) {
            clipeste(B_led3, 3000); 
            digitalWrite(B_led3, HIGH); 
            i++;
            last_step_time = current_time; 
        } 
        else if (i == 4 && current_time - last_step_time >= 1000) {
            clipeste(B_led4, 3000);
            digitalWrite(B_led4, HIGH); 
            i++;
            last_step_time = current_time; 
        } 
        else if (i == 5 && current_time - last_step_time >= 1000) {
            incarcarea_finalizata(B_led1, B_led2, B_led3, B_led4); 
            is_charging = false; 
        }
    }
}

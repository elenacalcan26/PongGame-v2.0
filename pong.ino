#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "pitches.h"

#define DOWN_PLAYER1 5
#define UP_PLAYER1 6
#define DOWN_PLAYER2 2
#define UP_PLAYER2 3
#define BUZZ 8
#define REV_P2 4
#define REV_P1 7

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define RADIUS 5
#define TIME_PICKUP 5000

#define OLED_MOSI   9 // SDA
#define OLED_CLK   10 // SCL
#define OLED_DC    11
#define OLED_CS    12 //NU AM
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// variabile globale folosite in cod

int gamePoints[] = {3, 5, 7};

int melody[] = {
  NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, NOTE_B4, NOTE_E5,
  NOTE_E5, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5,
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, NOTE_D5, NOTE_FS5,
  NOTE_FS5, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
};

// The note duration, 8 = 8th note, 4 = quarter note, etc.
int durations[] = {
  8, 8, 8, 4, 4, 4,
  4, 5, 8, 8, 8, 8,
  8, 8, 8, 4, 4, 4,
  4, 5, 8, 8, 8, 8
};

int songLength = sizeof(melody) / sizeof(melody[0]);

const unsigned long BALL_RATE = 1;

bool appeard_rect = false;
bool appeard_circle = false;

int maxScore = 3;
int player1Score = 0;
int player2Score = 0;

int page = 1;
long lastTime = 0;

const uint8_t PLAYER_X = 115;
uint8_t player_y = 16;
const uint8_t PADDLE_HEIGHT = 12;

unsigned long paddle_update;
const uint8_t PLAYER2_X = 12;
uint8_t player2_y = 16;

int up_state1 = 0;
int down_state1 = 0;
int up_state2 = 0;
int down_state2 = 0;
int rev_state1 = 0;
int rev_state2 = 0;

bool update = false;
bool resetBall = false;
bool gameStarted = false;
bool selMaxPoints = false;

int rev_player1 = 1;
int rev_player2 = 1;
int player_ball = 0;

uint8_t ball_x = 64, ball_y = 32;
uint8_t ball_dir_x = 1, ball_dir_y = 1;
unsigned long ball_update;

uint8_t rect_x = -1, rect_y = -1;
uint8_t width = -1, height = -1;
uint8_t circle_x = -1, circle_y = -1;

void drawCourt();
void drawScore();
void drawReverseMoves();
void clearReverseMoves();

void setup() {
  Serial.begin(9600);

  pinMode(BUZZ, OUTPUT);
  pinMode(DOWN_PLAYER1, INPUT_PULLUP);
  pinMode(UP_PLAYER1, INPUT_PULLUP);
  pinMode(DOWN_PLAYER2, INPUT_PULLUP);
  pinMode(UP_PLAYER2, INPUT_PULLUP);
  pinMode(REV_P2, INPUT_PULLUP);
  pinMode(REV_P1, INPUT_PULLUP);

  unsigned long start = millis();

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.display();
  delay(2000);

  display.clearDisplay();

  display.setTextSize(1.95);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 30);
  display.println(F("Pong Game V2.0"));

  display.display();

  delay(1000);
  display.clearDisplay();

  while (millis() - start < 2000);

  ball_update = millis();
  paddle_update = ball_update;
}


void loop() {

  if (!selMaxPoints && !gameStarted) {
    selectMaxPoints();
  }


  if (!gameStarted && selMaxPoints) {
    display.clearDisplay();
    drawProgressBar();

    delay(3000);

    display.clearDisplay();

    // se pregateste terenul de joc
    display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, SSD1306_WHITE);
    display.drawFastVLine(PLAYER2_X, player2_y, PADDLE_HEIGHT, SSD1306_WHITE);
    drawCourt();
    drawScore();
    display.display();

    gameStarted = true;
  }

  if (gameStarted) {
    gameLogic();
  }
}

void selectMaxPoints() {

  /**
     Se afiseaza o pagina de meniu cu 3 optiuni ale jocului.
     Utilizatorii se pot juca pana la 3, 5 sau 7 puncte.
     Se navigheaza prin meniu folosind butoanele player-ului 2.
  */

  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Select Game Points:"));

  up_state1 = digitalRead(UP_PLAYER1);
  down_state1 = digitalRead(DOWN_PLAYER1);
  rev_state1 = digitalRead(REV_P1);

  if (up_state1 == LOW) {
    // se duce in sus
    delay(200);
    tone(BUZZ, 450, 50);
    page--;
  }

  if (down_state1 == LOW) {
    // se duce in jos
    delay(200);
    tone(BUZZ, 450, 50);
    page++;
  }

  // afisarea paginii de meniu cu optiunile sale
  if (page == 0) {
    display.setCursor(10, 15);
    display.drawRect(10, 15, 100, 10, WHITE);
    display.fillRect(10, 15, 100, 10, SSD1306_WHITE);
    display.setTextColor(BLACK, WHITE);
    display.setCursor(50, 17.5);
    display.print(3);

    display.setTextColor(WHITE, BLACK);
    display.setCursor(10, 25);
    display.drawRect(10, 25, 100, 10, WHITE);
    display.fillRect(10, 25, 100, 10, SSD1306_BLACK);
    display.setCursor(50, 27.5);
    display.print(5);

    display.setCursor(10, 35);
    display.drawRect(10, 35, 100, 10, WHITE);
    display.fillRect(10, 35, 100, 10, SSD1306_BLACK);
    display.setCursor(50, 37.5);
    display.print(7);

  } else if (page == 1) {
    display.setCursor(10, 15);
    display.drawRect(10, 15, 100, 10, WHITE);
    display.fillRect(10, 15, 100, 10, BLACK);
    display.setTextColor(WHITE, BLACK);
    display.setCursor(50, 17.5);
    display.print(3);

    display.setTextColor(BLACK, WHITE);
    display.setCursor(10, 25);
    display.drawRect(10, 25, 100, 10, WHITE);
    display.fillRect(10, 25, 100, 10, WHITE);
    display.setCursor(50, 27.5);
    display.print(5);

    display.setTextColor(WHITE, BLACK);

    display.setCursor(10, 35);
    display.drawRect(10, 35, 100, 10, WHITE);
    display.fillRect(10, 35, 100, 10, SSD1306_BLACK);
    display.setCursor(50, 37.5);
    display.print(7);
  } else if (page == 2) {

    display.setTextColor(WHITE, BLACK);

    display.setCursor(10, 15);
    display.drawRect(10, 15, 100, 10, WHITE);
    display.fillRect(10, 15, 100, 10, BLACK);
    display.setCursor(50, 17.5);
    display.print(3);

    display.setCursor(10, 25);
    display.drawRect(10, 25, 100, 10, WHITE);
    display.fillRect(10, 25, 100, 10, BLACK);
    display.setCursor(50, 27.5);
    display.print(5);

    display.setTextColor(BLACK, WHITE);

    display.setCursor(10, 35);
    display.drawRect(10, 35, 100, 10, WHITE);
    display.fillRect(10, 35, 100, 10, WHITE);
    display.setCursor(50, 37.5);
    display.print(7);
  }

  page = page > 2 ? 2 : page;
  page = page < 0 ? 0 : page;

  display.display();

  if (rev_state1 == LOW) {
    // se selecteaza optiunea
    tone(BUZZ, 500, 50);
    selMaxPoints = true;
    maxScore = gamePoints[page];
  }

}

void gameLogic() {
  /**
     Defineste logica jocului de pong
  */
  drawCourt();

  unsigned long time = millis();

  if (resetBall) {
    if (player1Score == maxScore || player2Score == maxScore) {
      gameOver();
    } else {
      // un jucator a inscris
      display.fillScreen(BLACK);
      drawCourt();
      drawScore();
      appeard_rect = false;
      appeard_circle = false;

      // se alege random in ce directie se va deplasa bila la inceputul rundei

      ball_x = random(25, 70);
      ball_y = random(3, 63);

      do {
        ball_dir_x = random(-1, 2);
      } while (ball_dir_x == 0);

      do {
        ball_dir_y = random(-1, 2);
      } while (ball_dir_y == 0);

      resetBall = false;
      player_ball = 0;
      circle_x = -1;
      circle_y = -1;

    }
  }


  if (appeard_rect == false) {

    // se alege random pozitia obstacolului si dimensiunile acestuia
    rect_x = random(15, 90);
    rect_y = random(15, 40);
    height = random(10, 20);
    width = random(5, 15);

    display.drawRect(rect_x, rect_y, width, height, SSD1306_WHITE);
    display.fillRect(rect_x, rect_y, width, height, SSD1306_WHITE);

    display.display();

    appeard_rect = true;
  }

  long elapsedTime = millis();
  if (!appeard_circle && (elapsedTime - lastTime) > TIME_PICKUP) {
    // pe ecran va aparea un element de pic up dupa un anumit timp

    lastTime = elapsedTime;

    // se alege random pozitia elementului de pick-up
    circle_x = random(15, 100);
    circle_y = random(15, 50);

    display.drawCircle(circle_x, circle_y, RADIUS, SSD1306_WHITE);
    display.fillCircle(circle_x, circle_y, RADIUS, SSD1306_WHITE);

    display.display();

    appeard_circle = true;
  }

  // se iau starile butoanelor celor 2 jucatori
  up_state1 = digitalRead(UP_PLAYER1);
  down_state1 = digitalRead(DOWN_PLAYER1);
  up_state2 = digitalRead(UP_PLAYER2);
  down_state2 = digitalRead(DOWN_PLAYER2);
  rev_state2 = digitalRead(REV_P2);
  rev_state1 = digitalRead(REV_P1);

  display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
  display.drawFastVLine(PLAYER2_X, player2_y, PADDLE_HEIGHT, BLACK);

  if (time > ball_update) {
    uint8_t new_x = ball_x + ball_dir_x;
    uint8_t new_y = ball_y + ball_dir_y;

    // verificare perete vertical
    if (new_x == 0 || new_x == 127) {

      // se decide ce jucator a inscris
      if (new_x == 0) {
        player2Score += 1;
        display.fillScreen(BLACK);

        display.display();
        soundPoint();
        resetBall = true;

      }
      else if (new_x == 127) {
        player1Score += 1;
        display.fillScreen(BLACK);

        display.display();
        soundPoint();
        resetBall = true;
      }

      changeBallDirOX(new_x);
    }

    // verific daca mingea a lovit un perete orizontal
    if (new_y == 0 || new_y == 63) {
      soundBounce();
      changeBallDirOY(new_y);
    }


    // verific daca un jucator a lovit bila
    if (new_x == PLAYER2_X && new_y >= player2_y && new_y <= player2_y + PADDLE_HEIGHT) {
      changeBallDirOX(new_x);
      player_ball = 2;
    }

    if (new_x == PLAYER_X && new_y >= player_y && new_y <= player_y + PADDLE_HEIGHT)
    {
      changeBallDirOX(new_x);

      player_ball = 1;
    }

    if (appeard_rect == true) {
      // verific daca mingea a lovit un obstacol aparut random

      if ( new_x >= rect_x && new_x <= rect_x + width && new_y >= rect_y && new_y <= rect_y + height)
      {

        // ricoseaza
        changeBallDir(new_x, new_y);

        tone(BUZZ, 350, 50);

        display.drawRect(rect_x, rect_y, width, height, SSD1306_BLACK);
        display.fillRect(rect_x, rect_y, width, height, SSD1306_BLACK);

        rect_x = -1;
        rect_y = -1;
        appeard_rect = false;

        if (appeard_circle) {

          // fac aceste afisari pentru ca cele 2 elemente sunt suprapuse

          display.drawCircle(circle_x, circle_y, RADIUS, SSD1306_WHITE);
          display.fillCircle(circle_x, circle_y, RADIUS, SSD1306_WHITE);
        }
      }

    }

    // verific daca bila a lovit elementul de pick-up
    if (appeard_circle ) {
      int dist_x = new_x - circle_x;
      int dist_y = new_y - circle_y;
      int dist = sqrt(dist_x * dist_x + dist_y * dist_y);

      if (dist <= RADIUS) {

        tone(BUZZ, 392, 50);

        display.drawCircle(circle_x, circle_y, RADIUS, SSD1306_BLACK);
        display.fillCircle(circle_x, circle_y, RADIUS, SSD1306_BLACK);

        circle_x = -10;
        circle_y  = -10;

        appeard_circle = false;

        clearReverseMoves();
        lastTime = millis();

        if (appeard_rect) {
          // afisare element suprapus (pt ca dispre o parte din acesta cand se apeleaz fill cu BLACK)
          display.drawRect(rect_x, rect_y, width, height, SSD1306_WHITE);
          display.fillRect(rect_x, rect_y, width, height, SSD1306_WHITE);
        }

        // se verifica de care jucator a fost luat elementul
        if (player_ball == 1) {
          rev_player1 += 1;
        } else if (player_ball == 2) {
          rev_player2 += 1;
        }

        drawReverseMoves();
      }

    }


    // verific daca un jucator a schimbat directia bilei

    if (rev_state2 == LOW && rev_player2 > 0) {

      delay(100);

      tone(BUZZ, 500, 50);

      clearReverseMoves();

      rev_player2 -= 1;

      changeBallDirOX(new_x);

      drawReverseMoves();

      player_ball = 2;

    }

    if (rev_state1 == LOW && rev_player1 > 0) {
      delay(100);

      tone(BUZZ, 500, 50);

      clearReverseMoves();

      rev_player1 -= 1;

      changeBallDirOX(new_x);

      drawReverseMoves();

      player_ball = 1;

    }

    // afiare minge
    display.drawPixel(ball_x, ball_y, BLACK);
    display.drawPixel(new_x, new_y, WHITE);
    ball_x = new_x;
    ball_y = new_y;

    ball_update += BALL_RATE;

    update = true;
  }


  // se verifica ce buton este apasat si in functie de asta se modifica pozitia jucatorului
  if (up_state1 == LOW) {
    player_y += 1;
    update = true;
  }

  if (down_state1 == LOW) {
    player_y -= 1;
    update = true;
  }

  if (up_state2 == LOW) {
    player2_y += 1;
    update = true;
  }

  if (down_state2 == LOW) {
    player2_y -= 1;
    update = true;
  }

  if (player2_y < 1) player2_y = 1;
  if (player2_y + PADDLE_HEIGHT > 63) player2_y = 63 - PADDLE_HEIGHT;
  display.drawFastVLine(PLAYER2_X, player2_y, PADDLE_HEIGHT, SSD1306_WHITE);


  if (player_y < 1) player_y = 1;
  if (player_y + PADDLE_HEIGHT > 63) player_y = 63 - PADDLE_HEIGHT;

  display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, SSD1306_WHITE);


  if (update == true) {
    display.display();
    update = false;
  }
}

void changeBallDirOX(uint8_t new_x) {
  // schimba directia bilei pe axa OX

  ball_dir_x = ball_dir_x * (-1);
  new_x += ball_dir_x + ball_dir_x;
}

void changeBallDirOY(uint8_t new_y) {
  // schimba directia bilei pe axa OY

  ball_dir_y = ball_dir_y * (-1);
  new_y += ball_dir_y + ball_dir_y;
}

void changeBallDir(uint8_t new_x, uint8_t new_y) {
  // se schimba directia bilei pe ambele axe de coordonate
  changeBallDirOX(new_x);

  changeBallDirOY(new_y);
}

void drawCourt() {
  // se afiseaza laturile unui dreptungi ce reprezint terenul de joc
  display.drawRect(0, 0, 128, 64, SSD1306_WHITE);
}

void soundBounce()
{
  tone(BUZZ, 500, 50);
}
void soundPoint()
{
  tone(BUZZ, 250, 50);
}

void drawScore() {
  // se afizeaza scorul si numarul de miscari de "reverse" pe care le pot face juctorii

  display.setTextSize(2);

  display.setCursor(45, 0);
  display.println(player1Score);

  display.setCursor(75, 0);
  display.println(player2Score);

  display.display();

  clearReverseMoves();
  drawReverseMoves();
}

void drawReverseMoves() {
  // afisare numar de miscari de reverse ale jucatorilor

  display.setTextSize(1.5);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 2);
  display.print("R:");
  display.println(rev_player2);

  display.setTextSize(1.5);
  display.setCursor(95, 2);
  display.print("R:");
  display.println(rev_player1);

  display.display();
}

void clearReverseMoves() {
  // se face clea pentru a nu se suprapune pixelii pe ecran
  display.setTextSize(1.5);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(25, 2);
  display.print("R:");
  display.println(rev_player2);

  display.setTextSize(1.5);
  display.setCursor(95, 2);
  display.print("R:");
  display.println(rev_player1);

  display.display();
}

void gameOver() {
  // final de joc => se afiseaza castigatorul
  display.fillScreen(BLACK);
  if (player1Score > player2Score)
  {
    display.setCursor(20, 4);
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.print("1st Won");
  } else if (player2Score > player1Score)
  {
    display.setCursor(40, 4);
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.print("2nd WON");
  }

  delay(200);
  display.display();

  // sarbatorire castigare
  playSong();

  delay(1000);
  player2Score = player1Score = 0;
  selMaxPoints = false;
  gameStarted = false;
  rev_player1 = 1;
  rev_player2 = 1;

  display.clearDisplay();

  unsigned long start = millis();
  while (millis() - start < 2000);
  ball_update = millis();
  paddle_update = ball_update;
  resetBall = true;
}

void drawProgressBar() {
  /**
     Se afiseaza o bara de loading inainte de inceprea jocului.
     Ca sa inceapa jocul bara trebuie sa ajunga la 100%
  */
  int progress = 0;
  int x = 0;
  int y = 40;
  int w = 120;
  int h = 20;

  display.setTextSize(1);
  display.setCursor(13, 10);
  display.println(F("Loading..."));

  for (progress = 0; progress <= 100; progress++) {

    float bar = ((float)(w - 4) / 100) * progress;

    display.drawRect(x, y, w, h, WHITE);

    display.fillRect(x + 2, y + 2, bar , h - 4, WHITE);

    display.setCursor((w / 2) - 3, y + 5 );

    display.setTextSize(1);

    if ( progress >= 50) {
      display.setTextColor(BLACK, WHITE); // 'inverted' text
    } else {
      display.setTextColor(WHITE, BLACK);
    }

    display.print(progress);

    display.print("%");

    display.display();
    delay(50);
  }

  display.setTextColor(WHITE, BLACK);
}


void playSong() {

  /**
     a-ha - Take on me

     Element surpriza din proiect.
     Atunci cand cineva castiga jocul, se va sarbatori prin cantarea melodiei la buzzer :).
  */

  for (int thisNote = 0; thisNote < songLength; thisNote++) {
    // calculeaza durata fiecarei note
    int duration = 1000 / durations[thisNote];
    tone(BUZZ, melody[thisNote], duration);
    // se calculeaza delay-ul pentru fiecare nota
    int pause = duration * 1.3;
    delay(pause);
    noTone(BUZZ);
  }
}

/*
Connection quide:

parts needed:
1x TFT 1.8 Display
1x 9V battery connector
1x Arduino Uno/nano/maybe some other
2x resistors

Always be careful and make sure you know what you
are doing while building electrical circuits!!

TFT 1.8 Display:
Vcc   ->  5v
Gnd   ->  Gnd
Cs    ->  10
Reset ->  8
A0    ->  9
SDA   ->  11
Sck   ->  13
LED   ->  3V3

Buttons:
My buttons have side1 and side2, which are 
connected when the button is pressed
B11 describes Button 1, side 1

2    ->  B11
B11  ->  Resistor1  ->  Gnd
B12  ->  3
4    ->  B21
B21  ->  Resistor2  ->  Gnd
B22  ->  5

Power:
+  ->  Vin
-  ->  Gnd

*/

#include <TFT.h> 
#include <SPI.h>
#define cs 10
#define dc 9
#define rst 8

const int BtnLeftIn = 3;
const int BtnLeftOut = 2;
const int BtnRightIn = 4;
const int BtnRightOut = 5;

const int tileSize = 8;
const int boardSize = 14;
const int amountOfSquares = 14*14; 

bool borders = true;
bool leftDown = false;
bool rightDown = false;
bool allowClick = true;
bool gameOver = false;

bool emptySquares[amountOfSquares]; 
const int directions[8] = {0, -1, 1, 0, 0, 1, -1, 0};
//                           up   right left   down
TFT TFTscreen = TFT(cs, dc, rst);


struct Pair {
/*
  Used to store coordinates
*/
  int first;
  int second;
};


class Snake {
/*
  Hold inside all the information about the snake as 
  well as the functions for updating the snake based
  on user actions

  The snake is a queue, where the head of the queue
  is the tail of the snake and vise versa
*/
private:
  Pair* array;
  int capacity;
  int head;
  int tail;
  int direction = 1;
  
  int fruitX = 1;
  int fruitY = 1;

  void resize(int newCapacity) {
    Pair* temp = new Pair[newCapacity];

    for (int i = 0; i < count; i++) {
      temp[i] = array[(head + i) % capacity];
    }

    capacity = newCapacity;
    head = 0;
    tail = count;

    delete[] array;
    array = temp;
  }

  void rotateSnake(int num) {
    // Rotates the snake (if player commands so)
    // num is -1, 0 or 1
    direction = (direction+num)%4;
    if (direction<0) {
      direction += 4;
    }
  }

  void newFruit() {
    /*
    Randomizes a new place for the fruit based on
    the list of free squares
    */
    int square = random(0, boardSize*boardSize-count-1);
    //Serial.print("New: ");
    //Serial.println(square);
    int free = 0;
    int current = 0;
    while (true) {
      if (emptySquares[current]) {
        if (free == square) {
          fruitX = square/boardSize;
          fruitY = square%boardSize;
          break;
        }
        free ++;
      }
      current ++;
    }
  }

  Pair getLast() {
    if (count == 0) {
        return Pair();
    }
    // The index of the most recently added element is (tail - 1 + capacity) % capacity
    int lastIndex = (tail - 1 + capacity) % capacity;
    return array[lastIndex];
  }

  void drawFruit() {
    TFTscreen.stroke(210, 0, 0);
    TFTscreen.rect(tileSize*(fruitX+1), tileSize*(fruitY+1), tileSize, tileSize);
    TFTscreen.stroke(210, 210, 210);
  }

  void drawSquare(int x, int y) {
    //TFTscreen.rect(xStart, yStart, width, height) 
    TFTscreen.rect(tileSize*(x+1), tileSize*(y+1), tileSize, tileSize);
  }

  void eraseSquare(int x, int y) {
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.rect(tileSize*(x+1), tileSize*(y+1), tileSize, tileSize);
    TFTscreen.stroke(210, 210, 210);
  }


public:
  int count;
  Snake() : capacity(2), head(0), tail(0), count(0) {
    array = new Pair[capacity];
  }

  bool updateSnake(int num, bool grow) {
    rotateSnake(num);
    Pair h = getLast();
    int newX = h.first + directions[direction*2];
    int newY = h.second + directions[direction*2 + 1];
    // check if out of bounds
    if (0 > newX || boardSize <= newX ||
        0 > newY || boardSize <= newY) {
          return true;
        }
    // Check collision, note: positions not updated yet
    for (int i = count-4; i>0; i-=2) {
      if (newX == array[(head+i)%capacity].first && newY == array[(head+i)%capacity].second) {
        return true;
      }
    }
    // Check if we eat
    if (newX == fruitX && newY == fruitY) {
      grow = true;
      newFruit();
    }
    
    push({newX, newY});
    drawSquare(newX, newY);
    if (!grow) {
      Pair removed = pop();
      eraseSquare(removed.first, removed.second);
    }
    
    drawFruit();
    return false;
  }

  void push(Pair p) {
    if (count == capacity) {
      resize(capacity * 2);
    }

    array[tail] = p;
    emptySquares[p.first*amountOfSquares+p.second] = false;
    tail = (tail + 1) % capacity;
    count++;
  }

  Pair pop() {
    if (count == 0) {
      return Pair();
    }

    Pair result = array[head];
    head = (head + 1) % capacity;
    count--;

    emptySquares[result.first*amountOfSquares+result.second] = true;

    return result;
  }

  int size() {
    return count;
  }

  void drawSnake() {
    TFTscreen.background(0, 0, 0);
    for (int i = head; i < count+head; i++) {
      drawSquare(array[i%capacity].first, array[i%capacity].second);
    }
  }

  void drawWalls() {
    TFTscreen.rect(7, 7, 8*boardSize+2, 8*boardSize+2);
  }

  void resetSnake() {
    while (count != 0) {
      pop();
    }
    push({5, 5});
    push({5, 6});
    push({5, 7});
    push({5, 8});
    newFruit();
  }

  ~Snake() {
    delete[] array;
  }
};



Snake snake;


void setup() {
  //snake.push({5, 4});
  snake.push({5, 5});
  snake.push({5, 6});
  snake.push({5, 7});
  snake.push({5, 8});

  pinMode(BtnLeftIn, INPUT);
  pinMode(BtnLeftOut, OUTPUT);
  pinMode(BtnRightIn, INPUT);
  pinMode(BtnRightOut, OUTPUT);
  digitalWrite(BtnLeftOut, HIGH);
  digitalWrite(BtnRightOut, HIGH);
  TFTscreen.begin();
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(210, 210, 210);
  TFTscreen.setTextSize(2);
  TFTscreen.setRotation(0);
  snake.drawSnake();
  snake.drawWalls();

  for (int i = 0; i < amountOfSquares; i++) {
    emptySquares[i] = false;
  }
}

int counter = 0;
int difficulty = 5;
bool inGame = true;
bool inDifficultyMenu = false;
bool inGameMenu = false;

void drawDifficulty() {
  //TFTscreen.rect(xStart, yStart, width, height)
}
void harder() {
  
}
void easier() {

}

void loop() {
  
  if (inGame) {
    delay(200/difficulty);
    counter += 1;

    // check if button pressed
    if (digitalRead(BtnLeftIn) == HIGH) {
      if (allowClick) {
        leftDown = true;
        rightDown = false;
      }
      allowClick = false;
    } else if (digitalRead(BtnRightIn) == HIGH) {
      if (allowClick) {
        rightDown = true;
        leftDown = false;
      }
      allowClick = false;
    } else {
      allowClick = true;
    }
    
    if (counter == 10){
      
      counter=0;
      int n = 0;
      if (leftDown) {n = 1;}
      if (rightDown) {n = -1;}

      if (gameOver) {

      } else {
        if (snake.updateSnake(n, false)) {
          gameOver = true;
          TFTscreen.background(0, 0, 0);
          TFTscreen.text("Game Over", 5, 5);
          String text = "Score: " + String(snake.count);
          TFTscreen.text(text.c_str(), 5, 35);

          TFTscreen.setTextSize(1);

          TFTscreen.text("New game", 35, 73);
          TFTscreen.circle(15, 75, 5);

          TFTscreen.text("Change", 40, 103);
          TFTscreen.text("Difficulty", 30, 118);
          TFTscreen.circle(105, 115, 5);

          TFTscreen.setTextSize(2);

          inGame = false;
          inGameMenu = true;
        }
      }
      
      leftDown = false;
      rightDown = false;
    }
  } else if (inDifficultyMenu) {
    delay(50);
    if (digitalRead(BtnLeftIn) == HIGH && digitalRead(BtnRightIn) == HIGH) {
      // start the game
      inGame = true;
      gameOver = false;
      TFTscreen.background(0, 0, 0);
      snake.drawWalls();
    }
    
  } else if (inGameMenu) {
    delay(50);
    if (digitalRead(BtnRightIn) == HIGH) {
      // restart game
      snake.resetSnake();
      inDifficultyMenu = false;
      inGame = true;
      gameOver = false;
      TFTscreen.background(0, 0, 0);
      snake.drawWalls();
    }
  }
}

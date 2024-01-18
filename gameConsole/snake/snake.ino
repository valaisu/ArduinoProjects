#include <TFT.h> 
#include <SPI.h>
#define cs 10
#define dc 9
#define rst 8

int BtnLeftIn = 3;
int BtnLeftOut = 2;
int BtnRightIn = 5;
int BtnRightOut = 4;


TFT TFTscreen = TFT(cs, dc, rst);

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


int tileSize = 8;
int boardSize = 14;
bool borders = true;
// Buttons
bool leftDown = false;
bool rightDown = false;

bool allowClick = true;

int counter = 0;

bool gameOver = false;

int directions[8] = {0, -1, 1, 0, 0, 1, -1, 0};
//                     up   right left   down

const int amountOfSquares = 14*14; 
bool emptySquares[amountOfSquares]; 


struct Pair {
  int first;
  int second;
};


class Snake {
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

public:
  int count;
  Snake() : capacity(2), head(0), tail(0), count(0) {
    array = new Pair[capacity];
  }

  void newFruit() {
    /*
    board has n squares
    keep list of empty squares
    snake len = x
    take rand int between r = [0, n-x-1]
    choose emptySquares[r]
    */
    int square = random(0, boardSize*boardSize-count-1);

    Serial.print("New: ");
    Serial.println(square);
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

  void turn(int num) {
    // num is -1, 0 or 1
    direction = (direction+num)%4;
    if (direction<0) {
      direction += 4;
    }
  }

  void drawWalls() {
    TFTscreen.rect(8, 8, 8*boardSize, 8*boardSize);
    //TFTscreen.rect(1, 1, tileSize, (boardSize+2)*tileSize, tileSize);
    //TFTscreen.rect(0, (boardSize+1)*tileSize, (boardSize+2)*tileSize, (boardSize+2)*tileSize);
  }

  bool updateSnake(int num, bool grow) {
    turn(num);
    Pair h = getLast();
    // check here if out of bounds or self collision
    int newX = h.first + directions[direction*2];
    int newY = h.second + directions[direction*2 + 1];
    if (0 > newX || boardSize <= newX ||
        0 > newY || boardSize <= newY) {
          Serial.print("REKT");
          return true;
        }
    // Check collision, positions not updated yet
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
    //drawSnake();
    drawWalls();
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
      return Pair(); // Return an empty Pair if queue is empty
    }

    Pair result = array[head];
    head = (head + 1) % capacity;
    count--;

    emptySquares[result.first*amountOfSquares+result.second] = true;

    return result;
  }

  Pair getLast() {
    if (count == 0) {
        return Pair(); // Return an empty Pair if the queue is empty
    }

    // The index of the most recently added element is (tail - 1 + capacity) % capacity
    int lastIndex = (tail - 1 + capacity) % capacity;
    return array[lastIndex];
  }

  int size() {
    return count;
  }

  void drawFruit() {
    TFTscreen.stroke(210, 0, 0);
    TFTscreen.rect(tileSize*(fruitX+1), tileSize*(fruitY+1), tileSize, tileSize);
    TFTscreen.stroke(210, 210, 210);
  }

  // drawing could be done more efficiently
  void drawSquare(int x, int y) {
    //TFTscreen.rect(xStart, yStart, width, height) 
    TFTscreen.rect(tileSize*(x+1), tileSize*(y+1), tileSize, tileSize);
  }

  void eraseSquare(int x, int y) {
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.rect(tileSize*(x+1), tileSize*(y+1), tileSize, tileSize);
    TFTscreen.stroke(210, 210, 210);
  }

  void drawSnake() {
    TFTscreen.background(0, 0, 0);

    for (int i = head; i < count+head; i++) {
      drawSquare(array[i%capacity].first, array[i%capacity].second);
    }
  }

  ~Snake() {
    delete[] array;
  }
};





Snake snake;


void setup() {
  // put your setup code here, to run once:
  //snake.push({5, 4});
  snake.push({5, 5});
  snake.push({5, 6});
  snake.push({5, 7});
  snake.push({5, 8});

  Serial.begin(9600);

  pinMode(BtnLeftIn, INPUT);
  pinMode(BtnLeftOut, OUTPUT);
  pinMode(BtnRightIn, INPUT);
  pinMode(BtnRightOut, OUTPUT);
  digitalWrite(BtnLeftOut, HIGH);
  digitalWrite(BtnRightOut, HIGH);
  TFTscreen.begin();
  TFTscreen.background(0, 0, 0);
  snake.drawSnake();
  TFTscreen.stroke(210, 210, 210);
  TFTscreen.setTextSize(2);

  for (int i = 0; i < amountOfSquares; i++) {
    emptySquares[i] = false; // Initialize each element to false
  }
}

void loop() {
  
  delay(40);
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
    if (leftDown) {n = -1;}
    if (rightDown) {n = 1;}

    if (gameOver) {

    } else {
      if (snake.updateSnake(n, false)) {
        gameOver = true;
        TFTscreen.background(0, 0, 0);
        TFTscreen.text("Game Over", 5, 45);
        String text = "Score: " + String(snake.count);
        TFTscreen.text(text.c_str(), 5, 75);
      }
    }
    
    leftDown = false;
    rightDown = false;
  }

}

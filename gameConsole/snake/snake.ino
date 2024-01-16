#include <TFT.h> 
#include <SPI.h>
#define cs 10
#define dc 9
#define rst 8

int BtnLeftIn = 2;
int BtnLeftOut = 3;
int BtnState = 0;

TFT TFTscreen = TFT(cs, dc, rst);



int tileSize = 8;
int boardSize = 10;
bool borders = true;
// Buttons
bool leftDown = false;
bool rightDown = false;

int counter = 0;

int directions[8] = {0, -1, 1, 0, 0, 1, -1, 0};
//                     up   right left   down


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
  int count;
  int direction = 1;

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
  Snake() : capacity(2), head(0), tail(0), count(0) {
    array = new Pair[capacity];
  }

  void turn(int num) {
    // num is -1, 0 or 1
    direction = (direction+num)%4;
    if (direction<0) {
      direction += 4;
    }
  }

  void updateSnake(int num, bool grow) {
    turn(num);
    Pair h = getLast();
    Serial.println("");
    Serial.print("Old: ");
    Serial.println(h.first);
    Serial.println(h.second);
    Serial.print("Addition: ");
    Serial.println(directions[direction*2]);
    Serial.println(directions[direction*2+1]);
    Serial.print("New: ");
    Serial.println(h.first+directions[direction*2]);
    Serial.println(h.second+directions[direction*2+1]);
    Serial.println(direction*2);
    Serial.println(direction*2+1);
    Serial.print("Directions: ");
    Serial.print((-1)%4);
    
    for (int i = 0; i < 8; i++) {
      Serial.println(directions[i]);
    }

    

    push({h.first+directions[direction*2], h.second+directions[direction*2+1]});
    if (!grow) {
      pop();
    }

    Serial.println("The array: ");

    for (int i = 0; i < count; i++) {

      Serial.print(array[i].first);
      Serial.print(" ");
      Serial.println(array[i].second);
    }

    drawSnake();
  }

  void push(Pair p) {
    if (count == capacity) {
      resize(capacity * 2);
    }

    array[tail] = p;
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

    // Optional: shrink the array size if too many elements are removed
    if (count > 0 && count == capacity / 4) {
      resize(capacity / 2);
    }

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

  // drawing could be done more efficiently
  void drawSquare(int x, int y) {
    //TFTscreen.rect(xStart, yStart, width, height) 
    TFTscreen.rect(tileSize*(x+1), tileSize*(y+1), tileSize, tileSize);
  }

  void drawSnake() {
    TFTscreen.background(0, 0, 0);

    Serial.print("head: ");
    Serial.println(head);

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
  snake.push({5, 5});
  snake.push({5, 6});
  snake.push({5, 7});
  snake.push({5, 8});

  Serial.begin(9600);

  pinMode(BtnLeftIn, INPUT);
  pinMode(BtnLeftOut, OUTPUT);
  digitalWrite(BtnLeftOut, HIGH);
  TFTscreen.begin();
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(210, 210, 210);
  TFTscreen.setTextSize(2);
}

void loop() {
  
  delay(50);
  counter += 1;

  // check if button pressed
  if (digitalRead(BtnLeftIn) == LOW) {
    leftDown = true;
    rightDown = false;
  /*} else if (buttonRight) {
    rightDown = true;
    leftDown = false;
  */
  }
  
  if (counter == 10){
    
    counter=0;
    // update game
    int n = 0;
    if (leftDown) {n = -1;}
    if (rightDown) {n = 1;}
    Serial.println("Direction: ");
    Serial.print(n);
    Serial.println();
    snake.updateSnake(n, false);
    leftDown = false;
    rightDown = false;
  }

}

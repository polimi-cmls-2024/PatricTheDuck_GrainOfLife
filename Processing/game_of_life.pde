//library import for OSC and network communications
import oscP5.*;
import netP5.*;

//variables declaration and initialization
OscP5 oscP5;
NetAddress myRemoteLocation;
int width = 100;
int height = 100;
int x = 10;
int y = 10;
int offsetX;
int offsetY;
int button =0;
int cellSize = 10;
int[][] grid = new int[width][height];
int[][] showedGrid = new int[width][height];
int[][] pointedGrid = new int [width][height];
int[][] twoGliderMess = new int[12][6];
int[] pointedCell = new int [2];

//Panel and OSC server boot
void settings() {
  size(width * cellSize, height * cellSize);
  oscP5 = new OscP5(this, 35790);
  myRemoteLocation = new NetAddress("127.0.0.1", 57120);
}

//map grid and pointed cell initialization
void setup() {
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      pointedGrid[i][j] = 0;
    }
  }
  grid[0][0] = 1;
  showedGrid = grid;

  //inizializzazione 2 glider mess
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 6; j++) {
      twoGliderMess[i][j] = 0;
    }
  }
  //first block
  twoGliderMess[2][0] = 1;
  twoGliderMess[0][1] = 1;
  twoGliderMess[2][1] = 1;
  twoGliderMess[1][2] = 1;
  twoGliderMess[2][2] = 1;
  
  //second block
  twoGliderMess[11][2] = 1;
  twoGliderMess[9][4] = 1;
  twoGliderMess[10][4] = 1;
  twoGliderMess[10][5] = 1;
  twoGliderMess[11][5] = 1;
  
  pointedCell[0] = 50;
  pointedCell[1] = 50;
}


//Main looped function, information on how to color every pixel
//gameLoop() function is called, there is the logic for interactive game of life
void draw() {
  background(255);
  //inizializzazione random
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      if (showedGrid[i][j] == 0) {
        fill(0);
        /*
        float midX = width / 2.0f;
        float midY = height / 2.0f;
        if (i < midX && j < midY) {
          fill(205, 205, 0); // Mustard yellow
        } else if (i >= midX && j < midY) {
          fill(0, 153, 76); // Dark teal
        } else if (i < midX && j >= midY) {
          fill(32, 74, 135); // Dark blue
        } else {
          fill(127, 127, 127); 
        }
        */
      }
      if(showedGrid[i][j] == 1){
        fill(255);  // white, pixel vivi
      }
      if(showedGrid[i][j] == 2){
        fill(255,0,0);  // rossi, pixel puntati
      }
      rect(i * cellSize, j * cellSize, cellSize, cellSize);
    }
  }
  gameLoop(); 
}


//main game loop function.
//detailed description throughout the single code blocks
void gameLoop() {
  //nextGrid initialization, this is where we compute the evolution of the game of life map for the t = t+1
  int[][] nextGrid = new int[width][height];
  System.gc();
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      pointedGrid[i][j] = 0;
    }
  }  
  
  //this block contains the logic for Conways game of life
        for (int i = 0; i < width; i++) {
          for (int j = 0; j < height; j++) { 
          //conteggio vivi morti
            int aliveNeighbors = countAliveNeighbors(i, j);
            if (grid[i][j] == 0 && aliveNeighbors == 3) {
              nextGrid[i][j] = 1;  // pixel morti diventano vivi con 3 adiacenti vivi
            } else if (grid[i][j] == 1 && (aliveNeighbors < 2 || aliveNeighbors > 3)) {
              nextGrid[i][j] = 0;  // pixel vivi muoiono con meno di 2 o piú di 3 piexel vivi adiacenti
            } else {
              nextGrid[i][j] = grid[i][j];  
            }
          }
         }
      grid = nextGrid;
      
      //after having computed the evolution at time t+1, we send data to Supercollider reguarding the runtime evolution
      //of the game of life evolution
      OscMessage myMessage = new OscMessage("/play");
      myMessage.add(numberOfAlives()); 
      myMessage.add(pointedGrid[0]);
      myMessage.add(pointedGrid[1]);
      oscP5.send(myMessage, myRemoteLocation); 

      //update of the pointing cursor and index out of bounds checking before every array call  
      if(pointedCell[0]+offsetX>=0 && pointedCell[0]+offsetX<width && pointedCell[1]+offsetY>=0 && pointedCell[1]+offsetY<height){
        pointedCell[0] = pointedCell[0] + offsetX;
        pointedCell[1] = pointedCell[1] + offsetY;
      }
      if(pointedCell[0]>=0 && pointedCell[0]<width && pointedCell[1]>=0 && pointedCell[1]<height){
        pointedGrid[pointedCell[0]][pointedCell[1]] = 1;
        for (int i = 0; i < width; i++) {
          for (int j = 0; j < height; j++) { 
            if(showedGrid[i][j]==2 && i!=pointedCell[0] && j!=pointedCell[1]){
              showedGrid[i][j] = nextGrid[i][j];
            }
            if(pointedGrid[i][j]==1 && pointedCell[0]>6 && pointedCell[0]<width-6 && 
              pointedCell[1]>4 && pointedCell[1]<height-4){
                draw2GliderMess(pointedCell[0],pointedCell[1]);
                i=i+6;
                j=j+3;
            }else{
              showedGrid[i][j] = nextGrid[i][j];
            }
          }
        }
      }
}

//function that takes a pixel coordinate as input and returns the number of alive pixels around the input one 
int countAliveNeighbors(int x, int y) {
  int count = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      int neighborX = (x + i + width) % width;
      int neighborY = (y + j + height) % height;
      if(grid[neighborX][neighborY]==1){
        count += grid[neighborX][neighborY];
      }
    }
  }
  //la cella corrente non viene contata
  count -= grid[x][y];
  return count;
}


//function used to drop a 2-glider-mess at pointed position.
//not used in the final version of the project, it is still here for 
//his utility during debugging
void mousePressed() {
  int[] cell = getGridCell(mouseX, mouseY);
  int cellX = cell[0];
  int cellY = cell[1];
  if(cellX>7 && cellX<width-7 && cellY>4 && cellY<height-4){
  for(int i=0 ; i<12 ; i++){
    for(int j=0; j<6 ; j++){
      if(twoGliderMess[i][j]==1){
        showedGrid[cell[0]-6+i][cell[1]-3+j] = 1;
        grid[cell[0]-6+i][cell[1]-3+j] = 1;
    }
  }
  }
 } 
}

//function that takes a normalized screen coordinate as input and returns the
//corresponding game of life pixel coordinate
int[] getGridCell(int x, int y) {
  int[] cell = new int[2];
  cell[0] = x / cellSize; // Normalize mouseX
  cell[1] = y / cellSize; // Normalize mouseY
  return cell;
}

//boolean function that returns true if the pointed cell has changed from the 
//previous iteration, used in the gameLoop() function. Obviously returns flse otherwise
boolean pointedCellChanged(int x, int y){
  if(x==pointedCell[0] && y==pointedCell[1]){
    return false;
  }else{
    return true;
  }
}

//function that prints a 2-glider-mess centered at the given coordinates
//taken as input
void draw2GliderMess(int curr_i, int curr_j){
  for(int i=0 ; i<width ; i++){
      for(int j=0; j<height ; j++){
        if(showedGrid[i][j] == 2){
          showedGrid[i][j] = 0;
        }
      }
    }
    for(int i=0 ; i<12 ; i++){
      for(int j=0; j<6 ; j++){
        if(twoGliderMess[i][j]==1){
          showedGrid[curr_i-6+i][curr_j-3+j] = 2;
        }
      }
    }
}

//function that returns the overall number of alive pixels
int numberOfAlives(){
  int count=0;
  for(int i=0 ; i<width ; i++){
    for(int j=0 ; j<height ; j++){
      if (grid[i][j] == 1){
        count++;
      }
    }
  }
  return count;
}

//async function called everytime an OSC message is received from any source.
//3 main if clauses reguarding the three main type of messages that can be received
//from arduino. More details innside the function implementation 
void oscEvent(OscMessage theOscMessage) {
  //1) coordinates type of message, the message is composed by 2 arguments. They represents
  //movement of the joystick on the two different axis
  if(theOscMessage.addrPattern().equals("/coordinates")){
      int inputX = theOscMessage.get(0).intValue(); 
      int inputY = theOscMessage.get(1).intValue(); 
      offsetX = (inputX - 506)/200;
      offsetY = -((inputY - 502))/200;
  }
  
  //OSC MESSAGE RELATIVE TO SINGLE BUTTON CLICK
  if(theOscMessage.addrPattern().equals("/button")){
    if (theOscMessage.arguments().length >= 1) {
      int button = theOscMessage.get(0).intValue(); 
      if(button ==1){
        if(pointedCell[0]>7 && pointedCell[0]<width-7 && pointedCell[1]>4 && pointedCell[1]<height-4){
          for(int i=0 ; i<12 ; i++){
            for(int j=0; j<6 ; j++){
              if(twoGliderMess[i][j]==1){
                showedGrid[pointedCell[0]-6+i][pointedCell[1]-3+j] = 1;
                grid[pointedCell[0]-6+i][pointedCell[1]-3+j] = 1;
              }    
            }
          }
        }
      }
    }
  }
  
  //OSC MESSAGE RELATIVE TO MAP RESET 
  if(theOscMessage.addrPattern().equals("/reset")){
    for(int i=0 ; i<width ; i++){
      for(int j=0 ; j<height ; j++){
        showedGrid[i][j] = 0;
        grid[i][j] = 0;
        pointedCell[0] = 50;
        pointedCell[0] = 50;
      }
    }
  }
  
}
    





  

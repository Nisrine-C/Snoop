#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <raylib.h>

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define SNAKE_LENGTH   256
#define SQUARE_SIZE     90
#define BORDER_SIZE     20
#define MAX_BLOCKS1      8
#define MAX_BLOCKS2     14
#define MAX_BIRDS        4
#define COLUMNS          9
#define ROWS             8

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct Bird {
    Vector2 position;
    Vector2 size;
    bool active;
} Bird;


typedef struct Snoopy {
    Vector2 position;
    Vector2 size;
    int speed;
    int direction; //0 down 1 right 2 up 3 left
    Texture2D textures[4] ;  // New member for the texture
} Snoopy;

typedef struct Ball {
    Vector2 position;
    Vector2 size;
    int speed;
    int direction; //0 down 1 right 2 up 3 left
} Ball;

typedef struct Block {
    Vector2 position;
    Vector2 size;
    int type; //0 static 1 moveable
    int direction; //0 down 1 right 2 up 3 left
} Block;


//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = SQUARE_SIZE*COLUMNS + BORDER_SIZE*2;
static const int screenHeight = SQUARE_SIZE*ROWS + BORDER_SIZE*2;

static int framesCounter = 0;
static int counter = 60;
static int time = 0;
static bool gameOver = false;
static int level = 1;
static int MAX_BLOCKS = 0;
static bool isWin = false;

static Music music;
static Sound birdsFx;
static Sound loseFx;
static Sound winFx;
static bool soundPlayedWin = false;
static int score =0 ;
static bool allBirdsInactive = true;

static Texture2D birdTexture;
static Texture2D blockTexture;
static Texture2D ballTexture;


Bird birds[MAX_BIRDS];
Block blocks[MAX_BLOCKS2];
Snoopy snoopy;
Ball ball;

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void InitLevel(int n);
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
  // Initialization 
  //---------------------------------------------------------

  InitWindow(screenWidth, screenHeight, "Snoopy's magic show");

  InitAudioDevice();

  InitGame();

  #if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 30, 1);
  #else
    SetTargetFPS(24);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update and Draw
        //----------------------------------------------------------------------------------
        UpdateDrawFrame();
        //----------------------------------------------------------------------------------
    }
  #endif
    // De-Initialization
    //--------------------------------------------------------------------------------------
    // Unload loaded data (textures, sounds, models...)
    UnloadGame();
    // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

  return 0;
}


//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(void)
{
  birdsFx = LoadSound("./assets/sound/pickup.ogg");
  loseFx = LoadSound("./assets/sound/lose.ogg");
  winFx = LoadSound("./assets/sound/win.ogg");
  music = LoadMusicStream("./assets/sound/soundtrack.mp3");
  snoopy.textures[0] = LoadTexture("./assets/img/snoopy_down.png");
  snoopy.textures[1] = LoadTexture("./assets/img/snoopy_right.png");
  snoopy.textures[2] = LoadTexture("./assets/img/snoopy_up.png");
  snoopy.textures[3] = LoadTexture("./assets/img/snoopy_left.png");
  birdTexture = LoadTexture("./assets/img/bird.png");
  blockTexture = LoadTexture("./assets/img/block.png");
  ballTexture = LoadTexture("./assets/img/ball.png");

  snoopy.size = (Vector2){SQUARE_SIZE-2 , SQUARE_SIZE-2 };
  snoopy.speed = SQUARE_SIZE;
  snoopy.direction = 0;

  ball.size = (Vector2){SQUARE_SIZE/2-5 , SQUARE_SIZE/2-5 };;
  ball.speed = SQUARE_SIZE / 6;
  ball.direction = 45;

  for (int i = 0; i < MAX_BIRDS; i++) {
    birds[i].size = (Vector2){ SQUARE_SIZE-3, SQUARE_SIZE-3 };
    birds[i].active = true;
  }

  InitLevel(level);
  PlayMusicStream(music);
}

// Draw game (one frame)
void DrawGame(void)
{
  BeginDrawing();

  ClearBackground(WHITE);

    if (!gameOver){
      // Draw grid lines
      for (int i = 0; i < screenWidth / SQUARE_SIZE + 1; i++)
        {
          DrawLineEx((Vector2){SQUARE_SIZE * i + BORDER_SIZE, BORDER_SIZE},
          (Vector2){SQUARE_SIZE * i + BORDER_SIZE, screenHeight - BORDER_SIZE},
          5, DARKGREEN);
        }

        for (int i = 0; i < screenHeight / SQUARE_SIZE + 1; i++){
          DrawLineEx((Vector2){BORDER_SIZE, SQUARE_SIZE * i + BORDER_SIZE},
          (Vector2){screenWidth - BORDER_SIZE, SQUARE_SIZE * i + BORDER_SIZE},
          5, DARKGREEN);  
        }

        // Draw snoopy
        DrawTexturePro(snoopy.textures[snoopy.direction], (Rectangle){0, 0, snoopy.size.x, snoopy.size.y},
        (Rectangle){snoopy.position.x, snoopy.position.y, snoopy.size.x, snoopy.size.y}, (Vector2){0, 0}, 0.0f, WHITE);

        //Draw birds
        for (int i = 0; i < MAX_BIRDS; i++) {
          if(birds[i].active){
            DrawTexturePro(birdTexture, (Rectangle){0, 0, birds[i].size.x, birds[i].size.y},
            (Rectangle){birds[i].position.x, birds[i].position.y, birds[i].size.x, birds[i].size.y}, (Vector2){0, 0}, 0.0f, WHITE);
          }
        }

        //Draw ball
        DrawTexturePro(ballTexture, (Rectangle){0, 0, ball.size.x, ball.size.y},
        (Rectangle){ball.position.x, ball.position.y, ball.size.x, ball.size.y}, (Vector2){0, 0}, 0.0f, WHITE);

        //Draw blocks
        for (int i = 0; i < MAX_BLOCKS; i++) {
          DrawTexturePro(blockTexture, (Rectangle){0, 0, blocks[i].size.x, blocks[i].size.y},
            (Rectangle){blocks[i].position.x, blocks[i].position.y, blocks[i].size.x, blocks[i].size.y}, (Vector2){0, 0}, 0.0f, WHITE);
        }
    } else {
       // Draw score
      DrawText(TextFormat("SCORE: %d", score), GetScreenWidth()/2 - MeasureText("SCORE: 0", 20)/2, GetScreenHeight()/2 - 100, 20, GRAY);
      if(isWin && level == 2){
         // Draw instruction to play again
        DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
      }else if (isWin && level ==1){
        DrawText("PRESS [ENTER] TO PLAY NEXT", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY NEXT", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
      }else{
         // Draw instruction to play again
        DrawText("PRESS [ENTER] TO PLAY AGAIN", GetScreenWidth()/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, GetScreenHeight()/2 - 50, 20, GRAY);
      }
     }
    EndDrawing();
}

// Update game (one frame)
void UpdateGame(void)
{
      // Audio
        // SoundTrack
          //----------------------------------------------------------------------------------
          UpdateMusicStream(music);   // Update music buffer with new stream data
          float timePlayed = 0.0f;  
          // Get normalized time played for current music stream
          timePlayed = GetMusicTimePlayed(music)/GetMusicTimeLength(music);
          bool soundPlayed = false;

          if (timePlayed > 1.0f) {
            StopMusicStream(music);
            PlayMusicStream(music);
          }   // Make sure time played is no longer than music
          //----------------------------------------------------------------------------------
        //
        // Soundfx
        //
        // Logic
          bool ballCanMove = true;
          // Check for key input to restart the game
          if (gameOver && IsKeyPressed(KEY_ENTER)) {
            if(!isWin){
              score = 0;
              framesCounter = 0;
              gameOver = false;
              soundPlayedWin = false;
              PlayMusicStream(music);
              for (int i = 0; i < MAX_BIRDS; i++) {
                birds[i].active = true;
              }
              InitLevel(level);
            }else{
              if(level == 1){
                level++;
                framesCounter = 0;
                gameOver = false;
                soundPlayedWin = false;
                PlayMusicStream(music);
                for (int i = 0; i < MAX_BIRDS; i++) {
                    birds[i].active = true;
                }
                InitLevel(level);
              }else{
                score = 0;
                level = 1;
                framesCounter = 0;
                gameOver = false;
                soundPlayedWin = false;
                PlayMusicStream(music);
                for (int i = 0; i < MAX_BIRDS; i++) {
                  birds[i].active = true;
                }
                InitLevel(level);
              }
            }
        }
          if ((framesCounter % 4) == 0) {
            bool canMove = true;
            if (IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT) && snoopy.position.y + snoopy.speed < screenHeight - BORDER_SIZE) {
              canMove = true;
              for (int i = 0; i < MAX_BLOCKS; i++) {
                  if(blocks[i].type==1){
                    if (snoopy.position.y + SQUARE_SIZE == blocks[i].position.y && 
                        snoopy.position.x == blocks[i].position.x) {
                      if (snoopy.direction == blocks[i].direction && blocks[i].position.y + SQUARE_SIZE < screenHeight - BORDER_SIZE) {
                        blocks[i].position.y += SQUARE_SIZE;
                      } else {
                        canMove = false;
                      }
                    }
                  } else if (snoopy.position.y + SQUARE_SIZE == blocks[i].position.y && snoopy.position.x == blocks[i].position.x) {
                    canMove = false;
                  }
                }
              if(canMove){
                snoopy.position.y += snoopy.speed;
              }else{
                canMove = false;
              }
              snoopy.direction = 0;
            }
            if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_UP) && snoopy.position.x + snoopy.speed < screenWidth - BORDER_SIZE) {
                canMove = true;
                snoopy.direction = 1; 
                for (int i = 0; i < MAX_BLOCKS; i++) {
                  if(blocks[i].type==1){
                    if (snoopy.position.y == blocks[i].position.y &&
                    snoopy.position.x + snoopy.speed == blocks[i].position.x ) {
                      if (snoopy.direction == blocks[i].direction && blocks[i].position.x + SQUARE_SIZE < screenWidth - BORDER_SIZE) {
                        blocks[i].position.x += SQUARE_SIZE;
                      } else {
                        canMove = false;
                      }
                    }
                    } else if (snoopy.position.x + snoopy.speed == blocks[i].position.x && snoopy.position.y == blocks[i].position.y) {
                    canMove = false;
                  }
                }
                if(canMove){
                  snoopy.position.x += snoopy.speed;
                }else{
                  canMove = false;
                }
            } 
            if (IsKeyDown(KEY_UP) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT) && snoopy.position.y - snoopy.speed > 0 ) {
                canMove = true;
                snoopy.direction = 2;
                for (int i = 0; i < MAX_BLOCKS; i++) {
                  if(blocks[i].type==1){
                    if (snoopy.position.y - SQUARE_SIZE == blocks[i].position.y && 
                        snoopy.position.x == blocks[i].position.x) {
                      if (snoopy.direction == blocks[i].direction && blocks[i].position.y - SQUARE_SIZE > 0) {
                        blocks[i].position.y -= SQUARE_SIZE;
                      } else {
                        canMove = false;
                      }
                    }
                  } else if (snoopy.position.y - SQUARE_SIZE == blocks[i].position.y && snoopy.position.x == blocks[i].position.x) {
                    canMove = false;
                  }
                }
                if(canMove){
                  snoopy.position.y -= snoopy.speed;
                }else{
                  canMove = false;
                }
                
            }
            if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_UP) && snoopy.position.x - snoopy.speed > 0) {
                canMove = true;
                snoopy.direction = 3;
                for (int i = 0; i < MAX_BLOCKS; i++) {
                  if(blocks[i].type==1){
                    if (snoopy.position.y == blocks[i].position.y &&
                    snoopy.position.x - snoopy.speed == blocks[i].position.x ) {
                      if (snoopy.direction == blocks[i].direction && blocks[i].position.x - SQUARE_SIZE > 0) {
                        blocks[i].position.x -= SQUARE_SIZE;
                      } else {
                        canMove = false;
                      }
                    }
                  } else if (snoopy.position.x -snoopy.speed == blocks[i].position.x && snoopy.position.y == blocks[i].position.y) {
                    canMove = false;
                  }
                }
                if(canMove){
                  snoopy.position.x -= snoopy.speed;
                }else{
                  canMove = false;
                }
                
            }
          }
          // Check if all birds are inactive
          allBirdsInactive = true;
          for (int i = 0; i < MAX_BIRDS; i++) {
              if (birds[i].active) {
                  allBirdsInactive = false;
                  break; // Exit the loop early if at least one bird is still active
              }
          }
          if(allBirdsInactive){
            if(gameOver && !soundPlayedWin){
                PlaySound( winFx);
                WaitTime(2.0);
                StopSound(winFx);
              soundPlayedWin = true;
          }
          }
          // Update Ball movement
          if((framesCounter % 1) == 0){
            if(ballCanMove)
            {ball.position.x += ball.speed * cos(DEG2RAD * ball.direction);
            ball.position.y += ball.speed * sin(DEG2RAD * ball.direction);
            // Check collision with walls for Ball
            if (ball.position.x <= BORDER_SIZE || ball.position.x >= screenWidth - BORDER_SIZE - ball.size.x) {
                ball.direction = 180 - ball.direction; // Bounce horizontally
            }
            if (ball.position.y <= BORDER_SIZE || ball.position.y >= screenHeight - BORDER_SIZE - ball.size.y) {
                ball.direction = 360 - ball.direction; // Bounce vertically
            }
            // Iterate through each block
          for (int i = 0; i < MAX_BLOCKS; i++) {
            if (ball.position.x < blocks[i].position.x + blocks[i].size.x &&
              ball.position.x + ball.size.x > blocks[i].position.x &&
              ball.position.y < blocks[i].position.y + blocks[i].size.y &&
              ball.position.y + ball.size.y > blocks[i].position.y) 
            {
              float overlapLeft = ball.position.x + ball.size.x - blocks[i].position.x;
              float overlapRight = blocks[i].position.x + blocks[i].size.x - ball.position.x;
              float overlapTop = ball.position.y + ball.size.y - blocks[i].position.y;
              float overlapBottom = blocks[i].position.y + blocks[i].size.y - ball.position.y;

              float minOverlapX = fminf(overlapLeft, overlapRight);
              float minOverlapY = fminf(overlapTop, overlapBottom);

              // Adjust the ball's position based on the minimum overlap
              if (minOverlapX < minOverlapY) {
                // Adjust horizontally
                ball.position.x += (overlapLeft < overlapRight) ? -minOverlapX : minOverlapX;
                ball.direction = 180 - ball.direction;
              } else {
                // Adjust vertically
                ball.position.y += (overlapTop < overlapBottom) ? -minOverlapY : minOverlapY;
                ball.direction = (360 - ball.direction) % 360;
              }
            }
          }
          // Collision between Snoopy and the ball
          if (CheckCollisionRecs((Rectangle){snoopy.position.x, snoopy.position.y, snoopy.size.x, snoopy.size.y},
                                (Rectangle){ball.position.x, ball.position.y, ball.size.x, ball.size.y}))
          {
            gameOver = true; 
            StopMusicStream(music);
            PlaySound(loseFx);
          }}


          }

          // Set gameOver to true if all birds are inactive
          if (allBirdsInactive) {
            if (!gameOver)
            {
              score += (counter - framesCounter / 24) * 100;
              }
              gameOver = true;
              isWin = true;
              time = framesCounter / 24;
              StopMusicStream(music);
          }

          //Collision between snoopy and birds
          for (int i = 0; i < MAX_BIRDS; i++) {
            if ((snoopy.position.x < (birds[i].position.x + birds[i].size.x) && (snoopy.position.x + snoopy.size.x) > birds[i].position.x) &&
                (snoopy.position.y < (birds[i].position.y + birds[i].size.y) && (snoopy.position.y + snoopy.size.y) > birds[i].position.y))
            {
              if (!soundPlayed && birds[i].active) {
                PlaySound(birdsFx);
                soundPlayed = true;
              }
              birds[i].active = false; // Set the game over condition if Snoopy collides with a bird
              soundPlayed = false;
            }
          }

          if((framesCounter /24) == counter){
            gameOver = true;
            StopMusicStream(music);
            PlaySound(loseFx);
          }
          // Increment frame counter
          framesCounter++;
}

// Unload game variables
void UnloadGame(void)
{
  UnloadMusicStream(music);   // Unload music stream buffers from RAM
  for (int i = 0; i < MAX_BIRDS; i++)
    UnloadTexture(birdTexture);

  for (int i = 0; i < 4; i++)
    UnloadTexture(snoopy.textures[i]);
  
  CloseAudioDevice();         // Close audio device (music streaming is automatically stopped)
  CloseWindow();              // Close window and OpenGL context
}

// Update and Draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}

void InitLevel(int n) {
  switch(n){
    case 1:
      MAX_BLOCKS = MAX_BLOCKS1;
      for (int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i].size = (Vector2){ SQUARE_SIZE-2, SQUARE_SIZE-2};
      } 
      birds[0].position = (Vector2){BORDER_SIZE+1, BORDER_SIZE+1};
      birds[1].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*8)+1, BORDER_SIZE+1 };
      birds[2].position = (Vector2){ BORDER_SIZE+1 , (BORDER_SIZE+SQUARE_SIZE*7)+1};
      birds[3].position = (Vector2){ (BORDER_SIZE+SQUARE_SIZE*8)+1,(BORDER_SIZE+SQUARE_SIZE*7)+1 };


      snoopy.position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*3)+1, (BORDER_SIZE+SQUARE_SIZE*4)+1};
      
      blocks[0].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*4)+1, (BORDER_SIZE+SQUARE_SIZE*2)+1};
      blocks[1].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*4)+1, (BORDER_SIZE+SQUARE_SIZE*3)+1};
      blocks[2].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*4)+1, (BORDER_SIZE+SQUARE_SIZE*4)+1};
      blocks[3].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*4)+1, (BORDER_SIZE+SQUARE_SIZE*5)+1};

      blocks[0].type = 1;blocks[0].direction = 1;
      blocks[1].type = 1;blocks[1].direction = 1;
      blocks[2].type = 1;blocks[2].direction = 1;
      blocks[3].type = 1;blocks[3].direction = 1;

      blocks[4].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*1)+1, (BORDER_SIZE+SQUARE_SIZE*1)+1};
      blocks[5].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*7)+1, (BORDER_SIZE+SQUARE_SIZE*1)+1};
      blocks[6].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*1)+1, (BORDER_SIZE+SQUARE_SIZE*6)+1};
      blocks[7].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*7)+1, (BORDER_SIZE+SQUARE_SIZE*6)+1};
      blocks[4].type = 0;
      blocks[5].type = 0;
      blocks[6].type = 0;
      blocks[7].type = 0;

      ball.position = (Vector2){BORDER_SIZE+SQUARE_SIZE*4, BORDER_SIZE};
      break;
    case 2:
      MAX_BLOCKS = MAX_BLOCKS2;
      for (int i = 0; i < MAX_BLOCKS; i++) {
        blocks[i].size = (Vector2){ SQUARE_SIZE-2, SQUARE_SIZE-2};
      } 
      birds[0].position = (Vector2){BORDER_SIZE+1, BORDER_SIZE+1};
      birds[1].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*8)+1, BORDER_SIZE+1 };
      birds[2].position = (Vector2){ BORDER_SIZE+1 , (BORDER_SIZE+SQUARE_SIZE*7)+1};
      birds[3].position = (Vector2){ (BORDER_SIZE+SQUARE_SIZE*8)+1,(BORDER_SIZE+SQUARE_SIZE*7)+1 };


      snoopy.position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*4)+1, (BORDER_SIZE+SQUARE_SIZE*4)+1};
      
      blocks[6].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*3)+1, (BORDER_SIZE+SQUARE_SIZE*2)+1};
      blocks[7].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*2)+1, (BORDER_SIZE+SQUARE_SIZE*3)+1};
      blocks[8].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*2)+1, (BORDER_SIZE+SQUARE_SIZE*4)+1};
      blocks[9].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*3)+1, (BORDER_SIZE+SQUARE_SIZE*5)+1};
      
      blocks[10].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*5)+1, (BORDER_SIZE+SQUARE_SIZE*2)+1};
      blocks[11].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*6)+1, (BORDER_SIZE+SQUARE_SIZE*3)+1};
      blocks[12].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*6)+1, (BORDER_SIZE+SQUARE_SIZE*4)+1};
      blocks[13].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*5)+1, (BORDER_SIZE+SQUARE_SIZE*5)+1};

      blocks[6].type = 1;blocks[6].direction = 3;
      blocks[7].type = 1;blocks[7].direction = 3;
      blocks[8].type = 1;blocks[8].direction = 3;
      blocks[9].type = 1;blocks[9].direction = 3;

      blocks[10].type = 1;blocks[10].direction = 1;
      blocks[11].type = 1;blocks[11].direction = 1;
      blocks[12].type = 1;blocks[12].direction = 1;
      blocks[13].type = 1;blocks[13].direction = 1;

      blocks[0].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*1)+1, (BORDER_SIZE+SQUARE_SIZE*1)+1};
      blocks[1].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*7)+1, (BORDER_SIZE+SQUARE_SIZE*1)+1};
      blocks[2].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*1)+1, (BORDER_SIZE+SQUARE_SIZE*6)+1};
      blocks[3].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*7)+1, (BORDER_SIZE+SQUARE_SIZE*6)+1};
      blocks[4].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*4)+1, (BORDER_SIZE+SQUARE_SIZE*6)+1};
      blocks[5].position = (Vector2){(BORDER_SIZE+SQUARE_SIZE*4)+1, (BORDER_SIZE+SQUARE_SIZE*1)+1};

      blocks[0].type = 0;
      blocks[1].type = 0;
      blocks[2].type = 0;
      blocks[3].type = 0;
      blocks[4].type = 0;
      blocks[5].type = 0;

      ball.position = (Vector2){BORDER_SIZE+SQUARE_SIZE*4, BORDER_SIZE};
      break;
  }
}
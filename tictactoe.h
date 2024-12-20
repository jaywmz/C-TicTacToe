/*
TO INSTALL RAYLIB:
1) OPEN MSYS2
2) Run: pacman -S mingw-w64-x86_64-raylib
3) DONE

TO RUN PROGRAM:
1) In terminal: gcc -fcommon -o tictactoe tictactoeMain.c tictactoeGUI.c tictactoeLogic.c -lraylib -lopengl32 -lgdi32 -lwinmm
2) Then: .\tictactoe
*/

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <string.h>
#include <math.h>

// Structure to represent each example in the Naive Bayes model
typedef struct {
    char features[9]; // 9 features
    char target[10];  // 10th feature is the target
} Example;

// Structure to store counts for each feature and target combination
typedef struct {
    char feature;  // Feature
    char target;   // Target
    int count;     // Count
} FeatureCount;

typedef struct {
    char feature;  // Feature
    char target;   // Target
    double probability;     // Probability 
} ProbabilityCount;

int countIndex;  // Index for the current count in the counts array
FeatureCount counts[9][6];  // Array to store the counts of features
int numTraining;  // Number of training examples
extern int label[2];
ProbabilityCount probability[9][6];

// Structure definition for player settings
typedef struct {
    int aiType;         // 1 for perfect AI, 2 for imperfect AI (default to perfect)
    int turn;           // 1 for X, 2 for O (default to X)
    int gamemodeChoice; // 1 for 1P, 2 for 2P (default to 1P)
    int numOfTurns;     // Number of turns taken
    int whoWon;         // -1 = X win, 0 = Draw, 1 = O win
    int loopState;      // 0 for main menu, 1 for game screen
} PlayerSettings;
PlayerSettings player;     

// Structure to manage game status flags
typedef struct {
    int showRestartMessage;     // Flag for showing restart message
    int validInputFlag;         // Flag for valid input
    int showInvalidMsg;         // Flag for showing invalid message
    int aiMadeMove;             // Flag indicating whether AI made a move
    int scoreboardUpdated;      // Flag indicating whether the scoreboard is updated
} GameStatus;
GameStatus gameStatus;

// Rectangle buttons for UI interaction
Rectangle backButton;         // Back button 
Rectangle restartButton;      // Restart button

// Structure to store game scoreboard
typedef struct {
    int player2Wins;   // O wins
    int player1Wins;   // X wins
    int draws;         // Draws
} Scoreboard;
Scoreboard scoreboard;  

// Structure to store properties of the game screen
typedef struct {
    const int screenWidth;    // Set the screen width
    const int screenHeight;   // Set the screen height
    const int squareSize;     // Set the square size for the Tic-Tac-Toe grid
    int boardX;               // Set the initial X-coordinate
    int boardY;               // Set the initial Y-coordinate
} ScreenProperties;
ScreenProperties screenProps;

// Structure to represent the game board
typedef struct {
    int winningCombos[8][3];  // Winning combinations
    int board[9];             // Initialize the board with empty spaces (0)
} GameBoard;
GameBoard gameBoard;

// Structure to store game sounds
typedef struct {
    Sound moveSound;        // Sound for when a move is made
    Sound restartSound;     // Sound for when the restart button is pressed
    Sound backgroundSound;  // Background music
} GameSounds;
GameSounds gameSounds;

// Function prototypes
int minimaxImperfect(int* board, int comp, int depth);
int minimax(int* board, int comp);
int win(const int* board);
int swapTurn(int* currentTurn);
int checkInput(int* choice);
int insertChoice(int* choice);
int makeAIMove(int* aiType);
void drawGame();
void restartGame();
void initializeGame(int* gamemodeChoice);
void gameLoop();
void handleInput();
int isBoardFull(const int* board);
int checkButton(Rectangle* buttons);
void updateScoreboard(int* winner);

// Naive Bayes
int initialiseNaiveBayes();
void train(Example* examples, int numExamples, FeatureCount counts[9][6], int label[2]);
char predict(char* features, ProbabilityCount probability[9][6], int bestmoveindex);
int readExamplesFromFile(const char* filename, Example** examples);
void splitDataset(Example* allExamples, int numExamples, Example** trainingSet, int* numTraining, Example** testingSet, int* numTesting, double splitRatio);
int naiveBayes(int board[9], FeatureCount counts[9][6], int numCounts, int numExamples);
double learn(FeatureCount counts[9][6], ProbabilityCount probability[9][6]);
void currentfeatures(char features[9], int bestmoveindex);
void testingAccuracy(Example* testingSet, int numTesting, ProbabilityCount probability[9][6], int numCounts);

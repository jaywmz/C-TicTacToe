#include "tictactoe.h"

// Initialize player settings
PlayerSettings player = {
  1, // aiType 
  1, // turn  
  0, // gamemodeChoice
  0, // numOfTurns 
  0, // whoWon 
  0  // loopState 
};

// Initialize scoreboard
Scoreboard scoreboard = {
  0, // player2Wins
  0, // player1Wins
  0  // draws
};

// Initialize game board with winning combinations and an empty board
GameBoard gameBoard = {
    .winningCombos = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},  // Horizontal
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},  // Vertical
        {0, 4, 8}, {2, 4, 6}             // Diagonal
    },
    .board = {0, 0, 0, 0, 0, 0, 0, 0, 0}  // Initialize the board with empty spaces (0)
};

// Initialize game status flags
GameStatus gameStatus = {
    .showRestartMessage = 1,  // Show restart message initially
    .validInputFlag = 0,      // Flag for valid input (initialize to 0)
    .showInvalidMsg = 0,      // Flag for showing invalid message (initialize to 0)
    .aiMadeMove = 0,          // Flag indicating whether AI made a move (initialize to 0)
    .scoreboardUpdated = 0    // Flag indicating whether the scoreboard is updated (initialize to 0)
};

// Initialize screen properties
ScreenProperties screenProps = {
    .screenWidth = 800,   // Set the screen width
    .screenHeight = 800,  // Set the screen height
    .squareSize = 150,    // Set the square size for the Tic-Tac-Toe grid
    .boardX = 0,          // Set the initial X-coordinate
    .boardY = 0           // Set the initial Y-coordinate
};

// Initialize rectangles for UI interaction
Rectangle backButton = {20, 20, 100, 45};      // Define the position and size of the back button
Rectangle restartButton = {700, 10, 75, 75 };  // Define the position and size of the restart button

ProbabilityCount probability[9][6];
FeatureCount counts[9][6];  // Array to hold feature counts
int countIndex = 0;  // Index for the counts array
int label[2] = {0,0};

char features[] = {'x', 'o', 'b'};  // Array to hold the different features (x, o, b)
char targets[] = {'p', 'n'};  // Array to hold the different targets (p, n)

int main(){
    // Initialise the Naive Bayes ML
    initialiseNaiveBayes();
    // Initialize the game
    initializeGame(&player.gamemodeChoice);

    // Unload sounds and close audio device
    UnloadSound(gameSounds.moveSound);
    UnloadSound(gameSounds.restartSound);
    UnloadSound(gameSounds.backgroundSound);
    CloseAudioDevice();

  return 0;
}

int initialiseNaiveBayes(){
    // Read examples from file
    Example* allExamples;
    int numExamples = readExamplesFromFile("tictactoe.data", &allExamples);
    if (numExamples == 0){
      printf("Failed to read file or file is empty.\n");
      return 1;
    } else {
      printf("Successfully read %d examples from the file.\n", numExamples);
    }

    // Split data into training and testing sets
    Example* trainingSet;
    Example* testingSet;
    extern int numTraining;
    int numTesting;
    double splitRatio = 0.8;  // 80% of the data for training
    splitDataset(allExamples, numExamples, &trainingSet, &numTraining, &testingSet, &numTesting, splitRatio);

    // Print the number of examples in each set
    printf("Number of training examples: %d\n", numTraining);
    printf("Number of testing examples: %d\n", numTesting);
    printf("Total number of examples: %d\n", numExamples);

    // Verify the split function
    if (numTraining + numTesting == numExamples) {
      printf("The splitExamples function works correctly.\n");
    } else {
      printf("The splitExamples function does not work correctly.\n");
    }


    // Initialize feature counts
    for (int k = 0; k < 9; k++)
    {
      for (int i = 0; i < sizeof(features) / sizeof(features[0]); i++) { 
          for (int j = 0; j < sizeof(targets) / sizeof(targets[0]); j++) { 
              counts[k][countIndex].feature = features[i];
              counts[k][countIndex].target = targets[j];
              counts[k][countIndex].count = 0;
              countIndex++;
          }
      }
      countIndex = 0;
    }

    // Initialize probability counts 
    for (int k = 0; k < 9; k++)
    {
      for (int i = 0; i < sizeof(features) / sizeof(features[0]); i++) { 
          for (int j = 0; j < sizeof(targets) / sizeof(targets[0]); j++) { 
              probability[k][countIndex].feature = features[i];
              probability[k][countIndex].target = targets[j];
              probability[k][countIndex].probability = 0;
              countIndex++;
          }
      }
      countIndex = 0;
    }

    
    // Train the model
    train(trainingSet, numTraining, counts, label);

    learn(counts,probability);

    printf("\nTraining Accuracy:");
    testingAccuracy(testingSet, numTesting, probability, countIndex);
    printf("\nTest Accuracy:");
    testingAccuracy(trainingSet, numTraining, probability, countIndex);

}

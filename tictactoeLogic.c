#include "tictactoe.h"
#include <math.h>
#include <string.h>

int bestmoveindex;
int random; 

// Function to check if a button has been clicked
int checkButton(Rectangle* buttons) {
    // Get the current mouse position
    Vector2 mousePos = GetMousePosition();

    // Loop through each button
    for (int i = 0; i < 5; i++)
    {
        // Check if the mouse position collides with the button rectangle
        if (CheckCollisionPointRec(mousePos, buttons[i])) 
            return i;  // Return the index of the button that was clicked
    }

    // If no button was clicked, return -1
    return -1;
}

// Function to check if the board is full
int isBoardFull(const int* board) {
    for (int i = 0; i < 9; i++) {
        if (board[i] == 0) {
            return 0; // There's an empty space, the board is not full
        }
    }
    return 1; // All spaces are filled
}


// Function to implement the minimax algorithm with imperfect decision making
int minimaxImperfect(int* board, int comp, int depth) {
    // Check if the game has been won
    int winner = win(board);

    // If the game has been won, return the winner
    if (winner != 0) {
        return winner * comp;
    }

    // If the maximum depth has been reached, return 0
    if (depth == 0) {
        return 0;  // Heuristic evaluation here
    }

    // Initialize the best move and the best score
    int move = -1;
    int score = -2; // Losing moves are preferred to no move

    // Loop through each position on the board
    for (int i = 0; i < 9; ++i) {
        // If the position is empty
        if (board[i] == 0) {
            // Try the move
            board[i] = comp;
            // Recursively call minimaxImperfect to evaluate the move
            int thisScore = -minimaxImperfect(board, comp * -1, depth - 1);
            // Undo the move
            board[i] = 0;

            // If the move is better than the current best move, update the best move and the best score
            if (thisScore > score) {
                score = thisScore;
                move = i;
            }
        }
    }

    // If no move has been found, return 0
    if (move == -1) {
        return 0;
    }

    // Return the score of the best move
    return score;
}

int minimax(int* board, int comp) {
    int winner = win(board);  // Check for winner
    if(winner != 0) return winner*comp;  // If game over, return winner

    int move = -1;
    int score = -2;  // Losing moves are preferred to no move
    int i;
    for(i = 0; i < 9; ++i) {  // For all moves,
        if(board[i] == 0) {  // If legal,
            board[i] = comp;  // Try the move
            int thisScore = -minimax(board, comp*-1);  // Recurse
            if(thisScore > score) {  // Pick the one that's worst for the opponent
                score = thisScore;
                move = i;
            }
            board[i] = 0;  // Reset board after try
        }
    }
    if(move == -1) return 0;  // If no move, return 0
    return score;  // Return score of best move
}

// Function to train the model
void train(Example* examples, int numExamples, FeatureCount counts[9][6], int label[2]) {
    // Loop through each example
    for (int i = 0; i < numExamples; i++) {
        if (examples[i].target[0] == 'p')
        {
            label[0] = label[0] + 1;
        }
        else if (examples[i].target[0] == 'n')
        {
            label[1] = label[1] + 1;
        }
        // Loop through each feature in the example
        for (int j = 0; j < 9; j++) {
            char feature = examples[i].features[j];
            char target = examples[i].target[0];

            // Update the count of each feature-target pair
            for (int k = 0; k < 6; k++) {
                if (counts[j][k].feature == feature && counts[j][k].target == target) {
                    counts[j][k].count++;
                    break;
                }
            }
        }
    }
}

double learn(FeatureCount counts[9][6], ProbabilityCount probability[9][6]){

    for (int k = 0; k < 9; k++)
    {
        for (int i = 0; i < 6; i++)
        {
            if (probability[k][i].target == 'p')
            {
                probability[k][i].probability = (counts[k][i].count + 1.0)/(label[0]+3.0) ;
            }
            else if (probability[k][i].target == 'n')
            {
                probability[k][i].probability = (counts[k][i].count + 1.0)/(label[1]+3.0) ;
            }
            
            printf("Grid: %d, Feature: %c, Target: %c, Probability: %lf\n", k+1, probability[k][i].feature, probability[k][i].target, probability[k][i].probability);
        }
    }

}

int readExamplesFromFile(const char* filename, Example** examples) {
    FILE* file = fopen("tic-tac-toe.data", "r");  // Open the file
    if (file == NULL) {  // Check if file opened successfully
        fprintf(stderr, "Error opening file: %s\n", filename);
        return 0;
    }

    int numExamples = 0;
    char line[100]; 
    while (fgets(line, sizeof(line), file) != NULL) {  // Count lines in the file
        numExamples++;
    }

    *examples = malloc(numExamples * sizeof(Example));  // Allocate memory for examples

    rewind(file);  // Rewind the file to the beginning

    // Read examples from the file
    for (int i = 0; i < numExamples; i++) {
        if (fscanf(file, "%c,%c,%c,%c,%c,%c,%c,%c,%c,%s\n",
                   &(*examples)[i].features[0], &(*examples)[i].features[1], &(*examples)[i].features[2],
                   &(*examples)[i].features[3], &(*examples)[i].features[4], &(*examples)[i].features[5],
                   &(*examples)[i].features[6], &(*examples)[i].features[7], &(*examples)[i].features[8],
                   (*examples)[i].target) != 10) {  // Check if reading was successful
            fprintf(stderr, "Error reading from file: %s\n", filename);
            free(*examples);  // Free allocated memory
            fclose(file);  // Close the file
            return 0;
        }
    }

    fclose(file);  // Close the file
    return numExamples;  // Return the number of examples
}

void splitDataset(Example* allExamples, int numExamples, Example** trainingSet, int* numTraining, Example** testingSet, int* numTesting, double splitRatio) {
    // Shuffle the examples randomly
    for (int i = numExamples - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        // Swap examples[i] and examples[j]
        Example temp = allExamples[i];
        allExamples[i] = allExamples[j];
        allExamples[j] = temp;
    }

    // Calculate the number of examples for training and testing
    int numTrainingExamples = (int)(numExamples * splitRatio);
    int numTestingExamples = numExamples - numTrainingExamples;

    // Allocate memory for training set
    *trainingSet = malloc(numTrainingExamples * sizeof(Example));

    // Copy examples for training set
    for (int i = 0; i < numTrainingExamples; i++) {
        (*trainingSet)[i] = allExamples[i];
    }

    // Allocate memory for testing set
    *testingSet = malloc(numTestingExamples * sizeof(Example));

    // Copy examples for testing set
    for (int i = numTrainingExamples; i < numExamples; i++) {
        (*testingSet)[i - numTrainingExamples] = allExamples[i];
    }

    // Set the number of training and testing examples
    *numTraining = numTrainingExamples;
    *numTesting = numTestingExamples;
}


// Function to check win status
int win(const int* board) {
    // Winning combinations
    unsigned wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
    int i;
    for(i = 0; i < 8; ++i) {
        // If any winning combination is found, return the winner (1 or 2)
        if(board[wins[i][0]] != 0 &&
           board[wins[i][0]] == board[wins[i][1]] &&
           board[wins[i][0]] == board[wins[i][2]])
            return board[wins[i][0]];
    }
    // If no winner, return 0
    return 0;
}

// Function to swap turns
int swapTurn(int* currentTurn){
    if(*currentTurn == 1){
        *currentTurn = 2;   // If it's X's turn, make it O's turn
    }else{
        *currentTurn = 1;   // If it's O's turn, make it X's turn
    }

    return 0;
}

// Function to check if a grid cell already has input
int checkInput(int* choice){
    if (gameBoard.board[*choice] != 0) {
        return 1;   // If cell is not empty, return 1
    } else {
        return 0;   // If cell is empty, return 0
    }
}

// Function to insert player's choice onto the game board
int insertChoice(int* choice){
    gameStatus.validInputFlag = checkInput(choice);  // Check if the chosen cell is empty

    if(gameStatus.validInputFlag == 1){
        gameStatus.showInvalidMsg = 1;  // If cell is not empty, set flag to show invalid message
    }else{
        gameStatus.showInvalidMsg = 0;  // If cell is empty, reset flag to not show invalid message
        if(player.turn == 1){
            gameBoard.board[*choice] = -1;  // If it's player 1's turn, mark the cell with -1
            swapTurn(&player.turn);  // Swap turns
            player.numOfTurns += 1;  // Increment the number of turns
        }else{
            gameBoard.board[*choice]= 1;  // If it's player 2's turn, mark the cell with 1
            swapTurn(&player.turn);  // Swap turns
            player.numOfTurns += 1;  // Increment the number of turns
        }
    }
    return 0;
}

void currentfeatures(char features[9], int numcounts)
{
    predict(features,probability,numcounts);
}

char predict(char* features, ProbabilityCount probability[9][6], int numcounts)
{
    double maxPossibilityP;
    double maxPossibilityN;
    double possibility[2] = {(double)(label[0])/(double)(label[0]+label[1]),(double)(label[1])/(double)(label[0]+label[1])};

    //printf("\nPossibility of p\n");
    for (int k = 0; k < 9; k++)
    {
        for (int i = 0; i < 6; i++)
        {
            if (features[k] == probability[k][i].feature && probability[k][i].target == 'p')
            {
                //printf("\nFeature of current state: %c, Feature in db: %c, Probability is: %lf, possibility is: %lf\n", features[k],probability[k][i].feature,probability[k][i].probability, possibility[0]);
                possibility[0] = possibility[0] * probability[k][i].probability;
            }
        }
    }

    //printf("\n\n");
    //printf("\nPossibility of n\n");

    for (int k = 0; k < 9; k++)
    {
        for (int i = 0; i < 6; i++)
        {
            if (features[k] == probability[k][i].feature && probability[k][i].target == 'n')
            {
                //printf("\nFeature of current state: %c, Feature in db: %c, Probability is: %lf, possibility is: %lf\n", features[k],probability[k][i].feature,probability[k][i].probability, possibility[1]);
                possibility[1] = possibility[1] * probability[k][i].probability;
            }
        }
    }

    //printf("\n\n");

    

    if (possibility[0] > maxPossibilityP)
    {
        maxPossibilityP = possibility[0];
        //printf("The features is: %s",features);
        //printf("The index is: %d",bestmoveindex);
    }

    if (possibility[1] > maxPossibilityN)
    {
        maxPossibilityN = possibility[1];
    }

    if (possibility[0] > possibility[1] && possibility[1] == maxPossibilityN) // Choosing best move
    {
        //printf("\nAI is able to choose from:");
        //printf("\nThe features that AI can choose: %s",features);
        bestmoveindex = numcounts;
        //printf("\nThe index that AI can choose: %d",bestmoveindex);
    }
    else if (possibility[1] > possibility[0])
    {
        random = 1;
        //printf("\nAI is making a random move");
    }
    
    //printf("\npossibility of p = %lf\n", possibility[0]);
    //printf("\npossibility of n = %lf\n", possibility[1]);

    //printf("\nThe greatest possibility for p: %lf\n",maxPossibilityP);
    //printf("\nThe greatest possibility for n: %lf\n",maxPossibilityN);

    return (possibility[0] > possibility[1]) ? 'p' : 'n';
}

int naiveBayes(int board[9], FeatureCount counts[9][6], int numCounts, int numExamples) {
    // Generate features from the current state of the board
    
    //int row, col;
    char features[9];

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (board[i * 3 + j] == 1)
            {
                features[i * 3 + j] = 'o';
            }
            else if (board[i * 3 + j] == 0)
            {
                features[i * 3 + j] = 'b';
            }
            else if (board[i * 3 + j] == -1)
            {
                features[i * 3 + j] = 'x';
            }
            //printf("%d ", board[i * 3 + j]);
        }
        printf("\n");
    }
    printf("\n\n");
    currentfeatures(features,numCounts);

    int move;
    int emptyCells[9];  // Array to store indices of empty cells
    int numEmptyCells = 0;  // Initialize the number of empty cells

    // Iterate through each cell on the game board
    for (int l = 0; l < 9; ++l) {
        if (gameBoard.board[l] == 0) {
            // If the cell is empty, add its index to the array of empty cells
            emptyCells[numEmptyCells++] = l;
        }
    }

    // Check if there are any empty cells
    if (numEmptyCells > 0) {
        // If there are empty cells, choose a random index from the array
        for (int i = 0; i < numEmptyCells; ++i) {
        if (emptyCells[i] == bestmoveindex) {
            // If bestmoveindex is in the array, set move to bestmoveindex
            move = bestmoveindex;
            break; // Exit the loop once the move is set
        }
        else if (random == 1)
        {
            move = emptyCells[rand() % numEmptyCells];
        }
    }
    }
    return move;
} 

// Function for AI to make a move
int makeAIMove(int* aiType){
    int move = -1;  // Initialize best move
    int bestScore = -2;  // Initialize best score
    for (int i = 0; i < 9; ++i) {  // For each cell on the board
        if (gameBoard.board[i] == 0) {  // If the cell is empty
            gameBoard.board[i] = 1;  // Assume AI is 'O'

            // Calculate score based on AI type
            if (*aiType == 1) {  // Perfect AI
                int score = -minimax(gameBoard.board, -1);
                if (score > bestScore) {  // If score is better than best score
                    bestScore = score;  // Update best score
                    move = i;  // Update best move
                }
            } else if (*aiType == 2) {  // Imperfect AI Medium
                int score = -minimaxImperfect(gameBoard.board, -1, 2);
                if (score > bestScore) {
                    bestScore = score;
                    move = i;
                }
            } else if (*aiType == 3) {  // Imperfect AI Easy
                int score = -minimaxImperfect(gameBoard.board, -1, 0);
                if (score > bestScore) {
                    bestScore = score;
                    move = i;
                }
            } else if (*aiType == 4) {  // Naive Bayes AI
                move = naiveBayes(gameBoard.board, counts, i, numTraining);
            } 
            
            gameBoard.board[i] = 0;  // Reset cell after game 
        }
    } 
    insertChoice(&move);  // Make the best move
}

void testingAccuracy(Example* testingSet, int numTesting, ProbabilityCount probability[9][6], int numCounts) {
    int correct = 0;

    Example* p_testingSet = testingSet;
    int confusionMatrix[2][2] = {{0, 0}, {0, 0}};

    char* predictions = malloc(numTesting * sizeof(int));
    char* actual = malloc(numTesting * sizeof(int));    

    for (int i = 0; i < numTesting; i++) {
        char predicted = predict(p_testingSet[i].features, probability, 0);
        if (predicted == p_testingSet[i].target[0]) {
            correct++;
        }
        predictions[i] = predicted;
        actual[i] = p_testingSet[i].target[0];

    }

    for (int i = 0; i < numTesting; i++) {
    if (actual[i] == 'p' && predictions[i] == 'p') {
        confusionMatrix[0][0]++; // TP
    } else if (actual[i] == 'p' && predictions[i] == 'n') {
        confusionMatrix[0][1]++; // FN
    } else if (actual[i] == 'n' && predictions[i] == 'p') {
        confusionMatrix[1][0]++; // FP
    } else if (actual[i] == 'n' && predictions[i] == 'n') {
        confusionMatrix[1][1]++; // TN
    }

    }
    
    printf("\nAccuracy: %f", (double)correct / numTesting);
    printf("\nConfusion Matrix:\n");
    printf("\t\tPredicted: YES\tPredicted: NO\n");
    printf("Actual: YES\t%d\t\t%d\n", confusionMatrix[0][0], confusionMatrix[0][1]);
    printf("Actual: NO \t%d\t\t%d\n", confusionMatrix[1][0], confusionMatrix[1][1]);

    free(predictions);
    free(actual);

}
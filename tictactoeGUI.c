#include "tictactoe.h"

// This function initializes the game
void initializeGame(int* gamemodeChoice) {

    InitAudioDevice(); // Initialize the audio device

    // Load the sounds
    gameSounds.moveSound = LoadSound("mixkit-retro-game-notification-212.wav"); // Sound for when a move is made
    gameSounds.restartSound = LoadSound("mixkit-arcade-bonus-alert-767.wav"); // Sound for when the game is restarted
    gameSounds.backgroundSound = LoadSound("pop.mp3"); // Background music

    PlaySound(gameSounds.backgroundSound); // Start playing the background music
    
    // Initialization code (window, AI type, etc.)
    player.whoWon = 0; // -1 = X win, 0 = Draw, 1 = O win

    // Start the game loop, passing the game mode choice
    gameLoop(*gamemodeChoice); 

    CloseWindow(); // Close the window when the game is over
}

// Main game loop
void gameLoop() {

    // Initialize window and textures
    InitWindow(screenProps.screenWidth, screenProps.screenHeight, "TIC TAC TOE");
    SetTargetFPS(60);
    RenderTexture2D screen = LoadRenderTexture(screenProps.screenWidth, screenProps.screenHeight);
    RenderTexture2D X = LoadRenderTexture(screenProps.squareSize, screenProps.squareSize);
    RenderTexture2D O = LoadRenderTexture(screenProps.squareSize, screenProps.squareSize);

    // Initialize volume for background sound
    float volume = 1.0f;
    SetSoundVolume(gameSounds.backgroundSound, volume);

    // Main game loop
    while (!WindowShouldClose()) {
        handleInput(); // Handle player's input
        int pressedButton;

        // Define buttons
        Rectangle buttons[6] = {
		(Rectangle){250, 260, 300, 100}, 
		(Rectangle){250, 380, 300, 100},
		(Rectangle){250, 500, 300, 100},
        (Rectangle){250, 610, 300, 100},
        backButton,
        restartButton
		};

        // Check if the M key is pressed to mute/unmute the background music
        if (IsKeyPressed(KEY_M)) { // If M key is pressed
            if (volume != 0.0f) {
                volume = 0.0f; // Mute
            } else {
                volume = 1.0f; // Unmute
            }
            SetSoundVolume(gameSounds.backgroundSound, volume);
        }

        // Check if the background music is playing, if not, play it
        if (!IsSoundPlaying(gameSounds.backgroundSound)) {
            PlaySound(gameSounds.backgroundSound);
            }

        // Game states
        switch (player.loopState)
        {
            case 0:
                // Home screen
                pressedButton = checkButton(buttons);
                if ((pressedButton == 0 || pressedButton == 1) && IsMouseButtonPressed(0)){
                    // Check if Multiplayer = 0 or Single player = 1
                    player.loopState = (pressedButton == 0) ? 3 : 1;
                    player.gamemodeChoice = (pressedButton == 0) ? 2 : 1;
                } else if (pressedButton == 2 && IsMouseButtonPressed(0)) {
                    player.gamemodeChoice = 1;
                    player.aiType = 4; // Naive Bayes AI
                    player.loopState = 3; // Move to the game state
                } else if (pressedButton == 3 && IsMouseButtonPressed(0)) {
                    // Exit the game
                    CloseWindow();
                    break;
                }

                BeginDrawing();
                BeginTextureMode(screen);
                ClearBackground(GRAY);
                DrawText("Tic-Tac-Toe", 100, 60, 90, BLACK);
                
                // Draw buttons
                for (int i = 0; i < 4; i++)
                {
                    DrawRectangleRounded(buttons[i], 0.2, 5, (pressedButton == i) ? BLACK:DARKGRAY);
                    DrawRectangleRounded((Rectangle){buttons[i].x+10, buttons[i].y+10, 280, 80}, 0.2, 5, WHITE);
                }
                
                // Draw button labels
                DrawText("Two Player", 275, 288, 45, (pressedButton == 0) ? BLACK:DARKGRAY);
                DrawText("Vs AI", 330, 408, 45, (pressedButton == 1) ? BLACK:DARKGRAY);
                DrawText("Vs AI (ML)", 290, 528, 45, (pressedButton == 2) ? BLACK:DARKGRAY);
                DrawText("Exit", 360, 648, 45, (pressedButton == 3) ? BLACK:DARKGRAY);
                
                EndTextureMode();
                DrawTextureRec(screen.texture, (Rectangle){0, 0, screenProps.screenWidth, -screenProps.screenHeight}, (Vector2){0, 0}, WHITE);
                EndDrawing();
                break;
            case 1:
                // AI type selection menu
                pressedButton = checkButton(buttons);

                // Check which AI difficulty was selected
                if ((pressedButton == 0 || pressedButton == 1 || pressedButton == 2) && IsMouseButtonPressed(0)) {
                    // Assign AI type based on button pressed (0 = Easy, 1 = Medium, 2 = Hard)
                    player.aiType = (pressedButton == 0) ? 3 : (pressedButton == 1) ? 2 : 1;
                    player.loopState = 3; // Move to the game state
                } else if (pressedButton == 3 && IsMouseButtonPressed(0)) {
                    // Go back to the home screen
                    player.loopState = 0;
                }

                BeginDrawing();
                BeginTextureMode(screen);
                ClearBackground(GRAY);
                DrawText("Select Difficulty", 85, 100, 75, BLACK);

                // Draw the buttons
                for (int i = 0; i < 4; i++)
                {
                    DrawRectangleRounded(buttons[i], 0.2, 5, (pressedButton == i) ? BLACK:DARKGRAY);
                    DrawRectangleRounded((Rectangle){buttons[i].x+10, buttons[i].y+10, 280, 80}, 0.2, 5, WHITE);
                }

                // Draw button labels
                DrawText("Easy", 345, 288, 45, (pressedButton == 0) ? BLACK:DARKGRAY);
                DrawText("Medium", 330, 408, 45, (pressedButton == 1) ? BLACK:DARKGRAY);
                DrawText("Hard", 350, 528, 45, (pressedButton == 2) ? BLACK:DARKGRAY);
                DrawText("Home", 350, 638, 45, (pressedButton == 3) ? BLACK:DARKGRAY);

                EndTextureMode();
                DrawTextureRec(screen.texture, (Rectangle){0, 0, screenProps.screenWidth, -screenProps.screenHeight}, (Vector2){0, 0}, WHITE);
                EndDrawing();
                break;

            case 3:
                // Game state
                static int wasInHomeMenu = 0;

                // If transitioning from the home menu, reset the game state
                if (wasInHomeMenu == 1) {
                    restartGame(); // Reset the game
                    wasInHomeMenu = 0;
                    gameStatus.scoreboardUpdated = false;
                    scoreboard.player1Wins = 0;  // Reset player 1 wins
                    scoreboard.player2Wins = 0;  // Reset player 2 wins
                    scoreboard.draws = 0;        // Reset draws
                }

                drawGame(); // Draw the game board

                // Check if the back button was pressed
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    Vector2 mousePosition = GetMousePosition();
                    if (CheckCollisionPointTriangle(mousePosition,
                        (Vector2){25, 55}, (Vector2){140, 55}, (Vector2){82.5, 15})) {
                        player.loopState = 0; // Go back to the home screen
                        wasInHomeMenu = 1;
                    }
                }

                // Update the scoreboard if the game has ended
                if (!gameStatus.scoreboardUpdated && (player.whoWon != 0 || isBoardFull(gameBoard.board))) {
                    updateScoreboard(&player.whoWon);
                    gameStatus.scoreboardUpdated = true;
                }
                break;
        }
    }
}

void restartGame() {
    // Reset game board to empty state
    for (int i = 0; i < 9; i++) {
        gameBoard.board[i] = 0;
    }
    
    // Reset game state variables
    player.turn = 1; // Set turn to player 1
    player.numOfTurns = 0; // Reset turn count
    player.whoWon = 0; // Reset winner status
    gameStatus.showInvalidMsg = 0; // Reset invalid move message
    gameStatus.aiMadeMove = 0; // Reset AI move status
    
    gameStatus.scoreboardUpdated = false; // Reset scoreboard update status
    // Reset AI-related variables if applicable
}

void handleInput() {
    // Check if left mouse button is pressed, game is not over, and game state is in play mode
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && player.whoWon == 0 && player.loopState == 3) {
        // Get mouse position
        Vector2 mousePosition = GetMousePosition();
        int x = (int)mousePosition.x;
        int y = (int)mousePosition.y;

        // Check if click is within the game board
        if (x >= screenProps.boardX && x <= (screenProps.boardX + 3 * screenProps.squareSize) &&
            y >= screenProps.boardY && y <= (screenProps.boardY + 3 * screenProps.squareSize)) {
            // Calculate the grid cell clicked based on mouse position
            int col = (x - screenProps.boardX) / screenProps.squareSize;
            int row = (y - screenProps.boardY) / screenProps.squareSize;

            // Convert grid cell to game choice (1-9)
            int choice = row * 3 + col + 1;

            // Check if choice is valid and empty, then make the move
            if (choice >= 1 && choice <= 9 && gameBoard.board[choice - 1] == 0) {
                choice -= 1;
                insertChoice(&choice);
                PlaySound(gameSounds.moveSound); // Play move sound
            } else {
                gameStatus.showInvalidMsg = 1;  // Show invalid move message
            }
        }
    }
}

// Function to update scoreboard based on the winner of the game
void updateScoreboard(int* winner) {
    // Increment player 2 wins if O won
    if (*winner == 1) {
        scoreboard.player2Wins++;
    } 
    // Increment player 1 wins if X won
    else if (*winner == -1) {
        scoreboard.player1Wins++;
    } 
    // Increment draws if it's a draw
    else if (*winner == 2) {
        scoreboard.draws++;
    }
    // Mark scoreboard as updated
    gameStatus.scoreboardUpdated = true;
}




// Function to draw the game GUI
void drawGame() {
    BeginDrawing();
    ClearBackground(GRAY);
    // Calculate the board position
    screenProps.boardX = (screenProps.screenWidth - 3 * screenProps.squareSize) / 2;
    screenProps.boardY = (screenProps.screenHeight - 3 * screenProps.squareSize) / 2;
    const char* resultMessage = "";
    
    int winningCombo[3] = { -1, -1, -1 };
    player.whoWon = win(gameBoard.board);

    // Draw the back button
    DrawTriangle((Vector2){25, 55}, (Vector2){140, 55}, (Vector2){82.5, 15}, WHITE);
    DrawText("BACK", 55, 35, 20, BLACK);

    // Draw the restart button
    DrawRectangleRounded(restartButton, 0.2, 5, WHITE);
    DrawText("RESTART", screenProps.screenWidth - 98, 35, 15, BLACK);

    // Handle the back button press
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePosition = GetMousePosition();
        if (CheckCollisionPointTriangle(mousePosition,
            (Vector2){25, 55}, (Vector2){140, 55}, (Vector2){82.5, 15})) {
            player.loopState = 0; // Go back to the home screen
            restartGame(); // Reset the game state
        }
    }

    // Handle the restart button press
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePosition = GetMousePosition();
        if (CheckCollisionPointTriangle(mousePosition,
            (Vector2){25, 55}, (Vector2){140, 55}, (Vector2){82.5, 15})) {
            player.loopState = 0; // Go back to the home screen
            restartGame(); // Reset the game state
        } else if (CheckCollisionPointRec(mousePosition, restartButton)) {
            restartGame(); // Restart the game
            PlaySound(gameSounds.restartSound); // Play restart sound
        }
    }
    
    // Draw the scoreboard
    DrawText(TextFormat("Player 1 Wins: %d", scoreboard.player1Wins), 50, 650, 20, WHITE);
    DrawText(TextFormat("Player 2 Wins: %d", scoreboard.player2Wins), 300, 650, 20, WHITE);
    DrawText(TextFormat("Draws: %d", scoreboard.draws), 600, 650, 20, WHITE);



    if (player.whoWon != 0) {
        // Identify the winning combination
        for (int i = 0; i < 8; i++) {
            if (gameBoard.board[gameBoard.winningCombos[i][0]] == player.whoWon &&
                gameBoard.board[gameBoard.winningCombos[i][1]] == player.whoWon &&
                gameBoard.board[gameBoard.winningCombos[i][2]] == player.whoWon) {
                // Store the winning combination
                winningCombo[0] = gameBoard.winningCombos[i][0];
                winningCombo[1] = gameBoard.winningCombos[i][1];
                winningCombo[2] = gameBoard.winningCombos[i][2];
                break;
            }
        }
    }
 
    // Draw grid lines
    DrawLineEx((Vector2){screenProps.boardX + screenProps.squareSize, screenProps.boardY}, (Vector2){screenProps.boardX + screenProps.squareSize, screenProps.boardY + 3 * screenProps.squareSize}, 10, WHITE);
    DrawLineEx((Vector2){screenProps.boardX + 2 * screenProps.squareSize, screenProps.boardY}, (Vector2){screenProps.boardX + 2 * screenProps.squareSize, screenProps.boardY + 3 * screenProps.squareSize}, 10, WHITE);
    DrawLineEx((Vector2){screenProps.boardX, screenProps.boardY + screenProps.squareSize}, (Vector2){screenProps.boardX + 3 * screenProps.squareSize, screenProps.boardY + screenProps.squareSize}, 10, WHITE);
    DrawLineEx((Vector2){screenProps.boardX, screenProps.boardY + 2 * screenProps.squareSize}, (Vector2){screenProps.boardX + 3 * screenProps.squareSize, screenProps.boardY + 2 * screenProps.squareSize}, 10, WHITE);

    // Adjust the positions for drawing X and O symbols within the cells
    for (int i = 0; i < 9; i++) {
        int row = i / 3;
        int col = i % 3;
        int cellX = screenProps.boardX + col * screenProps.squareSize;
        int cellY = screenProps.boardY + row * screenProps.squareSize;
        if (gameBoard.board[i] == 1) {
            // Draw O in the cell
            DrawCircle(cellX + screenProps.squareSize / 2, cellY + screenProps.squareSize / 2, screenProps.squareSize / 2 - 20, WHITE);
        } else if (gameBoard.board[i] == -1) {
            // Draw X in the cell
            DrawLineEx((Vector2){cellX + 20, cellY + 20}, (Vector2){cellX + screenProps.squareSize - 20, cellY + screenProps.squareSize - 20}, 10, WHITE);
            DrawLineEx((Vector2){cellX + screenProps.squareSize - 20, cellY + 20}, (Vector2){cellX + 20, cellY + screenProps.squareSize - 20}, 10, WHITE);
        }
    }
    

    // Determine game result and set appropriate message
    player.whoWon = win(gameBoard.board);
    if (player.whoWon == -1) {
        resultMessage = "PLAYER 1 (X) WINS, PRESS R TO RESTART!";
        gameStatus.showRestartMessage = 1;
    } else if (player.whoWon == 1 && player.gamemodeChoice == 2) {
        resultMessage = "PLAYER 2 (O) WINS, PRESS R TO RESTART!";
        gameStatus.showRestartMessage = 1;
    } else if (player.whoWon == 1 && player.gamemodeChoice == 1) {
        // Different messages for different AI types
        if (player.aiType == 1) {
            resultMessage = "PERFECT MINIMAX AI WINS, PRESS R TO RESTART!";
        } else if (player.aiType == 2) {
            resultMessage = "IMPERFECT MINIMAX AI WINS, PRESS R TO RESTART!";
        } else if (player.aiType == 3) {
            resultMessage = "EASY MODE AI WINS, PRESS R TO RESTART!";
        } else if (player.aiType == 4) {
            resultMessage = "MACHINE LEARNING AI WINS, PRESS R TO RESTART!";
        }
        gameStatus.showRestartMessage = 1;
    } else if (isBoardFull(gameBoard.board)) {
        // Message for draw
        resultMessage = "DRAW, PRESS R TO RESTART!!!";
        gameStatus.showRestartMessage = 1;
        player.whoWon = 2; // Signify Draw
    }

    // Show invalid move message if needed
    if (gameStatus.showInvalidMsg == 1) {
        resultMessage = "Please select an empty box!";
    }
    // Calculate the width and center position of the result message
    int messageWidth = MeasureText(resultMessage, 25); 
    int centerX = (screenProps.screenWidth - messageWidth) / 2;
    int bottomY = screenProps.screenHeight - 50; // Adjust this value as needed

    // Draw the result message centered at the bottom
    DrawText(resultMessage, centerX, bottomY, 25, WHITE);

    

    // Display player's turn or AI thinking message
    if (player.turn == 1) {
        DrawText("Player 1 (X)'s turn...", 250, 50, 30, WHITE);
    } else if (player.turn == 2 && player.gamemodeChoice == 2) {
        DrawText("Player 2 (O)'s turn...", 250, 50, 30, WHITE);
    } else {
        DrawText("Computer is thinking...", 250, 50, 30, WHITE);
        // AI's turn (if no winner yet, then AI makes a move)
        if (player.whoWon == 0) {
            gameStatus.aiMadeMove = makeAIMove(&player.aiType);
            

        }
    }

    // Check for user input to restart or exit the game only when the game has ended
    if (gameStatus.showRestartMessage == 1) {
        if (IsKeyPressed(KEY_R)) {
            restartGame(); // Restart the game
            PlaySound(gameSounds.restartSound); // Play restart sound
        } else if (IsKeyPressed(KEY_ESCAPE)) {
            CloseWindow(); // Close the game window
        }
    }

    int lineThickness = 5; // Set the thickness of the strike-through line

    // Draw the strike-through effect if there is a win
    if (player.whoWon != 0) {
        int cellSize = screenProps.squareSize - 20; // Adjust the size of the strike-through line
        if (winningCombo[0] != -1 && winningCombo[1] != -1 && winningCombo[2] != -1) {
            // Calculate the start and end points of the strike-through line
            int startX = screenProps.boardX + (winningCombo[0] % 3) * screenProps.squareSize + screenProps.squareSize / 2;
            int startY = screenProps.boardY + (winningCombo[0] / 3) * screenProps.squareSize + screenProps.squareSize / 2;
            int endX = screenProps.boardX + (winningCombo[2] % 3) * screenProps.squareSize + screenProps.squareSize / 2;
            int endY = screenProps.boardY + (winningCombo[2] / 3) * screenProps.squareSize + screenProps.squareSize / 2;
            Color lineColor = BLACK; // Set the color for the line
            // Draw the strike-through line
            DrawLineEx((Vector2){startX, startY}, (Vector2){endX, endY}, lineThickness, lineColor);
        }
    }

    EndDrawing(); // End the drawing
}
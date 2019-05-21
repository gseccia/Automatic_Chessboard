Feature: Chessboard logic is able to manage errors in chessmen movements
As the Chessboard routine logic 
In order to mantain a coherence between chessmen movements and legal states of the game
I want to forbid illegal movements
And stop if a coherence state could not be restored

  Scenario: Player makes a single illegal movement without captures
    Given that it is the player turn
    And player ends his turn
    And the player's move is illegal
    And He has not captured any piece
    When chessgame logic detects the error
    Then the Chessboard restores the previous state
    And gives back the turn to the player
    
  Scenario: Player makes a single illegal movement with captures
    Given that it is the player turn
    And player ends his turn
    And the player's move is illegal
    And He has captured a piece
    When chessgame logic detects the error
    Then Chessboard asks for the manual restoring by the player
    And wait a certain period of time before asserting a not restorable scenario
  
  Scenario: Multiple movements or missing chessmen
    Given that player ends his turn
    And player moves more than one piece
    And the player's move is illegal
    Or there are missing pieces
    When chessgame logic detects the error
    Then Chessboard asks for the manual restoring by the player
    And wait a certain period of time before asserting a not restorable scenario
  
  Scenario: At the end of Chessboard turn status is not coherent
    Given that it is the Chessboard turn
    And Chessboard has made its move
    When chessgame logic detects that state is not the expected one
    Then Chessboard asks for the manual restoring by the player
    And wait a certain period of time before asserting a not restorable scenario
    
  Scenario: Coherent situation is not restorable
    Given that a waiting timer expires
    Then game is aborted and Chessboard wins
   
    

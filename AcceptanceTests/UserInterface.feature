Feature: Chessboard has user interface
 As a user
 In order to interact with the Chessboard
 I need to use a hardware interface formed by an LCD display and an analog joystick
  
  Scenario: Choice of the color
    Given that player starts a game
    And Chessboard is ready to start 
    And shows on the display the possible color choice
    When player selects the choice with the joystick
    Then the color is assigned to player
    
  Scenario: The pawn reach the end of the chessboard
    Given that is the player turn 
    And he put a pawn on the opposite side of the board
    And he ends his turn
    When Chessboard logic detects this situation
    Then shows on the display the possible choices for the pawn change
    And the player can set with the joystick his choice
    
  Scenario: Player is informed of an errore situation
    Given that the Chessboard logic detects an error situation
    Then user is informed of the error through the display
    And shows a timer 
    And suggests what to do to restore the previous state
    
  Scenario: The user ends his turn
    Given that the user has made his move
    When he wants to end the turn
    Then push the joystick's button
    
  Scnario: The Chessboard ends its turn
    Given that the Chessboard ends its move
    When it wants to end the turn
    Then shows on the display that it is the player turn

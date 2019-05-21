Feature: Moving chessmen
As a player
In order to play against the Chessboard
I want that the opponent chessmen move autonomously

  Scenario: Chessboard starts its turn
    Given that player ends his turn
    And Chessboard logic is activated
    When Chessboard selects its move
    Then puts the chessman in the proper position

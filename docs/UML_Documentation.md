# SNOW MAN GAME - UML Diagram Documentation

## Project Overview

**SNOW MAN GAME** is a typing-based arcade game developed in C++ using the ncurses library. Players must type falling words correctly to build snowmen before time runs out. The game features three difficulty levels, item systems, and score tracking.

---

## 1. Class Diagram

### 1.1 Overview

The class diagram illustrates the static structure of the SNOW MAN GAME system, showing all classes, their attributes, methods, and relationships.

### 1.2 Abstract Classes

#### Screen (Abstract)
- **Purpose**: Defines the interface for all screen types in the game
- **Key Methods**:
  - `UpdateScreen()`: Refreshes the display content
  - `resizeScreen()`: Handles terminal resize events
  - `shapeScreen()`: Configures screen layout

#### FallingObject (Abstract)
- **Purpose**: Base class for all objects that fall from the top of the screen
- **Key Attributes**:
  - `x, y`: Current position coordinates
  - `fallSpeed`: Speed of descent
  - `isActive`: Whether the object is currently visible
  - `hasReachedBottom`: Flag for penalty detection
- **Key Methods**:
  - `fall()`: Moves the object downward
  - `draw()`: Pure virtual method for rendering (implemented by subclasses)

### 1.3 Screen Classes

#### InitialScreen
- **Purpose**: Displays the title screen and level selection menu
- **Responsibilities**:
  - Show game title with ASCII art
  - Allow player to select difficulty level (1-3)
  - Handle navigation with UP/DOWN keys
  - Start game when 'P' is pressed

#### PlayScreen
- **Purpose**: Main game screen where gameplay occurs
- **Responsibilities**:
  - Render game area with falling word blocks
  - Display score panel, timer, and snowman collection
  - Process user keyboard input
  - Coordinate GameManager and SentenceManager
- **Key Private Methods**:
  - `drawFrame()`: Renders the game border and sections
  - `drawBigSnowman()`: Displays completion animation
  - `drawInfoPanel()`: Shows time, score, and status

### 1.4 Game Logic Classes

#### GameManager
- **Purpose**: Central controller for game state and rules
- **Key Responsibilities**:
  - Track and update remaining time
  - Manage score calculation and multipliers
  - Control word block generation timing
  - Apply item effects and penalties
  - Detect game end conditions
- **Important Attributes**:
  - `timeLimit`: Level-dependent (180/150/120 seconds)
  - `scoreMultiplier`: Modified by SCORE_BOOST item
  - `collectedSnowmen`: Count of completed snowmen

#### SentenceManager
- **Purpose**: Manages words, input validation, and falling objects
- **Key Responsibilities**:
  - Load sentences from Dictionary
  - Create and manage WordBlock objects
  - Spawn and manage ItemBox objects
  - Validate user input against target words
- **Composition**: Contains InputHandler and Dictionary

#### Dictionary
- **Purpose**: Stores and provides sentences for each difficulty level
- **Data Structure**: `map<int, vector<string>>` - maps level to sentence list
- **Key Methods**:
  - `getRandomSentenceWords()`: Returns 8 words from a random sentence
  - `splitSentenceIntoWords()`: Parses sentence into word array

#### InputHandler
- **Purpose**: Manages the 8 input fields for word entry
- **Key Features**:
  - Tracks current active input field (0-7)
  - Handles keyboard input (characters, backspace, enter)
  - Validates completed inputs

### 1.5 Game Object Classes

#### WordBlock (extends FallingObject)
- **Purpose**: Represents a falling word that the player must type
- **Key Attributes**:
  - `text`: The word to display
  - `orderIndex`: Position in the sentence (0-7)
  - `isInInput`: Whether word has been correctly typed
- **Behavior**: Falls until typed correctly or reaches bottom

#### ItemBox (extends FallingObject)
- **Purpose**: Special falling object that grants random effects
- **Item Types** (enum ItemType):
  - `TIME_BONUS`: Adds 10 seconds
  - `TIME_MINUS`: Subtracts 10 seconds
  - `SCORE_BOOST`: Doubles score multiplier
- **Usage**: Player types "random" to activate

### 1.6 Relationships

| Relationship | Type | Description |
|--------------|------|-------------|
| Screen → InitialScreen, PlayScreen | Inheritance | Screen classes implement abstract interface |
| FallingObject → WordBlock, ItemBox | Inheritance | Game objects inherit falling behavior |
| PlayScreen ◆→ GameManager | Composition | PlayScreen owns and manages GameManager |
| PlayScreen ◆→ SentenceManager | Composition | PlayScreen owns and manages SentenceManager |
| SentenceManager ◆→ InputHandler | Composition | SentenceManager contains InputHandler |
| SentenceManager ◆→ Dictionary | Composition | SentenceManager contains Dictionary |
| SentenceManager ◇→ WordBlock | Aggregation | Manages collection of WordBlocks |
| SentenceManager ◇→ ItemBox | Aggregation | Manages collection of ItemBoxes |
| InitialScreen ⋯→ PlayScreen | Dependency | Creates PlayScreen on game start |

---

## 2. Use-Case Diagram

### 2.1 Overview

The use-case diagram shows the functional requirements of the system from the player's perspective, illustrating what actions are available and how they relate.

### 2.2 Primary Actor

**Player**: The human user who interacts with the game through keyboard input.

### 2.3 Main Use Cases

| Use Case | Description |
|----------|-------------|
| UC1: Start Game | Launch the game application |
| UC2: Select Level | Choose difficulty (Easy/Medium/Hard) |
| UC3: Play Game | Engage in the main gameplay loop |
| UC4: End Game | Exit the game (ESC or time up) |

### 2.4 Gameplay Use Cases

| Use Case | Description |
|----------|-------------|
| UC5: Type Word | Enter characters to match falling words |
| UC6: Navigate Input Field | Move between 8 input fields (TAB/arrows) |
| UC7: Use Item | Type "random" to activate item box |
| UC8: Earn Score | Gain points for completing snowmen |
| UC9: Complete Snowman | Successfully type all 8 words |

### 2.5 System Use Cases

| Use Case | Description |
|----------|-------------|
| UC15: Create WordBlock | System generates falling word objects |
| UC16: WordBlock Fall | Words descend over time |
| UC17: Manage Time | Countdown timer and penalty tracking |
| UC18: Calculate Score | Compute final score with bonuses |

### 2.6 Relationships

#### Include Relationships
- **UC1 includes UC2**: Starting the game requires level selection
- **UC3 includes UC5, UC15, UC16, UC17**: Gameplay involves typing, word generation, falling mechanics, and time management

#### Extend Relationships
- **UC7 extends to UC10, UC11, UC12**: Item usage can trigger time bonus, time penalty, or score boost
- **UC16 extends to UC13**: Words reaching bottom triggers fall penalty (-10 sec)
- **UC9 extends to UC14, UC8**: Completing snowman leads to next sentence and score gain

---

## 3. Activity Diagrams

### 3.1 Main Game Flow (activity_diagram_main.puml)

#### Purpose
Shows the complete flow of the game from launch to termination, including initialization, game loop, and cleanup.

#### Key Phases

**1. Initialization Phase**
- Game starts at main.cpp entry point
- InitialScreen created and displayed
- Player selects level using UP/DOWN keys
- Press 'P' to start or 'Q' to quit

**2. PlayScreen Setup**
- ncurses library initialized with colors
- GameManager created with level-specific time limit
- SentenceManager loads random sentence from Dictionary
- Word order randomized using Fisher-Yates shuffle

**3. Game Loop (repeat-while)**
The main loop consists of two partitions:

*Screen Update:*
- Parallel execution of time update, item spawning, and completion check
- Word blocks and item boxes advance positions
- Penalty applied if words reach bottom
- All visual elements rendered

*Input Processing:*
- Non-blocking input with 50ms timeout
- Key handling: ESC (quit), TAB/DOWN (next field), UP (previous field)
- Character input forwarded to InputHandler
- "random" input triggers item activation

**4. Termination Phase**
- Game over screen displayed
- Final score shown
- Resources cleaned up
- Return to InitialScreen

### 3.2 Word Input Processing (activity_diagram_wordinput.puml)

#### Purpose
Details the logic for handling each keystroke and validating word matches.

#### Key Decision Points

1. **ESC Key**: Immediately ends game
2. **TAB/DOWN Key**: Cycles to next input field (wraps from 7 to 0)
3. **UP Key**: Cycles to previous input field (wraps from 0 to 7)
4. **BACKSPACE**: Removes last character if input not empty
5. **ENTER**: Submits current input for validation

#### Special Input: "random"
When player types "random":
- System searches for active ItemBox with lowest Y position
- If found, applies random effect (TIME_BONUS, TIME_MINUS, or SCORE_BOOST)
- Input field cleared after use

#### Word Matching Process
- Compare lowercased input with lowercased target word
- If match found:
  - Corresponding WordBlock deactivated
  - correctMatches counter incremented
- If all 8 words matched:
  - Snowman completion animation triggered
  - collectedSnowmen increased
  - After 2 seconds, next sentence loaded

### 3.3 WordBlock Generation & Fall (activity_diagram_wordblock.puml)

#### Purpose
Illustrates the lifecycle of WordBlock objects from creation to removal.

#### Swimlanes
- **GameManager**: Controls timing and game state
- **SentenceManager**: Creates and manages WordBlocks
- **WordBlock**: Individual word object behavior

#### Word Generation Logic
- Words created every 3 seconds
- Order determined by pre-shuffled wordOrder array
- Generation pauses when waitingForCompletion is true
- Each word placed at random X position, Y=3 (top)

#### Fall Mechanics
- Every 1 second, all active WordBlocks move down
- Y position increases by fallSpeed
- If Y reaches (maxHeight - 3):
  - hasReachedBottom flag set
  - timePenalty triggered (-10 seconds)

#### Completion Handling
- When correctMatches reaches 8:
  - waitingForCompletion set to true
  - After animation, prepareNextRound() resets state

### 3.4 ItemBox System (activity_diagram_itembox.puml)

#### Purpose
Describes the item box spawning, falling, and activation mechanics.

#### Swimlanes
- **SentenceManager**: Manages ItemBox collection
- **ItemBox**: Individual item object
- **GameManager**: Applies item effects

#### Spawn Logic
- One ItemBox spawned every 30 seconds
- Random ItemType assigned (0, 1, or 2)
- Appears at random X position, Y=3

#### Fall Behavior
- Uses fallAccumulator for sub-integer movement
- fallSpeed = 0.8 (slower than WordBlocks)
- **No penalty** when reaching bottom (simply disappears)

#### Activation Process
1. Player types "random" in any input field
2. System finds active ItemBox closest to bottom
3. applyRandomEffect() returns ItemType
4. GameManager applies corresponding effect:
   - TIME_BONUS: timeAdjustment += 10
   - TIME_MINUS: timeAdjustment -= 10
   - SCORE_BOOST: scoreMultiplier = 2
5. Effect message displayed for 3 seconds

---

## 4. Design Patterns Used

### 4.1 Template Method Pattern
- **Location**: Screen abstract class
- **Implementation**: Defines skeleton of screen behavior (UpdateScreen, resizeScreen, shapeScreen) while allowing subclasses to provide specific implementations

### 4.2 Strategy Pattern (Partial)
- **Location**: FallingObject and its subclasses
- **Implementation**: WordBlock and ItemBox override fall() method with different behaviors (WordBlock triggers penalty, ItemBox does not)

### 4.3 Composition Pattern
- **Location**: PlayScreen, SentenceManager
- **Implementation**: Complex objects built from simpler components (PlayScreen contains GameManager and SentenceManager)

### 4.4 Observer-like Behavior
- **Location**: GameManager ↔ SentenceManager interaction
- **Implementation**: GameManager queries SentenceManager state (correctMatches) to determine game flow

---

## 5. File Structure

```
docs/
├── UML_Documentation.md          # This documentation file
└── plantuml/
    ├── class_diagram.puml        # Class structure and relationships
    ├── usecase_diagram.puml      # Functional requirements
    ├── activity_diagram_main.puml       # Main game flow
    ├── activity_diagram_wordinput.puml  # Input processing logic
    ├── activity_diagram_wordblock.puml  # Word lifecycle
    └── activity_diagram_itembox.puml    # Item system
```

---

## 6. Summary

The SNOW MAN GAME architecture follows object-oriented design principles:

- **Encapsulation**: Each class has clear responsibilities
- **Inheritance**: Common behavior extracted to abstract classes
- **Polymorphism**: FallingObject subclasses implement different fall behaviors
- **Composition**: Complex objects built from simpler components

The system is designed to be maintainable and extensible, allowing easy addition of new item types, difficulty levels, or visual themes.

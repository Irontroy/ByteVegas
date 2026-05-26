#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

struct BlackjackCard {
    int suit;  // 0=Karo, 1=Kreuz, 2=Herz, 3=Pik
    int value; // 0=Ass, 1=2, ..., 9=10, 10=Bube, 11=Dame, 12=König

    int getScoreValue() const {
        if (value == 0) return 11;
        if (value >= 10) return 10;
        return value + 1;
    }
};

enum class GameState { MENU, BLACKJACK, ROULETTE, SLOTS };

// Ergebnis einer abgeschlossenen Blackjack-Runde
enum class BjResult { NONE, PLAYER_WIN, DEALER_WIN, PUSH, PLAYER_BUST, NATURAL_BJ };

class Game
{
public:
    Game();
    void run();

private:
    void initWindow();
    void initFonts();
    void initTexts();
    void initShapes();

    void pollEvents();
    void update(float dt);
    void render();

    void renderMenu();
    void renderGame();
    void renderSlots();
    void renderBlackjack();
    void renderRoulette();

    void applyHoverStyle(sf::RectangleShape& box, bool hovered);

    // ── Slot-Logik ─────────────────────────────
    void        startSpin();
    void        updateSpinAnimation(float dt);
    void        updateWinBlink(float dt);
    void        finalizeSpin();
    int         calcSlotWin() const;
    std::string randomSymbol() const;

    // -- Blackjack-Logik ---------------------
    void        initDeck();
    void        startBlackjackRound();
    void        playerHit();
    void        playerStand();
    void        playerDoubleDown();
    void        runDealerAI();
    void        evaluateRound();
    int         calcHandScore(const std::vector<BlackjackCard>& hand) const;

    // -- Roulette-Logik ----------------------
    void        spinRoulette();
    void        updateRouletteAnimation(float dt);
    void        evaluateRouletteSpin();
    int         generateWinningNumber() const;
    sf::Color   getRouletteNumberColor(int number) const;
    bool        isRedNumber(int number) const;
    int         getWheelPositionForNumber(int number) const;

    // ── Window & State ─────────────────────────
    sf::RenderWindow window;
    GameState        currentState;

    // ── Economy ────────────────────────────────
    int playerBalance;

    // ── Font ───────────────────────────────────
    sf::Font font;

    // ── Menu texts ─────────────────────────────
    sf::Text textTitle;
    sf::Text textSubtitle;
    sf::Text textBalance;
    sf::Text textCard1, textCard2, textCard3;
    sf::Text textEscHint;

    // ── Menu shapes ────────────────────────────
    sf::RectangleShape cardBox1, cardBox2, cardBox3;
    sf::RectangleShape titleDivider;

    // ── Hover-State ────────────────────────────
    bool hoverBox1{false}, hoverBox2{false}, hoverBox3{false};

    // ── HUD ────────────────────────────────────
    sf::Text           textHUD;
    sf::RectangleShape hudBox;

    // ══════════════════════════════════════════
    //   SLOTS
    // ══════════════════════════════════════════
    std::string              slots[3];
    std::vector<std::string> symbolWeights;
    int         currentBet    { 10 };
    std::string slotResultMsg;
    int         lastWin       { 0 };

    bool  isSpinning   { false };
    float spinElapsed  { 0.f };
    float flickerAccum { 0.f };
    bool  reel1Stopped { false };
    bool  reel2Stopped { false };
    bool  reel3Stopped { false };

    static constexpr float STOP1        = 1.0f;
    static constexpr float STOP2        = 1.5f;
    static constexpr float STOP3        = 2.0f;
    static constexpr float FLICKER_FAST = 0.06f;
    static constexpr float FLICKER_SLOW = 0.18f;

    bool  winBlinking     { false };
    float winBlinkTimer   { 0.f };
    float winBlinkPhase   { 0.f };
    bool  winBlinkVisible { true };

    static constexpr float WIN_BLINK_DURATION = 3.2f;
    static constexpr float WIN_BLINK_INTERVAL = 0.20f;

    sf::RectangleShape slotCabinetBox;
    sf::RectangleShape slotPayline;
    sf::RectangleShape slotReelBox[3];
    sf::RectangleShape slotBetBox;
    sf::Text           textSlotTitle;
    sf::Text           textSlotReel[3];
    sf::Text           textSlotBet;
    sf::Text           textSlotResult;
    sf::Text           textSlotControls;

    // ══════════════════════════════════════════
    //   BLACKJACK
    // ══════════════════════════════════════════
    std::vector<BlackjackCard> deck;
    std::vector<BlackjackCard> playerHand;
    std::vector<BlackjackCard> dealerHand;
    sf::Texture                cardsTexture;

    int      playerScore         { 0 };
    int      dealerScore         { 0 };
    bool     blackjackGameActive { false };
    bool     dealerRevealed      { false };
    int      blackjackBet        { 20 };
    int      blackjackBetOriginal { 20 };
    bool     blackjackDoubledDown { false };
    BjResult bjResult            { BjResult::NONE };

    // Shapes
    sf::RectangleShape bjDealerArea;
    sf::RectangleShape bjPlayerArea;
    sf::RectangleShape bjTableLine;
    sf::RectangleShape bjBetBox;
    sf::RectangleShape bjResultBox;

    // Texte
    sf::Text textBjTitle;
    sf::Text textBjBet;
    sf::Text textBjDealerLabel;
    sf::Text textBjDealerHand;
    sf::Text textBjDealerScore;
    sf::Text textBjPlayerLabel;
    sf::Text textBjPlayerHand;
    sf::Text textBjPlayerScore;
    sf::Text textBjResult;    // Grosses Ergebnis: "YOU WIN!" etc.
    sf::Text textBjStatus;    // Aktions-Hinweis
    sf::Text textBjControls;  // Dauerhafte Tastenbelegung

    // ══════════════════════════════════════════
    //   ROULETTE
    // ══════════════════════════════════════════
    // Multi-bet system
    int   rouletteNumberBets[37] {0};   // bets placed on individual numbers 0..36
    int   betAmountRed         { 0 };
    int   betAmountBlack       { 0 };
    int   betAmountEven        { 0 };
    int   betAmountOdd         { 0 };
    int   selectedRouletteNumber { -1 }; // -1 = no highlighted number
    int   rouletteChip         { 20 };  // currently selected chip to add when clicking
    bool  rouletteSpinning    { false };
    int   winningNumber       { -1 };
    float rouletteTimer       { 0.f };
    float wheelAngle          { 0.f };
    float ballAngle           { 0.f };
    // Animation helpers
    float ballAngleStart      { 0.f };
    float ballAngleFinal      { 0.f };   // final ball angle (relative to wheel) — pocket offset of winning segment
    float wheelAngleStart     { 0.f };   // snapshot of wheelAngle when spin begins
    float wheelAngleTarget    { 0.f };   // absolute wheel angle (deg) where winning number sits at top
    float wheelAngularVel     { 0.f };
    float ballSpeed           { 0.f };
    float ballRadiusCurrent   { 0.f };
    float ballRadiusStart     { 0.f };
    float ballRadiusTarget    { 0.f };

    static constexpr float ROULETTE_SPIN_DURATION = 3.0f;
    static constexpr float WHEEL_RADIUS = 140.f;
    static constexpr float BALL_RADIUS = 8.f;
    static constexpr float WHEEL_CENTER_X = 250.f;
    static constexpr float WHEEL_CENTER_Y = 200.f;

    sf::RectangleShape rouletteWheelArea;
    sf::RectangleShape rouletteGrid[37];
    sf::RectangleShape betRedButton;
    sf::RectangleShape betBlackButton;
    sf::RectangleShape betEvenButton;
    sf::RectangleShape betOddButton;
    sf::Text           textRouletteTitle;
    sf::Text           textRouletteBetOptions[4];
    sf::Text           textRouletteBet;
    sf::Text           textRouletteResult;
    sf::Text           textRouletteControls;
    sf::Text           textRouletteWinningNumber;
};
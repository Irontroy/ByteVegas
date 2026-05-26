#include "Game.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdlib>
#include <ctime>
#include <cmath>

// ═══════════════════════════════════════════════════════
//   Farb-Palette
// ═══════════════════════════════════════════════════════
namespace Color
{
    const sf::Color BG_MENU        (18,  18,  24);
    const sf::Color GOLD           (212, 175,  55);
    const sf::Color GOLD_BRIGHT    (255, 215,   0);
    const sf::Color GOLD_DIM       (160, 130,  40);
    const sf::Color CARD_BG        (30,  30,  40);
    const sf::Color CARD_BG_HOV    (42,  42,  58);
    const sf::Color LIGHT_GRAY     (200, 200, 210);
    const sf::Color MID_GRAY       (140, 140, 155);
    const sf::Color BG_BLACKJACK   (20,  90,  50);
    const sf::Color BG_ROULETTE    (120,  20,  30);
    const sf::Color BG_SLOTS       (60,  20,  80);
    const sf::Color HUD_BG         (0,   0,   0, 160);
    // Slots
    const sf::Color CABINET_BG     (28,  18,  42);
    const sf::Color CABINET_BORDER (212, 175,  55);
    const sf::Color CABINET_BLINK  (255, 215,   0);
    const sf::Color CABINET_WIN2   (255, 140,   0);
    const sf::Color REEL_BG        (38,  15,  58);
    const sf::Color REEL_SPIN      (78,  38, 108);
    const sf::Color REEL_BORDER    (120,  75, 155);
    const sf::Color PAYLINE_COLOR  (255,  50,  50);
    const sf::Color WIN_TEXT       (255, 215,   0);
    const sf::Color LOSE_TEXT      (220,  60,  60);
    const sf::Color JACKPOT_TEXT   (255, 255, 100);
    const sf::Color BET_BOX_BG     (0,   0,   0, 140);
    // Blackjack
    const sf::Color BJ_AREA_BG     (15,  75,  40);
    const sf::Color BJ_AREA_BORDER (40, 140,  70);
    const sf::Color BJ_DIVIDER     (30, 110,  55);
    const sf::Color BJ_BET_BG      (0,   0,   0, 160);
    const sf::Color BJ_CARD_TEXT   (255, 255, 220);
    const sf::Color BJ_SCORE       (212, 175,  55);
    const sf::Color BJ_BUST        (220,  60,  60);
}

// ─────────────────────────────────────────────────────
//  Blackjack-Layout (600px Fensterhoehe)
//
//   y:  44        Titel "B L A C K J A C K"
//   y:  68        Dealer-Label
//   y:  78–248    bjDealerArea  (170px)
//   y:  88        Dealer-Karten (inside area)
//   y: 210        Dealer-Score  (inside area)
//   y: 252        Trennlinie (2px)
//   ------ Ergebnis-Text sitzt bei y=248, ZWISCHEN den Bereichen ------
//   y: 260        Spieler-Label
//   y: 270–440    bjPlayerArea  (170px)
//   y: 280        Spieler-Karten (inside area)
//   y: 406        Spieler-Score  (inside area)
//   y: 450–490    bjBetBox      (40px)
//   y: 460        textBjBet     (inside bet box)
//   y: 500–530    textBjStatus  (klar unterhalb bet box)
//   y: 544        textBjControls (ganz unten, 14px)
// ─────────────────────────────────────────────────────
namespace BjLayout
{
    // Dealer-Bereich
    const float DEALER_AREA_Y   = 78.f;
    const float DEALER_AREA_H   = 170.f;
    const float DEALER_LABEL_Y  = 84.f;
    const float DEALER_HAND_Y   = 104.f;
    const float DEALER_SCORE_Y  = 213.f;

    // Trennbereich
    const float DIVIDER_Y       = 252.f;
    const float RESULT_Y        = 440.f;   // moved down to sit between player area and bet box

    // Spieler-Bereich
    const float PLAYER_AREA_Y   = 262.f;
    const float PLAYER_AREA_H   = 170.f;
    const float PLAYER_LABEL_Y  = 268.f;
    const float PLAYER_HAND_Y   = 288.f;
    const float PLAYER_SCORE_Y  = 399.f;

    // Bet-Box
    const float BET_BOX_Y       = 442.f;
    const float BET_BOX_H       = 40.f;
    const float BET_TEXT_Y      = 452.f;   // mittig in bet box

    // Status (klar UNTER der bet box)
    const float STATUS_Y        = 494.f;

    // Controls (ganz unten)
    const float CONTROLS_Y      = 546.f;

    // Gemeinsam
    // Widerer Bereich für HD: zentriert bei 1280px
    const float AREA_W          = 900.f;
    const float AREA_X          = (1280.f - AREA_W) / 2.f;
}

namespace Layout
{
    const float CAB_W=800.f, CAB_H=230.f;
    const float CAB_X=(1280.f-CAB_W)/2.f, CAB_Y=170.f;
    const float REEL_W=150.f, REEL_H=170.f;
    const float REEL_GAP=16.f;
    const float REEL_Y=CAB_Y+(CAB_H-REEL_H)/2.f;
    const float REEL_TOTAL_W = 3.f*REEL_W + 2.f*REEL_GAP;
    const float REEL_X0 = CAB_X + (CAB_W - REEL_TOTAL_W)/2.f;
    const float REEL_X1 = REEL_X0 + REEL_W + REEL_GAP;
    const float REEL_X2 = REEL_X1 + REEL_W + REEL_GAP;
    const float PAY_Y=CAB_Y+CAB_H/2.f-1.f;
    const float PAY_X=CAB_X, PAY_W=CAB_W;
}

// ═══════════════════════════════════════════════════════
//   Roulette-Kessel-Konstanten
// ═══════════════════════════════════════════════════════
namespace RouletteWheel
{
    // Europäische Roulette-Kessel-Reihenfolge im Uhrzeigersinn
    static const int wheelSequence[] = {
        0, 32, 15, 19, 4, 21, 2, 25, 17, 34, 6, 27, 13, 36, 11, 30, 8, 23, 10, 5,
        24, 16, 33, 1, 20, 14, 31, 9, 22, 18, 29, 7, 28, 12, 35, 3, 26
    };
    static constexpr int WHEEL_SIZE = 37;
}

// ═══════════════════════════════════════════════════════
//   Hilfsfunktionen
// ═══════════════════════════════════════════════════════
static void centerTextH(sf::Text& t, float winW, float y)
{
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(std::floor(b.left + b.width/2.f), std::floor(b.top + b.height/2.f));
    t.setPosition(std::floor(winW/2.f), y);
}

static void centerTextInBox(sf::Text& t, const sf::RectangleShape& box)
{
    t.setOrigin(0.f, 0.f);
    sf::FloatRect tb = t.getLocalBounds();
    sf::FloatRect bb = box.getGlobalBounds();
    t.setOrigin(std::floor(tb.left + tb.width/2.f),
                std::floor(tb.top  + tb.height/2.f));
    t.setPosition(std::floor(bb.left + bb.width/2.f),
                  std::floor(bb.top  + bb.height/2.f));
}

// ═══════════════════════════════════════════════════════
//   Konstruktor
// ═══════════════════════════════════════════════════════
Game::Game()
    : currentState(GameState::MENU)
    , playerBalance(1000)
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    symbolWeights.insert(symbolWeights.end(),  2, "[ 7 ]");
    symbolWeights.insert(symbolWeights.end(),  8, "[BAR]");
    symbolWeights.insert(symbolWeights.end(), 14, "[ O ]");
    symbolWeights.insert(symbolWeights.end(), 16, "[ X ]");

    initWindow();
    initFonts();
    initShapes();
    initTexts();

    slots[0] = slots[1] = slots[2] = "[ ? ]";
    initDeck();
}

void Game::initWindow()
{
    window.create(sf::VideoMode(1280, 720), "ByteVegas",
                  sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
}

void Game::initFonts()
{
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
        std::cerr << "[ERROR] Font nicht geladen\n";
}

void Game::initShapes()
{
    // ── Menu ────────────────────────────────────
    const float BW=200.f,BH=90.f,BG=40.f,BY=340.f;
    const float BX0=(1280.f-(3.f*BW+2.f*BG))/2.f;
    auto mkBox=[&](sf::RectangleShape& b,float x){
        b.setSize({BW,BH}); b.setPosition(x,BY);
        b.setFillColor(Color::CARD_BG);
        b.setOutlineColor(Color::GOLD_DIM);
        b.setOutlineThickness(1.5f);
    };
    mkBox(cardBox1,BX0); mkBox(cardBox2,BX0+BW+BG); mkBox(cardBox3,BX0+2.f*(BW+BG));

    titleDivider.setSize({800.f,1.f});
    titleDivider.setFillColor(Color::GOLD_DIM);
    titleDivider.setOrigin(400.f,0.f);
    titleDivider.setPosition(640.f,160.f);

    // ── HUD ─────────────────────────────────────
    hudBox.setSize({210.f,38.f}); hudBox.setPosition(8.f,6.f);
    hudBox.setFillColor(Color::HUD_BG);
    hudBox.setOutlineColor(sf::Color(255,255,255,60));
    hudBox.setOutlineThickness(1.f);

    // ── Slots ────────────────────────────────────
    slotCabinetBox.setSize({Layout::CAB_W,Layout::CAB_H});
    slotCabinetBox.setPosition(Layout::CAB_X,Layout::CAB_Y);
    slotCabinetBox.setFillColor(Color::CABINET_BG);
    slotCabinetBox.setOutlineColor(Color::CABINET_BORDER);
    slotCabinetBox.setOutlineThickness(5.f);

    slotPayline.setSize({Layout::PAY_W,3.f});
    slotPayline.setPosition(Layout::PAY_X,Layout::PAY_Y);
    slotPayline.setFillColor(Color::PAYLINE_COLOR);

    const float rx[3]={Layout::REEL_X0,Layout::REEL_X1,Layout::REEL_X2};
    for(int i=0;i<3;++i){
        slotReelBox[i].setSize({Layout::REEL_W,Layout::REEL_H});
        slotReelBox[i].setPosition(rx[i],Layout::REEL_Y);
        slotReelBox[i].setFillColor(Color::REEL_BG);
        slotReelBox[i].setOutlineColor(Color::REEL_BORDER);
        slotReelBox[i].setOutlineThickness(2.f);
    }
    slotBetBox.setFillColor(Color::BET_BOX_BG);
    slotBetBox.setOutlineColor(sf::Color(130,80,160,180));
    slotBetBox.setOutlineThickness(1.f);

    // ── Blackjack ────────────────────────────────
    bjDealerArea.setSize({BjLayout::AREA_W, BjLayout::DEALER_AREA_H});
    bjDealerArea.setPosition(BjLayout::AREA_X, BjLayout::DEALER_AREA_Y);
    bjDealerArea.setFillColor(Color::BJ_AREA_BG);
    bjDealerArea.setOutlineColor(Color::BJ_AREA_BORDER);
    bjDealerArea.setOutlineThickness(1.5f);

    bjPlayerArea.setSize({BjLayout::AREA_W, BjLayout::PLAYER_AREA_H});
    bjPlayerArea.setPosition(BjLayout::AREA_X, BjLayout::PLAYER_AREA_Y);
    bjPlayerArea.setFillColor(Color::BJ_AREA_BG);
    bjPlayerArea.setOutlineColor(Color::BJ_AREA_BORDER);
    bjPlayerArea.setOutlineThickness(1.5f);

    bjTableLine.setSize({BjLayout::AREA_W, 2.f});
    bjTableLine.setPosition(BjLayout::AREA_X, BjLayout::DIVIDER_Y);
    bjTableLine.setFillColor(Color::BJ_DIVIDER);

    bjBetBox.setSize({BjLayout::AREA_W, BjLayout::BET_BOX_H});
    // ensure the blackjack bet bar is exactly centered at X=640
    bjBetBox.setOrigin(BjLayout::AREA_W / 2.f, 0.f);
    bjBetBox.setPosition(640.f, BjLayout::BET_BOX_Y);
    bjBetBox.setFillColor(Color::BJ_BET_BG);
    bjBetBox.setOutlineColor(sf::Color(40,140,70,180));
    bjBetBox.setOutlineThickness(1.f);

    // bjResultBox: schmaler Streifen genau im Spalt zwischen den Bereichen
    // Dealer endet bei y=248, Spieler beginnt bei y=262 => 14px Luft + Trennlinie
    // Wir zeichnen den Text ohne Box-Shape; bjResultBox bleibt minimal
    bjResultBox.setSize({0.f,0.f});  // nicht verwendet als Overlay
    bjResultBox.setFillColor(sf::Color(0,0,0,0));
    bjResultBox.setOutlineThickness(0.f);

    // ── Roulette ────────────────────────────────
    rouletteWheelArea.setSize({0.f,0.f});  // nicht verwendet, wird gezeichnet
    rouletteWheelArea.setFillColor(sf::Color(0,0,0,0));
    rouletteWheelArea.setOutlineThickness(0.f);

    const float gridW = 60.f;
    const float gridH = 60.f;
    const float gridGap = 4.f;
    const float gridStartX = 520.f;
    const float gridStartY = 150.f;

    // Zero cell: tall vertical bar left of the three rows
    float zeroW = gridW;
    float zeroH = gridH * 3.f + gridGap * 2.f;
    float zeroX = gridStartX - zeroW - gridGap - 6.f;
    float zeroY = gridStartY;
    rouletteGrid[0].setSize({zeroW, zeroH});
    rouletteGrid[0].setPosition(zeroX, zeroY);
    rouletteGrid[0].setFillColor(sf::Color(20, 100, 45));
    rouletteGrid[0].setOutlineColor(sf::Color(0,0,0,0));
    rouletteGrid[0].setOutlineThickness(0.f);

    // Numeric grid: 3 rows x 12 cols with ordering per spec
    for(int col=0; col<12; ++col){
        for(int row=0; row<3; ++row){
            int n = 1 + row + 3*col; // produces 1..36 in required order
            int idx = n;
            float x = gridStartX + (gridW + gridGap) * col;
            float y = gridStartY + (gridH + gridGap) * row;
            rouletteGrid[idx].setSize({gridW, gridH});
            rouletteGrid[idx].setPosition(x, y);
            rouletteGrid[idx].setFillColor(getRouletteNumberColor(n));
            rouletteGrid[idx].setOutlineColor(sf::Color(0,0,0,0));
            rouletteGrid[idx].setOutlineThickness(0.f);
        }
    }

    const float BBW = 240.f;
    const float BBH = 56.f;
    const float BBG = 22.f;
    const float BBX0 = (1280.f - (4.f * BBW + 3.f * BBG)) / 2.f;
    const float BBY = 560.f;

    betRedButton.setSize({BBW, BBH});
    betRedButton.setPosition(BBX0, BBY);
    betRedButton.setFillColor(sf::Color(180, 20, 30));
    betRedButton.setOutlineColor(sf::Color(0,0,0,0));
    betRedButton.setOutlineThickness(0.f);

    betBlackButton.setSize({BBW, BBH});
    betBlackButton.setPosition(BBX0 + (BBW + BBG) * 1, BBY);
    betBlackButton.setFillColor(sf::Color(30, 30, 30));
    betBlackButton.setOutlineColor(sf::Color(0,0,0,0));
    betBlackButton.setOutlineThickness(0.f);

    betEvenButton.setSize({BBW, BBH});
    betEvenButton.setPosition(BBX0 + (BBW + BBG) * 2, BBY);
    betEvenButton.setFillColor(Color::CARD_BG);
    betEvenButton.setOutlineColor(sf::Color(0,0,0,0));
    betEvenButton.setOutlineThickness(0.f);

    betOddButton.setSize({BBW, BBH});
    betOddButton.setPosition(BBX0 + (BBW + BBG) * 3, BBY);
    betOddButton.setFillColor(Color::CARD_BG);
    betOddButton.setOutlineColor(sf::Color(0,0,0,0));
    betOddButton.setOutlineThickness(0.f);
}

void Game::initTexts()
{
    // ── Menu ────────────────────────────────────
    textTitle.setFont(font); textTitle.setString("Welcome to ByteVegas");
    textTitle.setCharacterSize(50); textTitle.setFillColor(Color::GOLD);
    textTitle.setStyle(sf::Text::Bold);
    centerTextH(textTitle,1280.f,62.f);

    textSubtitle.setFont(font); textSubtitle.setString("Where Fortunes Are Made");
    textSubtitle.setCharacterSize(17); textSubtitle.setFillColor(Color::MID_GRAY);
    textSubtitle.setStyle(sf::Text::Italic);
    centerTextH(textSubtitle,1280.f,126.f);

    textBalance.setFont(font); textBalance.setCharacterSize(24);
    textBalance.setFillColor(Color::LIGHT_GRAY);

    auto mkCardText=[&](sf::Text& t,const std::string& l1,
                         const std::string& l2,const sf::RectangleShape& box){
        t.setFont(font); t.setString(l1+"\n"+l2);
        t.setCharacterSize(19); t.setFillColor(Color::LIGHT_GRAY);
        centerTextInBox(t,box);
    };
    mkCardText(textCard1,"[ 1 ]","Blackjack",cardBox1);
    mkCardText(textCard2,"[ 2 ]","Roulette", cardBox2);
    mkCardText(textCard3,"[ 3 ]","Slots",    cardBox3);

    textEscHint.setFont(font);
    textEscHint.setString("[ESC] Exit current game  |  [ESC in Menu] Quit");
    textEscHint.setCharacterSize(15); textEscHint.setFillColor(Color::MID_GRAY);
    centerTextH(textEscHint,1280.f,510.f);

    // ── HUD ─────────────────────────────────────
    textHUD.setFont(font); textHUD.setCharacterSize(18);
    textHUD.setFillColor(Color::GOLD); textHUD.setPosition(18.f,13.f);

    // ── Slots ────────────────────────────────────
    textSlotTitle.setFont(font);
    textSlotTitle.setString("B Y T E V E G A S   S L O T S");
    textSlotTitle.setCharacterSize(22); textSlotTitle.setFillColor(Color::GOLD);
    textSlotTitle.setStyle(sf::Text::Bold);
    centerTextH(textSlotTitle,1280.f,128.f);

    for(int i=0;i<3;++i){
        textSlotReel[i].setFont(font); textSlotReel[i].setCharacterSize(38);
        textSlotReel[i].setFillColor(Color::GOLD_BRIGHT);
        textSlotReel[i].setStyle(sf::Text::Bold);
    }
    textSlotBet.setFont(font); textSlotBet.setCharacterSize(21);
    textSlotBet.setFillColor(Color::LIGHT_GRAY);
    textSlotResult.setFont(font); textSlotResult.setCharacterSize(46);
    textSlotResult.setStyle(sf::Text::Bold); textSlotResult.setString("");
    textSlotControls.setFont(font); textSlotControls.setCharacterSize(15);
    textSlotControls.setFillColor(Color::MID_GRAY);
    textSlotControls.setString("[SPACE] Drehen | [UP/DOWN] Einsatz anpassen | [ESC] Menue");
    centerTextH(textSlotControls,1280.f,680.f);

    // ── Blackjack ────────────────────────────────

    // Titel (above dealer area)
    textBjTitle.setFont(font);
    textBjTitle.setString("B L A C K J A C K");
    textBjTitle.setCharacterSize(22); textBjTitle.setFillColor(Color::GOLD);
    textBjTitle.setStyle(sf::Text::Bold);
    centerTextH(textBjTitle,1280.f,48.f);

    // Dealer section
    textBjDealerLabel.setFont(font);
    textBjDealerLabel.setString("DEALER");
    textBjDealerLabel.setCharacterSize(14);
    textBjDealerLabel.setFillColor(Color::MID_GRAY);
    textBjDealerLabel.setStyle(sf::Text::Bold);
    textBjDealerLabel.setPosition(BjLayout::AREA_X+8.f, BjLayout::DEALER_LABEL_Y);

    textBjDealerHand.setFont(font);
    textBjDealerHand.setCharacterSize(30);
    textBjDealerHand.setFillColor(Color::BJ_CARD_TEXT);
    textBjDealerHand.setStyle(sf::Text::Bold);
    textBjDealerHand.setPosition(BjLayout::AREA_X+8.f, BjLayout::DEALER_HAND_Y);

    textBjDealerScore.setFont(font);
    textBjDealerScore.setCharacterSize(16);
    textBjDealerScore.setFillColor(Color::BJ_SCORE);
    textBjDealerScore.setPosition(BjLayout::AREA_X+8.f, BjLayout::DEALER_SCORE_Y);

    // Player section
    textBjPlayerLabel.setFont(font);
    textBjPlayerLabel.setString("YOUR HAND");
    textBjPlayerLabel.setCharacterSize(14);
    textBjPlayerLabel.setFillColor(Color::MID_GRAY);
    textBjPlayerLabel.setStyle(sf::Text::Bold);
    textBjPlayerLabel.setPosition(BjLayout::AREA_X+8.f, BjLayout::PLAYER_LABEL_Y);

    textBjPlayerHand.setFont(font);
    textBjPlayerHand.setCharacterSize(30);
    textBjPlayerHand.setFillColor(Color::BJ_CARD_TEXT);
    textBjPlayerHand.setStyle(sf::Text::Bold);
    textBjPlayerHand.setPosition(BjLayout::AREA_X+8.f, BjLayout::PLAYER_HAND_Y);

    textBjPlayerScore.setFont(font);
    textBjPlayerScore.setCharacterSize(16);
    textBjPlayerScore.setFillColor(Color::BJ_SCORE);
    textBjPlayerScore.setPosition(BjLayout::AREA_X+8.f, BjLayout::PLAYER_SCORE_Y);

    // Bet text (inside bet box)
    textBjBet.setFont(font);
    textBjBet.setCharacterSize(18);
    textBjBet.setFillColor(Color::LIGHT_GRAY);
    // position set in update()

    // Result text: centered, between the two areas
    // Floats in the gap at y=RESULT_Y (230), fontsize 28 so it fits cleanly
    textBjResult.setFont(font);
    textBjResult.setCharacterSize(28);
    textBjResult.setStyle(sf::Text::Bold);
    textBjResult.setString("");

    // Status text: BELOW bet box, clearly separated
    textBjStatus.setFont(font);
    textBjStatus.setCharacterSize(17);
    textBjStatus.setFillColor(Color::GOLD);
    // position set in update()

    // Controls: bottom of screen
    textBjControls.setFont(font);
    textBjControls.setCharacterSize(18);
    textBjControls.setFillColor(Color::MID_GRAY);
    textBjControls.setString(
        "[SPACE] Neue Runde | [H] Karte ziehen (Hit) | [S] Halten (Stand) | [UP/DOWN] Einsatz anpassen | [ESC] Menue");
    centerTextH(textBjControls,1280.f,685.f);

    // ── Roulette ────────────────────────────────
    textRouletteTitle.setFont(font);
    textRouletteTitle.setString("R O U L E T T E");
    textRouletteTitle.setCharacterSize(28);
    textRouletteTitle.setFillColor(Color::GOLD);
    textRouletteTitle.setStyle(sf::Text::Bold);
    centerTextH(textRouletteTitle, 1280.f, 34.f);

    // Bet options
    std::string betLabels[4]={"ROT","SCHWARZ","GERADE","UNGERADE"};
    sf::RectangleShape* buttonShapes[4] = {
        &betRedButton, &betBlackButton, &betEvenButton, &betOddButton
    };
    for(int i=0;i<4;++i){
        textRouletteBetOptions[i].setFont(font);
        textRouletteBetOptions[i].setString(betLabels[i]);
        textRouletteBetOptions[i].setCharacterSize(18);
        textRouletteBetOptions[i].setFillColor(Color::LIGHT_GRAY);
        centerTextInBox(textRouletteBetOptions[i], *buttonShapes[i]);
    }

    // Bet amount display
    textRouletteBet.setFont(font);
    textRouletteBet.setCharacterSize(20);
    textRouletteBet.setFillColor(Color::LIGHT_GRAY);

    // Result display
    textRouletteResult.setFont(font);
    textRouletteResult.setCharacterSize(40);
    textRouletteResult.setFillColor(Color::JACKPOT_TEXT);
    textRouletteResult.setStyle(sf::Text::Bold);
    textRouletteResult.setString("");

    // Winning number display
    textRouletteWinningNumber.setFont(font);
    textRouletteWinningNumber.setCharacterSize(48);
    textRouletteWinningNumber.setFillColor(Color::WIN_TEXT);
    textRouletteWinningNumber.setStyle(sf::Text::Bold);
    textRouletteWinningNumber.setString("");

    // Controls
    textRouletteControls.setFont(font);
    textRouletteControls.setCharacterSize(18);
    textRouletteControls.setFillColor(Color::MID_GRAY);
    textRouletteControls.setString(
        "[CLICK] Zahl oder Sonderfeld auswaehlen | [SPACE] Rad drehen | [RECHTSKLICK] Einsatz entfernen | [ESC] Menue");
    textRouletteControls.setOrigin(
        textRouletteControls.getLocalBounds().left + textRouletteControls.getLocalBounds().width / 2.0f,
        textRouletteControls.getLocalBounds().top + textRouletteControls.getLocalBounds().height / 2.0f);
    textRouletteControls.setPosition(640.f, 685.f);
}

// ═══════════════════════════════════════════════════════
//   Blackjack-Logik
// ═══════════════════════════════════════════════════════
void Game::initDeck()
{
    if (cardsTexture.getNativeHandle() == 0) {
        if (!cardsTexture.loadFromFile("cards.png")) {
            if (!cardsTexture.loadFromFile("../cards.png")) {
                std::cerr << "[ERROR] cards.png konnte weder im aktuellen noch im Root-Ordner gefunden werden!\n";
            }
        }
    }

    deck.clear();
    for(int suit=0;suit<4;++suit){
        for(int value=0; value<13; ++value){
            deck.push_back(BlackjackCard{ suit, value });
        }
    }

    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::shuffle(deck.begin(),deck.end(),rng);
}

int Game::calcHandScore(const std::vector<BlackjackCard>& hand) const
{
    int total = 0;
    int aces = 0;
    for(const auto& card : hand){
        total += card.getScoreValue();
        if(card.value == 0) ++aces;
    }
    while(total > 21 && aces > 0){
        total -= 10;
        --aces;
    }
    return total;
}

void Game::startBlackjackRound()
{
    if(playerBalance < blackjackBet){
        return;
    }
    if(deck.size() < 10) initDeck();

    playerBalance -= blackjackBet;
    playerHand.clear();
    dealerHand.clear();

    playerHand.push_back(deck.back()); deck.pop_back();
    dealerHand.push_back(deck.back()); deck.pop_back();
    playerHand.push_back(deck.back()); deck.pop_back();
    dealerHand.push_back(deck.back()); deck.pop_back();

    playerScore       = calcHandScore(playerHand);
    dealerScore       = calcHandScore(dealerHand);
    blackjackGameActive = true;
    dealerRevealed    = false;
    bjResult          = BjResult::NONE;
    blackjackBetOriginal = blackjackBet;
    blackjackDoubledDown = false;
    textBjResult.setString("");

    if(playerScore == 21){
        dealerRevealed = true;
        bjResult = BjResult::NATURAL_BJ;
        evaluateRound();
        return;
    }

}

void Game::playerHit()
{
    if(!blackjackGameActive || deck.empty()) return;
    playerHand.push_back(deck.back()); deck.pop_back();
    playerScore = calcHandScore(playerHand);
    if(playerScore > 21){
        bjResult = BjResult::PLAYER_BUST;
        evaluateRound();
    }
}

void Game::playerStand()
{
    if(!blackjackGameActive) return;
    dealerRevealed = true;
    runDealerAI();
}

void Game::playerDoubleDown()
{
    // Check conditions: exactly 2 cards and sufficient balance
    if(!blackjackGameActive || playerHand.size() != 2) return;
    if(playerBalance < blackjackBet){
        return;
    }

    // Deduct the bet again and double it
    playerBalance -= blackjackBet;
    blackjackBet *= 2;
    blackjackDoubledDown = true;

    // Deal exactly one more card
    if(!deck.empty()){
        playerHand.push_back(deck.back()); deck.pop_back();
    }
    playerScore = calcHandScore(playerHand);

    // Check for bust
    if(playerScore > 21){
        bjResult = BjResult::PLAYER_BUST;
        evaluateRound();
    } else {
        // Auto-stand: proceed directly to dealer
        dealerRevealed = true;
        runDealerAI();
    }
}

void Game::runDealerAI()
{
    dealerScore = calcHandScore(dealerHand);
    while(dealerScore < 17){
        if(deck.empty()) initDeck();
        dealerHand.push_back(deck.back()); deck.pop_back();
        dealerScore = calcHandScore(dealerHand);
    }
    evaluateRound();
}

void Game::evaluateRound()
{
    blackjackGameActive = false;
    playerScore = calcHandScore(playerHand);
    dealerScore = calcHandScore(dealerHand);

    std::string resultStr;
    sf::Color   resultColor;

    if(bjResult == BjResult::NATURAL_BJ){
        int win = static_cast<int>(blackjackBet * 2.5f);
        playerBalance += win;
        resultStr   = "BLACKJACK! +" + std::to_string(win - blackjackBet) + "$";
        resultColor = Color::JACKPOT_TEXT;
        bjResult    = BjResult::PLAYER_WIN;
    }
    else if(bjResult == BjResult::PLAYER_BUST){
        resultStr   = "BUST - Dealer wins";
        resultColor = Color::BJ_BUST;
    }
    else if(dealerScore > 21){
        playerBalance += blackjackBet * 2;
        resultStr   = "DEALER BUSTS - You win! +" + std::to_string(blackjackBet) + "$";
        resultColor = Color::WIN_TEXT;
        bjResult    = BjResult::PLAYER_WIN;
    }
    else if(playerScore > dealerScore){
        playerBalance += blackjackBet * 2;
        resultStr   = "YOU WIN! +" + std::to_string(blackjackBet) + "$";
        resultColor = Color::WIN_TEXT;
        bjResult    = BjResult::PLAYER_WIN;
    }
    else if(dealerScore > playerScore){
        resultStr   = "DEALER WINS";
        resultColor = Color::BJ_BUST;
        bjResult    = BjResult::DEALER_WIN;
    }
    else {
        playerBalance += blackjackBet;
        resultStr   = "PUSH - Bet returned";
        resultColor = Color::GOLD;
        bjResult    = BjResult::PUSH;
    }

    textBjResult.setString(resultStr);
    textBjResult.setFillColor(resultColor);
    // place the blackjack result text closer under the bet bar, centered at X=640
    {
        sf::FloatRect rb = textBjResult.getLocalBounds();
        textBjResult.setOrigin(rb.left + rb.width/2.f, rb.top + rb.height/2.f);
        textBjResult.setPosition(640.f, 565.f);
    }

    // Reset Double Down state
    blackjackBet = blackjackBetOriginal;
    blackjackDoubledDown = false;
}

// ═══════════════════════════════════════════════════════
//   Hover
// ═══════════════════════════════════════════════════════
void Game::applyHoverStyle(sf::RectangleShape& box, bool hovered)
{
    box.setOutlineColor   (hovered ? Color::GOLD_BRIGHT : Color::GOLD_DIM);
    box.setOutlineThickness(hovered ? 3.f : 1.5f);
    box.setFillColor      (hovered ? Color::CARD_BG_HOV : Color::CARD_BG);
}

// ═══════════════════════════════════════════════════════
//   Roulette-Logik
// ═══════════════════════════════════════════════════════
int Game::generateWinningNumber() const
{
    return std::rand() % 37;  // 0-36 inclusive
}

bool Game::isRedNumber(int number) const
{
    // Offizielle Roulette-Rot-Nummern (unabhängig von Kessel-Position)
    static const int redNumbers[] = {1, 3, 5, 7, 9, 12, 14, 16, 18, 19, 21, 23, 25, 27, 30, 32, 34, 36};
    for (int red : redNumbers) {
        if (number == red) return true;
    }
    return false;
}

sf::Color Game::getRouletteNumberColor(int number) const
{
    if (number == 0) return sf::Color(20, 100, 45);
    if (isRedNumber(number)) return sf::Color(180, 20, 30);
    return sf::Color(30, 30, 30);
}

int Game::getWheelPositionForNumber(int number) const
{
    // Findet die Position einer Nummer in der Kessel-Reihenfolge
    for (int i = 0; i < RouletteWheel::WHEEL_SIZE; ++i) {
        if (RouletteWheel::wheelSequence[i] == number) {
            return i;
        }
    }
    return 0;  // Fallback
}

void Game::spinRoulette()
{
    if(rouletteSpinning) return;

    // Calculate total bet across numbers and special bets
    int totalBet = 0;
    for(int i=0;i<37;++i) totalBet += rouletteNumberBets[i];
    totalBet += betAmountRed + betAmountBlack + betAmountEven + betAmountOdd;
    if(totalBet <= 0){
        textRouletteResult.setString("PLACE A BET!");
        textRouletteResult.setFillColor(Color::MID_GRAY);
        return;
    }
    if(playerBalance < totalBet){
        textRouletteResult.setString("NOT ENOUGH BALANCE!");
        textRouletteResult.setFillColor(Color::BJ_BUST);
        centerTextH(textRouletteResult, 1280.f, 300.f);
        return;
    }

    // Deduct total bet from balance and start spin
    playerBalance -= totalBet;
    winningNumber = generateWinningNumber();
    rouletteSpinning = true;
    rouletteTimer = 0.f;
    textRouletteResult.setString("");
    textRouletteWinningNumber.setString("");

    // Compute target angles based on chosen winning number's wheel position
    int wheelPos = getWheelPositionForNumber(winningNumber);
    const float PI = 3.14159265359f;
    const float PI_180 = PI / 180.f;
    float anglePer = 2.f * PI / 37.f;

    // --- Wheel target (in degrees) ---
    // Segment i is drawn at: i * anglePer + wheelAngle (in radians).
    // We want segment wheelPos at the very top of the wheel.
    // In SFML the top of the circle = -PI/2 radians (negative Y axis).
    // So: wheelPos * anglePer + wheelAngleTargetRad = -PI/2
    //     wheelAngleTargetRad = -PI/2 - wheelPos * anglePer
    float wheelAngleTargetRad = -PI / 2.f - wheelPos * anglePer;
    float baseWheelDeg = wheelAngleTargetRad * 180.f / PI;

    // Snapshot current wheel angle so we interpolate FROM here
    wheelAngleStart = wheelAngle;

    // Add full rotations so the wheel visibly spins multiple extra turns
    float extraSpins = 5.f * 360.f;
    float diff = (baseWheelDeg + extraSpins) - wheelAngleStart;
    while (diff < extraSpins) diff += 360.f;  // always spin forward
    wheelAngleTarget = wheelAngleStart + diff;

    // --- Ball target (relative to wheel rotation) ---
    // Rendering computes: ballWorldAngle = ballAngle + wheelRotRad
    // At the end: ballWorldAngle must equal -PI/2 (top of wheel)
    // So: ballAngleFinal + wheelAngleTargetRad = -PI/2
    //     ballAngleFinal = -PI/2 - wheelAngleTargetRad
    // But wheelAngleTargetRad = -PI/2 - wheelPos * anglePer, therefore:
    //     ballAngleFinal = -PI/2 - (-PI/2 - wheelPos * anglePer) = wheelPos * anglePer
    // This is beautifully simple: the ball just needs to land at the segment's
    // angular offset within the wheel — exactly where the pocket is.
    ballAngleFinal = wheelPos * anglePer;

    // Random start angle for the ball (in radians)
    ballAngleStart = static_cast<float>((std::rand() % 360) * PI_180);
    ballAngle = ballAngleStart;
    ballSpeed = 12.f;
    ballRadiusStart = WHEEL_RADIUS - 12.f;
    ballRadiusTarget = WHEEL_RADIUS * 0.45f;
    ballRadiusCurrent = ballRadiusStart;
    wheelAngularVel = 900.f;
}

void Game::updateRouletteAnimation(float dt)
{
    if(!rouletteSpinning) return;

    rouletteTimer += dt;
    float spinProgress = rouletteTimer / ROULETTE_SPIN_DURATION;
    if(spinProgress > 1.f) spinProgress = 1.f;

    // ── Easing: cubic ease-out (fast start, gentle stop) ──
    // t goes 0→1, ease goes 0→1 with zero velocity at t=1
    float t = spinProgress;
    float ease = 1.f - (1.f - t) * (1.f - t) * (1.f - t);

    // ── Wheel: lerp from start angle to computed target angle ──
    // At ease=0 → wheelAngleStart, at ease=1 → wheelAngleTarget
    wheelAngle = wheelAngleStart + (wheelAngleTarget - wheelAngleStart) * ease;

    // ── Ball angle: also eases toward its final world-space target ──
    // The ball decelerates faster than the wheel (settles into the pocket first)
    float ballT = std::min(1.f, t * 1.15f); // ball arrives ~15% earlier visually
    float ballEase = 1.f - (1.f - ballT) * (1.f - ballT) * (1.f - ballT);
    ballAngle = ballAngleStart + (ballAngleFinal - ballAngleStart) * ballEase;

    // Keep ball "velocity" for visual reference (used elsewhere for flicker effects)
    ballSpeed *= 0.96f;

    // ── Ball radius: spiral inward ──
    ballRadiusCurrent = ballRadiusStart + (ballRadiusTarget - ballRadiusStart) * ease;

    // ── Final frame: snap exactly to target (no jump — we're already there) ──
    if(rouletteTimer >= ROULETTE_SPIN_DURATION){
        rouletteSpinning = false;
        wheelAngle = wheelAngleTarget;   // exact, no jump
        ballAngle = ballAngleFinal;      // exact, no jump
        ballRadiusCurrent = ballRadiusTarget;
        ballSpeed = 0.f;
        evaluateRouletteSpin();
    }
}

void Game::evaluateRouletteSpin()
{
    std::string resultStr;
    sf::Color resultColor;

    int totalWon = 0;
    // Number bets: pay 36x
    for(int i=0;i<37;++i){
        if(rouletteNumberBets[i] > 0 && i == winningNumber){
            totalWon += rouletteNumberBets[i] * 36;
        }
    }
    // Color and parity bets: pay 2x
    if(betAmountRed > 0 && winningNumber != 0 && isRedNumber(winningNumber)) totalWon += betAmountRed * 2;
    if(betAmountBlack > 0 && winningNumber != 0 && !isRedNumber(winningNumber)) totalWon += betAmountBlack * 2;
    if(betAmountEven > 0 && winningNumber > 0 && (winningNumber % 2 == 0)) totalWon += betAmountEven * 2;
    if(betAmountOdd  > 0 && winningNumber > 0 && (winningNumber % 2 == 1)) totalWon += betAmountOdd * 2;

    if(winningNumber == 0){
        resultStr = "0 - HAUS GEWINNT!";
        resultColor = Color::BJ_BUST;
    } else if(totalWon > 0){
        playerBalance += totalWon;
        resultStr = "GEWONNEN! +" + std::to_string(totalWon) + "$";
        resultColor = Color::WIN_TEXT;
    } else {
        resultStr = "VERLOREN!";
        resultColor = Color::BJ_BUST;
    }

    textRouletteResult.setString(resultStr);
    textRouletteResult.setFillColor(resultColor);
    textRouletteWinningNumber.setString(std::to_string(winningNumber));
    textRouletteWinningNumber.setFillColor(getRouletteNumberColor(winningNumber));
    // NOTE: Bets are NOT cleared here. They persist until the player starts the next round (presses SPACE).
}

// ═══════════════════════════════════════════════════════
//   Slot-Logik
// ═══════════════════════════════════════════════════════
std::string Game::randomSymbol() const
{ return symbolWeights[std::rand() % symbolWeights.size()]; }

void Game::startSpin()
{
    if(playerBalance < currentBet){ slotResultMsg="NOT ENOUGH BALANCE!"; lastWin=0; return; }
    playerBalance -= currentBet;
    slotResultMsg.clear(); lastWin=0; isSpinning=true; winBlinking=false;
    spinElapsed=0.f; flickerAccum=0.f;
    reel1Stopped=reel2Stopped=reel3Stopped=false;
    slots[0]=randomSymbol(); slots[1]=randomSymbol(); slots[2]=randomSymbol();
    slotCabinetBox.setOutlineColor(Color::CABINET_BORDER);
}

void Game::updateSpinAnimation(float dt)
{
    spinElapsed+=dt; flickerAccum+=dt;
    float nxt  = !reel1Stopped ? STOP1 : (!reel2Stopped ? STOP2 : STOP3);
    float toStop = nxt - spinElapsed;
    float blend  = std::max(0.f, std::min(1.f, 1.f - toStop/0.4f));
    float ivl    = FLICKER_FAST + blend*(FLICKER_SLOW - FLICKER_FAST);
    if(flickerAccum >= ivl){
        flickerAccum = 0.f;
        if(!reel1Stopped) textSlotReel[0].setString(randomSymbol());
        if(!reel2Stopped) textSlotReel[1].setString(randomSymbol());
        if(!reel3Stopped) textSlotReel[2].setString(randomSymbol());
    }
    if(!reel1Stopped && spinElapsed>=STOP1){
        reel1Stopped=true; textSlotReel[0].setString(slots[0]);
        slotReelBox[0].setFillColor(Color::REEL_BG);
    }
    if(!reel2Stopped && spinElapsed>=STOP2){
        reel2Stopped=true; textSlotReel[1].setString(slots[1]);
        slotReelBox[1].setFillColor(Color::REEL_BG);
    }
    if(!reel3Stopped && spinElapsed>=STOP3){
        reel3Stopped=true; textSlotReel[2].setString(slots[2]);
        slotReelBox[2].setFillColor(Color::REEL_BG);
        finalizeSpin();
    }
    if(!reel1Stopped) slotReelBox[0].setFillColor(Color::REEL_SPIN);
    if(!reel2Stopped) slotReelBox[1].setFillColor(Color::REEL_SPIN);
    if(!reel3Stopped) slotReelBox[2].setFillColor(Color::REEL_SPIN);
}

void Game::updateWinBlink(float dt)
{
    if(!winBlinking) return;
    winBlinkTimer+=dt; winBlinkPhase+=dt;
    if(winBlinkPhase >= WIN_BLINK_INTERVAL){
        winBlinkPhase=0.f; winBlinkVisible=!winBlinkVisible;
        slotCabinetBox.setOutlineColor(winBlinkVisible ? Color::CABINET_BLINK : Color::CABINET_WIN2);
        slotCabinetBox.setOutlineThickness(winBlinkVisible ? 7.f : 4.f);
    }
    slotPayline.setFillColor(winBlinkVisible ? Color::PAYLINE_COLOR : Color::GOLD_BRIGHT);
    if(winBlinkTimer >= WIN_BLINK_DURATION){
        winBlinking=false;
        slotCabinetBox.setOutlineColor(Color::CABINET_BORDER);
        slotCabinetBox.setOutlineThickness(5.f);
        slotPayline.setFillColor(Color::PAYLINE_COLOR);
    }
}

void Game::finalizeSpin()
{
    isSpinning=false; lastWin=calcSlotWin(); playerBalance+=lastWin;
    if(slots[0]=="[ 7 ]" && slots[1]=="[ 7 ]" && slots[2]=="[ 7 ]")
        slotResultMsg="*** JACKPOT! ***  +" + std::to_string(lastWin) + " $";
    else if(lastWin>0)
        slotResultMsg="YOU WIN!  +" + std::to_string(lastWin) + " $";
    else
        slotResultMsg="TRY AGAIN";
    if(lastWin>0){ winBlinking=true; winBlinkTimer=0.f; winBlinkPhase=0.f; winBlinkVisible=true; }
}

int Game::calcSlotWin() const
{
    if(slots[0]==slots[1] && slots[1]==slots[2]){
        if(slots[0]=="[ 7 ]") return currentBet*10;
        if(slots[0]=="[BAR]") return currentBet*4;
        return currentBet*2;
    }
    bool p01=(slots[0]==slots[1]), p12=(slots[1]==slots[2]), p02=(slots[0]==slots[2]);
    if(p01||p12||p02){
        const std::string& sym = p01?slots[0]:(p12?slots[1]:slots[0]);
        if(sym=="[ 7 ]") return currentBet*3;
        if(sym=="[BAR]") return currentBet*2;
    }
    return 0;
}

// ═══════════════════════════════════════════════════════
//   Game Loop
// ═══════════════════════════════════════════════════════
void Game::run()
{
    sf::Clock clock;
    while(window.isOpen()){
        float dt = clock.restart().asSeconds();
        if(dt > 0.1f) dt = 0.1f;
        pollEvents(); update(dt); render();
    }
}

// ═══════════════════════════════════════════════════════
//   Events
// ═══════════════════════════════════════════════════════
void Game::pollEvents()
{
    sf::Vector2i mp  = sf::Mouse::getPosition(window);
    sf::Vector2f mpF((float)mp.x, (float)mp.y);

    sf::Event event;
    while(window.pollEvent(event)){
        if(event.type==sf::Event::Closed){ window.close(); return; }

        if(event.type==sf::Event::KeyPressed){
            switch(currentState){

            case GameState::MENU:
                if     (event.key.code==sf::Keyboard::Num1)   currentState=GameState::BLACKJACK;
                else if(event.key.code==sf::Keyboard::Num2)   currentState=GameState::ROULETTE;
                else if(event.key.code==sf::Keyboard::Num3)   currentState=GameState::SLOTS;
                else if(event.key.code==sf::Keyboard::Escape) window.close();
                break;

            case GameState::ROULETTE:
                if(event.key.code==sf::Keyboard::Escape){
                    currentState=GameState::MENU;
                    rouletteSpinning=false;
                    textRouletteResult.setString("");
                    textRouletteWinningNumber.setString("");
                }
                else if(event.key.code==sf::Keyboard::Space && !rouletteSpinning){
                    spinRoulette();
                }
                else if(!rouletteSpinning){
                    // Adjust currently selected chip amount with Up/Down
                    if(event.key.code==sf::Keyboard::Up){
                        rouletteChip = std::min(rouletteChip + 10, (playerBalance/10)*10);
                        if(rouletteChip < 10) rouletteChip = 10;
                    }
                    else if(event.key.code==sf::Keyboard::Down){
                        rouletteChip = std::max(rouletteChip - 10, 10);
                    }
                }
                break;

            case GameState::SLOTS:
                if(event.key.code==sf::Keyboard::Escape){
                    currentState=GameState::MENU; isSpinning=false; winBlinking=false;
                    slotResultMsg.clear();
                    slotCabinetBox.setOutlineColor(Color::CABINET_BORDER);
                    slotCabinetBox.setOutlineThickness(5.f);
                    slotPayline.setFillColor(Color::PAYLINE_COLOR);
                    for(int i=0;i<3;++i) slotReelBox[i].setFillColor(Color::REEL_BG);
                }
                else if(event.key.code==sf::Keyboard::Space && !isSpinning) startSpin();
                else if(!isSpinning){
                    if(event.key.code==sf::Keyboard::Up){
                        currentBet=std::min(currentBet+10,(playerBalance/10)*10);
                        if(currentBet<10) currentBet=10;
                    } else if(event.key.code==sf::Keyboard::Down)
                        currentBet=std::max(currentBet-10,10);
                }
                break;

            case GameState::BLACKJACK:
                if(event.key.code==sf::Keyboard::Escape){
                    currentState=GameState::MENU;
                    blackjackGameActive=false;
                    bjResult=BjResult::NONE;
                    textBjResult.setString("");
                }
                else if(event.key.code==sf::Keyboard::Space && !blackjackGameActive)
                    startBlackjackRound();
                else if(!blackjackGameActive){
                    // Einsatz anpassen nur außerhalb einer Runde
                    if(event.key.code==sf::Keyboard::Up){
                        blackjackBet=std::min(blackjackBet+10,(playerBalance/10)*10);
                        if(blackjackBet<10) blackjackBet=10;
                    } else if(event.key.code==sf::Keyboard::Down)
                        blackjackBet=std::max(blackjackBet-10,10);
                }
                else if(blackjackGameActive){
                    if     (event.key.code==sf::Keyboard::H) playerHit();
                    else if(event.key.code==sf::Keyboard::S) playerStand();
                    else if(event.key.code==sf::Keyboard::D) playerDoubleDown();
                }
                break;
            }
        }

        if(event.type==sf::Event::MouseButtonPressed &&
           event.mouseButton.button==sf::Mouse::Left &&
           currentState==GameState::MENU){
            if     (cardBox1.getGlobalBounds().contains(mpF)) currentState=GameState::BLACKJACK;
            else if(cardBox2.getGlobalBounds().contains(mpF)) currentState=GameState::ROULETTE;
            else if(cardBox3.getGlobalBounds().contains(mpF)) currentState=GameState::SLOTS;
        }

        if(event.type==sf::Event::MouseButtonPressed &&
           event.mouseButton.button==sf::Mouse::Left &&
           currentState==GameState::ROULETTE &&
           !rouletteSpinning){
            // Add chip to special bets
            if(betRedButton.getGlobalBounds().contains(mpF)){
                betAmountRed += rouletteChip;
            }
            else if(betBlackButton.getGlobalBounds().contains(mpF)){
                betAmountBlack += rouletteChip;
            }
            else if(betEvenButton.getGlobalBounds().contains(mpF)){
                betAmountEven += rouletteChip;
            }
            else if(betOddButton.getGlobalBounds().contains(mpF)){
                betAmountOdd += rouletteChip;
            }
            else {
                // Number cells: add chip to clicked number (supports multiple bets)
                for(int i=0; i<37; ++i){
                    if(rouletteGrid[i].getGlobalBounds().contains(mpF)){
                        rouletteNumberBets[i] += rouletteChip;
                        selectedRouletteNumber = i;
                        break;
                    }
                }
            }
        }

        if(event.type==sf::Event::MouseButtonPressed &&
           event.mouseButton.button==sf::Mouse::Right &&
           currentState==GameState::ROULETTE &&
           !rouletteSpinning){
            if(betRedButton.getGlobalBounds().contains(mpF)){
                betAmountRed -= rouletteChip;
                if(betAmountRed < 0) betAmountRed = 0;
            }
            else if(betBlackButton.getGlobalBounds().contains(mpF)){
                betAmountBlack -= rouletteChip;
                if(betAmountBlack < 0) betAmountBlack = 0;
            }
            else if(betEvenButton.getGlobalBounds().contains(mpF)){
                betAmountEven -= rouletteChip;
                if(betAmountEven < 0) betAmountEven = 0;
            }
            else if(betOddButton.getGlobalBounds().contains(mpF)){
                betAmountOdd -= rouletteChip;
                if(betAmountOdd < 0) betAmountOdd = 0;
            }
            else {
                for(int i=0; i<37; ++i){
                    if(rouletteGrid[i].getGlobalBounds().contains(mpF)){
                        rouletteNumberBets[i] -= rouletteChip;
                        if(rouletteNumberBets[i] < 0) rouletteNumberBets[i] = 0;
                        selectedRouletteNumber = i;
                        break;
                    }
                }
            }
        }
    }

    if(currentState==GameState::MENU){
        hoverBox1=cardBox1.getGlobalBounds().contains(mpF);
        hoverBox2=cardBox2.getGlobalBounds().contains(mpF);
        hoverBox3=cardBox3.getGlobalBounds().contains(mpF);
    } else { hoverBox1=hoverBox2=hoverBox3=false; }
}

// ═══════════════════════════════════════════════════════
//   Update
// ═══════════════════════════════════════════════════════
void Game::update(float dt)
{
    // HUD & Balance (always)
    textBalance.setString("Your Balance:  $" + std::to_string(playerBalance));
    centerTextH(textBalance,1280.f,218.f);
    if(currentState == GameState::ROULETTE){
        textHUD.setString("$  " + std::to_string(playerBalance) + "   Chip: $" + std::to_string(rouletteChip));
    } else {
        textHUD.setString("$  " + std::to_string(playerBalance));
    }

    // Menu hover
    applyHoverStyle(cardBox1,hoverBox1);
    applyHoverStyle(cardBox2,hoverBox2);
    applyHoverStyle(cardBox3,hoverBox3);
    centerTextInBox(textCard1,cardBox1);
    centerTextInBox(textCard2,cardBox2);
    centerTextInBox(textCard3,cardBox3);
    textCard1.setFillColor(hoverBox1?Color::GOLD_BRIGHT:Color::LIGHT_GRAY);
    textCard2.setFillColor(hoverBox2?Color::GOLD_BRIGHT:Color::LIGHT_GRAY);
    textCard3.setFillColor(hoverBox3?Color::GOLD_BRIGHT:Color::LIGHT_GRAY);

    // ── Slots ────────────────────────────────────
    if(currentState==GameState::SLOTS){
        if(isSpinning)  updateSpinAnimation(dt);
        if(winBlinking) updateWinBlink(dt);
        for(int i=0;i<3;++i) centerTextInBox(textSlotReel[i],slotReelBox[i]);
        textSlotBet.setString(
            "BET:  $" + std::to_string(currentBet) +
            "    BALANCE:  $" + std::to_string(playerBalance));
        // give the slot bet box a consistent width (same as cabinet) and center it
        slotBetBox.setSize({ Layout::CAB_W, 38.f });
        slotBetBox.setOrigin(Layout::CAB_W / 2.f, 0.f);
        slotBetBox.setPosition(640.f, 414.f);
        // center the text exactly in the middle of the bet box
        {
            sf::FloatRect b = textSlotBet.getLocalBounds();
            textSlotBet.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
            textSlotBet.setPosition(640.f, slotBetBox.getPosition().y + slotBetBox.getGlobalBounds().height / 2.0f);
        }
        if(!slotResultMsg.empty()){
            bool isJack=(slotResultMsg.find("JACKPOT")!=std::string::npos);
            bool isWin=(lastWin>0);
            textSlotResult.setString(slotResultMsg);
            textSlotResult.setCharacterSize(isJack?50:42);
            textSlotResult.setFillColor(
                isJack?Color::JACKPOT_TEXT:isWin?Color::WIN_TEXT:Color::LOSE_TEXT);
            // place result text directly under the slot bet box, centered at X=640
            {
                float resultY = slotBetBox.getPosition().y + slotBetBox.getGlobalBounds().height + 14.f;
                sf::FloatRect rb = textSlotResult.getLocalBounds();
                textSlotResult.setOrigin(rb.left + rb.width/2.f, rb.top + rb.height/2.f);
                textSlotResult.setPosition(640.f, resultY);
            }
        }
        textSlotControls.setFillColor(isSpinning?sf::Color(70,70,90):Color::MID_GRAY);
    }

    // ── Blackjack ────────────────────────────────
    if(currentState==GameState::BLACKJACK){
        // Bet text centered inside bet box
        textBjBet.setString(
            "BET:  $" + std::to_string(blackjackBet) +
            "    |    BALANCE:  $" + std::to_string(playerBalance));
        // center the blackjack bet text exactly in the bjBetBox
        {
            sf::FloatRect b = textBjBet.getLocalBounds();
            textBjBet.setOrigin(b.left + b.width / 2.0f, b.top + b.height / 2.0f);
            textBjBet.setPosition(640.f, bjBetBox.getPosition().y + bjBetBox.getGlobalBounds().height / 2.0f);
        }

        if(!blackjackGameActive && bjResult==BjResult::NONE){
            // Idle state
            textBjDealerScore.setString("");
            textBjPlayerScore.setString("");
        }
        else if(blackjackGameActive){
            textBjDealerScore.setString("Showing: " + std::to_string(dealerHand[0].getScoreValue()));
            textBjDealerScore.setFillColor(Color::BJ_SCORE);

            playerScore = calcHandScore(playerHand);
            bool bust = (playerScore > 21);
            textBjPlayerScore.setString(
                "Total: " + std::to_string(playerScore) + (bust ? "  BUST!" : ""));
            textBjPlayerScore.setFillColor(bust ? Color::BJ_BUST : Color::BJ_SCORE);
        }
        else{
            bool dealerBust = (dealerScore > 21);
            textBjDealerScore.setString(
                "Total: " + std::to_string(dealerScore) + (dealerBust ? "  BUST!" : ""));
            textBjDealerScore.setFillColor(dealerBust ? Color::BJ_BUST : Color::BJ_SCORE);

            playerScore = calcHandScore(playerHand);
            textBjPlayerScore.setString("Total: " + std::to_string(playerScore));
            textBjPlayerScore.setFillColor(Color::BJ_SCORE);
        }
    }

    // -- Roulette ------------------------------------
    if(currentState==GameState::ROULETTE){
        if(rouletteSpinning) updateRouletteAnimation(dt);

        // Highlight numbers that have bets or are currently touched
        for(int i=0;i<37;++i){
            bool hasBet = (rouletteNumberBets[i] > 0);
            bool selected = (selectedRouletteNumber == i);
            bool highlight = hasBet || selected;
            rouletteGrid[i].setOutlineColor(highlight ? Color::GOLD_BRIGHT : sf::Color(0,0,0,0));
            rouletteGrid[i].setOutlineThickness(highlight ? 3.f : 0.f);
        }

        auto updateButtonStyle=[&](sf::RectangleShape& btn, sf::Text& label, int type){
            int amt = (type==0?betAmountRed:(type==1?betAmountBlack:(type==2?betAmountEven:betAmountOdd)));
            bool selected = (amt > 0);
            btn.setOutlineColor(selected ? Color::GOLD_BRIGHT : sf::Color(0,0,0,0));
            btn.setOutlineThickness(selected ? 3.f : 0.f);
            btn.setFillColor(selected ? Color::CARD_BG_HOV : Color::CARD_BG);
            label.setFillColor(selected ? Color::GOLD_BRIGHT : Color::LIGHT_GRAY);
            centerTextInBox(label, btn);
        };

        updateButtonStyle(betRedButton, textRouletteBetOptions[0], 0);
        updateButtonStyle(betBlackButton, textRouletteBetOptions[1], 1);
        updateButtonStyle(betEvenButton, textRouletteBetOptions[2], 2);
        updateButtonStyle(betOddButton, textRouletteBetOptions[3], 3);

        // Show currently selected chip value
        textRouletteBet.setString("Chip: $" + std::to_string(rouletteChip));
        // Spinning state
        if(rouletteSpinning){
            textRouletteControls.setFillColor(sf::Color(70,70,90));
        } else {
            textRouletteControls.setFillColor(Color::MID_GRAY);
        }
    }
}

// ═══════════════════════════════════════════════════════
//   Rendering
// ═══════════════════════════════════════════════════════
void Game::render()
{
    switch(currentState){
    case GameState::MENU:      renderMenu();      break;
    case GameState::SLOTS:     renderSlots();     break;
    case GameState::BLACKJACK: renderBlackjack(); break;
    case GameState::ROULETTE:  renderRoulette();  break;
    default:                   renderGame();      break;
    }
    window.display();
}

void Game::renderMenu()
{
    window.clear(Color::BG_MENU);
    window.draw(textTitle); window.draw(textSubtitle);
    window.draw(titleDivider); window.draw(textBalance);
    window.draw(cardBox1); window.draw(textCard1);
    window.draw(cardBox2); window.draw(textCard2);
    window.draw(cardBox3); window.draw(textCard3);
    window.draw(textEscHint);
}

void Game::renderGame()
{
    window.clear(Color::BG_ROULETTE);
    window.draw(hudBox); window.draw(textHUD);
}

void Game::renderSlots()
{
    window.clear(Color::BG_SLOTS);
    window.draw(hudBox); window.draw(textHUD);
    window.draw(textSlotTitle);
    window.draw(slotCabinetBox);
    for(int i=0;i<3;++i){ window.draw(slotReelBox[i]); window.draw(textSlotReel[i]); }
    window.draw(slotPayline);
    window.draw(slotBetBox); window.draw(textSlotBet);
    if(!slotResultMsg.empty() && !isSpinning) window.draw(textSlotResult);
    window.draw(textSlotControls);
}

void Game::renderBlackjack()
{
    window.clear(Color::BG_BLACKJACK);

    window.draw(hudBox);
    window.draw(textHUD);
    window.draw(textBjTitle);

    // Playing field
    window.draw(bjDealerArea);
    window.draw(bjPlayerArea);
    window.draw(bjTableLine);

    // Dealer and player labels/scores
    window.draw(textBjDealerLabel);
    window.draw(textBjDealerScore);
    window.draw(textBjPlayerLabel);
    window.draw(textBjPlayerScore);

    unsigned int texW = cardsTexture.getSize().x;
    unsigned int texH = cardsTexture.getSize().y;
    bool haveCards = (cardsTexture.getNativeHandle() != 0 && texW >= 13 && texH >= 4);

    int cardW = 0;
    int cardH = 0;
    float drawW = 80.f;
    float drawH = 110.f;
    if(haveCards){
        cardW = static_cast<int>(texW / 13u);
        cardH = static_cast<int>(texH / 4u);
        if(cardW > 0 && cardH > 0){
            float scale = std::min(drawW / cardW, drawH / cardH);
            drawW = cardW * scale;
            drawH = cardH * scale;
        } else {
            haveCards = false;
        }
    }

    sf::Sprite cardSprite;
    if(haveCards) cardSprite.setTexture(cardsTexture);

    const float dealerX = BjLayout::AREA_X + 120.f;
    const float dealerY = BjLayout::DEALER_HAND_Y - 10.f;
    const float playerX = BjLayout::AREA_X + 120.f;
    const float playerY = BjLayout::PLAYER_HAND_Y - 10.f;
    const float cardGap = 12.f;

    for(size_t i=0; i<dealerHand.size(); ++i){
        float x = dealerX + i * (drawW + cardGap);
        float y = dealerY;

        if(haveCards){
            if(i == 1 && blackjackGameActive && !dealerRevealed){
                sf::RectangleShape back({drawW, drawH});
                back.setPosition(x, y);
                back.setFillColor(sf::Color(18, 40, 120));
                back.setOutlineColor(sf::Color::White);
                back.setOutlineThickness(2.f);
                window.draw(back);
            } else {
                int col = dealerHand[i].value;
                int row = dealerHand[i].suit;
                cardSprite.setTextureRect(sf::IntRect(col * cardW, row * cardH, cardW, cardH));
                cardSprite.setScale(drawW / cardW, drawH / cardH);
                cardSprite.setPosition(x, y);
                window.draw(cardSprite);
            }
        }
    }

    for(size_t i=0; i<playerHand.size(); ++i){
        float x = playerX + i * (drawW + cardGap);
        float y = playerY;

        if(haveCards){
            int col = playerHand[i].value;
            int row = playerHand[i].suit;
            cardSprite.setTextureRect(sf::IntRect(col * cardW, row * cardH, cardW, cardH));
            cardSprite.setScale(drawW / cardW, drawH / cardH);
            cardSprite.setPosition(x, y);
            window.draw(cardSprite);
        }
    }

    if(!blackjackGameActive && bjResult != BjResult::NONE)
        window.draw(textBjResult);

    // Bet strip
    window.draw(bjBetBox);
    window.draw(textBjBet);

    // Controls clearly below everything
    window.draw(textBjControls);
}

void Game::renderRoulette()
{
    window.clear(Color::BG_BLACKJACK);

    window.draw(hudBox); window.draw(textHUD);
    window.draw(textRouletteTitle);

    const float PI = 3.14159265359f;
    float centerX = 220.f;
    float centerY = 320.f;
    float wheelRadius = WHEEL_RADIUS;
    float innerRadius = 45.f;
    float outerRadius = wheelRadius;

    // Wheel background + rim
    sf::CircleShape wheelBg(wheelRadius + 18.f);
    wheelBg.setFillColor(sf::Color(24, 18, 12));
    wheelBg.setOutlineColor(sf::Color(90, 60, 20));
    wheelBg.setOutlineThickness(6.f);
    wheelBg.setOrigin(wheelRadius + 18.f, wheelRadius + 18.f);
    wheelBg.setPosition(centerX, centerY);
    window.draw(wheelBg);

    // Draw 37 segments using the wheel sequence
    const float anglePer = 2.f * PI / 37.f;
    for(int i=0;i<37;++i){
        int number = RouletteWheel::wheelSequence[i];
        float startA = i * anglePer + (wheelAngle * PI/180.f);
        float endA   = startA + anglePer;

        sf::ConvexShape seg;
        seg.setPointCount(4);
        seg.setPoint(0, sf::Vector2f(centerX + std::cos(startA) * innerRadius, centerY + std::sin(startA) * innerRadius));
        seg.setPoint(1, sf::Vector2f(centerX + std::cos(endA)   * innerRadius, centerY + std::sin(endA)   * innerRadius));
        seg.setPoint(2, sf::Vector2f(centerX + std::cos(endA)   * outerRadius, centerY + std::sin(endA)   * outerRadius));
        seg.setPoint(3, sf::Vector2f(centerX + std::cos(startA) * outerRadius, centerY + std::sin(startA) * outerRadius));
        seg.setFillColor(getRouletteNumberColor(number));
        seg.setOutlineColor(sf::Color(20,20,20));
        seg.setOutlineThickness(1.f);
        window.draw(seg);

        // Draw number near outer edge, rotated
        float midA = (startA + endA) * 0.5f;
        sf::Text ntext;
        ntext.setFont(font);
        ntext.setString(std::to_string(number));
        ntext.setCharacterSize(12);
        ntext.setFillColor(number==0?sf::Color(200,240,200):sf::Color::White);
        sf::FloatRect ntb = ntext.getLocalBounds();
        ntext.setOrigin(ntb.left + ntb.width / 2.f,
                        ntb.top  + ntb.height / 2.f);
        float tx = centerX + std::cos(midA) * (outerRadius - 14.f);
        float ty = centerY + std::sin(midA) * (outerRadius - 14.f);
        ntext.setPosition(tx, ty);
        float rotation = midA * 180.f / PI + 90.f;
        if(rotation > 90.f && rotation < 270.f) rotation += 180.f;
        ntext.setRotation(rotation);
        window.draw(ntext);
    }

    // Inner circle
    sf::CircleShape wheelInner(innerRadius - 8.f);
    wheelInner.setFillColor(sf::Color(18,30,18));
    wheelInner.setOutlineColor(sf::Color(80,80,80));
    wheelInner.setOutlineThickness(2.f);
    wheelInner.setOrigin(innerRadius - 8.f, innerRadius - 8.f);
    wheelInner.setPosition(centerX, centerY);
    window.draw(wheelInner);

    // Ball rendering using physics state
    float wheelRotRad = wheelAngle * PI/180.f;
    float ballWorldAngle = ballAngle + wheelRotRad;
    float bx = centerX + std::cos(ballWorldAngle) * ballRadiusCurrent;
    float by = centerY + std::sin(ballWorldAngle) * ballRadiusCurrent;
    sf::CircleShape ball(BALL_RADIUS);
    ball.setOrigin(BALL_RADIUS, BALL_RADIUS);
    ball.setPosition(bx, by);
    ball.setFillColor(sf::Color::White);
    ball.setOutlineColor(sf::Color::Yellow);
    ball.setOutlineThickness(2.f);
    window.draw(ball);

    // Result display: centered in the free space between the number grid and the buttons
    if(!rouletteSpinning && winningNumber >= 0){
        const float resultX = 780.f;
        const float numberY = 440.f;
        const float resultY = 490.f;

        sf::Color winningColor = (winningNumber == 0 ? sf::Color(180, 220, 180) : sf::Color::White);
        sf::FloatRect wnb = textRouletteWinningNumber.getLocalBounds();
        textRouletteWinningNumber.setOrigin(wnb.left + wnb.width / 2.0f, wnb.top + wnb.height / 2.0f);
        textRouletteWinningNumber.setPosition(resultX, numberY);
        textRouletteWinningNumber.setFillColor(winningColor);
        window.draw(textRouletteWinningNumber);

        sf::FloatRect rb = textRouletteResult.getLocalBounds();
        textRouletteResult.setOrigin(rb.left + rb.width / 2.0f, rb.top + rb.height / 2.0f);
        textRouletteResult.setPosition(resultX, resultY);
        window.draw(textRouletteResult);
    }

    // Grid mit Zahlenfeldern and bet overlays
    sf::Text gridNumberText;
    gridNumberText.setFont(font);
    gridNumberText.setCharacterSize(28);
    gridNumberText.setFillColor(sf::Color::White);
    for(int i=0; i<37; ++i){
        window.draw(rouletteGrid[i]);
        gridNumberText.setString(i == 0 ? "0" : std::to_string(i));
        centerTextInBox(gridNumberText, rouletteGrid[i]);
        window.draw(gridNumberText);
        if(rouletteNumberBets[i] > 0){
            sf::Text betText;
            betText.setFont(font);
            betText.setCharacterSize(12);
            betText.setFillColor(Color::JACKPOT_TEXT);
            betText.setStyle(sf::Text::Bold);
            betText.setString("$" + std::to_string(rouletteNumberBets[i]));
            sf::FloatRect bb = rouletteGrid[i].getGlobalBounds();
            sf::FloatRect tb2 = betText.getLocalBounds();
            betText.setOrigin(tb2.left + tb2.width, tb2.top);
            betText.setPosition(bb.left + bb.width - 4.f, bb.top + 4.f);
            window.draw(betText);
        }
    }

    // Bet options (buttons and their amounts)
    window.draw(betRedButton);
    window.draw(betBlackButton);
    window.draw(betEvenButton);
    window.draw(betOddButton);
    for(int i=0;i<4;++i) window.draw(textRouletteBetOptions[i]);
    sf::Text sb;
    sb.setFont(font); sb.setCharacterSize(14); sb.setFillColor(Color::JACKPOT_TEXT);
    if(betAmountRed > 0){ sb.setString("$" + std::to_string(betAmountRed)); centerTextInBox(sb, betRedButton); sb.move(0.f, -20.f); window.draw(sb); }
    if(betAmountBlack > 0){ sb.setString("$" + std::to_string(betAmountBlack)); centerTextInBox(sb, betBlackButton); sb.move(0.f, -20.f); window.draw(sb); }
    if(betAmountEven > 0){ sb.setString("$" + std::to_string(betAmountEven)); centerTextInBox(sb, betEvenButton); sb.move(0.f, -20.f); window.draw(sb); }
    if(betAmountOdd > 0){ sb.setString("$" + std::to_string(betAmountOdd)); centerTextInBox(sb, betOddButton); sb.move(0.f, -20.f); window.draw(sb); }

    // Controls
    window.draw(textRouletteControls);
}
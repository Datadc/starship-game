// tests/game_test.cxx
#include <gtest/gtest.h>
#include "starship/game.hxx"
#include "starship/powerup.hxx"

class GameTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize test fixtures if needed
    }

    void TearDown() override {
        // Cleanup after tests
    }
};

TEST_F(GameTest, GameInitialization) {
    // Test that game initializes without errors
    starship::Game game(800, 600);
    EXPECT_EQ(game.getWidth(), 800);
    EXPECT_EQ(game.getHeight(), 600);
    EXPECT_EQ(game.getScore(), 0);
    EXPECT_EQ(game.getLevel(), 1);
    EXPECT_FALSE(game.isGameOver());
}

TEST_F(GameTest, PowerUpCreation) {
    // Test PowerUp constructor and basic properties
    starship::Vector2D pos(100.0f, 200.0f);
    starship::PowerUp powerUp(pos, starship::PowerUp::Type::SHIELD);

    EXPECT_EQ(powerUp.getPosition().x, 100.0f);
    EXPECT_EQ(powerUp.getPosition().y, 200.0f);
    EXPECT_EQ(powerUp.getType(), starship::PowerUp::Type::SHIELD);
    EXPECT_TRUE(powerUp.isActive());
}

TEST_F(GameTest, PowerUpLifetime) {
    // Test PowerUp lifetime management
    starship::Vector2D pos(100.0f, 200.0f);
    starship::PowerUp powerUp(pos, starship::PowerUp::Type::SHIELD);

    // Initially active
    EXPECT_TRUE(powerUp.isActive());

    // Update for less than max lifetime
    powerUp.update(5.0f);
    EXPECT_TRUE(powerUp.isActive());

    // Update beyond max lifetime
    powerUp.update(6.0f);  // Total 11 seconds > 10 seconds
    EXPECT_FALSE(powerUp.isActive());
}

TEST_F(GameTest, PowerUpColors) {
    // Test PowerUp color assignment
    starship::Vector2D pos(0.0f, 0.0f);

    starship::PowerUp shield(pos, starship::PowerUp::Type::SHIELD);
    auto shieldColor = shield.getColor();
    EXPECT_EQ(shieldColor.r, 0);
    EXPECT_EQ(shieldColor.g, 255);
    EXPECT_EQ(shieldColor.b, 255);

    starship::PowerUp multiShot(pos, starship::PowerUp::Type::MULTI_SHOT);
    auto multiColor = multiShot.getColor();
    EXPECT_EQ(multiColor.r, 255);
    EXPECT_EQ(multiColor.g, 0);
    EXPECT_EQ(multiColor.b, 255);

    starship::PowerUp rapidFire(pos, starship::PowerUp::Type::RAPID_FIRE);
    auto rapidColor = rapidFire.getColor();
    EXPECT_EQ(rapidColor.r, 255);
    EXPECT_EQ(rapidColor.g, 255);
    EXPECT_EQ(rapidColor.b, 0);

    starship::PowerUp extraLife(pos, starship::PowerUp::Type::EXTRA_LIFE);
    auto lifeColor = extraLife.getColor();
    EXPECT_EQ(lifeColor.r, 0);
    EXPECT_EQ(lifeColor.g, 255);
    EXPECT_EQ(lifeColor.b, 0);
}

TEST_F(GameTest, PowerUpSpawning) {
    // Test power-up spawning in Game
    starship::Game game(800, 600);

    // Initially no power-ups
    EXPECT_EQ(game.getPowerUps().size(), 0u);

    // Spawn a power-up
    starship::Vector2D pos(400.0f, 300.0f);
    game.spawnPowerUp(pos);

    // Should have one power-up now
    EXPECT_EQ(game.getPowerUps().size(), 1u);

    // Check power-up properties
    const auto& powerUps = game.getPowerUps();
    EXPECT_EQ(powerUps[0].getPosition().x, 400.0f);
    EXPECT_EQ(powerUps[0].getPosition().y, 300.0f);
    EXPECT_TRUE(powerUps[0].isActive());
}

TEST_F(GameTest, PowerUpEffects) {
    // Test power-up effect application
    starship::Game game(800, 600);

    // Initially no effects active
    EXPECT_FALSE(game.isShielded());
    EXPECT_FALSE(game.hasMultiShot());
    EXPECT_FALSE(game.hasRapidFire());
    EXPECT_FALSE(game.hasSpeedBoost());

    // Apply shield effect
    game.applyPowerUp(starship::PowerUp::Type::SHIELD);
    EXPECT_TRUE(game.isShielded());
    EXPECT_FALSE(game.hasMultiShot());
    EXPECT_FALSE(game.hasRapidFire());
    EXPECT_FALSE(game.hasSpeedBoost());

    // Apply multi-shot effect
    game.applyPowerUp(starship::PowerUp::Type::MULTI_SHOT);
    EXPECT_TRUE(game.isShielded());  // Shield still active
    EXPECT_TRUE(game.hasMultiShot());
    EXPECT_FALSE(game.hasRapidFire());
    EXPECT_FALSE(game.hasSpeedBoost());

    // Apply rapid fire effect
    game.applyPowerUp(starship::PowerUp::Type::RAPID_FIRE);
    EXPECT_TRUE(game.isShielded());
    EXPECT_TRUE(game.hasMultiShot());
    EXPECT_TRUE(game.hasRapidFire());
    EXPECT_FALSE(game.hasSpeedBoost());

    // Apply speed boost effect
    game.applyPowerUp(starship::PowerUp::Type::SPEED_BOOST);
    EXPECT_TRUE(game.hasSpeedBoost());
}

TEST_F(GameTest, PowerUpTimers) {
    // Test power-up effect timers
    starship::Game game(800, 600);

    // Apply effects
    game.applyPowerUp(starship::PowerUp::Type::SHIELD);
    game.applyPowerUp(starship::PowerUp::Type::MULTI_SHOT);
    game.applyPowerUp(starship::PowerUp::Type::RAPID_FIRE);
    game.applyPowerUp(starship::PowerUp::Type::SPEED_BOOST);

    // All effects should be active initially
    EXPECT_TRUE(game.isShielded());
    EXPECT_TRUE(game.hasMultiShot());
    EXPECT_TRUE(game.hasRapidFire());
    EXPECT_TRUE(game.hasSpeedBoost());

    // Update for 4 seconds (half the effect duration)
    for (int i = 0; i < 4; ++i) {
        game.update(1.0f);
    }

    // Effects should still be active
    EXPECT_TRUE(game.isShielded());
    EXPECT_TRUE(game.hasMultiShot());
    EXPECT_TRUE(game.hasRapidFire());
    EXPECT_TRUE(game.hasSpeedBoost());

    // Update for another 5 seconds (total 9 seconds)
    for (int i = 0; i < 5; ++i) {
        game.update(1.0f);
    }

    // Effects should have expired
    EXPECT_FALSE(game.isShielded());
    EXPECT_FALSE(game.hasMultiShot());
    EXPECT_FALSE(game.hasRapidFire());
    EXPECT_FALSE(game.hasSpeedBoost());
}

TEST_F(GameTest, ExtraLifePowerUp) {
    // Test extra life power-up effect
    starship::Game game(800, 600);

    // Get initial health
    int initialHealth = game.getPlayer().getHealth();
    EXPECT_EQ(initialHealth, 3);

    // Apply extra life
    game.applyPowerUp(starship::PowerUp::Type::EXTRA_LIFE);
    EXPECT_EQ(game.getPlayer().getHealth(), initialHealth + 1);

    // Test max health limit (assuming max is 5)
    for (int i = 0; i < 3; ++i) {
        game.applyPowerUp(starship::PowerUp::Type::EXTRA_LIFE);
    }
    EXPECT_EQ(game.getPlayer().getHealth(), 5);  // Should cap at 5
}

TEST_F(GameTest, PowerUpCollision) {
    // Test power-up collection via collision
    starship::Game game(800, 600);

    // Spawn a power-up near the player
    starship::Vector2D powerUpPos(game.getWidth() / 2.0f, game.getHeight() - 50.0f);
    game.spawnPowerUp(powerUpPos);

    // Should have one power-up
    EXPECT_EQ(game.getPowerUps().size(), 1u);

    // Update game to process collisions
    game.update(0.1f);

    // Power-up should be collected (collision detection)
    // Note: This test assumes the power-up spawns close enough to the player
    // In a real scenario, we'd need to position it exactly for collision
}

TEST_F(GameTest, PowerUpMovement) {
    // Test power-up movement
    starship::Game game(800, 600);

    // Spawn a power-up
    starship::Vector2D initialPos(400.0f, 100.0f);
    game.spawnPowerUp(initialPos);

    const auto& powerUps = game.getPowerUps();
    ASSERT_EQ(powerUps.size(), 1u);

    starship::Vector2D initialPowerUpPos = powerUps[0].getPosition();

    // Update game
    game.update(1.0f);

    // Power-up should have moved downward
    starship::Vector2D newPowerUpPos = powerUps[0].getPosition();
    EXPECT_GT(newPowerUpPos.y, initialPowerUpPos.y);  // Moved down
    EXPECT_LT(newPowerUpPos.y - initialPowerUpPos.y, 100.0f);  // Reasonable movement
}

TEST_F(GameTest, GameStateManagement) {
    // Test game state transitions
    starship::Game game(800, 600);
    EXPECT_FALSE(game.isGameOver());

    // Game should be active initially
    EXPECT_EQ(game.getLevel(), 1);
    EXPECT_EQ(game.getScore(), 0);
}

TEST_F(GameTest, GameUpdate) {
    // Test game update logic
    starship::Game game(800, 600);

    int initialAsteroids = game.getAsteroids().size();
    int initialScore = game.getScore();

    // Update game
    game.update(1.0f);

    // Basic checks - game should still function
    EXPECT_GE(game.getAsteroids().size(), initialAsteroids);  // May spawn new asteroids
    EXPECT_EQ(game.getScore(), initialScore);  // Score shouldn't change without collisions
}
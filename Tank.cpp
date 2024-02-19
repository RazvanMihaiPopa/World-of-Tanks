// Tema2.cpp

#include "Tema2.h"
#include <iostream>

namespace m1 {
    // Default constructor (you were missing this)
    Tank::Tank() : x(0.0f), y(0.0f), z(0.0f),
        xMin(x - 3.0f / 2.0f), xMax(x + 3.0f / 2.0f),
        yMin(y), yMax(y + 1.6f / 2.0f),
        zMin(z - 2.0f / 2.0f), zMax(z + 2.0f / 2.0f),
        xMinUpper(x - 1.1f / 2.0f), xMaxUpper(x + 1.1f / 2.0f),
        yMinUpper(yMax), yMaxUpper(yMax + 1.2f / 2.0f),
        zMinUpper(z - 1.3f / 2.0f), zMaxUpper(x + 1.3f / 2.0f),
        hp(3) {}

    // Parameterized constructor
    Tank::Tank(float initialX, float initialY, float initialZ)
        : x(initialX), y(initialY), z(initialZ),
        xMin(x - 3.0f / 2.0f), xMax(x + 3.0f / 2.0f),
        yMin(y), yMax(y + 1.6f / 2.0f),
        zMin(z - 2.0f / 2.0f), zMax(z + 2.0f / 2.0f),
        xMinUpper(x - 1.1f / 2.0f), xMaxUpper(x + 1.1f / 2.0f),
        yMinUpper(yMax), yMaxUpper(yMax + 1.2f / 2.0f),
        zMinUpper(z - 1.3f / 2.0f), zMaxUpper(x + 1.3f / 2.0f) {}

    void Tank::move(float deltaTimeSeconds) {
        if (!isDestroyed) {
            if (changeTimer > 0) {
                if (movementState.compare("forward") == 0) {
                    setZ(z + deltaTimeSeconds * 2.0f * cos(angle));
                    setX(x + deltaTimeSeconds * 2.0f * sin(angle));
                }
                else if (movementState.compare("right") == 0) {
                    angle += deltaTimeSeconds * 2.0f;
                    angleCannon += deltaTimeSeconds * 2.0f;
                    if (angle > RADIANS(180)) {
                        angle -= RADIANS(360);
                    }
                    if (angleCannon > RADIANS(180)) {
                        angle -= RADIANS(360);
                    }
                }
                else if (movementState.compare("backward") == 0) {
                    setZ(z - deltaTimeSeconds * 2.0f * cos(angle));
                    setX(x - deltaTimeSeconds * 2.0f * sin(angle));
                }
                else {
                    angle -= deltaTimeSeconds * 2.0f;
                    angleCannon -= deltaTimeSeconds * 2.0f;
                    if (angle < RADIANS(-180)) {
                        angle += RADIANS(360);
                    }
                    if (angleCannon < RADIANS(-180)) {
                        angleCannon += RADIANS(360);
                    }
                }
                changeTimer -= 40.0f * deltaTimeSeconds;
            }
            else {
                if (movementState.compare("forward") == 0 || movementState.compare("backward") == 0) {
                    if (rand() % 2 == 0) {
                        movementState = "left";
                    }
                    else {
                        movementState = "right";
                    }
                    setChangeTimer(50.0f);
                } else if (movementState.compare("left") == 0 || movementState.compare("right") == 0) {
                    if (rand() % 2 == 0) {
                        movementState = "forward";
                        float secs = 2 + rand() % 2;
                        setChangeTimer(150.0f);
                    }
                    else {
                        movementState = "backward";
                        setChangeTimer(150.0f);
                    }
                }
            }
        }
    }

    void Tank::setChangeTimer(float timer) {
        changeTimer = timer;
    }

    void Tank::setX(float newX) {
        x = newX;
        xMin = x - 3.0f / 2.0f;
        xMax = x + 3.0f / 2.0f;
        xMinUpper = x - 1.6f / 2.0f;
        xMaxUpper = x + 1.6f / 2.0f;
    }

    void Tank::setY(float newY) {
        y = newY;
        yMin = 0;
        yMax = y + 1.1f;
        yMinUpper = 0;
        yMaxUpper = y + 1.1f;
    }

    void Tank::setZ(float newZ) {
        z = newZ;
        zMin = z - 1.5f;
        zMax = z + 1.5f;
        zMinUpper = z - 1.6f / 2.0f;
        zMaxUpper = z + 1.6f / 2.0f;
    }

    void Tank::setBasics(int tankIndex) {
        hp = 3;
        isDestroyed = false;
        movementState = "forward";
        isCharged = true;
        rechargeTimer = 3.0f;
        index = tankIndex;
        angle = 0;
        angleCannon = 0;
    }

    void Tank::setXYZ(float x, float y, float z) {
        setX(x);
        setY(y);
        setZ(z);
    }

    void Tank::playerInRange(Tank &t2, std::vector<Bullet> &bullets, float deltaTimeSeconds) {
        float distanceToPlayer = glm::abs(glm::distance(glm::vec3(t2.x, t2.y, t2.z),
            glm::vec3(x, y, z)));
        if (rechargeTimer > 0) {
            rechargeTimer -= deltaTimeSeconds;
        }
        else {
            isCharged = true;
            rechargeTimer = 3.5f;
        }

        // Check if player is within range
        if (distanceToPlayer < 10.0f && !isDestroyed) {
            angleCannon = atan2((t2.x - x), (t2.z - z));

            if (angleCannon < RADIANS(-180)) {
                angleCannon += RADIANS(360);
            }
            if (angleCannon > RADIANS(180)) {
                angleCannon -= RADIANS(360);
            }

            if (angleCannon >= RADIANS(-180) && angleCannon < RADIANS(-90)) {
                if ((x - t2.x) < 0 && (z - t2.z) < 0) {
                    angleCannon += RADIANS(180);
                }
            }
            if (angleCannon >= RADIANS(90) && angleCannon < RADIANS(180)) {
                if ((x - t2.x) > 0 && (t2.z - z) > 0) {
                    angleCannon += RADIANS(180);
                }
            }
            if (angleCannon > RADIANS(180)) {
                angleCannon -= RADIANS(360);
            }

            if (isCharged) {
                // Start shooting
                Bullet newBullet;
                newBullet.x = x;
                newBullet.y = y;
                newBullet.z = z;
                newBullet.angle = angleCannon; 
                newBullet.timer = 0.0f; 
                newBullet.index = index;
                bullets.push_back(newBullet);
                isCharged = false;
                rechargeTimer = 3.5f;
            }
        }
    }
}

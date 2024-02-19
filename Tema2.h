#pragma once

#include "components/simple_scene.h"
#include "lab_m1/Tema2/Tema2_camera.h"
#include <chrono>


namespace m1
{
    struct ViewportArea
    {
        ViewportArea() : x(0), y(0), width(1), height(1) {}
        ViewportArea(int x, int y, int width, int height)
            : x(x), y(y), width(width), height(height) {}
        int x;
        int y;
        int width;
        int height;
    };

    struct Bullet {
        float x, y, z;
        float angle, angleCannonUp;
        float timer;
        int index;
    };
    struct Building {
        float x, y, z;
        float scaleX, scaleY, scaleZ;
    };

    class Tank {
    public:
        float x, y, z, angle, angleCannon;
        float xMin, xMax, xMinUpper,xMaxUpper;
        float yMin, yMax, yMinUpper, yMaxUpper;
        float zMin, zMax, zMinUpper, zMaxUpper;
        int hp, index;
        bool isDestroyed, isCharged;
        std::string movementState;
        float changeTimer, rechargeTimer;

        Tank();
        Tank(float initialX, float initialY, float initialZ);

        void setX(float newX);
        void setY(float newY);
        void setZ(float newZ);
        void setChangeTimer(float timer);
        void move(float deltaTimeSeconds);
        void playerInRange(Tank &t, std::vector<Bullet> &bullets, float deltaTimeSeconds);
        void setBasics(int index);
        void setXYZ(float x, float y, float z);

    };

    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;

        void SetCameraBehindPlayer();

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        bool checkTankCollision(Tank t1, Tank t2);
        bool checkTankProjectileCollision(Tank t, Bullet b);
        void tankCollisionReaction(Tank &t1, Tank &t2);
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int hp);
        bool checkTankBuildingCollision(Tank t1, Building b);
        bool checkProjectileBuildingCollision(Building build, Bullet b);
        void tankBuildingCollisionReaction(Tank& t1, Building& b);
        void RenderTextureMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1, Texture2D* texture2);
     protected:
        implemented2::Camera2 *camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;

        // TODO(student): If you need any other class variables, define them here.
        bool orthoProjectionActive = false;
        float Z_FAR;
        float Z_NEAR;
        float right, left, bottom, top;
        float fov;
        float orthoDelta;
        float angularStepOY, angularUpperBody, cannonUpAngle;

        Tank playerTank;
        std::vector<Bullet> bullets;
        std::vector<Tank> enemies;
        std::vector<Building> buildings;
        int start;
        std::chrono::steady_clock::time_point startTime;
        std::unordered_map<std::string, Texture2D*> mapTextures;
    };
}   // namespace m1

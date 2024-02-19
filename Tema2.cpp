#include "lab_m1/Tema2/Tema2.h"

#include <vector>
#include <string>
#include <iostream>
#include "lab_m1/Tema2/transformTema3D.h"
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}

bool Tema2::checkTankCollision(Tank t1, Tank t2) {
    return (
        glm::abs(glm::length(glm::vec3(t1.x,t1.y,t1.z) - glm::vec3(t2.x, t2.y, t2.z))) < 3.0f
        );
}

bool Tema2::checkTankBuildingCollision(Tank t, Building b) {
    return (
        t.xMax >= b.x - 0.5f * b.scaleX &&
        t.xMin <= b.x + 0.5f * b.scaleX &&
        t.zMax >= b.z - 0.5f * b.scaleZ &&
        t.zMin <= b.z + 0.5f * b.scaleZ
        );
}

bool Tema2::checkProjectileBuildingCollision(Building build, Bullet b) {
    return (
        build.x + 0.5f * build.scaleX >= b.x - 0.05f * sin(b.angle) &&
        build.x - 0.5f * build.scaleX <= b.x + 0.05f * sin(b.angle) &&
        build.y + 0.5f * build.scaleY >= b.y + (0.815f - 0.05f) * sin(b.angleCannonUp) &&
        build.y - 0.5f * build.scaleY <= b.y + (0.815f + 0.05f) * sin(b.angleCannonUp) &&
        build.z + 0.5f * build.scaleZ >= b.z + (2.33f - 0.05f) * cos(b.angle) &&
        build.z - 0.5f * build.scaleZ <= b.z + (2.33f + 0.05f) * cos(b.angle)
        ); 
}

bool Tema2::checkTankProjectileCollision(Tank t, Bullet b) {
    return (
        t.xMax >= b.x - 0.05f * sin(b.angle) &&
        t.xMin <= b.x + 0.05f * sin(b.angle) &&
        t.yMax >= b.y + (0.815f - 0.05f) &&
        t.yMin <= b.y + (0.815f + 0.05f) &&
        t.zMax >= b.z + (2.33f - 0.05f) * cos(b.angle) &&
        t.zMin <= b.z + (2.33f + 0.05f) * cos(b.angle)
        );
}

void Tema2::tankBuildingCollisionReaction(Tank& t1, Building& b) {
    if (checkTankBuildingCollision(t1, b)) {
        glm::vec3 centre1 = glm::vec3(t1.x, t1.y, t1.z);
        glm::vec3 centre2 = glm::vec3(b.x, 0, b.z);
        glm::vec3 diff = centre2 - centre1;
        float distanceBetweenTanks = glm::abs(glm::length(centre2 - centre1));
        float pDistance = glm::abs(0.5f * b.scaleZ + 1.56f - distanceBetweenTanks);
      
        glm::vec3 pVector = pDistance * glm::normalize(diff);

        centre1 += pVector * -0.2f;
        t1.setX(centre1.x);
        t1.setY(centre1.y);
        t1.setZ(centre1.z);

    }
}

void Tema2::tankCollisionReaction(Tank &t1, Tank &t2) {
    if (checkTankCollision(t1, t2)) {
        glm::vec3 centre1 = glm::vec3(t1.x, t1.y, t1.z);
        glm::vec3 centre2 = glm::vec3(t2.x, t2.y, t2.z);
        glm::vec3 diff = centre2 - centre1;
        float distanceBetweenTanks = glm::abs(glm::length(centre2 - centre1));
        float pDistance = glm::abs(3.0f - distanceBetweenTanks);

        glm::vec3 pVector = pDistance * glm::normalize(diff);

        centre1 += pVector * -2.5f;
        t1.setX(centre1.x);
        t1.setY(centre1.y);
        t1.setZ(centre1.z);

        centre2 += pVector * 2.5f;
        t2.setX(centre2.x);
        t2.setY(centre2.y);
        t2.setZ(centre2.z);
    }
}

void Tema2::Init()
{
    renderCameraTarget = false;

    top = 10.f;
    bottom = .01f;
    left = .01f;
    right = 10.f;
    orthoDelta = 0.1f;
    Z_NEAR = .01f;
    Z_FAR = 200.0f;
    fov = 60.f;

    camera = new implemented2::Camera2();
    camera->Set(glm::vec3(0, 2.5f, -3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    const string sourceTextureDir = PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "textures");

    // Load textures
    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "buildingTexture.jpg").c_str(), GL_REPEAT);
        mapTextures["building"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "asfalt2.png").c_str(), GL_REPEAT);
        mapTextures["street"] = texture;
    }

    {
        Texture2D* texture = new Texture2D();
        texture->Load2D(PATH_JOIN(sourceTextureDir, "caramizi.png").c_str(), GL_REPEAT);
        mapTextures["bricks"] = texture;
    }

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("mainBody");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2"), "corpprincipal.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("upperBody");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2"), "corpdesus.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("tankTracks");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2"), "senile.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("cannon");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2"), "cannon.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("projectile");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "tema2"), "proiectil.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Shader* shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shadersT2", "vertexshader1.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shadersT2", "shader1.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    {
        Shader* shader = new Shader("TextureShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shadersT2", "vertexshader2.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shadersT2", "shader2.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    playerTank.setXYZ(0, 0, 0);
    playerTank.setBasics(0);
    cannonUpAngle = 0;

    for (int i = 0; i < 7; i++) {
        while (true) {
            bool notOk = false;
            Building b;
            int randomNr = rand() % 4;
            b.scaleX = 5.0f + rand() % 15;
            b.scaleY = 5.0f + rand() % 15;
            b.scaleZ = 5.0f + rand() % 15;
            if (randomNr % 4 == 0) {
                b.x = 20 + rand() % 30;
                b.y = b.scaleY * 0.5;
                b.z = 20 + rand() % 30;
            }
            else if (randomNr % 4 == 1) {
                b.x = 20 - rand() % 30;
                b.y = b.scaleY * 0.5;
                b.z = 20 + rand() % 30;
            }
            else if (randomNr % 4 == 2) {
                b.x = 20 + rand() % 30;
                b.y = b.scaleY * 0.5;
                b.z = 20 - rand() % 30;
            }
            else {
                b.x = 20 - rand() % 30;
                b.y = b.scaleY * 0.5;
                b.z = 20 - rand() % 30;
            }
            if (checkTankBuildingCollision(playerTank, b)) {
                notOk = true;
            }
            if (!notOk) {
                buildings.push_back(b);
                break;
            }
        }
    }

    Tank enemy;
    enemy.setBasics(1);
    enemy.setChangeTimer(0);

    for (int i = 0; i < 5; i++) {
        while (true) {
            bool notOk = false;
            if (rand() % 2 == 0) {
                enemy.setX(10 + rand() % 40);
                enemy.setY(0);
                enemy.setZ(10 + rand() % 40);
            }
            else {
                enemy.setX(-10 - rand() % 40);
                enemy.setY(0);
                enemy.setZ(-10 - rand() % 40);
            }
            for (auto &enemy2 : enemies) {
                if (checkTankCollision(enemy, enemy2)) {
                    notOk = true;
                }
            }
            for (auto& b : buildings) {
                if (checkTankBuildingCollision(enemy, b)) {
                    notOk = true;
                }
            }
            if (checkTankCollision(enemy, playerTank)) {
                notOk = true;
            }
            if (!notOk) {
                enemy.index = i + 1;
                enemies.push_back(enemy);
                break;
            }
        }
    }

    // Initialize angular steps
    angularStepOY = 0;
    angularUpperBody = 0;
    cannonUpAngle = 0;

    // TODO(student): After you implement the changing of the projection
    // parameters, remove hardcodings of these parameters
    projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, Z_NEAR, Z_FAR);
    startTime = std::chrono::steady_clock::now();
    start = 1;

}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0.7, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}

void Tema2::Update(float deltaTimeSeconds)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.01f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f));
        // TODO(student): Add or change the object colors
        RenderTextureMesh(meshes["plane"], shaders["TextureShader"], modelMatrix, mapTextures["street"], mapTextures["street"]);
    }

    std::chrono::steady_clock::time_point startTime2 = startTime;
    std::chrono::steady_clock::time_point currentTime = std::chrono::steady_clock::now();
    std::chrono::seconds elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime2);

    // Keep track of time
    if (elapsedTime.count() >= 90) {
        cout << "A trecut timpul.";
        window->Close();
    }

    // Render the buildings
    for (auto& building : buildings) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(building.x, building.y, building.z);
        modelMatrix *= transform3D::Scale(building.scaleX, building.scaleY, building.scaleZ);
        RenderTextureMesh(meshes["box"], shaders["TextureShader"], modelMatrix, mapTextures["bricks"], mapTextures["bricks"]);
    }

    // Wait for the cannon to be reloaded
    if (!playerTank.isCharged) {
        playerTank.rechargeTimer -= deltaTimeSeconds;
        if (playerTank.rechargeTimer <= 0) {
            playerTank.isCharged = true;
            playerTank.rechargeTimer = 2.5f;
        }
    }

    // Player tank
    glm::mat4 modelMatrix = glm::mat4(1);
    modelMatrix *= transform3D::Translate(playerTank.x, playerTank.y, playerTank.z);
    modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
    modelMatrix *= transform3D::RotateOY(playerTank.angle);
    RenderSimpleMesh(meshes["mainBody"], shaders["LabShader"], modelMatrix, glm::vec3(0.4, 0.8, 0.1), playerTank.hp);
    RenderSimpleMesh(meshes["tankTracks"], shaders["LabShader"], modelMatrix, glm::vec3(0.8, 0.8, 0.8), playerTank.hp);
    modelMatrix *= transform3D::RotateOY(angularUpperBody);
    RenderSimpleMesh(meshes["upperBody"], shaders["LabShader"], modelMatrix, glm::vec3(0.3, 0.5, 0), playerTank.hp);
    modelMatrix *= transform3D::RotateOX(cannonUpAngle);
    RenderSimpleMesh(meshes["cannon"], shaders["LabShader"], modelMatrix, glm::vec3(0.8, 0.8, 0.8), playerTank.hp);

    // Enemy movement and detection of player
    int goal = 0;
    for (auto& enemy : enemies) {
        if (enemy.isDestroyed) {
            goal++;
        }
        enemy.move(deltaTimeSeconds);
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(enemy.x, enemy.y, enemy.z);
        modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
        modelMatrix *= transform3D::RotateOY(enemy.angle);
        RenderSimpleMesh(meshes["mainBody"], shaders["LabShader"], modelMatrix, glm::vec3(1, 0, 0), enemy.hp);
        RenderSimpleMesh(meshes["tankTracks"], shaders["LabShader"], modelMatrix, glm::vec3(0.8, 0.8, 0.8), enemy.hp);

        enemy.playerInRange(playerTank, bullets, deltaTimeSeconds);
        modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(enemy.x, enemy.y, enemy.z);
        modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
        modelMatrix *= transform3D::RotateOY(enemy.angleCannon);
        RenderSimpleMesh(meshes["cannon"], shaders["LabShader"], modelMatrix, glm::vec3(0.8, 0.8, 0.8), enemy.hp);
        RenderSimpleMesh(meshes["upperBody"], shaders["LabShader"], modelMatrix, glm::vec3(0.8, 0, 0), enemy.hp);
    }
    if (goal == 5) {
        cout << "Ai castigat!" << endl;
    }

    // Shots are fired, checking for collisions with tanks or buildings
    for (auto& b : bullets) {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix *= transform3D::Translate(b.x, b.y, b.z);
        modelMatrix *= transform3D::Scale(0.5f, 0.5f, 0.5f);
        modelMatrix *= transform3D::RotateOY(b.angle);
        if (b.index == 0) {
            modelMatrix *= transform3D::RotateOX(b.angleCannonUp);
        }
        RenderSimpleMesh(meshes["projectile"], shaders["LabShader"], modelMatrix, glm::vec3(0,0,0), 3);

        b.z += deltaTimeSeconds * cos(b.angle) * 7.5f;
        if (b.index == 0) {
            if (b.timer < 2.5f && b.angleCannonUp < -0.14f) {
                b.y -= deltaTimeSeconds * sin(b.angleCannonUp);
            }
            else {
                b.y += deltaTimeSeconds * sin(b.angleCannonUp);
            }
        }
        else {
            b.y -= 0.5 * deltaTimeSeconds;
        }
        b.x += deltaTimeSeconds * sin(b.angle) * 7.5f;
        b.timer += 2.5f * deltaTimeSeconds;
        for (auto& enemy : enemies) {
            if (checkTankProjectileCollision(enemy, b) && enemy.hp > 0 && b.index != enemy.index) {
                enemy.hp--;
                b.y = -1.0f;
                cout << "Tank lovit." << endl;
            }
            if (enemy.hp <= 0) {
                enemy.isDestroyed = true;
            }
        }
        if (checkTankProjectileCollision(playerTank, b) && b.index != playerTank.index) {
            if (playerTank.hp > 0) {
                playerTank.hp--;
                cout << "Mai ai " << playerTank.hp << " vieti." << endl;
            }
            b.y = -1.0f;
            if (playerTank.hp <= 0) {
                playerTank.isDestroyed = true;
                cout << "Ai pierdut." << endl;
            }
        }
        for (auto& building : buildings) {
            if (checkProjectileBuildingCollision(building, b)) {
                //b.timer = 10;
                b.y = -1.0f;
                cout << "S-a lovit cladirea." << endl;
            }
        }
    }

    // Erase bullets
    for (auto it = bullets.begin(); it != bullets.end();) {
        if (it->y <= -0.825f) {
            it = bullets.erase(it);
        }
        else {
            ++it;
        }
    }

    // Check for all tank collisions
    for (int i = 0; i < enemies.size(); i++) {
        for (int j = 0; j < enemies.size(); j++) {
            if (i != j) {
                tankCollisionReaction(enemies.at(i), enemies.at(j));
            }
        }
        tankCollisionReaction(enemies.at(i), playerTank);
        for (auto& b : buildings) {
            tankBuildingCollisionReaction(enemies.at(i), b);
        }
    }
    for (auto& b : buildings) {
        tankBuildingCollisionReaction(playerTank, b);
    }

    // Render the camera target. This is useful for understanding where
    // the rotation point is, when moving in third-person camera mode.
    if (renderCameraTarget)
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    }
}


void Tema2::FrameEnd()
{
    DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
}


void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
}

void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color, int hp)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    GLint objectColorLoc = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(objectColorLoc, 1, glm::value_ptr(color));

    GLint hpLoc = glGetUniformLocation(shader->program, "hp");
    glUniform1i(hpLoc, hp);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


void Tema2::RenderTextureMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, Texture2D* texture1, Texture2D* texture2)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = camera->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix2 = projectionMatrix;
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix2));

    // TODO(student): Set any other shader uniforms that you need

    if (texture1)
    {
        // TODO(student): Do these:
        // - activate texture location 0
        glActiveTexture(GL_TEXTURE0);
        // - bind the texture1 ID
        glBindTexture(GL_TEXTURE_2D, texture1->GetTextureID());
        // - send theuniform value
        glUniform1i(glGetUniformLocation(shader->program, "texture_1"), 0);
        glUniform1i(glGetUniformLocation(shader->program, "mixtext"), 0);

    }

    if (texture2)
    {
        // TODO(student): Do these:
        // - activate texture location 1
        glActiveTexture(GL_TEXTURE1);
        // - bind the texture2 ID
        glBindTexture(GL_TEXTURE_2D, texture2->GetTextureID());
        // - send the uniform value
        glUniform1i(glGetUniformLocation(shader->program, "texture_2"), 1);
        glUniform1i(glGetUniformLocation(shader->program, "mixtext"), 1);

    }

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}

/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */

void Tema2::SetCameraBehindPlayer()
{
    float distanceBehind = 3.5f;
    float heightAbove = 2.5f;

    glm::vec3 cameraPosition;
    cameraPosition.x = playerTank.x - distanceBehind * sin(angularStepOY);
    cameraPosition.y = playerTank.y + heightAbove;
    cameraPosition.z = playerTank.z - distanceBehind * cos(angularStepOY);

    camera->Set(cameraPosition, glm::vec3(playerTank.x, 1, playerTank.z), glm::vec3(0, 1, 0));
}


void Tema2::OnInputUpdate(float deltaTime, int mods)
{
    // move the camera only if MOUSE_RIGHT button is pressed
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float cameraSpeed = 2.0f;

        if (window->KeyHold(GLFW_KEY_W)) {
            // TODO(student): Translate the camera forward
            camera->TranslateForward(deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_A)) {
            // TODO(student): Translate the camera to the left
            camera->TranslateRight(-deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_S)) {
            // TODO(student): Translate the camera backward
            camera->TranslateForward(-deltaTime * cameraSpeed);

        }

        if (window->KeyHold(GLFW_KEY_D)) {
            // TODO(student): Translate the camera to the right
            camera->TranslateRight(deltaTime * cameraSpeed);

        }

        if (window->KeyHold(GLFW_KEY_Q)) {
            // TODO(student): Translate the camera downward
            camera->TranslateUpward(-deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_E)) {
            // TODO(student): Translate the camera upward
            camera->TranslateUpward(deltaTime * cameraSpeed);
        }

        if (window->KeyHold(GLFW_KEY_1))
        {
            fov += deltaTime * cameraSpeed * 5.f;
            if (!orthoProjectionActive)
            {
                projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, Z_NEAR, Z_FAR);
            }
        }
        if (window->KeyHold(GLFW_KEY_2))
        {
            fov -= deltaTime * cameraSpeed * 5.f;
            if (!orthoProjectionActive)
            {
                projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, Z_NEAR, Z_FAR);
            }
        }
    }
    else {
        float tankSpeed = 3.0f;
        if (window->KeyHold(GLFW_KEY_W) && !playerTank.isDestroyed) {
            playerTank.setZ(playerTank.z + deltaTime * tankSpeed * cos(angularStepOY));
            playerTank.setX(playerTank.x + deltaTime * tankSpeed * sin(angularStepOY));
            SetCameraBehindPlayer();
        }

        if (window->KeyHold(GLFW_KEY_A) && !playerTank.isDestroyed) {
            // TODO(student): Translate the camera to the left
            angularStepOY += deltaTime * tankSpeed;
            playerTank.angle = angularStepOY;
            SetCameraBehindPlayer();
        }

        if (window->KeyHold(GLFW_KEY_S) && !playerTank.isDestroyed) {
            playerTank.setZ(playerTank.z - deltaTime * tankSpeed * cos(angularStepOY));
            playerTank.setX(playerTank.x - deltaTime * tankSpeed * sin(angularStepOY));
            // TODO(student): Translate the camera backward
            SetCameraBehindPlayer();

        }

        if (window->KeyHold(GLFW_KEY_D) && !playerTank.isDestroyed) {
            angularStepOY -= deltaTime * tankSpeed;
            playerTank.angle = angularStepOY;
            // TODO(student): Translate the camera to the right
            SetCameraBehindPlayer();
        }
    }

}


void Tema2::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_T)
    {
        renderCameraTarget = !renderCameraTarget;
    }

}


void Tema2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event

    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        float sensivityOX = 0.001f;
        float sensivityOY = 0.001f;

        if (window->GetSpecialKeyState() == 0) {
            renderCameraTarget = false;
            // TODO(student): Rotate the camera in first-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateFirstPerson_OX(sensivityOX * -deltaY);
            camera->RotateFirstPerson_OY(sensivityOY * -deltaX);

        }

        if (window->GetSpecialKeyState() & GLFW_MOD_CONTROL) {
            renderCameraTarget = true;
            // TODO(student): Rotate the camera in third-person mode around
            // OX and OY using `deltaX` and `deltaY`. Use the sensitivity
            // variables for setting up the rotation speed.
            camera->RotateThirdPerson_OX(sensivityOX * -deltaY);
            camera->RotateThirdPerson_OY(sensivityOY * -deltaX);

        }
    }
    angularUpperBody += 0.001f * -deltaX;
    if (cannonUpAngle + 0.001f * deltaY < 0) {
        cannonUpAngle += 0.001f * deltaY;
    }
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
    if (button == 1 && playerTank.isCharged && playerTank.hp > 0) {
        Bullet b;
        b.x = playerTank.x;
        b.y = playerTank.y;
        b.z = playerTank.z;
        b.angle = angularStepOY + angularUpperBody;
        b.angleCannonUp = cannonUpAngle;
        b.timer = 0;
        b.index = 0;
        bullets.push_back(b);
        playerTank.isCharged = false;
    }
}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Tema2::OnWindowResize(int width, int height)
{
}

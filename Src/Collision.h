#pragma once
#include "BoxCollider.h"

#define kEpsilon 1e-8f

class Collision
{
private:
    typedef struct {
        DirectX::SimpleMath::Vector2 min;
        DirectX::SimpleMath::Vector2 max;
    } AABB;
public:
    bool HasCollided(DirectX::SimpleMath::Vector2& collisionPoint)
    {
        AABB playerAABB = CreateAABB(player_collider);
        AABB terrainAABB = CreateAABB(terrain_collider);

        assert(player_collider);

        auto inside = isWithinContainer(playerAABB, terrainAABB, collisionPoint);

        if (ImGui::Begin("Collision"))
        {
            if (inside)
                ImGui::Text("object inside: True");
            else
                ImGui::Text("object inside: False");
        }

        ImGui::End();

        return !inside;
    }

    bool canMoveBack()
    {
        AABB playerAABB = CreateAABB(player_collider);
        AABB terrainAABB = CreateAABB(terrain_collider);

        return (playerAABB.min.y - terrainAABB.min.y) > kEpsilon; // bottom
    }

    bool canMoveForward()
    {
        AABB playerAABB = CreateAABB(player_collider);
        AABB terrainAABB = CreateAABB(terrain_collider);

        return (terrainAABB.max.y - playerAABB.max.y) > kEpsilon; // top
    }

    bool canMoveLeft()
    {
        AABB playerAABB = CreateAABB(player_collider);
        AABB terrainAABB = CreateAABB(terrain_collider);

        return (playerAABB.min.x - terrainAABB.min.x) > kEpsilon; // left
    }

    bool canMoveRight()
    {
        AABB playerAABB = CreateAABB(player_collider);
        AABB terrainAABB = CreateAABB(terrain_collider);

        return (terrainAABB.max.x - playerAABB.max.x) > kEpsilon; // right
    }

public:
    BoxCollider* player_collider;
    BoxCollider* terrain_collider;


private:
    Collision::AABB CreateAABB(BoxCollider* collider)
    {
        AABB aabb;
        auto position = collider->GetTransform().Translation();
        aabb.min = DirectX::SimpleMath::Vector2(position.x - collider->halfWidth, position.z - collider->halfHeight);
        aabb.max = DirectX::SimpleMath::Vector2(position.x + collider->halfWidth, position.z + collider->halfHeight);

        return aabb;
    }



    bool isWithinContainer(AABB object, AABB container, DirectX::SimpleMath::Vector2& collisionPoint)
    {
        auto position = player_collider->GetTransform().Translation();

        if ((object.min.x - container.min.x) <= kEpsilon) // left
        {
            collisionPoint.x = container.min.x;
            collisionPoint.y = object.min.y + (object.max.y - object.min.y) / 2;
            return false;
        }
        if ((container.max.x - object.max.x) <= kEpsilon) // right
        {
            collisionPoint.x = container.max.x;
            collisionPoint.y = object.min.y + (object.max.y - object.min.y) / 2;
            return false;
        }
        if ((object.min.y - container.min.y) <= kEpsilon) // bottom
        {
            collisionPoint.x = object.min.x + ((object.max.x - object.min.x) / 2);
            collisionPoint.y = container.min.y; 
            return false;
        }
        if ((container.max.y - object.max.y) <= kEpsilon) // top
        {
            collisionPoint.x = object.min.x + ((object.max.x - object.min.x) / 2); 
            collisionPoint.y = container.max.y;
            return false;
        }

        return true;
    }

    bool TestAABBOverlap(AABB a, AABB b)
    {
        float d1x = b.min.x - a.max.x;
        float d1y = b.min.y - a.max.y;
        float d2x = a.min.x - b.max.x;
        float d2y = a.min.y - b.max.y;

        if (d1x > 0.0f || d1y > 0.0f)
            return FALSE;

        if (d2x > 0.0f || d2y > 0.0f)
            return FALSE;

        return TRUE;
    }
};


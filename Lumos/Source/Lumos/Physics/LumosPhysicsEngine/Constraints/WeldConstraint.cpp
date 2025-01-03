#include "Precompiled.h"
#include "WeldConstraint.h"
#include "Physics/LumosPhysicsEngine/RigidBody3D.h"
#include "Graphics/Renderers/DebugRenderer.h"

namespace Lumos
{

    WeldConstraint::WeldConstraint(RigidBody3D* obj1, RigidBody3D* obj2)
        : m_pObj1(obj1)
        , m_pObj2(obj2)
        , m_positionOffset(obj2->GetPosition() - obj1->GetPosition())
        , m_orientation(obj2->GetOrientation())
    {
    }

    void WeldConstraint::ApplyImpulse()
    {
        LUMOS_PROFILE_FUNCTION();

        // Position
        Vec3 pos(m_positionOffset);
        pos = m_pObj1->GetOrientation() * pos;
        pos += m_pObj1->GetPosition();
        m_pObj2->SetPosition(pos);

        // Orientation
        m_pObj2->SetOrientation(m_pObj1->GetOrientation() * m_orientation);
    }

    void WeldConstraint::DebugDraw() const
    {
        Vec3 posA = m_pObj1->GetPosition();
        Vec3 posB = m_pObj2->GetPosition();

        DebugRenderer::DrawThickLine(posA, posB, 0.02f, false, Vec4(0.0f, 0.0f, 0.0f, 1.0f));
        DebugRenderer::DrawPoint(posA, 0.05f, false, Vec4(1.0f, 0.8f, 1.0f, 1.0f));
        DebugRenderer::DrawPoint(posB, 0.05f, false, Vec4(1.0f, 0.8f, 1.0f, 1.0f));
    }
}

#pragma once

#include "GeometryGenerator.h"
#include "DModel.h"

namespace hlab {

    using std::make_shared;
    // DModel에서는 Mesh Loading
    // DSkinnedMeshModel에서는 Animation Loading
    class DSkinnedMeshModel : public DModel {
    public:
        DSkinnedMeshModel(ComPtr<ID3D11Device>& device,
            ComPtr<ID3D11DeviceContext>& context,
            const string& basePath,
            const string& filename);
        DSkinnedMeshModel(ComPtr<ID3D11Device>& device,
            ComPtr<ID3D11DeviceContext>& context,
            const string& basePath,
            const string& filename,
            const string& animPath,
            const string& animFilename);
        virtual void Initialize(ComPtr<ID3D11Device>& device,
            ComPtr<ID3D11DeviceContext>& context,
            const string& basePath, const string& filename)override;

        GraphicsPSO& GetPSO(const bool wired) override {
            //defaultSolidPSO
            //skinnedSolidPSO
            return wired ? Graphics::skinnedWirePSO : Graphics::skinnedSolidPSO;
        }

        GraphicsPSO& GetReflectPSO(const bool wired) override {
            return wired ? Graphics::reflectSkinnedWirePSO
                : Graphics::reflectSkinnedSolidPSO;
        }

        GraphicsPSO& GetDepthOnlyPSO() override {
            return Graphics::depthOnlySkinnedPSO;
        }

        void InitMeshBuffers(ComPtr<ID3D11Device>& device, const MeshData& meshData,
            shared_ptr<Mesh>& newMesh) override;

        virtual void UpdateAnimation(ComPtr<ID3D11Device>& device,
            ComPtr<ID3D11DeviceContext>& context,
            string clipId, int frame, int type = 0) override;

        virtual void Render(ComPtr<ID3D11DeviceContext>& context) override;

        // SkinnedMesh는 BoundingBox를 그릴 때 Root의 Transform을 반영해야 합니다.
        // virtual void RenderWireBoundingBox(ComPtr<ID3D11DeviceContext> &context);
        // virtual void RenderWireBoundingSphere(ComPtr<ID3D11DeviceContext>
        // &context);
//        virtual void UpdatePosition(const Vector3& InDelta)
//        {
///*            m_accumulatedRootTransform =
//                Matrix::CreateTranslation(InDelta) *
//                m_accumulatedRootTransform;    */       
//            m_accumulatedRootTransform =
//                Matrix::CreateTranslation(InDelta) *
//                m_accumulatedRootTransform;
//        }

        //virtual void UpdatePosition(const Vector3& InDelta);
        //virtual void UpdateRotation(const Matrix& InDelta);
        void IntegrateRootTransformToWorldTransform(ComPtr<ID3D11DeviceContext>& context);
        virtual void UpdateVelocity(float dt) 
        {
            // 루트모션
            Vector3 prevPos = m_prevRootTransform.Translation();
            Vector3 curPos = m_accumulatedRootTransform.Translation();

            m_velocity = (curPos - prevPos).Length() / dt;
            m_prevRootTransform = m_accumulatedRootTransform;
        }
    public:
        // ConstantBuffer<SkinnedConsts> m_skinnedConsts;
        StructuredBuffer<Matrix> m_boneTransforms;
        AnimationData* m_aniData = nullptr;
        float m_velocity = 0.0f;
        Matrix m_prevRootTransform;
        Matrix m_accumulatedRootTransform;
        Matrix m_accumulatedRootTransformToLocal;

    };

} // namespace hlab
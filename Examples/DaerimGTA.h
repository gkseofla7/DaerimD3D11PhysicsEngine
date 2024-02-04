#pragma once
#include "AppBase.h"
#include "BillboardModel.h"
#include "SkinnedMeshModel.h"
#include "SkeletalMeshActor.h"
#include "DaerimsEngineBase.h"
namespace hlab
{

class DaerimGTA : public AppBase
{
public:
	DaerimGTA();
       ~DaerimGTA()
	{
    }
	bool InitScene() override;
	void InitPhysics(bool interactive);
	void UpdateLights(float dt) override;
	void UpdateGUI() override;
	void Update(float dt) override;
	void Render() override;

	void StepSimulation(float deltaTime);
	void CreateStack(const btTransform& t, int numStacks, int numSlices,
		btScalar halfExtent);
	btRigidBody* CreateDynamic(const btTransform& t,
		btCollisionShape* shape,
		const btVector3& velocity);
public:
	vector<shared_ptr<Model>>
		m_objects; // ���� ������ ����ȭ ������ �� ��� TODO: actor list�� ����
	shared_ptr<BillboardModel> m_fireball;
	shared_ptr<SkeletalMeshActor> m_character;


	btScalar stackZ = 10.0f;

	float m_simToRenderScale = 0.01f; // �ùķ��̼� ��ü�� �ʹ� ������ �Ҿ���
	};
}
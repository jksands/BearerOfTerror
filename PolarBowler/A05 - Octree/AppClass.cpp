#include "AppClass.h"
using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUpward(
		vector3(0.0f, 0.0f, 100.0f), //Position
		vector3(0.0f, 0.0f, 99.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	//bear
	//m_pBear = new Model();
	//m_pBear->Load("PB\\TexturedBear.obj");
	//m_pBearRB = new MyRigidBody(m_pBear->GetVertexList());

	m_pEntityMngr->AddEntity("PB\\TexturedBear.obj", "Bear");
	m_pEntityMngr->UsePhysicsSolver();

#ifdef DEBUG
	uint uInstances = 1;
	uint uSteves = 500;
#else
	uint uInstances = 1849;
#endif
	/*
	int nSquare = static_cast<int>(std::sqrt(uInstances));
	m_uObjects = nSquare * nSquare;
	uint uIndex = -1;
	for (int i = 0; i < nSquare; i++)
	{
		for (int j = 0; j < nSquare; j++)
		{
			uIndex++;
			m_pEntityMngr->AddEntity("PB\\TexturedBear.obj");
			vector3 v3Position = vector3(0.0f,0.0f, 40.0f);
			matrix4 m4Position = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Position);
		}
	}*/
	m_v3Bear = vector3(0.0f, 0.0f, 40.0f);
	for (int i = 0; i < uSteves; i++)
	{
		m_uObjects++;
		m_pEntityMngr->AddEntity("Minecraft\\Steve.obj");
		vector2 temp = vector2(glm::linearRand(-100,100), glm::linearRand(-100, 100));
		vector3 v3Position = vector3(temp.x, 0.0f, temp.y);
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position);
		m_pEntityMngr->UsePhysicsSolver();
	}
	m_uOctantLevels = 0;
	m_pEntityMngr->Update();
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Set model matrix to the bear
	//matrix4 mBear = glm::translate(m_v3Bear) * ToMatrix4(m_qBear) * ToMatrix4(m_qArcBall);
	//m_pBear->SetModelMatrix(mBear);
	//m_pBearRB->SetModelMatrix(mBear);

	//check for collisions -- FOR DEBUGGING
	//for (int i = 0; i < m_pEntityMngr->GetEntityCount(); i++)
	//{
	//	m_pBearRB->IsColliding(m_pEntityMngr->GetEntity(i)->GetRigidBody());
	//}

	//Render the bear
	//m_pBear->AddToRenderList();
	//m_pBearRB->AddToRenderList();
	
	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	//display octree
	m_pRoot->Display();
	
	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();
	
	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	
	//draw gui,
	DrawGUI();
	
	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//Release MyEntityManager
	MyEntityManager::ReleaseInstance();
	//SafeDelete(m_pBear);
	//SafeDelete(m_pBearRB);

	//release GUI
	ShutdownGUI();
}
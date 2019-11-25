#include "AppClass.h"
using namespace Simplex;
std::vector<MySolver*> Application::collided;
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
		vector2 temp = vector2(glm::linearRand(-m_fHalfWidth, m_fHalfWidth), glm::linearRand(-m_fHalfWidth, m_fHalfWidth));
		vector3 v3Position = vector3(temp.x, 0.0f, temp.y);
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position);
		m_pEntityMngr->UsePhysicsSolver();
	}

	// add the planes as Entities with their obj file

	// back wall
	m_pEntityMngr->AddEntity("PB\\PlaneExport.obj");
	matrix4 m4Position = glm::scale(glm::translate(vector3(0.0f, 0.0f, -m_fHalfWidth)), vector3(200.0f, 30.0f, 1.0f));
	m_pEntityMngr->SetModelMatrix(m4Position);
	m_pEntityMngr->UsePhysicsSolver();

	// front wall
	m_pEntityMngr->AddEntity("PB\\PlaneExport.obj");
	m4Position = glm::scale(glm::translate(vector3(0.0f, 0.0f, m_fHalfWidth)), vector3(200.0f, 30.0f, 1.0f));
	m_pEntityMngr->SetModelMatrix(m4Position);
	m_pEntityMngr->UsePhysicsSolver();

	// right wall
	m_pEntityMngr->AddEntity("PB\\PlaneExport.obj");
	m4Position = glm::rotate(glm::scale(glm::translate(vector3(m_fHalfWidth, 0.0f, 0.0f)), vector3(20000.0f, 30.0f, 1.0f)), (float)(PI / 2.0f), AXIS_Y); // need a rotate
	// m4Position = glm::scale(glm::rotate(glm::translate(vector3(m_fHalfWidth, 0.0f, 0.0f)), (float)(PI / 2.0f), AXIS_Y), vector3(1.0f, 30.0f, 200.0f));
	// m4Position = glm::scale(glm::translate(glm::rotate(IDENTITY_M4, (float)(PI / 2.0f), AXIS_Y), vector3(0.0f, 0.0f, m_fHalfWidth)), vector3(200.0f, 30.0f, 1.0f));
	m_pEntityMngr->SetModelMatrix(m4Position);
	m_pEntityMngr->UsePhysicsSolver();
	
	// left wall
	m_pEntityMngr->AddEntity("PB\\PlaneExport.obj");
	m4Position = glm::rotate(glm::scale(glm::translate(vector3(-m_fHalfWidth, 0.0f, 0.0f)), vector3(20000.0f, 30.0f, 1.0f)), (float)(PI / 2.0f), AXIS_Y); // need a rotate
	// m4Position = glm::scale(glm::rotate(glm::translate(vector3(m_fHalfWidth, 0.0f, 0.0f)), (float)(PI / 2.0f), AXIS_Y), vector3(1.0f, 30.0f, 200.0f));
	// m4Position = glm::scale(glm::translate(glm::rotate(IDENTITY_M4, (float)(PI / 2.0f), AXIS_Y), vector3(0.0f, 0.0f, m_fHalfWidth)), vector3(200.0f, 30.0f, 1.0f));
	m_pEntityMngr->SetModelMatrix(m4Position);
	m_pEntityMngr->UsePhysicsSolver();

	// // floor
	// m_pEntityMngr->AddEntity("PB\\planeObj.obj");
	// vector3 v3Position = vector3(0.0f, 0.0f, 0.0f);
	// matrix4 m4Position = glm::scale(glm::translate(vector3(0.0f, 0.0f, -m_fHalfWidth)), vector3(200.0f, 30.0f, 1.0f));
	// m_pEntityMngr->SetModelMatrix(m4Position);
	// m_pEntityMngr->UsePhysicsSolver();



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

	//for each element in our collided box, reassign their id, and remove from the list if they are no longer moving
	// std::vector<uint> toRemove;
	counter++;
	for (uint i = 0; i < collided.size(); i++)
	{
		// if (collided[i]->GetVelocity() == vector3(0))
		// {
		// 	for (int j = i; j < collided.size()- 1; j++)
		// 	{
		// 		if (j == collided.size() - 2)
		// 			continue;
		// 		// collided[j] = collided[j + 1];
		// 	}
		// 	// i--;
		// 	// collided.pop_back();
		// 	// toRemove.push_back(i);
		// }
		if (collided.size() > 0)
		{
			//m_pRoot->ReassignIDtoEntity(collided[i]->GetIDFromRigid());
		}
	}
	m_pRoot->AssignIDtoEntity();
	if (counter > 120)
	{
		collided.clear();
		MyEntity* temp = m_pEntityMngr->GetEntity(m_pEntityMngr->GetEntityIndex("Bear"));
		collided.push_back((*temp).GetSolver());
		counter = 0;
	}
	// for (uint i = 0; i < toRemove.size(); i++)
	// {
	// 	collided.erase(collided.begin() + toRemove[i]);
	// }
	
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
	//m_pRoot->Display();

	// cubes for the rooms

	// m_pMeshMngr->AddCubeToRenderList(IDENTITY_M4, glm::rgbColor(vector3(255, 0, 0))); // floor
	// m_pMeshMngr->AddPlaneToRenderList(glm::scale(glm::translate(IDENTITY_M4, vector3(0.0f, 0.0f, -m_fHalfWidth)), vector3(2.0f * m_fHalfWidth, 25.0f, 0.0f)), vector3(255.0f, 0.0f, 0.0f));// back wall
	// m_pMeshMngr->AddPlaneToRenderList(glm::rotate(glm::scale(glm::translate(IDENTITY_M4,vector3(0.0f, 0.0f, m_fHalfWidth)), vector3(2.0f * m_fHalfWidth, 25.0f, 0.0f)), (float)(PI), AXIS_Y), vector3(255.0f, 0.0f, 0.0f));// front wall 
	// m_pMeshMngr->AddPlaneToRenderList(glm::scale(glm::translate(glm::rotate(IDENTITY_M4, (float)(PI/2.0f), AXIS_Y), vector3(0.0f, 0.0f, -m_fHalfWidth)), vector3(2.0f * m_fHalfWidth, 25.0f, 0.0f)), vector3(0.0f, 255.0f, 0.0f)); // left wall
	// m_pMeshMngr->AddPlaneToRenderList(glm::scale(glm::translate(glm::rotate(IDENTITY_M4, (float)(-PI / 2.0f), AXIS_Y), vector3(0.0f, 0.0f, -m_fHalfWidth)), vector3(2.0f * m_fHalfWidth, 25.0f, 0.0f)), vector3(0.0f, 0.0f, 255.0f)); // right wall
	m_pMeshMngr->AddPlaneToRenderList(glm::scale(glm::rotate(IDENTITY_M4, (float)(-PI / 2.0f), AXIS_X), vector3(2.0f * m_fHalfWidth, 2.0f * m_fHalfWidth, 0.0f)), vector3(255.0f)); // floor
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
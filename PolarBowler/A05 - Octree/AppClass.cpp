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

#ifdef DEBUG
	uint uInstances = 1;
	uint uSteves = 100;
#else
	uint uInstances = 1849;
#endif
	int nSquare = static_cast<int>(std::sqrt(uInstances));
	m_uObjects = nSquare * nSquare;
	uint uIndex = -1;
	for (int i = 0; i < nSquare; i++)
	{
		for (int j = 0; j < nSquare; j++)
		{
			uIndex++;
			m_pEntityMngr->AddEntity("PB\\Bear.obj");
			vector3 v3Position = vector3(0.0f,0.0f, 40.0f);
			matrix4 m4Position = glm::translate(v3Position);
			m_pEntityMngr->SetModelMatrix(m4Position);
		}
	}
	for (int i = 0; i < uSteves; i++)
	{

		m_pEntityMngr->AddEntity("Minecraft\\Steve.obj");
		vector2 temp = vector2(glm::linearRand(-34,34), glm::linearRand(-34,34));
		vector3 v3Position = vector3(temp.x, 0.0f, temp.y);
		matrix4 m4Position = glm::translate(v3Position);
		m_pEntityMngr->SetModelMatrix(m4Position);
	}


	m_uOctantLevels = 1;
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
	m_pMeshMngr->AddPlaneToRenderList(glm::scale(glm::translate(IDENTITY_M4, vector3(0.0f, 0.0f, -35.0f)), vector3(90.0f, 25.0f, 0.0f)), vector3(255.0f, 0.0f, 0.0f));// back wall
	m_pMeshMngr->AddPlaneToRenderList(glm::rotate(glm::scale(glm::translate(IDENTITY_M4,vector3(0.0f, 0.0f, 50.0f)), vector3(90.0f, 25.0f, 0.0f)), (float)(PI), AXIS_Y), vector3(255.0f, 0.0f, 0.0f));// front wall 
	m_pMeshMngr->AddPlaneToRenderList(glm::scale(glm::translate(glm::rotate(IDENTITY_M4, (float)(PI/2.0f), AXIS_Y), vector3(-5.0f, 0.0f, -40.0f)), vector3(90.0f, 25.0f, 0.0f)), vector3(0.0f, 255.0f, 0.0f)); // left wall
	m_pMeshMngr->AddPlaneToRenderList(glm::scale(glm::translate(glm::rotate(IDENTITY_M4, (float)(-PI / 2.0f), AXIS_Y), vector3(5.0f, 0.0f, -40.0f)), vector3(90.0f, 25.0f, 0.0f)), vector3(0.0f, 0.0f, 255.0f)); // right wall
	m_pMeshMngr->AddPlaneToRenderList(glm::scale(glm::rotate(IDENTITY_M4, (float)(-PI / 2.0f), AXIS_X), vector3(90.0f, 100.0f, 0.0f)), vector3(0.0f, 0.0f, 0.0f)); // floor
	
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
	//release GUI
	ShutdownGUI();
}
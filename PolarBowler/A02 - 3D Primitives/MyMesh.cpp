#include "MyMesh.h"

void MyMesh::Init(void)
{
	m_bBinded = false;
	m_uVertexCount = 0;

	m_VAO = 0;
	m_VBO = 0;

	m_pShaderMngr = ShaderManager::GetInstance();
}
void MyMesh::Release(void)
{
	m_pShaderMngr = nullptr;

	if (m_VBO > 0)
		glDeleteBuffers(1, &m_VBO);

	if (m_VAO > 0)
		glDeleteVertexArrays(1, &m_VAO);

	m_lVertex.clear();
	m_lVertexPos.clear();
	m_lVertexCol.clear();
}
MyMesh::MyMesh()
{
	Init();
}
MyMesh::~MyMesh() { Release(); }
MyMesh::MyMesh(MyMesh& other)
{
	m_bBinded = other.m_bBinded;

	m_pShaderMngr = other.m_pShaderMngr;

	m_uVertexCount = other.m_uVertexCount;

	m_VAO = other.m_VAO;
	m_VBO = other.m_VBO;
}
MyMesh& MyMesh::operator=(MyMesh& other)
{
	if (this != &other)
	{
		Release();
		Init();
		MyMesh temp(other);
		Swap(temp);
	}
	return *this;
}
void MyMesh::Swap(MyMesh& other)
{
	std::swap(m_bBinded, other.m_bBinded);
	std::swap(m_uVertexCount, other.m_uVertexCount);

	std::swap(m_VAO, other.m_VAO);
	std::swap(m_VBO, other.m_VBO);

	std::swap(m_lVertex, other.m_lVertex);
	std::swap(m_lVertexPos, other.m_lVertexPos);
	std::swap(m_lVertexCol, other.m_lVertexCol);

	std::swap(m_pShaderMngr, other.m_pShaderMngr);
}
void MyMesh::CompleteMesh(vector3 a_v3Color)
{
	uint uColorCount = m_lVertexCol.size();
	for (uint i = uColorCount; i < m_uVertexCount; ++i)
	{
		m_lVertexCol.push_back(a_v3Color);
	}
}
void MyMesh::AddVertexPosition(vector3 a_v3Input)
{
	m_lVertexPos.push_back(a_v3Input);
	m_uVertexCount = m_lVertexPos.size();
}
void MyMesh::AddVertexColor(vector3 a_v3Input)
{
	m_lVertexCol.push_back(a_v3Input);
}
void MyMesh::CompileOpenGL3X(void)
{
	if (m_bBinded)
		return;

	if (m_uVertexCount == 0)
		return;

	CompleteMesh();

	for (uint i = 0; i < m_uVertexCount; i++)
	{
		//Position
		m_lVertex.push_back(m_lVertexPos[i]);
		//Color
		m_lVertex.push_back(m_lVertexCol[i]);
	}
	glGenVertexArrays(1, &m_VAO);//Generate vertex array object
	glGenBuffers(1, &m_VBO);//Generate Vertex Buffered Object

	glBindVertexArray(m_VAO);//Bind the VAO
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);//Bind the VBO
	glBufferData(GL_ARRAY_BUFFER, m_uVertexCount * 2 * sizeof(vector3), &m_lVertex[0], GL_STATIC_DRAW);//Generate space for the VBO

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)0);

	// Color attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(vector3), (GLvoid*)(1 * sizeof(vector3)));

	m_bBinded = true;

	glBindVertexArray(0); // Unbind VAO
}
void MyMesh::Render(matrix4 a_mProjection, matrix4 a_mView, matrix4 a_mModel)
{
	// Use the buffer and shader
	GLuint nShader = m_pShaderMngr->GetShaderID("Basic");
	glUseProgram(nShader); 

	//Bind the VAO of this object
	glBindVertexArray(m_VAO);

	// Get the GPU variables by their name and hook them to CPU variables
	GLuint MVP = glGetUniformLocation(nShader, "MVP");
	GLuint wire = glGetUniformLocation(nShader, "wire");

	//Final Projection of the Camera
	matrix4 m4MVP = a_mProjection * a_mView * a_mModel;
	glUniformMatrix4fv(MVP, 1, GL_FALSE, glm::value_ptr(m4MVP));
	
	//Solid
	glUniform3f(wire, -1.0f, -1.0f, -1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);  

	//Wire
	glUniform3f(wire, 1.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_POLYGON_OFFSET_LINE);
	glPolygonOffset(-1.f, -1.f);
	glDrawArrays(GL_TRIANGLES, 0, m_uVertexCount);
	glDisable(GL_POLYGON_OFFSET_LINE);

	glBindVertexArray(0);// Unbind VAO so it does not get in the way of other objects
}
void MyMesh::AddTri(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft)
{
	//C
	//| \
	//A--B
	//This will make the triangle A->B->C 
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);
}
void MyMesh::AddQuad(vector3 a_vBottomLeft, vector3 a_vBottomRight, vector3 a_vTopLeft, vector3 a_vTopRight)
{
	//C--D
	//|  |
	//A--B
	//This will make the triangle A->B->C and then the triangle C->B->D
	AddVertexPosition(a_vBottomLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopLeft);

	AddVertexPosition(a_vTopLeft);
	AddVertexPosition(a_vBottomRight);
	AddVertexPosition(a_vTopRight);
}
void MyMesh::GenerateCube(float a_fSize, vector3 a_v3Color)
{
	if (a_fSize < 0.01f)
		a_fSize = 0.01f;

	Release();
	Init();

	float fValue = a_fSize * 0.5f;
	//3--2
	//|  |
	//0--1

	vector3 point0(-fValue,-fValue, fValue); //0
	vector3 point1( fValue,-fValue, fValue); //1
	vector3 point2( fValue, fValue, fValue); //2
	vector3 point3(-fValue, fValue, fValue); //3

	vector3 point4(-fValue,-fValue,-fValue); //4
	vector3 point5( fValue,-fValue,-fValue); //5
	vector3 point6( fValue, fValue,-fValue); //6
	vector3 point7(-fValue, fValue,-fValue); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCuboid(vector3 a_v3Dimensions, vector3 a_v3Color)
{
	Release();
	Init();

	vector3 v3Value = a_v3Dimensions * 0.5f;
	//3--2
	//|  |
	//0--1
	vector3 point0(-v3Value.x, -v3Value.y, v3Value.z); //0
	vector3 point1(v3Value.x, -v3Value.y, v3Value.z); //1
	vector3 point2(v3Value.x, v3Value.y, v3Value.z); //2
	vector3 point3(-v3Value.x, v3Value.y, v3Value.z); //3

	vector3 point4(-v3Value.x, -v3Value.y, -v3Value.z); //4
	vector3 point5(v3Value.x, -v3Value.y, -v3Value.z); //5
	vector3 point6(v3Value.x, v3Value.y, -v3Value.z); //6
	vector3 point7(-v3Value.x, v3Value.y, -v3Value.z); //7

	//F
	AddQuad(point0, point1, point3, point2);

	//B
	AddQuad(point5, point4, point6, point7);

	//L
	AddQuad(point4, point0, point7, point3);

	//R
	AddQuad(point1, point5, point2, point6);

	//U
	AddQuad(point3, point2, point7, point6);

	//D
	AddQuad(point4, point5, point0, point1);

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
std::vector<vector3> MyMesh::GenerateCircle(float a_fRadius, int a_nSubdivisions, vector3 centerPos, bool flipped, vector3 a_v3Color)
{

	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	/*
		Calculate a_nSubdivisions number of points around a center point in a radial manner
		then call the AddTri function to generate a_nSubdivision number of faces
	*/

	// Stores calculated positions
	// std::vector<vector3> calcedPositions;
	calcedPositions.clear();


	// Defines the angle in RADIANS (I had it in degrees for the longest time, which had WONKY output
	float angleVal = 2 * PI / a_nSubdivisions;

	// This calculates all of the positions.  It ends up calculating the first pos twice to prevent
	// nullrefexception in the next loop.  Could be fixed through hardcoding
	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		// Calculate individual positions through trig
		calcedPositions.push_back(
			vector3(a_fRadius * cosf(angleVal * i),
				a_fRadius * sinf(angleVal * i),
				centerPos.z)
		);
	}

	// Iterate through amount of positions generated (-1 so it doesn't null ref excep)
	for (int i = 0; i < calcedPositions.size() - 1; i++)
	{
		// Add a triangle using points that have already been calculated.  Many points are shared so extra calculation is mitigated
		if (flipped)
		{
			AddTri(centerPos,
				calcedPositions[i],
				calcedPositions[i + 1]
			);
		}
		else
		{
			AddTri(centerPos,
				calcedPositions[i + 1],
				calcedPositions[i]
			);
		}
	}
	return calcedPositions;
}
std::vector<vector3> MyMesh::GenerateParametricCircle(float a_fRadius, int a_nSubdivisions, vector3 centerPos, float angleOfRotation, vector3 a_v3Color)
{
	std::vector<vector3> circlePoints;

	float stackAngle = 2 * PI / a_nSubdivisions;

	for (int i = 0; i <= a_nSubdivisions; i++)
	{
		circlePoints.push_back(vector3(
			// X coordinate found parametrically.  This generates a subdivision amount of points between the two tips
			(centerPos.x +  (a_fRadius * cosf((stackAngle * i))) * cosf(angleOfRotation)),
			// Y coordinate.  Always the same across all vertical circles
			centerPos.y + (a_fRadius * sinf((stackAngle * i))),
			// Z coordinate found parametrically.  This generates a subdivision amount of points between the two tips
			centerPos.z + ((a_fRadius * cosf((stackAngle * i))) * sinf(angleOfRotation))
		));
	}
	return circlePoints;
}
void MyMesh::GenerateCone(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	vector3 tip(0.0f, 0.0f, a_fHeight / 2);
	// Replace this with your code
	// Generates a circle WITHOUT completing the mesh!
	std::vector<vector3> circle1 = GenerateCircle(a_fRadius, a_nSubdivisions, vector3(0.0f,0.0f, -a_fHeight / 2), false, a_v3Color);
	// Iterate through amount of positions generated (-1 so it doesn't null ref excep)
	for (int i = 0; i < circle1.size() - 1; i++)
	{
		// Add triangles using the circle's generated points as well as the tip
		AddTri(
			calcedPositions[i],
			calcedPositions[i + 1],
			tip

		);
	}

	

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateCylinder(float a_fRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Top and bottom of the cylinder
	std::vector<vector3> circle1 = GenerateCircle(a_fRadius, a_nSubdivisions, vector3(0.0f, 0.0f, a_fHeight / 2), true, a_v3Color);
	std::vector<vector3> circle2 = GenerateCircle(a_fRadius, a_nSubdivisions, vector3(0.0f, 0.0f, -a_fHeight / 2), false, a_v3Color);
	
	for (int i = 0; i < circle1.size() - 1; i++)
	{
		// Simply add quads using the previously generated points
		AddQuad(circle2[i], circle2[i + 1], circle1[i], circle1[i + 1]);
	}
	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTube(float a_fOuterRadius, float a_fInnerRadius, float a_fHeight, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_fHeight < 0.01f)
		a_fHeight = 0.01f;

	if (a_nSubdivisions < 3)
		a_nSubdivisions = 3;
	if (a_nSubdivisions > 360)
		a_nSubdivisions = 360;

	Release();
	Init();

	// Generates the outer points for the top and bottm
	std::vector<vector3> circle1I = GenerateCircle(a_fInnerRadius, a_nSubdivisions, vector3(0.0f, 0.0f, a_fHeight / 2), true, a_v3Color);
	std::vector<vector3> circle1O = GenerateCircle(a_fOuterRadius, a_nSubdivisions, vector3(0.0f, 0.0f, a_fHeight / 2), true, a_v3Color);
	std::vector<vector3> circle2I = GenerateCircle(a_fInnerRadius, a_nSubdivisions, vector3(0.0f, 0.0f, -a_fHeight / 2), true, a_v3Color);
	std::vector<vector3> circle2O = GenerateCircle(a_fOuterRadius, a_nSubdivisions, vector3(0.0f, 0.0f, -a_fHeight / 2), true, a_v3Color);
	m_lVertexPos.clear();
	m_uVertexCount = 0;

	for (int i = 0; i < a_nSubdivisions; i++)
	{
		// Top
		AddQuad(circle1O[i], circle1O[i + 1], circle1I[i], circle1I[i + 1]);
		// Bottom
		AddQuad(circle2I[i], circle2I[i + 1], circle2O[i], circle2O[i + 1]);
		// Outside
		AddQuad(circle2O[i], circle2O[i + 1], circle1O[i], circle1O[i + 1]);
		// Inside
		AddQuad(circle1I[i], circle1I[i + 1], circle2I[i], circle2I[i + 1]);
	}

	// -------------------------------

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
void MyMesh::GenerateTorus(float a_fOuterRadius, float a_fInnerRadius, int a_nSubdivisionsA, int a_nSubdivisionsB, vector3 a_v3Color)
{
	if (a_fOuterRadius < 0.01f)
		a_fOuterRadius = 0.01f;

	if (a_fInnerRadius < 0.005f)
		a_fInnerRadius = 0.005f;

	if (a_fInnerRadius > a_fOuterRadius)
		std::swap(a_fInnerRadius, a_fOuterRadius);

	if (a_nSubdivisionsA < 3)
		a_nSubdivisionsA = 3;
	if (a_nSubdivisionsA > 360)
		a_nSubdivisionsA = 360;

	if (a_nSubdivisionsB < 3)
		a_nSubdivisionsB = 3;
	if (a_nSubdivisionsB > 360)
		a_nSubdivisionsB = 360;

	Release();
	Init();

	// Holds all circles generated
	std::vector<std::vector<vector3>> circles;
	// An individual circle
	std::vector<vector3> circle;
	// Calculates the radius of a single circle ONCE
	float a_fRadius = a_fInnerRadius + ((a_fOuterRadius - a_fInnerRadius) / 2);
	// Stores the current angle value
	float angleVal;
	// Stores the center of a circle
	vector3 center;
	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		// Calculates the angle val based on current iteration
		angleVal = 2 * PI / a_nSubdivisionsB * i;
		// Calculates the center based on current iteration
		center = vector3(a_fRadius * cosf(angleVal), 0.0f, a_fRadius * sinf(angleVal));

		// Generates a circle based on current iteration.
		// Generates it at angleval angle and also rotates it by that angle.  Generates it such that all circles are equidistant from the center
		circle = GenerateParametricCircle((a_fOuterRadius - a_fInnerRadius) / 2, a_nSubdivisionsB, center,
			angleVal, a_v3Color);

		// Add the circle to the list
		circles.push_back(circle);
	}

	for (int i = 0; i < a_nSubdivisionsA; i++)
	{
		// Add quads using points generated earlier
		if (i == circles.size() - 1)
		{
			for (int j = 0; j < a_nSubdivisionsB; j++)
			{
				AddQuad(circles[0][j], circles[i][j], circles[0][j + 1], circles[i][j + 1]);
			}
		}
		else
		{
			for (int j = 0; j < a_nSubdivisionsB; j++)
			{
				AddQuad(circles[i + 1][j], circles[i][j], circles[i + 1][j + 1], circles[i][j + 1]);
			}
		}
	}
	
	// -------------------------------
	
	// Adding information about color
	
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}

// My implementation of this method generates a set number of circles and rotates them around the Y axis
// Therefore, lower divisions look no sphere-like, but once you get past 10 it looks pretty great
void MyMesh::GenerateSphere(float a_fRadius, int a_nSubdivisions, vector3 a_v3Color)
{
	if (a_fRadius < 0.01f)
		a_fRadius = 0.01f;

	//Sets minimum and maximum of subdivisions
	if (a_nSubdivisions < 1)
	{
		GenerateCube(a_fRadius * 2.0f, a_v3Color);
		return;
	}
	if (a_nSubdivisions > 20)
		a_nSubdivisions = 20;

	Release();
	Init();

	// Angle of rotation around Y-axis
	float angleRot = 2 * PI / a_nSubdivisions;
	// Angle of rotation for creating the vert
	float stackAngle = PI / a_nSubdivisions;

	// This is the top point of the sphere.  Used to generate tris rather than quads for the top of the sphere
	vector3 topTip(0.0f, a_fRadius, 0.0f);
	// Ditto but for the bottom
	vector3 botTip(0.0f, -a_fRadius, 0.0f);

	// a list of lists of vector3s.  The inner list stores the circles themselves and the outer lists contains copies of them at different angles
	std::vector<std::vector<vector3>> circles;

	// generate j amount of vertical circles
	for (int j = 0; j < a_nSubdivisions; j++)
	{
		// push back a new vector of points
		circles.push_back(std::vector<vector3>());
		// generate i amount of vertices (that make up a single vertical circle
		for (int i = 1; i < a_nSubdivisions; i++)
		{
			circles[j].push_back(vector3(
				// X coordinate found parametrically.  This generates a subdivision amount of points between the two tips
				(a_fRadius * cosf((PI / 2) - (stackAngle * i))) * cosf(angleRot * j),
				// Y coordinate.  Always the same across all vertical circles
				a_fRadius * sinf((PI / 2) - (stackAngle * i)),
				// Z coordinate found parametrically.  This generates a subdivision amount of points between the two tips
				(a_fRadius * cosf((PI / 2) - (stackAngle * i))) * sinf(angleRot * j)
			));
		}
	}

	// After generating the points, add the quads/triangles
	for (int i = 0; i < circles.size(); i++)
	{
		// If you would exceed the list, simply go back to the start
		if (i == circles.size() - 1)
		{
			// Add the top triangle
			AddTri(circles[0][0], circles[i][0], topTip);
			// Add the bottom triangle (points in reverse order)
			AddTri(circles[i][circles[i].size() - 1], circles[0][circles[i].size() - 1], botTip);


			for (int j = 0; j < circles[i].size() - 1; j++)
			{
				AddQuad(circles[i][j], circles[0][j], circles[i][j + 1], circles[0][j + 1]);
			}
		}
		else
		{
			// Add the top triangle
			AddTri(circles[i + 1][0], circles[i][0], topTip);
			// Add the bottom triangle (in reverse order)
			AddTri(circles[i][circles[i].size() - 1], circles[i + 1][circles[i].size() - 1], botTip);


			for (int j = 0; j < circles[i].size() - 1; j++)
			{
				AddQuad(circles[i][j], circles[i + 1][j], circles[i][j + 1], circles[i + 1][j + 1]);
			}

		}
		
	}

	// Adding information about color
	CompleteMesh(a_v3Color);
	CompileOpenGL3X();
}
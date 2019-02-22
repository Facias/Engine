#include "Editor.h"
#include "boost/filesystem.hpp"
#include "EngineMain.h"

#undef max
#undef min

using namespace ENGINE;

void Editor::Init()
{



	editorMenuText[0] = textManager->createText("File", glm::vec2(5, 0), 18);
	editorMenuText[1] = textManager->createText("Edit", glm::vec2(50, 0), 18);
	editorMenuText[2] = textManager->createText("Console", glm::vec2(100, 0), 18);
	editorMenuText[3] = textManager->createText("Render", glm::vec2(180, 0), 18);
	for (int n = 0; n < 4; n++)
	{
		textManager->setShaderConst(editorMenuText[n], "vcolor", 1, 1, 1, 1);
		textManager->setVisible(editorMenuText[n], false);
	}


	selectMeshText[0] = textManager->createText("Entity :", glm::vec2(0), 20);
	selectMeshText[1] = textManager->createText("mesh name", glm::vec2(0), 18);
	selectMeshText[2] = textManager->createText("Move", glm::vec2(0), 18);
	selectMeshText[3] = textManager->createText("Rotate", glm::vec2(0), 18);
	selectMeshText[4] = textManager->createText("Scale", glm::vec2(0), 18);
	selectMeshText[5] = textManager->createText("Visible", glm::vec2(0), 18);
	selectMeshText[6] = textManager->createText("Transparent", glm::vec2(0), 18);
	selectMeshText[7] = textManager->createText("Culling", glm::vec2(0), 18);
	selectMeshText[8] = textManager->createText("Follow Cam", glm::vec2(0), 18);
	selectMeshText[9] = textManager->createText("Look at Cam", glm::vec2(0), 18);
	selectMeshText[10] = textManager->createText("Collision", glm::vec2(0), 18);
	selectMeshText[11] = textManager->createText("Reflectable", glm::vec2(0), 18);
	selectMeshText[12] = textManager->createText("Cast Shadow", glm::vec2(0), 18);
	selectMeshText[13] = textManager->createText("Water", glm::vec2(0), 18);
	selectMeshText[14] = textManager->createText("Read Depth", glm::vec2(0), 18);
	selectMeshText[15] = textManager->createText("Vertex Shader", glm::vec2(0), 18);
	selectMeshText[16] = textManager->createText("Fragment Shader", glm::vec2(0), 18);
	selectMeshText[17] = textManager->createText("%%", glm::vec2(0), 18); // vertex shader text
	selectMeshText[18] = textManager->createText("%%", glm::vec2(0), 18); // fragment shader text
	selectMeshText[19] = textManager->createText("Position", glm::vec2(0), 18);
	selectMeshText[20] = textManager->createText("Scale", glm::vec2(0), 18);
	selectMeshText[21] = textManager->createText("Rotation", glm::vec2(0), 18);
	selectMeshText[22] = textManager->createText("xx", glm::vec2(0), 18); //pos
	selectMeshText[23] = textManager->createText("yy", glm::vec2(0), 18); //pos
	selectMeshText[24] = textManager->createText("zz", glm::vec2(0), 18); //pos
	selectMeshText[25] = textManager->createText("xx", glm::vec2(0), 18); //scale
	selectMeshText[26] = textManager->createText("yy", glm::vec2(0), 18); //scale
	selectMeshText[27] = textManager->createText("zz", glm::vec2(0), 18); //scale
	selectMeshText[28] = textManager->createText("xx", glm::vec2(0), 18); //rot
	selectMeshText[29] = textManager->createText("yy", glm::vec2(0), 18); //rot
	selectMeshText[30] = textManager->createText("zz", glm::vec2(0), 18); //rot
	selectMeshText[31] = textManager->createText(" ", glm::vec2(0), 18); // click to open texture options

	for (int n = 0; n < 31; n++)
		textManager->setVisible(selectMeshText[n], false);

	selectMeshPanel[0] = spriteManager->createSprite(glm::vec2(90, 24));
	selectMeshPanel[1] = spriteManager->createSprite(glm::vec2(90, 24));
	selectMeshPanel[2] = spriteManager->createSprite(glm::vec2(90, 24));
	selectMeshPanel[3] = spriteManager->createSprite(glm::vec2(300, 400));
	for (int n = 0; n < 4; n++)
	{
		if (n <3) spriteManager->setColor(selectMeshPanel[n], glm::vec4(.3, .3, .4, .8));
		spriteManager->setTexture(selectMeshPanel[n], defaultTexture, 0);
		spriteManager->setVisible(selectMeshPanel[n], false);
	}
	spriteManager->setColor(selectMeshPanel[0], glm::vec4(.5, .5, 0, .8));
	spriteManager->setColor(selectMeshPanel[3], glm::vec4(.2, .2, .3, .8));

	// selected mesh attribute toggle buttons
	selectMeshButton[0] = spriteManager->createSprite(glm::vec2(16));
	selectMeshButton[1] = spriteManager->createSprite(glm::vec2(16));
	selectMeshButton[2] = spriteManager->createSprite(glm::vec2(16));
	selectMeshButton[3] = spriteManager->createSprite(glm::vec2(16));
	selectMeshButton[4] = spriteManager->createSprite(glm::vec2(16));
	selectMeshButton[5] = spriteManager->createSprite(glm::vec2(16));
	selectMeshButton[6] = spriteManager->createSprite(glm::vec2(16));
	selectMeshButton[7] = spriteManager->createSprite(glm::vec2(16));
	for (int n = 0; n < 8; n++)
	{
		spriteManager->setTexture(selectMeshButton[n], defaultTexture, 0);
		spriteManager->setVisible(selectMeshButton[n], false);
		spriteManager->setDepth(selectMeshButton[n], 9.75f);
	}

	fpsText = textManager->createText("FPS Here", glm::vec2(0), 36);

	textManager->setShaderConst(fpsText, "vcolor", 1.f, .7f, .1f, 1.f);
	int vv = textureManager->loadImage("editorGraphics/editorPanel1.png");
	textureManager->setWrap(vv, false, false);

	int axisShader = shaderManager->loadShader("vertex.vs", "axisShader.ps");

	// main ribbon
	editorRibbon[0] = spriteManager->createSprite(glm::vec2(400, 20));
	spriteManager->setTexture(editorRibbon[0], defaultTexture, 0);
	spriteManager->setColor(editorRibbon[0], glm::vec4(.2, .2, .3, .8));
	spriteManager->setPosition(editorRibbon[0], glm::vec2(0));
	spriteManager->setVisible(editorRibbon[0], false);

	
	// entity ribbon
	editorRibbon[1] = spriteManager->createSprite(glm::vec2(300, 20));
	spriteManager->setTexture(editorRibbon[1], defaultTexture, 0);
	spriteManager->setColor(editorRibbon[1], glm::vec4(.2, .2, .3, .8));
	spriteManager->setPosition(editorRibbon[1], glm::vec2(0));
	spriteManager->setVisible(editorRibbon[1], false);

	// bottom ribbon
	editorRibbon[2] = spriteManager->createSprite(glm::vec2(960, 36));
	spriteManager->setTexture(editorRibbon[2], defaultTexture, 0);
	spriteManager->setColor(editorRibbon[2], glm::vec4(.2, .2, .3, .8));
	spriteManager->setPosition(editorRibbon[2], glm::vec2(0));
	spriteManager->setVisible(editorRibbon[2], false);

	// make x axis reference
	axis_arrow[0] = meshManager->loadMesh("axis_arrow.obj");
	int sho = textureManager->loadImage("axis_arrowLight.png");
	meshManager->meshArray[axis_arrow[0]].texture[0] = textureManager->texArray[sho].id;
	meshManager->setShaderConst(axis_arrow[0], "vcolor", 1, 0, 0, 1);
	meshManager->meshArray[axis_arrow[0]].rot = glm::vec3(0, 90, 0);
	meshManager->meshArray[axis_arrow[0]].pos = glm::vec3(0, 5, 0);
	meshManager->meshArray[axis_arrow[0]].type = ENGINE::EDITOR;
	meshManager->meshArray[axis_arrow[0]].updateModelMatrix();
	meshManager->setShader(axis_arrow[0], axisShader);
	meshManager->setVisible(axis_arrow[0], false);

	// make y axis reference
	axis_arrow[1] = meshManager->loadMesh("axis_arrow.obj");
	meshManager->meshArray[axis_arrow[1]].texture[0] = textureManager->texArray[sho].id;
	meshManager->setShaderConst(axis_arrow[1], "vcolor", 0, 1, 0, 1);
	meshManager->meshArray[axis_arrow[1]].rot = glm::vec3(-90, 0, 0);
	meshManager->meshArray[axis_arrow[1]].pos = glm::vec3(0, 5, 0);
	meshManager->meshArray[axis_arrow[1]].type = ENGINE::EDITOR;
	meshManager->meshArray[axis_arrow[1]].updateModelMatrix();
	meshManager->setShader(axis_arrow[1], axisShader);
	meshManager->setVisible(axis_arrow[1], false);

	// make z axis reference
	axis_arrow[2] = meshManager->loadMesh("axis_arrow.obj");
	meshManager->meshArray[axis_arrow[2]].texture[0] = textureManager->texArray[sho].id;
	meshManager->setShaderConst(axis_arrow[2], "vcolor", 0, 0, 1, 1);
	meshManager->meshArray[axis_arrow[2]].pos = glm::vec3(0, 5, 0);
	meshManager->meshArray[axis_arrow[2]].rot = glm::vec3(0, 0, 0);
	meshManager->meshArray[axis_arrow[2]].type = ENGINE::EDITOR;
	meshManager->meshArray[axis_arrow[2]].updateModelMatrix();
	meshManager->setShader(axis_arrow[2], axisShader);
	meshManager->setVisible(axis_arrow[2], false);

	// make x radial axis reference
	radial_axis[0] = meshManager->loadMesh("radial_axis.obj");
	meshManager->meshArray[radial_axis[0]].texture[0] = textureManager->texArray[defaultTexture].id;
	meshManager->setShaderConst(radial_axis[0], "vcolor", .8f, 0.f, 0.f, 1.f);
	meshManager->meshArray[radial_axis[0]].type = ENGINE::EDITOR;
	meshManager->meshArray[radial_axis[0]].updateModelMatrix();
	meshManager->setShader(radial_axis[0], axisShader);
	meshManager->setVisible(radial_axis[0], false);

	// make y radial axis reference
	radial_axis[1] = meshManager->loadMesh("radial_axis.obj");
	meshManager->meshArray[radial_axis[1]].texture[0] = textureManager->texArray[defaultTexture].id;
	meshManager->setShaderConst(radial_axis[1], "vcolor", 0.f, .8f, 0.f, 1.f);
	meshManager->meshArray[radial_axis[1]].rot = glm::vec3(0, 0, 90);
	meshManager->meshArray[radial_axis[1]].type = ENGINE::EDITOR;
	meshManager->meshArray[radial_axis[1]].updateModelMatrix();
	meshManager->setShader(radial_axis[1], axisShader);
	meshManager->setVisible(radial_axis[1], false);

	// make z radial axis reference
	radial_axis[2] = meshManager->loadMesh("radial_axis.obj");
	meshManager->meshArray[radial_axis[2]].texture[0] = textureManager->texArray[defaultTexture].id;
	meshManager->setShaderConst(radial_axis[2], "vcolor", 0.f, 0.f, .8f, 1.f);
	meshManager->meshArray[radial_axis[2]].rot = glm::vec3(0, 90, 0);
	meshManager->meshArray[radial_axis[2]].type = ENGINE::EDITOR;
	meshManager->meshArray[radial_axis[2]].updateModelMatrix();
	meshManager->setShader(radial_axis[2], axisShader);
	meshManager->setVisible(radial_axis[2], false);

	axisControlPanelPos = glm::vec2(120, 20);
	movebutton_panelSpriteIndex = spriteManager->createSprite(glm::vec2(32, 192));
	spriteManager->setTexture(movebutton_panelSpriteIndex, defaultTexture, 0);
	spriteManager->setPosition(movebutton_panelSpriteIndex, glm::vec2(0, 0) + axisControlPanelPos);
	spriteManager->setColor(movebutton_panelSpriteIndex, glm::vec4(0.f, 0.f, 0.f, .8f));
	spriteManager->setVisible(movebutton_panelSpriteIndex, false);
	spriteManager->setDepth(movebutton_panelSpriteIndex, 10.0f);

	move_xy_buttonSpriteIndex = spriteManager->createSprite(glm::vec2(32, 32));
	int xy_icon = textureManager->loadImage("editorGraphics/xy_icon.png");
	spriteManager->setTexture(move_xy_buttonSpriteIndex, xy_icon, 0);
	spriteManager->setPosition(move_xy_buttonSpriteIndex, glm::vec2(0, 0) + axisControlPanelPos);
	spriteManager->setColor(move_xy_buttonSpriteIndex, glm::vec4(1, 1, 0, 1));
	spriteManager->setVisible(move_xy_buttonSpriteIndex, false);
	spriteManager->setDepth(move_xy_buttonSpriteIndex, 9.75f);

	move_xz_buttonSpriteIndex = spriteManager->createSprite(glm::vec2(32, 32));
	int xz_icon = textureManager->loadImage("editorGraphics/xz_icon.png");
	spriteManager->setTexture(move_xz_buttonSpriteIndex, xz_icon, 0);
	spriteManager->setPosition(move_xz_buttonSpriteIndex, glm::vec2(0, 32) + axisControlPanelPos);
	spriteManager->setVisible(move_xz_buttonSpriteIndex, false);
	spriteManager->setDepth(move_xz_buttonSpriteIndex, 9.75f);

	move_yz_buttonSpriteIndex = spriteManager->createSprite(glm::vec2(32, 32));
	int yz_icon = textureManager->loadImage("editorGraphics/yz_icon.png");
	spriteManager->setTexture(move_yz_buttonSpriteIndex, yz_icon, 0);
	spriteManager->setPosition(move_yz_buttonSpriteIndex, glm::vec2(0, 64) + axisControlPanelPos);
	spriteManager->setVisible(move_yz_buttonSpriteIndex, false);
	spriteManager->setDepth(move_yz_buttonSpriteIndex, 9.75f);

	move_screen_buttonSpriteIndex = spriteManager->createSprite(glm::vec2(32, 32));
	int screenview_icon = textureManager->loadImage("editorGraphics/screenview_icon.png");
	spriteManager->setTexture(move_screen_buttonSpriteIndex, screenview_icon, 0);
	spriteManager->setPosition(move_screen_buttonSpriteIndex, glm::vec2(0, 96) + axisControlPanelPos);
	spriteManager->setVisible(move_screen_buttonSpriteIndex, false);
	spriteManager->setDepth(move_screen_buttonSpriteIndex, 9.75f);

	move_xy_buttonState = true;
	move_xz_buttonState = false;
	move_yz_buttonState = false;
	move_screen_buttonState = false;

	//import mesh dialogue

	for (int n = 0; n < 10; n++)
	{
		fileDialogueWindowText[n] = textManager->createText("window text", glm::vec2(400, 0), 18);
		textManager->setDepth(fileDialogueWindowText[n], 9.2);
		textManager->setVisible(fileDialogueWindowText[n], false);
	}


	for (int n = 0; n < 40; n++)
	{
		fileDialogueText[n] = textManager->createText("something", glm::vec2(400, 18 * (n)), 18);
		textManager->setVisible(fileDialogueText[n], false);
		textManager->setDepth(fileDialogueText[n], 9.2);
	}
	importMeshDialogueOpen = false;
	exportMeshDialogueOpen = false;
	openSceneDialogueOpen = false;
	saveAsDialogueOpen = false;


	// window back panel
	fileDialoguePanel[0] = spriteManager->createSprite(glm::vec2(300, 400));
	spriteManager->setColor(fileDialoguePanel[0], glm::vec4(.2, .2, .3, 0.8));
	spriteManager->setTexture(fileDialoguePanel[0], defaultTexture, 0);
	spriteManager->setVisible(fileDialoguePanel[0], false);
	spriteManager->setDepth(fileDialoguePanel[0], 9.5);

	// close button
	fileDialoguePanel[1] = spriteManager->createSprite(glm::vec2(16));
	int close_iconImg = textureManager->loadImage("editorGraphics/close_icon.png");
	spriteManager->setTexture(fileDialoguePanel[1], close_iconImg, 0);
	spriteManager->setDepth(fileDialoguePanel[1], 9.0);
	spriteManager->setVisible(fileDialoguePanel[1], false);

	// top border
	fileDialoguePanel[2] = spriteManager->createSprite(glm::vec2(300, 20));
	spriteManager->setTexture(fileDialoguePanel[2], defaultTexture, 0);
	spriteManager->setColor(fileDialoguePanel[2], glm::vec4(.0, .2, .4, 1.0));
	spriteManager->setDepth(fileDialoguePanel[2], 9.25);
	spriteManager->setVisible(fileDialoguePanel[2], false);

	// left border
	fileDialoguePanel[3] = spriteManager->createSprite(glm::vec2(8, 380));
	spriteManager->setTexture(fileDialoguePanel[3], defaultTexture, 0);
	spriteManager->setColor(fileDialoguePanel[3], glm::vec4(.0, .2, .4, 1.0));
	spriteManager->setDepth(fileDialoguePanel[3], 9.25);
	spriteManager->setVisible(fileDialoguePanel[3], false);

	// right border
	fileDialoguePanel[4] = spriteManager->createSprite(glm::vec2(8, 380));
	spriteManager->setTexture(fileDialoguePanel[4], defaultTexture, 0);
	spriteManager->setColor(fileDialoguePanel[4], glm::vec4(.0, .2, .4, 1.0));
	spriteManager->setDepth(fileDialoguePanel[4], 9.25);
	spriteManager->setVisible(fileDialoguePanel[4], false);

	// bottom border
	fileDialoguePanel[5] = spriteManager->createSprite(glm::vec2(300, 8));
	spriteManager->setTexture(fileDialoguePanel[5], defaultTexture, 0);
	spriteManager->setColor(fileDialoguePanel[5], glm::vec4(.0, .2, .4, 1.0));
	spriteManager->setDepth(fileDialoguePanel[5], 9.25);
	spriteManager->setVisible(fileDialoguePanel[5], false);

	// scroll bar
	fileDialoguePanel[6] = spriteManager->createSprite(glm::vec2(8, 370));
	spriteManager->setTexture(fileDialoguePanel[6], defaultTexture, 0);
	spriteManager->setColor(fileDialoguePanel[6], glm::vec4(.0, .8, .8, 1.0));
	spriteManager->setDepth(fileDialoguePanel[6], 9.25);
	spriteManager->setVisible(fileDialoguePanel[6], false);

	// import mesh seletion highlight
	for (int n = 0; n < 20; n++)
	{
		importmesh_fileHighlight[n] = spriteManager->createSprite(glm::vec2(200, 18));
		spriteManager->setTexture(importmesh_fileHighlight[n], defaultTexture, 0);
		spriteManager->setColor(importmesh_fileHighlight[n], glm::vec4(1, .8, 0, .6));
		spriteManager->setVisible(importmesh_fileHighlight[n], false);
		spriteManager->setDepth(importmesh_fileHighlight[n], 9.4f);
	}
	fileSelectCount = 0;
	// drop down menu items
	for (int n = 0; n < 10; n++)
	{
		dropdownText[n] = textManager->createText("none", glm::vec2(0), 18);
		textManager->setVisible(dropdownText[n], false);
	}

	dropdownPanel[0] = spriteManager->createSprite(glm::vec2(100, 200));
	spriteManager->setPosition(dropdownPanel[0], glm::vec2(0, 20));
	spriteManager->setVisible(dropdownPanel[0], false);
	spriteManager->setColor(dropdownPanel[0], glm::vec4(.1, .1, .2, .8));

	//bottom editor panel items
	bottomEditorText[0] = textManager->createText("Camera", glm::vec2(0), 18);
	bottomEditorText[1] = textManager->createText("xx", glm::vec2(0), 18);
	bottomEditorText[2] = textManager->createText("yy", glm::vec2(0), 18);
	bottomEditorText[3] = textManager->createText("zz", glm::vec2(0), 18);
	bottomEditorText[4] = textManager->createText("xx", glm::vec2(0), 18);
	bottomEditorText[5] = textManager->createText("yy", glm::vec2(0), 18);
	bottomEditorText[6] = textManager->createText("zz", glm::vec2(0), 18);

	for (int n = 0; n < 10; n++)
		selectedMesh[n] = -1;
	selectCount = 0;

	dropdownOpen = -1;
	modificationState = EDIT_MOVE;

	editHistory = new EditHistory[100];
	editHistoryStep = 0;
	editHistoryMax = 0;

	createButton("test", glm::vec2(100), glm::vec2(100));
	mouseLeftPressed = false;
	mouseLeftReleased = false;

	createWindow(EDITOR_OPEN_PROJECT);
}

void Editor::Proc()
{
	windowProc(0);

	//if (glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL))
	if (!mouseLeftPressed && glfwGetMouseButton(control->window, GLFW_MOUSE_BUTTON_1))
	{
		mouseLeftDown = control->cursorPos;
		mouseLeftPressed = true;
	}

	if (mouseLeftPressed && !glfwGetMouseButton(control->window, GLFW_MOUSE_BUTTON_1))
	{
		mouseLeftUp = control->cursorPos;
		mouseLeftReleased = true;
	}

	if (mouseLeftPressed && mouseLeftReleased)
	{
		for (int n = 0; n < buttonStack.size; n++)
		{
		
			if (!buttonStack.item[n].isActive)
				continue;

			glm::vec2 minpos = buttonStack.item[n].pos;
			glm::vec2 maxpos = buttonStack.item[n].pos + buttonStack.item[n].size;

			if (mouseLeftDown.x > minpos.x && mouseLeftDown.x < maxpos.x)
				if (mouseLeftDown.y > minpos.y && mouseLeftDown.y < maxpos.y)
					if (mouseLeftUp.x > minpos.x && mouseLeftUp.x < maxpos.x)
						if (mouseLeftUp.y > minpos.y && mouseLeftUp.y < maxpos.y)
							printf("hit\n");

							
		}
		
		mouseLeftDown = glm::dvec2(-10);
		mouseLeftUp = glm::dvec2(-10);
		mouseLeftPressed = false;
		mouseLeftReleased = false;
	}

	

	//if (selectCount == 1)
	//	printf("%d \n", meshManager->meshArray[selectedMesh[0]].vertexCount);

	// show all items of the editor interface ******
	spriteManager->setVisible(move_xy_buttonSpriteIndex, true);
	spriteManager->setVisible(move_xz_buttonSpriteIndex, true);
	spriteManager->setVisible(move_yz_buttonSpriteIndex, true);
	spriteManager->setVisible(move_screen_buttonSpriteIndex, true);
	spriteManager->setVisible(movebutton_panelSpriteIndex, true);

	spriteManager->setVisible(editorRibbon[0], true);
	spriteManager->setVisible(editorRibbon[1], true);
	spriteManager->setVisible(editorRibbon[2], true);

	for (int n = 0; n < 4; n++)
		textManager->setVisible(editorMenuText[n], true);
	// text
	for (int n = 0; n < 31; n++)
		textManager->setVisible(selectMeshText[n], true);
	for (int n = 0; n < 4; n++)
		spriteManager->setVisible(selectMeshPanel[n], true);
	for (int n = 0; n < 8; n++)
		spriteManager->setVisible(selectMeshButton[n], true);

	// update positions of text and windows
	textManager->textArray[selectMeshText[0]].pos = glm::vec2(windowSize.x - 250, 0);
	textManager->textArray[selectMeshText[1]].pos = glm::vec2(windowSize.x - 170, 2);
	textManager->textArray[selectMeshText[2]].pos = glm::vec2(windowSize.x - 270, 26);
	textManager->textArray[selectMeshText[3]].pos = glm::vec2(windowSize.x - 180, 26);
	textManager->textArray[selectMeshText[4]].pos = glm::vec2(windowSize.x - 70, 26);
	textManager->textArray[selectMeshText[5]].pos = glm::vec2(windowSize.x - 280, 52);
	textManager->textArray[selectMeshText[6]].pos = glm::vec2(windowSize.x - 280, 72);
	textManager->textArray[selectMeshText[7]].pos = glm::vec2(windowSize.x - 280, 92);
	textManager->textArray[selectMeshText[8]].pos = glm::vec2(windowSize.x - 280, 112);
	textManager->textArray[selectMeshText[9]].pos = glm::vec2(windowSize.x - 280, 132);
	textManager->textArray[selectMeshText[10]].pos = glm::vec2(windowSize.x - 280, 152);
	textManager->textArray[selectMeshText[11]].pos = glm::vec2(windowSize.x - 280, 172);
	textManager->textArray[selectMeshText[12]].pos = glm::vec2(windowSize.x - 280, 192);
	textManager->textArray[selectMeshText[13]].pos = glm::vec2(windowSize.x - 280, 212);
	textManager->textArray[selectMeshText[14]].pos = glm::vec2(windowSize.x - 280, 232);

	// shaders
	textManager->textArray[selectMeshText[15]].pos = glm::vec2(windowSize.x - 280, 252);
	textManager->textArray[selectMeshText[16]].pos = glm::vec2(windowSize.x - 280, 272);
	textManager->textArray[selectMeshText[17]].pos = glm::vec2(windowSize.x - 280 + 120, 252);
	textManager->textArray[selectMeshText[18]].pos = glm::vec2(windowSize.x - 280 + 120, 272);
	if (selectedMesh[0] > 0 && selectedMesh[0] < meshManager->meshCount)
	{
		ENGINE::Shader tempshader = shaderManager->shaderArray[meshManager->meshArray[selectedMesh[0]].shaderIndex];
		textManager->setString(selectMeshText[17], tempshader.vertexName);
		textManager->setString(selectMeshText[18], tempshader.fragName);
	}

	// position, scale, rotation
	textManager->textArray[selectMeshText[19]].pos = glm::vec2(windowSize.x - 280, 292);
	textManager->textArray[selectMeshText[20]].pos = glm::vec2(windowSize.x - 280, 312);
	textManager->textArray[selectMeshText[21]].pos = glm::vec2(windowSize.x - 280, 332);

	if (selectedMesh[0] > 0 && selectedMesh[0] < meshManager->meshCount)
	{
		glm::vec3 pos = meshManager->getPosition(selectedMesh[0]);
		glm::vec3 scale = meshManager->getScale(selectedMesh[0]);
		glm::vec3 rot = meshManager->getRotation(selectedMesh[0]);

		char aa[20];
		//position
		sprintf_s(aa, "%.02f", pos.x);
		textManager->setString(selectMeshText[22], aa);
		sprintf_s(aa, "%.02f", pos.y);
		textManager->setString(selectMeshText[23], aa);
		sprintf_s(aa, "%.02f", pos.z);
		textManager->setString(selectMeshText[24], aa);

		//scale
		sprintf_s(aa, "%.02f", scale.x);
		textManager->setString(selectMeshText[25], aa);
		sprintf_s(aa, "%.02f", scale.y);
		textManager->setString(selectMeshText[26], aa);
		sprintf_s(aa, "%.02f", scale.z);
		textManager->setString(selectMeshText[27], aa);

		//rotation
		sprintf_s(aa, "%.02f", rot.x);
		textManager->setString(selectMeshText[28], aa);
		sprintf_s(aa, "%.02f", rot.y);
		textManager->setString(selectMeshText[29], aa);
		sprintf_s(aa, "%.02f", rot.z);
		textManager->setString(selectMeshText[30], aa);
	}


	textManager->textArray[selectMeshText[22]].pos = glm::vec2(windowSize.x - 280 + 70, 292);
	textManager->textArray[selectMeshText[23]].pos = glm::vec2(windowSize.x - 280 + 140, 292);
	textManager->textArray[selectMeshText[24]].pos = glm::vec2(windowSize.x - 280 + 210, 292);

	textManager->textArray[selectMeshText[25]].pos = glm::vec2(windowSize.x - 280 + 70, 312);
	textManager->textArray[selectMeshText[26]].pos = glm::vec2(windowSize.x - 280 + 140, 312);
	textManager->textArray[selectMeshText[27]].pos = glm::vec2(windowSize.x - 280 + 210, 312);

	textManager->textArray[selectMeshText[28]].pos = glm::vec2(windowSize.x - 280 + 70, 332);
	textManager->textArray[selectMeshText[29]].pos = glm::vec2(windowSize.x - 280 + 140, 332);
	textManager->textArray[selectMeshText[30]].pos = glm::vec2(windowSize.x - 280 + 210, 332);

	spriteManager->setPosition(selectMeshButton[0], glm::vec2(windowSize.x - 50, 54));
	spriteManager->setPosition(selectMeshButton[1], glm::vec2(windowSize.x - 50, 74));
	spriteManager->setPosition(selectMeshButton[2], glm::vec2(windowSize.x - 50, 114));
	spriteManager->setPosition(selectMeshButton[3], glm::vec2(windowSize.x - 50, 134));
	spriteManager->setPosition(selectMeshButton[4], glm::vec2(windowSize.x - 50, 154));
	spriteManager->setPosition(selectMeshButton[5], glm::vec2(windowSize.x - 50, 174));
	spriteManager->setPosition(selectMeshButton[6], glm::vec2(windowSize.x - 50, 194));
	spriteManager->setPosition(selectMeshButton[7], glm::vec2(windowSize.x - 50, 214));


	// bottom panel
	textManager->setPosition(bottomEditorText[0], glm::vec2(windowSize.x - 200, windowSize.y - 36));

	char aa[20];
	sprintf_s(aa, "%.02f", mainCamera->pos.x);
	textManager->setString(bottomEditorText[1], aa);
	sprintf_s(aa, "%.02f", mainCamera->pos.y);
	textManager->setString(bottomEditorText[2], aa);
	sprintf_s(aa, "%.02f", mainCamera->pos.z);
	textManager->setString(bottomEditorText[3], aa);

	sprintf_s(aa, "%.02f", mainCamera->forward.x);
	textManager->setString(bottomEditorText[4], aa);
	sprintf_s(aa, "%.02f", mainCamera->forward.y);
	textManager->setString(bottomEditorText[5], aa);
	sprintf_s(aa, "%.02f", mainCamera->forward.z);
	textManager->setString(bottomEditorText[6], aa);

	textManager->setPosition(bottomEditorText[1], glm::vec2(windowSize.x - 150, windowSize.y - 36));
	textManager->setPosition(bottomEditorText[2], glm::vec2(windowSize.x - 100, windowSize.y - 36));
	textManager->setPosition(bottomEditorText[3], glm::vec2(windowSize.x - 50, windowSize.y - 36));
	textManager->setPosition(bottomEditorText[4], glm::vec2(windowSize.x - 150, windowSize.y - 18));
	textManager->setPosition(bottomEditorText[5], glm::vec2(windowSize.x - 100, windowSize.y - 18));
	textManager->setPosition(bottomEditorText[6], glm::vec2(windowSize.x - 50, windowSize.y - 18));

	// user keyboard string input
	control->typeInput();
	textManager->setString(control->typeInText, control->typeIn);

	// file menu dropdown
	if (control->clickRegion(glm::vec2(0), glm::vec2(40, 18)))
	{
		dropdownOpen = 1;
		textManager->setString(dropdownText[0], "New");
		textManager->setString(dropdownText[1], "Open");
		textManager->setString(dropdownText[2], "Save");
		textManager->setString(dropdownText[3], "Save As");
		textManager->setString(dropdownText[4], "Import");
		textManager->setString(dropdownText[5], "Export");

		spriteManager->setVisible(dropdownPanel[0], true);

		for (int n = 0; n < 6; n++)
		{
			textManager->setVisible(dropdownText[n], true);
			textManager->setPosition(dropdownText[n], glm::vec2(5, 20 + 18 * n));
		}

		if (selectCount != 1)
		{
			textManager->setColor(dropdownText[5], glm::vec4(.5, .5, .5, 1));
		}
		else
		{
			textManager->setColor(dropdownText[5], glm::vec4(1));
		}
	}

	// while file menu dropdown open...
	if (dropdownOpen == 1)
	{
		// if select open option...
		if (control->clickRegion(glm::vec2(0, 38), glm::vec2(100, 55)))
		{
			closeImportMeshWindow();
			closeExportMeshWindow();
			closeSaveAsWindow();
			openOpenSceneWindow();
		}
		// if select save as option...
		if (control->clickRegion(glm::vec2(0, 74), glm::vec2(100, 91)))
		{
			closeImportMeshWindow();
			closeExportMeshWindow();
			closeOpenSceneWindow();
			openSaveAsWindow();
		}
		// if select import mesh option...
		if (control->clickRegion(glm::vec2(0, 92), glm::vec2(100, 109)))
		{
			closeOpenSceneWindow();
			closeExportMeshWindow();
			openImportMeshWindow();
		}
		// if select export mesh option...
		if (control->clickRegion(glm::vec2(0, 110), glm::vec2(100, 127)))
		{
			closeOpenSceneWindow();
			closeImportMeshWindow();
			openExportMeshWindow();
		}

		if (control->clickOutsideRegion(glm::vec2(0, 20), glm::vec2(100, 320)) && control->clickOutsideRegion(glm::vec2(0, 0), glm::vec2(40, 20)))
		{
			// close drop down menu
			spriteManager->setVisible(dropdownPanel[0], false);

			for (int n = 0; n < 6; n++)
			{
				textManager->setVisible(dropdownText[n], false);

			}
			dropdownOpen = -1;
		}

	}

	// undo operation
	if (glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(control->window, GLFW_KEY_Z) && !backstepped)
	{
		if (editHistoryStep > 1)
		{
			editHistoryStep--;
			int ind = editHistoryStep - 1;
			for (int n = 0; n < selectCount; n++)
			{
				meshManager->meshArray[editHistory[ind].meshIndex[n]].pos = editHistory[ind].pos[n];
				meshManager->meshArray[editHistory[ind].meshIndex[n]].rot = editHistory[ind].rot[n];
				meshManager->meshArray[editHistory[ind].meshIndex[n]].scale = editHistory[ind].scale[n];

				meshManager->meshArray[editHistory[ind].meshIndex[n]].updateModelMatrix();
			}
			updateAxis();

			backstepped = true;
		}
	}

	// redo operation
	if (glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(control->window, GLFW_KEY_Y) && !backstepped)
	{
		if (editHistoryStep < editHistoryMax)
		{
			for (int n = 0; n < selectCount; n++)
			{
				meshManager->meshArray[editHistory[editHistoryStep].meshIndex[n]].pos = editHistory[editHistoryStep].pos[n];
				meshManager->meshArray[editHistory[editHistoryStep].meshIndex[n]].rot = editHistory[editHistoryStep].rot[n];
				meshManager->meshArray[editHistory[editHistoryStep].meshIndex[n]].scale = editHistory[editHistoryStep].scale[n];

				meshManager->meshArray[editHistory[editHistoryStep].meshIndex[n]].updateModelMatrix();
			}
			updateAxis();
			editHistoryStep++;

			backstepped = true;
		}
	}
	if (!glfwGetKey(control->window, GLFW_KEY_Z) && !glfwGetKey(control->window, GLFW_KEY_Y) && backstepped)
	{
		backstepped = false;
	}

	if (selectCount == 1)
	{
		// visibility toggle
		if (control->clickRegion(glm::vec2(windowSize.x - 50, 54), glm::vec2(windowSize.x - 34, 70)))
		{
			if (meshManager->getVisible(selectedMesh[0]))
				meshManager->setVisible(selectedMesh[0], false);
			else
				meshManager->setVisible(selectedMesh[0], true);
		}
		if (meshManager->getVisible(selectedMesh[0]))
		{
			spriteManager->setColor(selectMeshButton[0], glm::vec4(1, 1, 0, 1));
		}
		else
		{
			spriteManager->setColor(selectMeshButton[0], glm::vec4(.5, .5, .5, 1));
		}

		// transparency toggle
		if (control->clickRegion(glm::vec2(windowSize.x - 50, 74), glm::vec2(windowSize.x - 34, 90)))
		{
			if (meshManager->getTransparent(selectedMesh[0]))
				meshManager->setTransparent(selectedMesh[0], false);
			else
				meshManager->setTransparent(selectedMesh[0], true);
		}
		if (control->clickRegion(glm::vec2(windowSize.x - 50, 54), glm::vec2(windowSize.x - 34, 70)))
		{
			if (meshManager->getVisible(selectedMesh[0]))
				meshManager->setVisible(selectedMesh[0], false);
			else
				meshManager->setVisible(selectedMesh[0], true);
		}
		if (meshManager->getTransparent(selectedMesh[0]))
		{
			spriteManager->setColor(selectMeshButton[1], glm::vec4(1, 1, 0, 1));
		}
		else
		{
			spriteManager->setColor(selectMeshButton[1], glm::vec4(.5, .5, .5, 1));
		}

		// follow cam toggle
		if (control->clickRegion(glm::vec2(windowSize.x - 50, 114), glm::vec2(windowSize.x - 34, 130)))
		{
			if (meshManager->getFollowCam(selectedMesh[0]))
				meshManager->setFollowCam(selectedMesh[0], false);
			else
				meshManager->setFollowCam(selectedMesh[0], true);
		}
		if (meshManager->getFollowCam(selectedMesh[0]))
		{
			spriteManager->setColor(selectMeshButton[2], glm::vec4(1, 1, 0, 1));
		}
		else
		{
			spriteManager->setColor(selectMeshButton[2], glm::vec4(.5, .5, .5, 1));
		}

		// lookat cam toggle
		if (control->clickRegion(glm::vec2(windowSize.x - 50, 134), glm::vec2(windowSize.x - 34, 150)))
		{
			if (meshManager->getLookatCam(selectedMesh[0]))
				meshManager->setLookatCam(selectedMesh[0], false);
			else
				meshManager->setLookatCam(selectedMesh[0], true);
		}
		if (meshManager->getLookatCam(selectedMesh[0]))
		{
			spriteManager->setColor(selectMeshButton[3], glm::vec4(1, 1, 0, 1));
		}
		else
		{
			spriteManager->setColor(selectMeshButton[3], glm::vec4(.5, .5, .5, 1));
		}

		// collision toggle 
		if (control->clickRegion(glm::vec2(windowSize.x - 50, 154), glm::vec2(windowSize.x - 34, 170)))
		{
			if (meshManager->getCollision(selectedMesh[0]))
				meshManager->setCollision(selectedMesh[0], false);
			else
				meshManager->setCollision(selectedMesh[0], true);
		}
		if (meshManager->getCollision(selectedMesh[0]))
		{
			spriteManager->setColor(selectMeshButton[4], glm::vec4(1, 1, 0, 1));
		}
		else
		{
			spriteManager->setColor(selectMeshButton[4], glm::vec4(.5, .5, .5, 1));
		}

		// reflectable toggle
		if (control->clickRegion(glm::vec2(windowSize.x - 50, 174), glm::vec2(windowSize.x - 34, 190)))
		{
			if (meshManager->getReflectable(selectedMesh[0]))
				meshManager->setReflectable(selectedMesh[0], false);
			else
				meshManager->setReflectable(selectedMesh[0], true);
		}
		if (meshManager->getReflectable(selectedMesh[0]))
		{
			spriteManager->setColor(selectMeshButton[5], glm::vec4(1, 1, 0, 1));
		}
		else
		{
			spriteManager->setColor(selectMeshButton[5], glm::vec4(.5, .5, .5, 1));
		}

		// cast shadow toggle
		if (control->clickRegion(glm::vec2(windowSize.x - 50, 194), glm::vec2(windowSize.x - 34, 210)))
		{
			if (meshManager->getCastShadow(selectedMesh[0]))
				meshManager->setCastShadow(selectedMesh[0], false);
			else
				meshManager->setCastShadow(selectedMesh[0], true);
		}
		if (meshManager->getCastShadow(selectedMesh[0]))
		{
			spriteManager->setColor(selectMeshButton[6], glm::vec4(1, 1, 0, 1));
		}
		else
		{
			spriteManager->setColor(selectMeshButton[6], glm::vec4(.5, .5, .5, 1));
		}

		// is water toggle
		if (control->clickRegion(glm::vec2(windowSize.x - 50, 214), glm::vec2(windowSize.x - 34, 230)))
		{
			if (meshManager->getIsWater(selectedMesh[0]))
				meshManager->setIsWater(selectedMesh[0], false);
			else
				meshManager->setIsWater(selectedMesh[0], true);
		}
		if (meshManager->getIsWater(selectedMesh[0]))
		{
			spriteManager->setColor(selectMeshButton[7], glm::vec4(1, 1, 0, 1));
		}
		else
		{
			spriteManager->setColor(selectMeshButton[7], glm::vec4(.5, .5, .5, 1));
		}
	}

	// pan camera w/ middle mouse button
	if (glfwGetMouseButton(control->window, GLFW_MOUSE_BUTTON_3) == GLFW_PRESS)
	{

		glm::vec2 delta = control->cursorPos - control->lastCursorPos;

		control->position += delta.x * control->right;
		control->position -= delta.y * control->up;
	}

	spriteManager->setPosition(editorRibbon[1], glm::vec2(windowSize.x - 300, 0));
	spriteManager->setPosition(editorRibbon[2], glm::vec2(0, windowSize.y - 36));
	spriteManager->setSize(editorRibbon[2], glm::vec2(windowSize.x, 36));

	spriteManager->setPosition(selectMeshPanel[0], glm::vec2(windowSize.x - 290, 24));
	spriteManager->setPosition(selectMeshPanel[1], glm::vec2(windowSize.x - 190, 24));
	spriteManager->setPosition(selectMeshPanel[2], glm::vec2(windowSize.x - 90, 24));
	spriteManager->setPosition(selectMeshPanel[3], glm::vec2(windowSize.x - 300, 52));

	// copy selected mesh to clipboard
	if (glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(control->window, GLFW_KEY_C))
	{
		for (int n = 0; n < selectCount; n++)
			copySelection[n] = selectedMesh[n];
		copySelectionCount = selectCount;
	}
	// paste copied mesh
	if (glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(control->window, GLFW_KEY_V) && !pasted)
	{
		for (int n = 0; n < copySelectionCount; n++)
			meshManager->copy(copySelection[n]);
		pasted = true;
	}

	if (pasted && (!glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL) || !glfwGetKey(control->window, GLFW_KEY_V)))
	{
		pasted = false;
	}

	// export selected mesh
	if (glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(control->window, GLFW_KEY_E) && !meshExported)
	{
		//for (int n = 0; n < copySelectionCount; n++)
		meshManager->exportMesh(selectedMesh[0]);
		meshExported = true;
	}

	if (!glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL) || !glfwGetKey(control->window, GLFW_KEY_E))
	{
		meshExported = false;
	}

	// import mesh
	if (glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(control->window, GLFW_KEY_I) && !importMeshDialogueOpen)
	{
		//openImportMeshWindow();
	}

	// while open scene window is open...
	if (openSceneDialogueOpen)
	{

		if (control->grabRegion(importMeshWindowPosition, importMeshWindowPosition + glm::vec2(300, 400)))
			interactWindow = true;

		if (control->grabRegion(importMeshWindowPosition, importMeshWindowPosition + glm::vec2(284, 16)))
			grabWindow = true;

		if (!control->mouse_leftDown)
		{
			grabWindow = false;
		}

		if (grabWindow)
		{
			importMeshWindowPosition += control->cursorPos - control->lastCursorPos;
			if (importMeshWindowPosition.y <= 0)
				importMeshWindowPosition.y = 0;
			if (importMeshWindowPosition.y >= windowSize.y - 52)
				importMeshWindowPosition.y = windowSize.y - 52;

			if (importMeshWindowPosition.x <= -250)
				importMeshWindowPosition.x = -250;
			if (importMeshWindowPosition.x >= windowSize.x - 50)
				importMeshWindowPosition.x = windowSize.x - 50;

			spriteManager->setPosition(fileDialoguePanel[0], importMeshWindowPosition);
			spriteManager->setPosition(fileDialoguePanel[1], importMeshWindowPosition + glm::vec2(284, 0));
			spriteManager->setPosition(fileDialoguePanel[2], importMeshWindowPosition);
			spriteManager->setPosition(fileDialoguePanel[3], importMeshWindowPosition + glm::vec2(0, 20));
			spriteManager->setPosition(fileDialoguePanel[4], importMeshWindowPosition + glm::vec2(292, 20));
			spriteManager->setPosition(fileDialoguePanel[5], importMeshWindowPosition + glm::vec2(0, 392));
			spriteManager->setPosition(fileDialoguePanel[6], importMeshWindowPosition + glm::vec2(284, 20));

			textManager->setPosition(fileDialogueWindowText[0], importMeshWindowPosition + glm::vec2(5, 0));
			textManager->setPosition(fileDialogueWindowText[1], importMeshWindowPosition + glm::vec2(200, 400));
		}


		float scrollSizeRatio = glm::min(1.0f, 380.0f / (18 * meshfileCount));
		// move scrollbar with mouse
		if (!grabWindow)
		{
			if (control->grabRegion(importMeshWindowPosition + glm::vec2(284, 20), importMeshWindowPosition + glm::vec2(292, 370)))
				grabScrollBar = true;

			if (!control->mouse_leftDown)
				grabScrollBar = false;

			if (grabScrollBar)
			{
				importMeshScrollPos += control->cursorPos.y - control->lastCursorPos.y;

			}
		}

		float scrollposratio;

		if (scrollSizeRatio < 1.0)
		{
			if (importMeshScrollPos <= 0) importMeshScrollPos = 0;
			if (importMeshScrollPos + 370 * scrollSizeRatio >= 370) importMeshScrollPos = 370 * (1 - scrollSizeRatio);
			scrollposratio = importMeshScrollPos / (370 * (1 - scrollSizeRatio));
		}
		else
		{
			importMeshScrollPos = 0;
			scrollposratio = 0;
		}
		spriteManager->setSize(fileDialoguePanel[6], glm::vec2(8, 370) * glm::vec2(1, scrollSizeRatio));
		spriteManager->setPosition(fileDialoguePanel[6], importMeshWindowPosition + glm::vec2(284, 20 + importMeshScrollPos));

		int slotposition = floor((float(control->cursorPos.y) - importMeshWindowPosition.y - 22 + (1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio) / 18.0);
		slotposition = glm::min(glm::max(0, slotposition), 40);

		for (int n = 0; n < 40; n++)
		{
			textManager->setShaderConst(fileDialogueText[n], "feather", 8, 0, 0, 0);
			textManager->setShaderConst(fileDialogueText[n], "scissor", importMeshWindowPosition.x, importMeshWindowPosition.y + 20, importMeshWindowPosition.x + 300, importMeshWindowPosition.y + 390);
			textManager->setPosition(fileDialogueText[n], importMeshWindowPosition + glm::vec2(10, 20 + 18 * (n)-(1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio));
		}

		//select file
		if (control->hoverRegion(importMeshWindowPosition + glm::vec2(8, 22), importMeshWindowPosition + glm::vec2(292, 398)))
		{

			float p = 20 + 18 * (slotposition)-(1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio;
			if (control->clickRegion(importMeshWindowPosition + glm::vec2(8, p), importMeshWindowPosition + glm::vec2(200, p + 18)))
			{
				//make sure this item is not already selected
				//if so, unselect it

				short alreadyselected = -1;
				if (importmesh_fileSelect[0] == slotposition + 1) alreadyselected = 0;

				// only one file can be selected at a time
				if (alreadyselected == -1)
				{
					spriteManager->setPosition(importmesh_fileHighlight[0], importMeshWindowPosition + glm::vec2(8, p));
					spriteManager->setVisible(importmesh_fileHighlight[0], true);
					spriteManager->setFeather(importmesh_fileHighlight[0], 8.f);
					spriteManager->setScissor(importmesh_fileHighlight[0], importMeshWindowPosition + glm::vec2(4, 20), importMeshWindowPosition + glm::vec2(292, 392));

					importmesh_fileSelect[0] = slotposition + 1;
					fileSelectCount = 1;
				}
				else
				{
					spriteManager->setVisible(importmesh_fileHighlight[0], false);
					fileSelectCount = 0;
				}
			}
		}

		for (int n = 0; n < fileSelectCount; n++)
		{
			float p = 20 + 18 * (importmesh_fileSelect[n] - 1) - (1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio;
			spriteManager->setPosition(importmesh_fileHighlight[n], importMeshWindowPosition + glm::vec2(8, p));
			spriteManager->setScissor(importmesh_fileHighlight[n], importMeshWindowPosition + glm::vec2(4, 20), importMeshWindowPosition + glm::vec2(292, 392));

		}

		if (fileSelectCount > 0)
		{
			textManager->setColor(fileDialogueWindowText[1], glm::vec4(1));
		}
		else
		{
			textManager->setColor(fileDialogueWindowText[1], glm::vec4(.5, .5, .5, 1));
		}

		//close open scene window
		if (control->clickRegion(glm::vec2(284, 0) + importMeshWindowPosition, glm::vec2(300, 16) + importMeshWindowPosition))
		{
			closeOpenSceneWindow();
		}
	}

	// while save as window is open...
	if (saveAsDialogueOpen)
	{
		if (control->grabRegion(importMeshWindowPosition, importMeshWindowPosition + glm::vec2(300, 400)))
			interactWindow = true;

		if (control->grabRegion(importMeshWindowPosition, importMeshWindowPosition + glm::vec2(284, 16)))
			grabWindow = true;

		if (!control->mouse_leftDown)
		{
			grabWindow = false;
		}

		if (grabWindow)
		{
			importMeshWindowPosition += control->cursorPos - control->lastCursorPos;
			if (importMeshWindowPosition.y <= 0)
				importMeshWindowPosition.y = 0;
			if (importMeshWindowPosition.y >= windowSize.y - 52)
				importMeshWindowPosition.y = windowSize.y - 52;

			if (importMeshWindowPosition.x <= -250)
				importMeshWindowPosition.x = -250;
			if (importMeshWindowPosition.x >= windowSize.x - 50)
				importMeshWindowPosition.x = windowSize.x - 50;

			spriteManager->setPosition(fileDialoguePanel[0], importMeshWindowPosition);
			spriteManager->setPosition(fileDialoguePanel[1], importMeshWindowPosition + glm::vec2(284, 0));
			spriteManager->setPosition(fileDialoguePanel[2], importMeshWindowPosition);
			spriteManager->setPosition(fileDialoguePanel[3], importMeshWindowPosition + glm::vec2(0, 20));
			spriteManager->setPosition(fileDialoguePanel[4], importMeshWindowPosition + glm::vec2(292, 20));
			spriteManager->setPosition(fileDialoguePanel[5], importMeshWindowPosition + glm::vec2(0, 392));
			spriteManager->setPosition(fileDialoguePanel[6], importMeshWindowPosition + glm::vec2(284, 20));

			textManager->setPosition(fileDialogueWindowText[0], importMeshWindowPosition + glm::vec2(5, 0));
			textManager->setPosition(fileDialogueWindowText[1], importMeshWindowPosition + glm::vec2(200, 400));
		}


		float scrollSizeRatio = glm::min(1.0f, 380.0f / (18 * meshfileCount));
		// move scrollbar with mouse
		if (!grabWindow)
		{
			if (control->grabRegion(importMeshWindowPosition + glm::vec2(284, 20), importMeshWindowPosition + glm::vec2(292, 370)))
				grabScrollBar = true;

			if (!control->mouse_leftDown)
				grabScrollBar = false;

			if (grabScrollBar)
			{
				importMeshScrollPos += control->cursorPos.y - control->lastCursorPos.y;

			}
		}

		float scrollposratio;

		if (scrollSizeRatio < 1.0)
		{
			if (importMeshScrollPos <= 0) importMeshScrollPos = 0;
			if (importMeshScrollPos + 370 * scrollSizeRatio >= 370) importMeshScrollPos = 370 * (1 - scrollSizeRatio);
			scrollposratio = importMeshScrollPos / (370 * (1 - scrollSizeRatio));
		}
		else
		{
			importMeshScrollPos = 0;
			scrollposratio = 0;
		}
		spriteManager->setSize(fileDialoguePanel[6], glm::vec2(8, 370) * glm::vec2(1, scrollSizeRatio));
		spriteManager->setPosition(fileDialoguePanel[6], importMeshWindowPosition + glm::vec2(284, 20 + importMeshScrollPos));

		int slotposition = floor((float(control->cursorPos.y) - importMeshWindowPosition.y - 22 + (1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio) / 18.0);
		slotposition = glm::min(glm::max(0, slotposition), 40);

		for (int n = 0; n < 40; n++)
		{
			textManager->setShaderConst(fileDialogueText[n], "feather", 8, 0, 0, 0);
			textManager->setShaderConst(fileDialogueText[n], "scissor", importMeshWindowPosition.x, importMeshWindowPosition.y + 20, importMeshWindowPosition.x + 300, importMeshWindowPosition.y + 390);
			textManager->setPosition(fileDialogueText[n], importMeshWindowPosition + glm::vec2(10, 20 + 18 * (n)-(1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio));
		}

		//select file
		if (control->hoverRegion(importMeshWindowPosition + glm::vec2(8, 22), importMeshWindowPosition + glm::vec2(292, 398)))
		{

			float p = 20 + 18 * (slotposition)-(1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio;
			if (control->clickRegion(importMeshWindowPosition + glm::vec2(8, p), importMeshWindowPosition + glm::vec2(200, p + 18)))
			{
				//make sure this item is not already selected
				//if so, unselect it

				short alreadyselected = -1;
				if (importmesh_fileSelect[0] == slotposition + 1) alreadyselected = 0;

				// only one file can be selected at a time
				if (alreadyselected == -1)
				{
					spriteManager->setPosition(importmesh_fileHighlight[0], importMeshWindowPosition + glm::vec2(8, p));
					spriteManager->setVisible(importmesh_fileHighlight[0], true);
					spriteManager->setFeather(importmesh_fileHighlight[0], 8.f);
					spriteManager->setScissor(importmesh_fileHighlight[0], importMeshWindowPosition + glm::vec2(4, 20), importMeshWindowPosition + glm::vec2(292, 392));

					importmesh_fileSelect[0] = slotposition + 1;
					fileSelectCount = 1;
				}
				else
				{
					spriteManager->setVisible(importmesh_fileHighlight[0], false);
					fileSelectCount = 0;
				}
			}
		}

		for (int n = 0; n < fileSelectCount; n++)
		{
			float p = 20 + 18 * (importmesh_fileSelect[n] - 1) - (1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio;
			spriteManager->setPosition(importmesh_fileHighlight[n], importMeshWindowPosition + glm::vec2(8, p));
			spriteManager->setScissor(importmesh_fileHighlight[n], importMeshWindowPosition + glm::vec2(4, 20), importMeshWindowPosition + glm::vec2(292, 392));

		}

		if (fileSelectCount > 0)
		{
			textManager->setColor(fileDialogueWindowText[1], glm::vec4(1));
		}
		else
		{
			textManager->setColor(fileDialogueWindowText[1], glm::vec4(.5, .5, .5, 1));
		}

		//close open scene window
		if (control->clickRegion(glm::vec2(284, 0) + importMeshWindowPosition, glm::vec2(300, 16) + importMeshWindowPosition))
		{
			closeSaveAsWindow();
		}
	}

	// while import mesh window is open...
	if (importMeshDialogueOpen)
	{
		if (control->grabRegion(importMeshWindowPosition, importMeshWindowPosition + glm::vec2(300, 400)))
			interactWindow = true;

		if (control->grabRegion(importMeshWindowPosition, importMeshWindowPosition + glm::vec2(284, 16)))
			grabWindow = true;

		if (!control->mouse_leftDown)
		{
			//interact_importMeshWindow = false;
			grabWindow = false;
		}

		if (grabWindow)
		{
			importMeshWindowPosition += control->cursorPos - control->lastCursorPos;
			if (importMeshWindowPosition.y <= 0)
				importMeshWindowPosition.y = 0;
			if (importMeshWindowPosition.y >= windowSize.y - 52)
				importMeshWindowPosition.y = windowSize.y - 52;

			if (importMeshWindowPosition.x <= -250)
				importMeshWindowPosition.x = -250;
			if (importMeshWindowPosition.x >= windowSize.x - 50)
				importMeshWindowPosition.x = windowSize.x - 50;

			spriteManager->setPosition(fileDialoguePanel[0], importMeshWindowPosition);
			spriteManager->setPosition(fileDialoguePanel[1], importMeshWindowPosition + glm::vec2(284, 0));
			spriteManager->setPosition(fileDialoguePanel[2], importMeshWindowPosition);
			spriteManager->setPosition(fileDialoguePanel[3], importMeshWindowPosition + glm::vec2(0, 20));
			spriteManager->setPosition(fileDialoguePanel[4], importMeshWindowPosition + glm::vec2(292, 20));
			spriteManager->setPosition(fileDialoguePanel[5], importMeshWindowPosition + glm::vec2(0, 392));
			spriteManager->setPosition(fileDialoguePanel[6], importMeshWindowPosition + glm::vec2(284, 20));

			textManager->setPosition(fileDialogueWindowText[0], importMeshWindowPosition + glm::vec2(5, 0));
			textManager->setPosition(fileDialogueWindowText[1], importMeshWindowPosition + glm::vec2(200, 400));

		}


		float scrollSizeRatio = glm::min(1.0f, 380.0f / (18 * meshfileCount));
		// move scrollbar with mouse
		if (!grabWindow)
		{
			if (control->grabRegion(importMeshWindowPosition + glm::vec2(284, 20), importMeshWindowPosition + glm::vec2(292, 370)))
				grabScrollBar = true;

			if (!control->mouse_leftDown)
				grabScrollBar = false;

			if (grabScrollBar)
			{
				importMeshScrollPos += control->cursorPos.y - control->lastCursorPos.y;

			}
		}

		float scrollposratio;

		if (scrollSizeRatio < 1.0)
		{
			if (importMeshScrollPos <= 0) importMeshScrollPos = 0;
			if (importMeshScrollPos + 370 * scrollSizeRatio >= 370) importMeshScrollPos = 370 * (1 - scrollSizeRatio);
			scrollposratio = importMeshScrollPos / (370 * (1 - scrollSizeRatio));
		}
		else
		{
			importMeshScrollPos = 0;
			scrollposratio = 0;
		}
		spriteManager->setSize(fileDialoguePanel[6], glm::vec2(8, 370) * glm::vec2(1, scrollSizeRatio));
		spriteManager->setPosition(fileDialoguePanel[6], importMeshWindowPosition + glm::vec2(284, 20 + importMeshScrollPos));

		int slotposition = floor((float(control->cursorPos.y) - importMeshWindowPosition.y - 22 + (1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio) / 18.0);
		slotposition = glm::min(glm::max(0, slotposition), 40);

		for (int n = 0; n < 40; n++)
		{
			textManager->setShaderConst(fileDialogueText[n], "feather", 8, 0, 0, 0);
			textManager->setShaderConst(fileDialogueText[n], "scissor", importMeshWindowPosition.x, importMeshWindowPosition.y + 20, importMeshWindowPosition.x + 300, importMeshWindowPosition.y + 390);
			textManager->setPosition(fileDialogueText[n], importMeshWindowPosition + glm::vec2(10, 20 + 18 * (n)-(1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio));
		}

		//select file
		if (control->hoverRegion(importMeshWindowPosition + glm::vec2(8, 22), importMeshWindowPosition + glm::vec2(292, 398)))
		{

			float p = 20 + 18 * (slotposition)-(1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio;
			if (control->clickRegion(importMeshWindowPosition + glm::vec2(8, p), importMeshWindowPosition + glm::vec2(200, p + 18)))
			{
				//make sure this item is not already selected
				//if so, unselect it

				short alreadyselected = -1;
				for (int n = 0; n < fileSelectCount; n++)
				{
					if (importmesh_fileSelect[n] == slotposition + 1) alreadyselected = n;
				}

				if (alreadyselected == -1)
				{
					spriteManager->setPosition(importmesh_fileHighlight[fileSelectCount], importMeshWindowPosition + glm::vec2(8, p));
					spriteManager->setVisible(importmesh_fileHighlight[fileSelectCount], true);
					spriteManager->setFeather(importmesh_fileHighlight[fileSelectCount], 8.f);
					spriteManager->setScissor(importmesh_fileHighlight[fileSelectCount], importMeshWindowPosition + glm::vec2(4, 20), importMeshWindowPosition + glm::vec2(292, 392));

					importmesh_fileSelect[fileSelectCount] = slotposition + 1;
					fileSelectCount++;
				}
				else
				{

					importmesh_fileSelect[alreadyselected] = importmesh_fileSelect[fileSelectCount - 1];
					float newpos = 20 + 18 * (importmesh_fileSelect[fileSelectCount - 1]) - (1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio;
					spriteManager->setPosition(importmesh_fileHighlight[alreadyselected], importMeshWindowPosition + glm::vec2(8, newpos));

					spriteManager->setVisible(importmesh_fileHighlight[fileSelectCount - 1], false);
					fileSelectCount--;
				}
			}
		}

		for (int n = 0; n < fileSelectCount; n++)
		{
			float p = 20 + 18 * (importmesh_fileSelect[n] - 1) - (1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio;
			spriteManager->setPosition(importmesh_fileHighlight[n], importMeshWindowPosition + glm::vec2(8, p));
			spriteManager->setScissor(importmesh_fileHighlight[n], importMeshWindowPosition + glm::vec2(4, 20), importMeshWindowPosition + glm::vec2(292, 392));

		}



		if (fileSelectCount > 0)
		{
			textManager->setColor(fileDialogueWindowText[1], glm::vec4(1));

			if (control->clickRegion(glm::vec2(200, 400) + importMeshWindowPosition, glm::vec2(300, 416) + importMeshWindowPosition))
			{
				char *str = textManager->textArray[fileDialogueText[importmesh_fileSelect[0] - 1]].str;
				int temp = meshManager->loadMesh(str);

				meshManager->setTexture(temp, defaultTexture, 0);
				meshManager->setTexture(temp, defaultTexture, 1);
				meshManager->meshArray[temp].texture[15] = shadowmapcomp_depth;
				meshManager->setShader(temp, defaultShader);

				closeImportMeshWindow();
			}
		}
		else
		{
			textManager->setColor(fileDialogueWindowText[1], glm::vec4(.5, .5, .5, 1));
		}

		//close import window
		if (control->clickRegion(glm::vec2(284, 0) + importMeshWindowPosition, glm::vec2(300, 16) + importMeshWindowPosition))
		{
			closeImportMeshWindow();
		}
	}

	// while export mesh window is open...
	if (exportMeshDialogueOpen)
	{
		if (control->grabRegion(importMeshWindowPosition, importMeshWindowPosition + glm::vec2(300, 400)))
			interactWindow = true;

		if (control->grabRegion(importMeshWindowPosition, importMeshWindowPosition + glm::vec2(284, 16)))
			grabWindow = true;

		if (!control->mouse_leftDown)
		{
			grabWindow = false;
		}

		if (grabWindow)
		{
			importMeshWindowPosition += control->cursorPos - control->lastCursorPos;
			if (importMeshWindowPosition.y <= 0)
				importMeshWindowPosition.y = 0;
			if (importMeshWindowPosition.y >= windowSize.y - 52)
				importMeshWindowPosition.y = windowSize.y - 52;

			if (importMeshWindowPosition.x <= -250)
				importMeshWindowPosition.x = -250;
			if (importMeshWindowPosition.x >= windowSize.x - 50)
				importMeshWindowPosition.x = windowSize.x - 50;

			spriteManager->setPosition(fileDialoguePanel[0], importMeshWindowPosition);
			spriteManager->setPosition(fileDialoguePanel[1], importMeshWindowPosition + glm::vec2(284, 0));
			spriteManager->setPosition(fileDialoguePanel[2], importMeshWindowPosition);
			spriteManager->setPosition(fileDialoguePanel[3], importMeshWindowPosition + glm::vec2(0, 20));
			spriteManager->setPosition(fileDialoguePanel[4], importMeshWindowPosition + glm::vec2(292, 20));
			spriteManager->setPosition(fileDialoguePanel[5], importMeshWindowPosition + glm::vec2(0, 392));
			spriteManager->setPosition(fileDialoguePanel[6], importMeshWindowPosition + glm::vec2(284, 20));

			textManager->setPosition(fileDialogueWindowText[0], importMeshWindowPosition + glm::vec2(5, 0));
			textManager->setPosition(fileDialogueWindowText[1], importMeshWindowPosition + glm::vec2(200, 400));

		}


		float scrollSizeRatio = glm::min(1.0f, 380.0f / (18 * meshfileCount));
		// move scrollbar with mouse
		if (!grabWindow)
		{
			if (control->grabRegion(importMeshWindowPosition + glm::vec2(284, 20), importMeshWindowPosition + glm::vec2(292, 370)))
				grabScrollBar = true;

			if (!control->mouse_leftDown)
				grabScrollBar = false;

			if (grabScrollBar)
			{
				importMeshScrollPos += control->cursorPos.y - control->lastCursorPos.y;

			}
		}

		float scrollposratio;

		if (scrollSizeRatio < 1.0)
		{
			if (importMeshScrollPos <= 0) importMeshScrollPos = 0;
			if (importMeshScrollPos + 370 * scrollSizeRatio >= 370) importMeshScrollPos = 370 * (1 - scrollSizeRatio);
			scrollposratio = importMeshScrollPos / (370 * (1 - scrollSizeRatio));
		}
		else
		{
			importMeshScrollPos = 0;
			scrollposratio = 0;
		}
		spriteManager->setSize(fileDialoguePanel[6], glm::vec2(8, 370) * glm::vec2(1, scrollSizeRatio));
		spriteManager->setPosition(fileDialoguePanel[6], importMeshWindowPosition + glm::vec2(284, 20 + importMeshScrollPos));

		int slotposition = floor((float(control->cursorPos.y) - importMeshWindowPosition.y - 22 + (1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio) / 18.0);
		slotposition = glm::min(glm::max(0, slotposition), 40);

		for (int n = 0; n < 40; n++)
		{
			textManager->setShaderConst(fileDialogueText[n], "feather", 8, 0, 0, 0);
			textManager->setShaderConst(fileDialogueText[n], "scissor", importMeshWindowPosition.x, importMeshWindowPosition.y + 20, importMeshWindowPosition.x + 300, importMeshWindowPosition.y + 390);
			textManager->setPosition(fileDialogueText[n], importMeshWindowPosition + glm::vec2(10, 20 + 18 * (n)-(1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio));
		}

		//select file
		if (control->hoverRegion(importMeshWindowPosition + glm::vec2(8, 22), importMeshWindowPosition + glm::vec2(292, 398)))
		{

			float p = 20 + 18 * (slotposition)-(1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio;
			if (control->clickRegion(importMeshWindowPosition + glm::vec2(8, p), importMeshWindowPosition + glm::vec2(200, p + 18)))
			{
				//make sure this item is not already selected
				//if so, unselect it

				short alreadyselected = -1;
				if (importmesh_fileSelect[0] == slotposition + 1) alreadyselected = 0;

				// only one file can be selected at a time
				if (alreadyselected == -1)
				{
					spriteManager->setPosition(importmesh_fileHighlight[0], importMeshWindowPosition + glm::vec2(8, p));
					spriteManager->setVisible(importmesh_fileHighlight[0], true);
					spriteManager->setFeather(importmesh_fileHighlight[0], 8.f);
					spriteManager->setScissor(importmesh_fileHighlight[0], importMeshWindowPosition + glm::vec2(4, 20), importMeshWindowPosition + glm::vec2(292, 392));

					importmesh_fileSelect[0] = slotposition + 1;
					fileSelectCount = 1;
				}
				else
				{
					spriteManager->setVisible(importmesh_fileHighlight[0], false);
					fileSelectCount = 0;
				}
			}
		}

		for (int n = 0; n < fileSelectCount; n++)
		{
			float p = 20 + 18 * (importmesh_fileSelect[n] - 1) - (1.0 / scrollSizeRatio - 1.0) * 370 * scrollposratio;
			spriteManager->setPosition(importmesh_fileHighlight[n], importMeshWindowPosition + glm::vec2(8, p));
			spriteManager->setScissor(importmesh_fileHighlight[n], importMeshWindowPosition + glm::vec2(4, 20), importMeshWindowPosition + glm::vec2(292, 392));

		}

		if (fileSelectCount > 0)
		{
			textManager->setColor(fileDialogueWindowText[1], glm::vec4(1));
		}
		else
		{
			textManager->setColor(fileDialogueWindowText[1], glm::vec4(.5, .5, .5, 1));
		}

		//close import window
		if (control->clickRegion(glm::vec2(284, 0) + importMeshWindowPosition, glm::vec2(300, 16) + importMeshWindowPosition))
		{
			closeExportMeshWindow();
		}
	}

	// set modification type (move, rotate, scale)
	// translation mode
	if (control->clickRegion(glm::vec2(windowSize.x - 290, 24), glm::vec2(windowSize.x - 200, 48)))
	{
		spriteManager->setColor(selectMeshPanel[0], glm::vec4(.5, .5, 0, .8));
		spriteManager->setColor(selectMeshPanel[1], glm::vec4(.3, .3, .4, .8));
		spriteManager->setColor(selectMeshPanel[2], glm::vec4(.3, .3, .4, .8));
		modificationState = EDIT_MOVE;

	}

	// rotation mode
	if (control->clickRegion(glm::vec2(windowSize.x - 190, 24), glm::vec2(windowSize.x - 100, 48)))
	{
		spriteManager->setColor(selectMeshPanel[0], glm::vec4(.3, .3, .4, .8));
		spriteManager->setColor(selectMeshPanel[1], glm::vec4(.5, .5, 0, .8));
		spriteManager->setColor(selectMeshPanel[2], glm::vec4(.3, .3, .4, .8));
		modificationState = EDIT_ROTATE;

	}

	// scaling mode
	if (control->clickRegion(glm::vec2(windowSize.x - 90, 24), glm::vec2(windowSize.x - 0, 48)))
	{
		spriteManager->setColor(selectMeshPanel[0], glm::vec4(.3, .3, .4, .8));
		spriteManager->setColor(selectMeshPanel[1], glm::vec4(.3, .3, .4, .8));
		spriteManager->setColor(selectMeshPanel[2], glm::vec4(.5, .5, .0, .8));
		modificationState = EDIT_SCALE;

	}

	// rotating mode
	if (modificationState == EDIT_ROTATE)
	{
		meshManager->setVisible(axis_arrow[0], false);
		meshManager->setVisible(axis_arrow[1], false);
		meshManager->setVisible(axis_arrow[2], false);

		meshManager->setVisible(radial_axis[0], true);
		meshManager->setVisible(radial_axis[1], true);
		meshManager->setVisible(radial_axis[2], true);
	}
	else
	{
		meshManager->setVisible(axis_arrow[0], true);
		meshManager->setVisible(axis_arrow[1], true);
		meshManager->setVisible(axis_arrow[2], true);

		meshManager->setVisible(radial_axis[0], false);
		meshManager->setVisible(radial_axis[1], false);
		meshManager->setVisible(radial_axis[2], false);
	}

	// set modification control plane (xy,xz,yz)
	//xy
	if (control->clickRegion(glm::vec2(0, 0) + axisControlPanelPos, glm::vec2(32, 31) + axisControlPanelPos))
	{
		if (!move_xy_buttonState)
		{
			spriteManager->setColor(move_xy_buttonSpriteIndex, glm::vec4(1, 1, 0, 1));
			spriteManager->setColor(move_xz_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			spriteManager->setColor(move_yz_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			spriteManager->setColor(move_screen_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			move_xy_buttonState = true;
			move_xz_buttonState = false;
			move_yz_buttonState = false;
			move_screen_buttonState = false;
		}
		else
		{
			spriteManager->setColor(move_xy_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			move_xy_buttonState = false;
		}
	}

	//xz
	if (control->clickRegion(glm::vec2(0, 32) + axisControlPanelPos, glm::vec2(32, 63) + axisControlPanelPos))
	{
		if (!move_xz_buttonState)
		{
			spriteManager->setColor(move_xy_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			spriteManager->setColor(move_xz_buttonSpriteIndex, glm::vec4(1, 1, 0, 1));
			spriteManager->setColor(move_yz_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			spriteManager->setColor(move_screen_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			move_xy_buttonState = false;
			move_xz_buttonState = true;
			move_yz_buttonState = false;
			move_screen_buttonState = false;
		}
		else
		{
			spriteManager->setColor(move_xz_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			move_xz_buttonState = false;
		}
	}

	//yz
	if (control->clickRegion(glm::vec2(0, 64) + axisControlPanelPos, glm::vec2(32, 95) + axisControlPanelPos))
	{
		if (!move_yz_buttonState)
		{
			spriteManager->setColor(move_xy_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			spriteManager->setColor(move_xz_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			spriteManager->setColor(move_yz_buttonSpriteIndex, glm::vec4(1, 1, 0, 1));
			spriteManager->setColor(move_screen_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			move_xy_buttonState = false;
			move_xz_buttonState = false;
			move_yz_buttonState = true;
			move_screen_buttonState = false;
		}
		else
		{
			spriteManager->setColor(move_yz_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			move_yz_buttonState = false;
		}
	}
	// screen view as movement plane
	if (control->clickRegion(glm::vec2(0, 96) + axisControlPanelPos, glm::vec2(32, 127) + axisControlPanelPos))
	{
		if (!move_screen_buttonState)
		{
			spriteManager->setColor(move_xy_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			spriteManager->setColor(move_xz_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			spriteManager->setColor(move_yz_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			spriteManager->setColor(move_screen_buttonSpriteIndex, glm::vec4(1, 1, 0, 1));
			move_xy_buttonState = false;
			move_xz_buttonState = false;
			move_yz_buttonState = false;
			move_screen_buttonState = true;
		}
		else
		{
			spriteManager->setColor(move_screen_buttonSpriteIndex, glm::vec4(1, 1, 1, 1));
			move_screen_buttonState = false;
		}
	}

	// select object
	if (!interactWindow)
	{
		if (glm::length(control->mouseDownPos - control->mouseUpPos) < 10 && control->downTimer > 0 && control->downTimer < 60)
		{
			glm::dvec2 pos2 = (2.0*(control->editorCursor / glm::dvec2(windowSize)) - 1.0)*glm::dvec2(1.0, -1.0);
			glm::vec3 pos3 = Engine::screenPointToVector(pos2, *mainCamera);
			pos3 = glm::normalize(pos3);
			meshManager->raycastAllMeshIgnoreStates(mainCamera->pos, mainCamera->pos + pos3 * 1400.f);

			meshManager->sortRayCastData();

			//printf("%f %f \n", meshManager->raycastDataArray[0].uv.x, meshManager->raycastDataArray[0].uv.y);

			//glm::vec4 soob = meshManager->textureValueFromUV(meshManager->raycastDataArray[0].meshInd,
			//														meshManager->raycastDataArray[0].uv,
			//														0);

			//printf(" %f %f %f %f \n", soob.r, soob.g, soob.b, soob.a);

			if (meshManager->raycastHitCount > 0)
			{
				// select one or multiple entities
				if (glfwGetKey(control->window, GLFW_KEY_LEFT_SHIFT))
				{
					if (selectCount == 0)
					{
						selectedMesh[0] = meshManager->raycastDataArray[0].meshInd;
						selectCount = 1;
					}
					else if (selectCount < 10)
					{
						char already_selected = -1;
						for (int n = 0; n < selectCount + 1; n++)
						{
							if (selectedMesh[n] == meshManager->raycastDataArray[0].meshInd)
							{
								already_selected = n;
								break;
							}
						}
						if (already_selected == -1 || already_selected == selectCount)
							selectedMesh[selectCount++] = meshManager->raycastDataArray[0].meshInd;

					}
				}

				// unselect specific entity
				if (glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL))
				{
					if (selectCount > 0)
					{
						char already_selected = -1;
						for (int n = 0; n < selectCount; n++)
						{
							if (selectedMesh[n] == meshManager->raycastDataArray[0].meshInd)
							{
								already_selected = n;
								break;
							}
						}
						if (already_selected >= 0)
						{
							if (already_selected == selectCount - 1)
							{
								selectedMesh[selectCount - 1] = -1;
								selectCount--;
							}
							else
							{
								selectedMesh[already_selected] = selectedMesh[selectCount - 1];
								selectCount--;
							}
						}
					}
				}

				// select one entity
				if (!glfwGetKey(control->window, GLFW_KEY_LEFT_SHIFT) && !glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL))
				{
					selectedMesh[0] = meshManager->raycastDataArray[0].meshInd;
					selectCount = 1;
				}


				// add history entry
				for (int n = 0; n < selectCount; n++)
				{
					editHistory[editHistoryStep].meshIndex[n] = selectedMesh[n];
					editHistory[editHistoryStep].pos[n] = meshManager->meshArray[selectedMesh[n]].pos;
					editHistory[editHistoryStep].rot[n] = meshManager->meshArray[selectedMesh[n]].rot;
					editHistory[editHistoryStep].scale[n] = meshManager->meshArray[selectedMesh[n]].scale;
				}
				editHistoryStep++;
				editHistoryMax = editHistoryStep;
			}

			updateAxis();

		}
	}

	if (selectCount > 0)
	{
		if (selectCount == 1)
			textManager->setString(selectMeshText[1], meshManager->meshArray[selectedMesh[0]].name);
		else
			textManager->setString(selectMeshText[1], "Multiple Selected");
	}
	else
	{
		textManager->setString(selectMeshText[1], "None");
	}
	
	// update mesh data
	// reload object
	if (selectCount == 1)
	{
		if (!loadingMesh)
		{
			if (glfwGetKey(control->window, GLFW_KEY_I) && glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL))
			{
				int index = selectedMesh[0];
				char tempstr[1024];
				strcpy(tempstr, meshManager->meshArray[index].name);
				strcat(tempstr, ".obj");


				printf("Reloading mesh : %s \n", tempstr);
				
				int temp = meshManager->loadMesh(tempstr);
				

				//meshManager->setTexture(temp, meshManager->meshArray[index].texture[0], 0);
				//meshManager->setTexture(temp, meshManager->meshArray[index].texture[1], 1);
				for (int n = 0; n<16; n++)
					meshManager->meshArray[temp].texture[n] = meshManager->meshArray[index].texture[n];

				meshManager->setShader(temp, meshManager->meshArray[index].shaderIndex);

				meshManager->meshArray[temp].collision = meshManager->meshArray[index].collision;
				meshManager->meshArray[temp].visible = meshManager->meshArray[index].visible;
				meshManager->meshArray[temp].culling = meshManager->meshArray[index].culling;
				meshManager->meshArray[temp].transparent = meshManager->meshArray[index].transparent;
				meshManager->meshArray[temp].castShadow = meshManager->meshArray[index].castShadow;
				meshManager->meshArray[temp].lookatCam = meshManager->meshArray[index].lookatCam;
				meshManager->meshArray[temp].lookatPos = meshManager->meshArray[index].lookatPos;
				meshManager->meshArray[temp].lookatPosState = meshManager->meshArray[index].lookatPosState;
				meshManager->meshArray[temp].follow = meshManager->meshArray[index].follow;
				meshManager->meshArray[temp].reflectable = meshManager->meshArray[index].reflectable;
				meshManager->meshArray[temp].isWater = meshManager->meshArray[index].isWater;

				meshManager->meshArray[temp].useVertexLight = meshManager->meshArray[index].useVertexLight;

				meshManager->meshArray[temp].pos = meshManager->meshArray[index].pos;
				meshManager->meshArray[temp].scale = meshManager->meshArray[index].scale;
				meshManager->meshArray[temp].rot = meshManager->meshArray[index].rot;

				meshManager->meshArray[temp].constantCount = meshManager->meshArray[index].constantCount;
				meshManager->meshArray[temp].constantMatCount = meshManager->meshArray[index].constantMatCount;

				for (int n = 0; n < meshManager->meshArray[index].constantCount; n++)
					meshManager->meshArray[temp].constantArray[n] = meshManager->meshArray[index].constantArray[n];

				for (int n = 0; n < meshManager->meshArray[index].constantMatCount; n++)
					meshManager->meshArray[temp].constantMatArray[n] = meshManager->meshArray[index].constantMatArray[n];


				//meshManager->meshArray[index].clear();
				meshManager->meshArray[index] = meshManager->meshArray[temp];
				meshManager->meshCount--;
				selectedMesh[0] = temp;
				loadingMesh = true;
				
			}
		}
		if (!glfwGetKey(control->window, GLFW_KEY_I))
			loadingMesh = false;
		//meshManager->meshArray[selectedMesh[0]]
	}


	//focus on mesh
	if (glfwGetKey(control->window, GLFW_KEY_Z) && !glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL))
	{
		if (selectCount > 0)
			focusOnMesh(selectedMesh[0]);
	}


	// attach meshes together
	if (glfwGetKey(control->window, GLFW_KEY_A) && glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL) && !meshAttached)
	{
		if (selectCount == 2)
		{
			int a = meshManager->attach(selectedMesh[0], selectedMesh[1]);
			int lookforID = meshManager->getID(a);
			int b = selectedMesh[0];
			int c = selectedMesh[1];

			meshAttached = true;
			meshManager->deleteMesh(b);
			meshManager->deleteMesh(c);
			for (int n = 0; n < meshManager->meshCount; n++)
				if (meshManager->getID(n) == lookforID)
				{
					selectedMesh[0] = n;
					selectCount = 1;
					break;
				}
		}
	}

	if (!glfwGetKey(control->window, GLFW_KEY_A) || !glfwGetKey(control->window, GLFW_KEY_LEFT_CONTROL))
	{
		meshAttached = false;
	}

	// modify entities
	if (!interactWindow)
	{
		// move selected entities 
		if (modificationState == EDIT_MOVE)
		{
			if (glfwGetMouseButton(control->window, GLFW_MOUSE_BUTTON_1))
			{
				glm::vec3 planeNorm;
				if (move_xy_buttonState)
					planeNorm = glm::vec3(0, 0, 1);
				if (move_xz_buttonState)
					planeNorm = glm::vec3(0, 1, 0);
				if (move_yz_buttonState)
					planeNorm = glm::vec3(1, 0, 0);
				if (move_screen_buttonState)
					planeNorm = -mainCamera->forward;

				glm::dvec2 pos2 = (2.0*(control->editorCursor / glm::dvec2(windowSize)) - 1.0)*glm::dvec2(1, -1);
				glm::vec3 pos3 = Engine::screenPointToVector(pos2, *mainCamera);
				pos3 = glm::normalize(pos3);

				// translate mesh
				if (!startMove)
				{
					if (control->downTimer < 2)
					{
						meshManager->raycastAllMeshIgnoreStates(mainCamera->pos, mainCamera->pos + pos3 * 1400.f);
						meshManager->sortRayCastData();
						if (meshManager->raycastHitCount > 0)
						{
							for (int n = 0; n < selectCount; n++)
							{
								if (selectedMesh[n] == meshManager->raycastDataArray[0].meshInd)
								{
									startpos = pos3;
									for (int i = 0; i < selectCount; i++)
									{

										hitpos[i] = Engine::raycastPlane(mainCamera->pos, mainCamera->pos + startpos * 1400.f, planeNorm, meshManager->raycastDataArray[0].pos);
										dif[i] = hitpos[i] - meshManager->meshArray[selectedMesh[i]].pos;
									}

									startMove = true;
									break;
								}
							}
						}
					}
				}
				else
				{
					for (int n = 0; n < selectCount; n++)
					{
						hitpos[n] = Engine::raycastPlane(mainCamera->pos, mainCamera->pos + pos3 * 1400.f, planeNorm, meshManager->raycastDataArray[0].pos);
					}
				}

				if (startMove)
				{
					lockCam = true;
					for (int n = 0; n < selectCount; n++)
					{
						if (glm::length(hitpos[n] - glm::vec3(-1)) > .01)
						{

							meshManager->meshArray[selectedMesh[n]].pos = hitpos[n] - glm::vec3(dif[n].x, dif[n].y, dif[n].z);;
							meshManager->meshArray[selectedMesh[n]].updateModelMatrix();

							updateAxis();
						}
					}
				}
			}
			else
			{
				if (startMove)
				{
					for (int n = 0; n < selectCount; n++)
					{
						editHistory[editHistoryStep].meshIndex[n] = selectedMesh[n];
						editHistory[editHistoryStep].pos[n] = meshManager->meshArray[selectedMesh[n]].pos;
						editHistory[editHistoryStep].rot[n] = meshManager->meshArray[selectedMesh[n]].rot;
						editHistory[editHistoryStep].scale[n] = meshManager->meshArray[selectedMesh[n]].scale;
					}
					editHistoryStep++;
					editHistoryMax = editHistoryStep;

				}
				startMove = false;
				lockCam = false;
			}
		}//endif modificationstate move

		 // rotate selected entities
		if (modificationState == EDIT_ROTATE)
		{
			if (glfwGetMouseButton(control->window, GLFW_MOUSE_BUTTON_1))
			{
				glm::dvec2 pos2 = (2.0*(control->editorCursor / glm::dvec2(windowSize)) - 1.0)*glm::dvec2(1, -1);
				glm::vec3 pos3 = Engine::screenPointToVector(pos2, *mainCamera);
				pos3 = glm::normalize(pos3);
				if (!startMove)
				{
					if (control->downTimer < 2)
					{
						meshManager->raycastAllMeshIgnoreStates(mainCamera->pos, mainCamera->pos + pos3 * 1400.f);
						meshManager->sortRayCastData();
						if (meshManager->raycastHitCount > 0)
						{
							for (int n = 0; n < selectCount; n++)
							{
								if (selectedMesh[n] == meshManager->raycastDataArray[0].meshInd)
								{
									startMove = true;
									break;
								}
							}
						}
					}
				}

				if (startMove)
				{
					lockCam = true;
					for (int n = 0; n < selectCount; n++)
					{
						if (glm::length(hitpos[n] - glm::vec3(-1)) > .01f)
						{
							if (move_xy_buttonState)
							{
								meshManager->meshArray[selectedMesh[n]].rot.x += .5f*(control->cursorPos.y - control->lastCursorPos.y);
							}
							if (move_xz_buttonState)
							{
								meshManager->meshArray[selectedMesh[n]].rot.y += .5f*(control->cursorPos.y - control->lastCursorPos.y);
							}
							if (move_yz_buttonState)
							{
								meshManager->meshArray[selectedMesh[n]].rot.z += .5f*(control->cursorPos.y - control->lastCursorPos.y);
							}
							meshManager->meshArray[selectedMesh[n]].updateModelMatrix();
						}
					}
				}
			}
			else
			{
				if (startMove)
				{
					for (int n = 0; n < selectCount; n++)
					{
						editHistory[editHistoryStep].meshIndex[n] = selectedMesh[n];
						editHistory[editHistoryStep].pos[n] = meshManager->meshArray[selectedMesh[n]].pos;
						editHistory[editHistoryStep].rot[n] = meshManager->meshArray[selectedMesh[n]].rot;
						editHistory[editHistoryStep].scale[n] = meshManager->meshArray[selectedMesh[n]].scale;
					}
					editHistoryStep++;
					editHistoryMax = editHistoryStep;

				}
				startMove = false;
				lockCam = false;
			}
		}

		// scale selected entities
		if (modificationState == EDIT_SCALE)
		{
			if (glfwGetMouseButton(control->window, GLFW_MOUSE_BUTTON_1))
			{

				glm::dvec2 pos2 = (2.0*(control->editorCursor / glm::dvec2(windowSize)) - 1.0)*glm::dvec2(1, -1);
				glm::vec3 pos3 = Engine::screenPointToVector(pos2, *mainCamera);
				pos3 = glm::normalize(pos3);
				if (!startMove)
				{
					if (control->downTimer < 2)
					{
						meshManager->raycastAllMeshIgnoreStates(mainCamera->pos, mainCamera->pos + pos3 * 1400.f);
						meshManager->sortRayCastData();
						if (meshManager->raycastHitCount > 0)
						{
							for (int n = 0; n < selectCount; n++)
							{
								if (selectedMesh[n] == meshManager->raycastDataArray[0].meshInd)
								{
									startMove = true;
									break;
								}
							}
						}
					}
				}

				if (startMove)
				{
					lockCam = true;
					for (int n = 0; n < selectCount; n++)
					{
						if (glm::length(hitpos[n] - glm::vec3(-1)) > .01)
						{
							if (move_xy_buttonState)
							{
								meshManager->meshArray[selectedMesh[n]].scale.x += .1*(control->cursorPos.x - control->lastCursorPos.x);
								if (meshManager->meshArray[selectedMesh[n]].scale.x < .001) meshManager->meshArray[selectedMesh[n]].scale.x = .001;

								meshManager->meshArray[selectedMesh[n]].scale.y -= .1*(control->cursorPos.y - control->lastCursorPos.y);
								if (meshManager->meshArray[selectedMesh[n]].scale.y < .001) meshManager->meshArray[selectedMesh[n]].scale.y = .001;
							}
							if (move_xz_buttonState)
							{
								meshManager->meshArray[selectedMesh[n]].scale.x += .1*(control->cursorPos.x - control->lastCursorPos.x);
								if (meshManager->meshArray[selectedMesh[n]].scale.x < .001) meshManager->meshArray[selectedMesh[n]].scale.x = .001;

								meshManager->meshArray[selectedMesh[n]].scale.z -= .1*(control->cursorPos.y - control->lastCursorPos.y);
								if (meshManager->meshArray[selectedMesh[n]].scale.z < .001) meshManager->meshArray[selectedMesh[n]].scale.z = .001;
							}

							if (move_yz_buttonState)
							{
								meshManager->meshArray[selectedMesh[n]].scale.y -= .1*(control->cursorPos.y - control->lastCursorPos.y);
								if (meshManager->meshArray[selectedMesh[n]].scale.y < .001) meshManager->meshArray[selectedMesh[n]].scale.y = .001;

								meshManager->meshArray[selectedMesh[n]].scale.z += .1*(control->cursorPos.x - control->lastCursorPos.x);
								if (meshManager->meshArray[selectedMesh[n]].scale.z < .001) meshManager->meshArray[selectedMesh[n]].scale.z = .001;
							}
							meshManager->meshArray[selectedMesh[n]].updateModelMatrix();


						}
					}
				}
			}
			else
			{
				if (startMove)
				{
					for (int n = 0; n < selectCount; n++)
					{
						editHistory[editHistoryStep].meshIndex[n] = selectedMesh[n];
						editHistory[editHistoryStep].pos[n] = meshManager->meshArray[selectedMesh[n]].pos;
						editHistory[editHistoryStep].rot[n] = meshManager->meshArray[selectedMesh[n]].rot;
						editHistory[editHistoryStep].scale[n] = meshManager->meshArray[selectedMesh[n]].scale;
					}
					editHistoryStep++;
					editHistoryMax = editHistoryStep;

				}
				startMove = false;
				lockCam = false;
			}
		}// endif scale
	}

	// delete mesh
	if (glfwGetKey(control->window, GLFW_KEY_DELETE) && selectCount >0)
	{
		for (int n = 0; n < selectCount; n++)
		{
			meshManager->deleteMesh(selectedMesh[0]);
			selectedMesh[0] = selectedMesh[selectCount - 1];
			selectCount--;
		}
	}

	control->grabbingState = false;
	if (importMeshDialogueOpen && grabWindow)
		control->grabbingState = true;
	if (interactWindow)
		control->grabbingState = true;

}

void Editor::Cleanup()
{

	spriteManager->setVisible(move_xy_buttonSpriteIndex, false);
	spriteManager->setVisible(move_xz_buttonSpriteIndex, false);
	spriteManager->setVisible(move_yz_buttonSpriteIndex, false);
	spriteManager->setVisible(move_screen_buttonSpriteIndex, false);
	spriteManager->setVisible(movebutton_panelSpriteIndex, false);

	spriteManager->setVisible(editorRibbon[0], false);
	spriteManager->setVisible(editorRibbon[1], false);
	spriteManager->setVisible(editorRibbon[2], false);

	meshManager->setVisible(axis_arrow[0], false);
	meshManager->setVisible(axis_arrow[1], false);
	meshManager->setVisible(axis_arrow[2], false);

	for (int n = 0; n < 4; n++)
		textManager->setVisible(editorMenuText[n], false);
	for (int n = 0; n < 31; n++)
		textManager->setVisible(selectMeshText[n], false);
	for (int n = 0; n < 4; n++)
		spriteManager->setVisible(selectMeshPanel[n], false);
	for (int n = 0; n < 8; n++)
		spriteManager->setVisible(selectMeshButton[n], false);

	//close import window
	closeImportMeshWindow();
}

void Editor::openOpenSceneWindow()
{
	importMeshWindowPosition = glm::vec2(350, 30);
	importMeshScrollPos = 0;

	textManager->setString(fileDialogueWindowText[0], "Open Scene");
	textManager->setColor(fileDialogueWindowText[0], glm::vec4(1));
	textManager->setPosition(fileDialogueWindowText[0], importMeshWindowPosition + glm::vec2(5, 0));
	textManager->setVisible(fileDialogueWindowText[0], true);

	textManager->setString(fileDialogueWindowText[1], "Open");
	textManager->setColor(fileDialogueWindowText[1], glm::vec4(1));
	textManager->setPosition(fileDialogueWindowText[1], importMeshWindowPosition + glm::vec2(200, 400));
	textManager->setVisible(fileDialogueWindowText[1], true);

	for (int n = 0; n < 40; n++)
	{
		textManager->setColor(fileDialogueText[n], glm::vec4(1));
		textManager->setPosition(fileDialogueText[n], importMeshWindowPosition + glm::vec2(10, 20 + 18 * (n)));
		textManager->setVisible(fileDialogueText[n], true);
	}

	spriteManager->setVisible(fileDialoguePanel[0], true);
	spriteManager->setPosition(fileDialoguePanel[0], importMeshWindowPosition);

	spriteManager->setVisible(fileDialoguePanel[1], true);
	spriteManager->setPosition(fileDialoguePanel[1], importMeshWindowPosition + glm::vec2(284, 0));

	spriteManager->setVisible(fileDialoguePanel[2], true);
	spriteManager->setPosition(fileDialoguePanel[2], importMeshWindowPosition);

	spriteManager->setVisible(fileDialoguePanel[3], true);
	spriteManager->setPosition(fileDialoguePanel[3], importMeshWindowPosition + glm::vec2(0, 16));

	spriteManager->setVisible(fileDialoguePanel[4], true);
	spriteManager->setPosition(fileDialoguePanel[4], importMeshWindowPosition + glm::vec2(292, 16));

	spriteManager->setVisible(fileDialoguePanel[5], true);
	spriteManager->setPosition(fileDialoguePanel[5], importMeshWindowPosition + glm::vec2(0, 392));

	spriteManager->setVisible(fileDialoguePanel[6], true);
	spriteManager->setPosition(fileDialoguePanel[6], importMeshWindowPosition + glm::vec2(284, 20));

	char textpos = 0;
	boost::filesystem::path p = boost::filesystem::current_path();
	boost::filesystem::directory_iterator it{ p };
	while (it != boost::filesystem::directory_iterator{})
	{
		std::string fl = it->path().string();
		fl.erase(0, p.string().length() + 1);
		std::string ext = fl;
		ext.erase(0, fl.length() - 3);
		if (std::strcmp(ext.c_str(), "kss") == 0)
		{
			strcpy(importString, fl.c_str());
			textManager->setString(fileDialogueText[textpos++], importString);
		}

		it++;
	}
	openSceneDialogueOpen = true;
	meshfileCount = textpos;
	meshfileCount = 40;
}

void Editor::closeOpenSceneWindow()
{
	for (int n = 0; n < 10; n++)
		textManager->setVisible(fileDialogueWindowText[n], false);

	for (int n = 0; n < 40; n++)
		textManager->setVisible(fileDialogueText[n], false);

	spriteManager->setVisible(fileDialoguePanel[0], false);
	spriteManager->setVisible(fileDialoguePanel[1], false);
	spriteManager->setVisible(fileDialoguePanel[2], false);
	spriteManager->setVisible(fileDialoguePanel[3], false);
	spriteManager->setVisible(fileDialoguePanel[4], false);
	spriteManager->setVisible(fileDialoguePanel[5], false);
	spriteManager->setVisible(fileDialoguePanel[6], false);

	for (int n = 0; n < fileSelectCount; n++)
		spriteManager->setVisible(importmesh_fileHighlight[n], false);

	openSceneDialogueOpen = false;
}

void Editor::openSaveAsWindow()
{
	importMeshWindowPosition = glm::vec2(350, 30);
	importMeshScrollPos = 0;

	textManager->setString(fileDialogueWindowText[0], "Save As");
	textManager->setColor(fileDialogueWindowText[0], glm::vec4(1));
	textManager->setPosition(fileDialogueWindowText[0], importMeshWindowPosition + glm::vec2(5, 0));
	textManager->setVisible(fileDialogueWindowText[0], true);

	textManager->setString(fileDialogueWindowText[1], "save");
	textManager->setColor(fileDialogueWindowText[1], glm::vec4(1));
	textManager->setPosition(fileDialogueWindowText[1], importMeshWindowPosition + glm::vec2(200, 400));
	textManager->setVisible(fileDialogueWindowText[1], true);

	for (int n = 0; n < 40; n++)
	{
		textManager->setColor(fileDialogueText[n], glm::vec4(1));
		textManager->setPosition(fileDialogueText[n], importMeshWindowPosition + glm::vec2(10, 20 + 18 * (n)));
		textManager->setVisible(fileDialogueText[n], true);
	}

	spriteManager->setVisible(fileDialoguePanel[0], true);
	spriteManager->setPosition(fileDialoguePanel[0], importMeshWindowPosition);

	spriteManager->setVisible(fileDialoguePanel[1], true);
	spriteManager->setPosition(fileDialoguePanel[1], importMeshWindowPosition + glm::vec2(284, 0));

	spriteManager->setVisible(fileDialoguePanel[2], true);
	spriteManager->setPosition(fileDialoguePanel[2], importMeshWindowPosition);

	spriteManager->setVisible(fileDialoguePanel[3], true);
	spriteManager->setPosition(fileDialoguePanel[3], importMeshWindowPosition + glm::vec2(0, 16));

	spriteManager->setVisible(fileDialoguePanel[4], true);
	spriteManager->setPosition(fileDialoguePanel[4], importMeshWindowPosition + glm::vec2(292, 16));

	spriteManager->setVisible(fileDialoguePanel[5], true);
	spriteManager->setPosition(fileDialoguePanel[5], importMeshWindowPosition + glm::vec2(0, 392));

	spriteManager->setVisible(fileDialoguePanel[6], true);
	spriteManager->setPosition(fileDialoguePanel[6], importMeshWindowPosition + glm::vec2(284, 20));

	char textpos = 0;
	boost::filesystem::path p = boost::filesystem::current_path();
	boost::filesystem::directory_iterator it{ p };
	while (it != boost::filesystem::directory_iterator{})
	{
		std::string fl = it->path().string();
		fl.erase(0, p.string().length() + 1);
		std::string ext = fl;
		ext.erase(0, fl.length() - 3);
		if (std::strcmp(ext.c_str(), "kss") == 0)
		{
			strcpy(importString, fl.c_str());
			textManager->setString(fileDialogueText[textpos++], importString);
		}

		it++;
	}
	saveAsDialogueOpen = true;
	meshfileCount = textpos;
	meshfileCount = 40;
}

void Editor::closeSaveAsWindow()
{
	for (int n = 0; n < 10; n++)
		textManager->setVisible(fileDialogueWindowText[n], false);

	for (int n = 0; n < 40; n++)
		textManager->setVisible(fileDialogueText[n], false);

	for (int n = 0; n < 7; n++)
	{
		spriteManager->setVisible(fileDialoguePanel[0], false);
	}

	for (int n = 0; n < fileSelectCount; n++)
		spriteManager->setVisible(importmesh_fileHighlight[n], false);

	saveAsDialogueOpen = false;
}

void Editor::openImportMeshWindow()
{
	importMeshWindowPosition = glm::vec2(350, 30);
	importMeshScrollPos = 0;

	textManager->setString(fileDialogueWindowText[0], "Import Mesh");
	textManager->setColor(fileDialogueWindowText[0], glm::vec4(1));
	textManager->setPosition(fileDialogueWindowText[0], importMeshWindowPosition + glm::vec2(5, 0));
	textManager->setVisible(fileDialogueWindowText[0], true);

	textManager->setString(fileDialogueWindowText[1], "Import Selected");
	textManager->setColor(fileDialogueWindowText[1], glm::vec4(1));
	textManager->setPosition(fileDialogueWindowText[1], importMeshWindowPosition + glm::vec2(200, 400));
	textManager->setVisible(fileDialogueWindowText[1], true);

	for (int n = 0; n < 40; n++)
	{
		textManager->setColor(fileDialogueText[n], glm::vec4(1));
		textManager->setPosition(fileDialogueText[n], importMeshWindowPosition + glm::vec2(10, 20 + 18 * (n)));
		textManager->setVisible(fileDialogueText[n], true);
	}

	spriteManager->setVisible(fileDialoguePanel[0], true);
	spriteManager->setPosition(fileDialoguePanel[0], importMeshWindowPosition);

	spriteManager->setVisible(fileDialoguePanel[1], true);
	spriteManager->setPosition(fileDialoguePanel[1], importMeshWindowPosition + glm::vec2(284, 0));

	spriteManager->setVisible(fileDialoguePanel[2], true);
	spriteManager->setPosition(fileDialoguePanel[2], importMeshWindowPosition);

	spriteManager->setVisible(fileDialoguePanel[3], true);
	spriteManager->setPosition(fileDialoguePanel[3], importMeshWindowPosition + glm::vec2(0, 16));

	spriteManager->setVisible(fileDialoguePanel[4], true);
	spriteManager->setPosition(fileDialoguePanel[4], importMeshWindowPosition + glm::vec2(292, 16));

	spriteManager->setVisible(fileDialoguePanel[5], true);
	spriteManager->setPosition(fileDialoguePanel[5], importMeshWindowPosition + glm::vec2(0, 392));

	spriteManager->setVisible(fileDialoguePanel[6], true);
	spriteManager->setPosition(fileDialoguePanel[6], importMeshWindowPosition + glm::vec2(284, 20));

	char textpos = 0;
	boost::filesystem::path p = boost::filesystem::current_path();
	boost::filesystem::directory_iterator it{ p };
	while (it != boost::filesystem::directory_iterator{})
	{
		std::string fl = it->path().string();
		fl.erase(0, p.string().length() + 1);
		std::string ext = fl;
		ext.erase(0, fl.length() - 3);
		if (std::strcmp(ext.c_str(), "obj") == 0)
		{
			strcpy(importString, fl.c_str());
			textManager->setString(fileDialogueText[textpos++], importString);
			printf("%s\n", textManager->textArray[fileDialogueText[textpos - 1]].str);
		}

		it++;
	}
	importMeshDialogueOpen = true;
	meshfileCount = textpos;
	meshfileCount = 40;
}

void Editor::closeImportMeshWindow()
{
	for (int n = 0; n < 10; n++)
		textManager->setVisible(fileDialogueWindowText[n], false);

	for (int n = 0; n < 40; n++)
		textManager->setVisible(fileDialogueText[n], false);

	spriteManager->setVisible(fileDialoguePanel[0], false);
	spriteManager->setVisible(fileDialoguePanel[1], false);
	spriteManager->setVisible(fileDialoguePanel[2], false);
	spriteManager->setVisible(fileDialoguePanel[3], false);
	spriteManager->setVisible(fileDialoguePanel[4], false);
	spriteManager->setVisible(fileDialoguePanel[5], false);
	spriteManager->setVisible(fileDialoguePanel[6], false);

	for (int n = 0; n < fileSelectCount; n++)
		spriteManager->setVisible(importmesh_fileHighlight[n], false);

	importMeshDialogueOpen = false;

}

void Editor::openExportMeshWindow()
{
	importMeshWindowPosition = glm::vec2(350, 30);
	importMeshScrollPos = 0;

	textManager->setString(fileDialogueWindowText[0], "Export Mesh");
	textManager->setColor(fileDialogueWindowText[0], glm::vec4(1));
	textManager->setPosition(fileDialogueWindowText[0], importMeshWindowPosition + glm::vec2(5, 0));
	textManager->setVisible(fileDialogueWindowText[0], true);

	textManager->setString(fileDialogueWindowText[1], "Export");
	textManager->setColor(fileDialogueWindowText[1], glm::vec4(1));
	textManager->setPosition(fileDialogueWindowText[1], importMeshWindowPosition + glm::vec2(200, 400));
	textManager->setVisible(fileDialogueWindowText[1], true);

	for (int n = 0; n < 40; n++)
	{
		textManager->setColor(fileDialogueText[n], glm::vec4(1));
		textManager->setPosition(fileDialogueText[n], importMeshWindowPosition + glm::vec2(10, 20 + 18 * (n)));
		textManager->setVisible(fileDialogueText[n], true);
	}

	spriteManager->setVisible(fileDialoguePanel[0], true);
	spriteManager->setPosition(fileDialoguePanel[0], importMeshWindowPosition);

	spriteManager->setVisible(fileDialoguePanel[1], true);
	spriteManager->setPosition(fileDialoguePanel[1], importMeshWindowPosition + glm::vec2(284, 0));

	spriteManager->setVisible(fileDialoguePanel[2], true);
	spriteManager->setPosition(fileDialoguePanel[2], importMeshWindowPosition);

	spriteManager->setVisible(fileDialoguePanel[3], true);
	spriteManager->setPosition(fileDialoguePanel[3], importMeshWindowPosition + glm::vec2(0, 16));

	spriteManager->setVisible(fileDialoguePanel[4], true);
	spriteManager->setPosition(fileDialoguePanel[4], importMeshWindowPosition + glm::vec2(292, 16));

	spriteManager->setVisible(fileDialoguePanel[5], true);
	spriteManager->setPosition(fileDialoguePanel[5], importMeshWindowPosition + glm::vec2(0, 392));

	spriteManager->setVisible(fileDialoguePanel[6], true);
	spriteManager->setPosition(fileDialoguePanel[6], importMeshWindowPosition + glm::vec2(284, 20));

	char textpos = 0;
	boost::filesystem::path p = boost::filesystem::current_path();
	boost::filesystem::directory_iterator it{ p };
	while (it != boost::filesystem::directory_iterator{})
	{
		std::string fl = it->path().string();
		fl.erase(0, p.string().length() + 1);
		std::string ext = fl;
		ext.erase(0, fl.length() - 3);
		if (std::strcmp(ext.c_str(), "obj") == 0)
		{
			strcpy(importString, fl.c_str());
			textManager->setString(fileDialogueText[textpos++], importString);
		}

		it++;
	}
	exportMeshDialogueOpen = true;
	meshfileCount = textpos;
	meshfileCount = 40;
}

void Editor::closeExportMeshWindow()
{
	for (int n = 0; n < 10; n++)
		textManager->setVisible(fileDialogueWindowText[n], false);

	for (int n = 0; n < 40; n++)
		textManager->setVisible(fileDialogueText[n], false);

	spriteManager->setVisible(fileDialoguePanel[0], false);
	spriteManager->setVisible(fileDialoguePanel[1], false);
	spriteManager->setVisible(fileDialoguePanel[2], false);
	spriteManager->setVisible(fileDialoguePanel[3], false);
	spriteManager->setVisible(fileDialoguePanel[4], false);
	spriteManager->setVisible(fileDialoguePanel[5], false);
	spriteManager->setVisible(fileDialoguePanel[6], false);

	for (int n = 0; n < fileSelectCount; n++)
		spriteManager->setVisible(importmesh_fileHighlight[n], false);

	exportMeshDialogueOpen = false;
}


void Editor::focusOnMesh(int index)
{
	float dist = glm::length(meshManager->meshArray[index].maxpos - meshManager->meshArray[index].minpos) / 2.0;
	glm::vec3 dir = glm::vec3(.577, .577, -.577);
	//glm::normalize(mainCamera.pos - meshManager.meshArray[index].pos);

	glm::vec3 newpos = (meshManager->meshArray[index].maxpos - meshManager->meshArray[index].minpos) / 2.0f + meshManager->meshArray[index].minpos;
	mainCamera->pos = dist * dir + newpos;
	control->position = mainCamera->pos;

	glm::mat4 orientation = Engine::lookat(mainCamera->pos, mainCamera->pos-dir, 0);
	mainCamera->forward = -glm::vec3(orientation[0][2], orientation[1][2], orientation[2][2]);
	mainCamera->right = glm::vec3(orientation[0][0], orientation[1][0], orientation[2][0]);
	mainCamera->up = glm::vec3(orientation[0][1], orientation[1][1], orientation[2][1]);

	control->direction = mainCamera->forward;
	control->right = mainCamera->right;
	control->up = mainCamera->up;
	control->activeCamera.rot = CameraManager::lookatAngle(*mainCamera, mainCamera->pos - dir);

	return;
}

void Editor::updateAxis()
{
	for (int n = 0; n < 3; n++)
	{
		if (selectCount > 0)
		{
			meshManager->meshArray[axis_arrow[n]].pos = meshManager->meshArray[selectedMesh[0]].pos;
			meshManager->meshArray[axis_arrow[n]].updateModelMatrix();

			meshManager->meshArray[radial_axis[n]].pos = meshManager->meshArray[selectedMesh[0]].pos;
			meshManager->meshArray[radial_axis[n]].updateModelMatrix();
		}
	}
}

// create window with the given widnowConfig property
int Editor::createWindow(int prop)
{
	EditorWindow temp;

	if (prop == EDITOR_OPEN_PROJECT)
	{
		temp.disposition = EDITOR_OPEN_PROJECT;
		temp.pos = glm::vec2(200, 200);
		temp.size = glm::vec2(400);

		int tspr;
		// head
		tspr = spriteManager->createSprite(glm::vec2(400, 20));
		spriteManager->setPosition(tspr, temp.pos);
		spriteManager->setType(tspr, spriteManager->EDITOR);
		spriteManager->setColor(tspr, glm::vec4(.0, .2, .4, 1.0));
		temp.spriteList.add(tspr);

		// sides
		tspr = spriteManager->createSprite(glm::vec2(2, 360));
		spriteManager->setPosition(tspr, temp.pos + glm::vec2(0,20));
		spriteManager->setType(tspr, spriteManager->EDITOR);
		spriteManager->setColor(tspr, glm::vec4(.0, .2, .4, 1.0));
		temp.spriteList.add(tspr);

		// sides
		tspr = spriteManager->createSprite(glm::vec2(2, 360));
		spriteManager->setPosition(tspr, temp.pos + glm::vec2(398, 20));
		spriteManager->setType(tspr, spriteManager->EDITOR);
		spriteManager->setColor(tspr, glm::vec4(.0, .2, .4, 1.0));
		temp.spriteList.add(tspr);

		//body
		tspr = spriteManager->createSprite(glm::vec2(400, 360));
		spriteManager->setPosition(tspr, temp.pos + glm::vec2(0,20));
		spriteManager->setType(tspr, spriteManager->EDITOR);
		spriteManager->setColor(tspr, glm::vec4(.2, .2, .3, 0.8));
		temp.spriteList.add(tspr);

		// foot
		tspr = spriteManager->createSprite(glm::vec2(400, 20));
		spriteManager->setPosition(tspr, temp.pos + glm::vec2(0,380));
		spriteManager->setType(tspr, spriteManager->EDITOR);
		spriteManager->setColor(tspr, glm::vec4(.0, .2, .4, 1.0));
		temp.spriteList.add(tspr);

		tspr = textManager->createText("Open Project");
		textManager->setPosition(tspr, glm::vec2(200));
		textManager->setSize(tspr, 18);
		textManager->setType(tspr, textManager->EDITOR);
		temp.textList.add(tspr);

		

		temp.exists = true;
		temp.visible = true;

		return windowStack.add(temp);
	}

	if (prop == EDITOR_IMAGE_IMPORT)
	{

	}

	if (prop == EDITOR_MESH_IMPORT)
	{

	}
	
	if (prop == EDITOR_MESH_EXPORT)
	{

	}

	if (prop == EDITOR_MESH_PROPERTIES)
	{

	}

	return -1;
}

void Editor::windowProc(int windowIndex)
{
	EditorWindow *wnd = &windowStack.item[windowIndex];
	if (wnd->disposition == EDITOR_OPEN_PROJECT)
	{

		if (wnd->visible)
		{

			for (int n = 0; n < wnd->spriteList.size; n++)
				drawReqStack.add(DrawRequest(wnd->spriteList.item[n], EDITOR_SPRITE));
			for (int n = 0; n < wnd->textList.size; n++)
				drawReqStack.add(DrawRequest(wnd->textList.item[n], EDITOR_TEXT));

			for (int n = 0; n < wnd->buttonList.size; n++)
				drawReqStack.add(DrawRequest(wnd->buttonList.item[n], EDITOR_BUTTON));
			
		}

	}

}

int Editor::createButton(char *str, glm::vec2 pos, glm::vec2 size)
{
	EditorButton temp;
	temp.SpriteID[0] = spriteManager->createSprite(size);
	spriteManager->setPosition(temp.SpriteID[0], pos);
	spriteManager->setColor(temp.SpriteID[0], glm::vec4(.5, .5, .5, 1));
	spriteManager->setType(temp.SpriteID[0], EDITOR);

	temp.textID = textManager->createText(str);
	textManager->setPosition(temp.textID, pos);
	textManager->setType(temp.textID, EDITOR);

	return buttonStack.add(temp);
}
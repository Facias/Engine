#ifndef EDITOR_H_
#define EDITOR_H_

#include "Stack.h"
#include "Queue.h"
#include "glm.hpp"

#include "Text.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Sprite.h"
#include "Camera.h"
#include "controls.h"



	struct EditHistory;

	// use this to tell the engine to draw specific entity
	// when it is ready to do so
	struct DrawRequest
	{
		DrawRequest()
		{
			index = 0;
			type = 0;
		}
		DrawRequest(int ind, char tp)
		{
			index = ind;
			type = tp;
		}
		DrawRequest(DrawRequest &other)
		{
			index = other.index;
			type = other.type;
		}
		int index;
		char type;
	};

	struct EditorWindow
	{
		Stack <int> textList;
		Stack <int> spriteList;
		Stack <int> buttonList;
		
		// the disposition value will determine the layout 
		// and functionality of the window
		short int disposition;
		glm::vec2 pos;
		glm::vec2 size;


		bool exists;
		bool visible;
	};

	struct EditorButton
	{
		EditorButton()
		{
			pos = glm::vec2(100);
			size = glm::vec2(100);

			exists = true;
			isActive = true;
			pressed = false;
			visible = true;
		}

		EditorButton(glm::vec2 button_pos, glm::vec2 button_size )
		{

			pos = button_pos;
			size = button_size;

			exists = true;
			isActive = true;
			pressed = false;
			visible = true;
		}

		int textID;
		int SpriteID[2];

		// if the button is no longer in use, 
		// the exists flag should be set to false.
		// This will tell the button manager to over-write
		// the location if a new button is created
		bool exists;
		
		// active buttons can be interacted with
		// inactivity should be indicated visually
		// with faded text and panel graphics
		bool isActive;
		bool visible;
		
		// button should remain pressed until 
		// its purpose has been acounted for
		bool pressed;

		glm::vec2 pos;
		glm::vec2 size;
	};



	struct Editor
	{

		Editor()
		{};

		ENGINE::MeshManager *meshManager;
		ENGINE::TextManager *textManager;
		ENGINE::SpriteManager *spriteManager;
		ENGINE::TextureManager *textureManager;
		ENGINE::ShaderManager *shaderManager;

		
		enum type
		{
			EDITOR_TEXT,
			EDITOR_SPRITE,
			EDITOR_GEOMETRY,
			EDITOR_BUTTON
		};

		int defaultTexture = 0;
		int defaultShader = 0;

		int shadowmapcomp_depth;

		Camera *mainCamera;
		Controls *control;

		glm::vec2 windowSize; 


		void Init();
		void Proc();
		void Cleanup();

		int fpsText;

		// mouse input
		glm::dvec2 mouseLeftDown;
		glm::dvec2 mouseLeftUp;
		bool mouseLeftPressed;
		bool mouseLeftReleased;
		int mouseLeftPressTime;

		// editor stuff
		glm::vec2 axisControlPanelPos;

		Stack <EditorButton> buttonStack;
		Stack <EditorWindow> windowStack;
		enum windowConfig
		{
			EDITOR_OPEN_PROJECT,
			EDITOR_IMAGE_IMPORT,
			EDITOR_MESH_IMPORT,
			EDITOR_MESH_EXPORT,
			EDITOR_MESH_PROPERTIES
		};
		int createWindow(int prop);
		void windowProc(int windowIndex);
		void closeWindow(int windowIndex);
		Queue <DrawRequest> drawReqStack;
		//buttons
		int createButton(char *str, glm::vec2 pos, glm::vec2 size);
		

		int selectedMesh[10];
		// number of entities selected
		char selectCount;

		int editorMenuText[16];
		int editorMode;

		
		int selectMeshText[50];
		int selectMeshPanel[20];
		int selectMeshButton[20];
		bool selectMeshButtonState[20];

		char importString[256];

		//operation history
		EditHistory *editHistory;
		int editHistoryStep;
		int editHistoryMax;
		bool backstepped;

		//copy mesh
		int copySelection[10];
		int copySelectionCount;
		bool pasted;
		bool meshAttached;
		bool meshExported;

		// load/save global items
		int fileDialogueText[50];
		int fileDialoguePanel[10];
		int fileDialogueWindowText[10];

		// open scene dialogue
		bool openSceneDialogueOpen;
		void openOpenSceneWindow();
		void closeOpenSceneWindow();

		// save as dialogue
		bool saveAsDialogueOpen;
		void openSaveAsWindow();
		void closeSaveAsWindow();

		// import mesh dialogue
		bool importMeshDialogueOpen;
		Stack<char*> importMeshFilename;
		glm::vec2 importMeshWindowPosition;
		bool grabWindow;
		float importMeshScrollPos;
		bool grabScrollBar;
		bool interactWindow;
		short int importmesh_fileSelect[20];
		int importmesh_fileHighlight[20];
		char fileSelectCount;
		short int meshfileCount;

		void openImportMeshWindow();
		void closeImportMeshWindow();

		// export mesh dialogue
		bool exportMeshDialogueOpen;
		void openExportMeshWindow();
		void closeExportMeshWindow();


		// file dropdown options 
		char dropdownOpen;
		int dropdownText[10];
		int dropdownPanel[4];

		int bottomEditorText[50];
		int axis_arrow[3];
		int radial_axis[3];
		void updateAxis();

		int editorRibbon[4];

		bool loadingMesh;

		// editor camera functions
		void focusOnMesh(int index);
		bool startMove;
		glm::vec3 startpos;
		glm::vec3 hitpos[10];
		glm::vec3 dif[10];
		bool lockCam;

		char modificationState;
		enum modifactionType
		{
			EDIT_MOVE,
			EDIT_ROTATE,
			EDIT_SCALE
		};

		int movebutton_panelSpriteIndex;

		bool move_xy_buttonState;
		int move_xy_buttonSpriteIndex;
		bool move_xz_buttonState;
		int move_xz_buttonSpriteIndex;
		bool move_yz_buttonState;
		int move_yz_buttonSpriteIndex;
		bool move_screen_buttonState;
		int move_screen_buttonSpriteIndex;

		
	};

	struct EditHistory
	{
		EditHistory()
		{
			for (int n = 0; n < 10; n++)
			{
				meshIndex[n] = -2;
				pos[n] = glm::vec3(0);
				rot[n] = glm::vec3(0);
				scale[n] = glm::vec3(1);
			}
		}
		EditHistory(const EditHistory &other)
		{
			for (int n = 0; n < 10; n++)
			{
				meshIndex[n] = other.meshIndex[n];
				pos[n] = other.pos[n];
				rot[n] = other.rot[n];
				scale[n] = other.scale[n];
			}
		}

		int meshIndex[10];
		glm::vec3 pos[10];
		glm::vec3 rot[10];
		glm::vec3 scale[10];
	};


#endif

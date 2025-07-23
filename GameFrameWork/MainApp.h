#pragma once
#include "Engine.h"

//#include <shobjidl.h>            // IFileOpenDialog
//#include <filesystem>      
     
//class GameTimer;

class MainApp : public Window
{
private:

    void UpdateTime();
    void UpdateInput();
    void UpdateLogic();

    //void Render();
    //void LoadAssets();

    void OnResize(int width, int height);
    void OnClose();

    //void BrowseForFolder();
    //void UpdateFileList();

	//std::shared_ptr<sample::D2DRenderer> m_Renderer = nullptr;
	//GameTimer m_GameTimer;
	//AssetManager m_AssetManager;
    //
	//std::string m_pathInput;
	//std::wstring m_folderPath;
	//std::wstring m_selectedFile;
    //
	//std::wstring m_selectedAssetKey;
    //
	//std::vector<SpriteAnimator> m_curSprites;
	//std::vector<std::wstring> m_fileList;

	bool m_showFolderPanel = false;
	bool m_bChangedFile = false;

public:

    MainApp() = default;
    virtual ~MainApp() = default;

    bool Initialize();

    void Run();

    void Finalize();

    bool OnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};


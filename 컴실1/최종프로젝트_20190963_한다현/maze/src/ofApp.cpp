/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.

	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("Maze Example"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	//isdfs, isbfs, isplay, isOpen 모두 false로 설정
	isdfs = false;
	isbfs = false; 
	isplay = 0; 
	isOpen = 0; 
	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 12, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	//
	// Create a menu using ofxWinMenu
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "File");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	menu->AddPopupItem(hPopup, "Open", false, false); // Not checked and not auto-checked

	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// View popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "View");

	bShowInfo = true;  // screen info display on
	menu->AddPopupItem(hPopup, "Show DFS", false, false); // Checked
	bTopmost = false; // app is topmost
	menu->AddPopupItem(hPopup, "Show BFS", false, false); // Not checked (default)
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	//
	//Play popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Play"); //play 팝업 메뉴 생성
	menu->AddPopupItem(hPopup, "Play", false, false); // No auto check

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if (title == "Open") { //Open을 선택한 경우
		readFile(); //readFile 함수 호출

	}
	if (title == "Exit") { //Exit을 사용하였을 경우 프로그램 종료
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if (title == "Show DFS") { //Show DFS를 선택한 경우 
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen) //파일이 열렸을 경우, isbfs와 isplay를 0으로 세팅하고 DFS 함수 호출
		{
			isbfs = 0;
			isplay = 0;
			DFS(); 
			bShowInfo = bChecked;
		}
		else //파일이 열리지 않았을 경우
			cout << "you must open file first" << endl;

	}

	if (title == "Show BFS") { //Show BFS를 선택한 경우
		doTopmost(bChecked); // Use the checked value directly
		if (isOpen) //파일이 열렸을 경우, isdfs와 isplay를 0으로 세팅하고 BFS 함수 호출 
		{
			isdfs = 0;
			isplay = 0;
			BFS();
			bShowInfo = bChecked;
		}
		else //파일이 열리지 않았을 경우
			cout << "you must open file first" << endl;
	}

	if (title == "Full screen") { //Full screen을 선택한 경우 
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if (title == "About") { //About을 선택한 경우 
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

	//
	//Play menu
	//
	if (title == "Play") { //Play를 선택한 경우 
		if (isOpen) { //파일이 열렸을 경우 
			//이전에 저장된 데이터가 남아있을 수 있기 때문에 isplay를 0으로 초기화하고, p_path를 비운다.
			isplay = 0; 
			p_path.clear();  

			//isdfs와 isbfs를 0으로
			isdfs = 0;
			isbfs = 0;

			//isplay를 1로 세팅하고, play_visit을 동적할당 받고, 모든 값을 0으로 초기화 한다. 
			isplay = 1;
			play_visit = new int[maze_row * maze_col]; 
			for (int i = 0; i < maze_row * maze_col; i++) {
				play_visit[i] = 0;
			}

			//시작점인 0을 p_path에 저장하고, play_visit[0]을 1로 설정한다. 
			play_visit[0] = 1; 
			p_path.push_back(0);
		}
		else { //파일이 열리지 않은 경우 
			cout << "you must open file first" << endl;
		}
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {
	//미로를 그리는 색깔과 선의 두께를 설정
	char str[256];
	ofSetColor(100);
	ofSetLineWidth(5);
	
	// 저장된 자료구조를 이용해 미로를 그린다.
	// 미로의 HEIGHT와 WIDTH만큼 반복문을 실행하고, 이차원 배열 maze에 저장된 값이 '-'나 '|'인 경우 두 점을 연결하여 선을 그린다. 
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (maze[i][j] == '-' && i%2 == 0) { //maze[i][j] 값이 '-'이고, i가 짝수인 경우 (홀수일 때는 방의 번호가 저장되어 있기 때문)
				//두 점을 연결하여 선을 그린다. 이 때 이차원 배열 dots는 .maz 파일에서 '+'의 x, y 좌표를 저장한 구조체 배열임. 
				ofDrawLine(dots[i/2][(j + 1) / 2].x1, dots[i/2][(j + 1) / 2].y1, dots[i/2][(j - 1) / 2].x1, dots[i/2][(j - 1) / 2].y1); 
			}
			else if (maze[i][j] == '|') { //maze[i][j] 값이 '|'인 경우
				//두 점을 연결하여 선을 그린다.
				ofDrawLine(dots[(i + 1) / 2][j/2].x1, dots[(i + 1) / 2][j/2].y1, dots[(i - 1) / 2][j/2].x1, dots[(i - 1) / 2][j/2].y1);
			}
		}
	}

	if (isdfs) //isdfs가 true인 경우 (DFS 함수가 성공적으로 종료되었을 경우)
	{
		//dfs 알고리즘이 방문한 모든 경로를 그리는 선의 색깔과 두께를 설정
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen) //파일이 열렸을 경우
			dfsdraw(); //dfsdraw 함수 실행
		else //파일이 열리지 않았을 경우 
			cout << "You must open file first" << endl;
		bShowInfo = 1; //bShowInfo를 1로 설정하고, bShowInfo를 화면에 출력할 때 사용할 색깔과 두께를 설정
		ofSetColor(100); 
		ofSetLineWidth(5); 
	}
	if (isbfs) //isbfs가 true인 경우 (BFS 함수가 성공적으로 종료되었을 경우)
	{
		//dfs 알고리즘이 방문한 모든 경로를 그리는 선의 색깔과 두께를 설정
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen) //파일이 열렸을 경우
			bfsdraw(); //bfsdraw 함수 실행
		else //파일이 열리지 않았을 경우 
			cout << "You must open file first" << endl;
		bShowInfo = 1; //bShowInfo를 1로 설정하고, bShowInfo를 화면에 출력할 때 사용할 색깔과 두께를 설정
		ofSetColor(100);
		ofSetLineWidth(5);
	}
	if (isplay == 1) { //isplay가 1일 때 (play가 진행중일 때)
		//시작점과 도착점 표시
		ofSetColor(0, 255, 0); //시작점의 색깔 green
		ofDrawCircle(rooms[0].x1, rooms[0].y1, 5); //0번 노드에 반지름 5인 원 그림
		ofSetColor(255, 0, 0); //도착점의 색깔 red
		ofDrawCircle(rooms[maze_col * maze_row - 1].x1, rooms[maze_col * maze_row - 1].y1, 5); //마지막 노드에 반지름 5인 원 그림

		play_draw(); //play_draw 함수 호출

		bShowInfo = 1; //bShowInfo를 1로 설정하고, bShowInfo를 화면에 출력할 때 사용할 색깔과 두께를 설정
		ofSetColor(100);
		ofSetLineWidth(5);
	}
	else if (isplay == 2) { //isplay가 2일 때 (도착점에 도착했을 때)
		success_draw(); //success_play 함수 호출
		bShowInfo = 1; //bShowInfo를 1로 설정하고, bShowInfo를 화면에 출력할 때 사용할 색깔과 두께를 설정
		ofSetColor(100);
		ofSetLineWidth(5);
	}
	if (bShowInfo) { //bShowInfo가 true일 때 
		// Show keyboard duplicates of menu functions
		sprintf(str, " comsil project");
		myFont.drawString(str, 15, ofGetHeight() - 20);
	}

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if (bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else {
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
		// Show the cursor again
		ofShowCursor();
		// Restore topmost state
		if (bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if (bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if (GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
} // end doTopmost


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// Escape key exit has been disabled but it can be checked here
	if (key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// Remove or show screen info
	if (key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	if (isplay == 1 && !p_path.empty()) { //isplay가 1이고 p_path가 비어있지 않을 때, 즉 play가 진행 중일 때 
		if (key == OF_KEY_DOWN) { //아래 화살표가 입력되었을 때
			int next = p_path.back() + maze_col; //next는 아래쪽 방의 번호
			movement(next);
		}
		else if (key == OF_KEY_UP) { //위 화살표가 입력되었을 때
			int next = p_path.back() - maze_col; //next는 위쪽 방의 번호
			movement(next);
		}
		else if (key == OF_KEY_RIGHT) { //오른쪽 화살표가 입력되었을 때
			int next = p_path.back() + 1; //next는 오른쪽 방의 번호
			movement(next);
		}
		else if (key == OF_KEY_LEFT) { //왼쪽 화살표가 입력되었을 때
			int next = p_path.back() - 1; //next는 왼쪽 방의 번호
			movement(next);
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
bool ofApp::readFile()
{
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file"); //.maz 파일을 선택하는 dialog
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName); //파일 이름 출력
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(fileName);
			isdfs = 0; //isdfs와 isbfs를 0으로 초기화
			isbfs = 0;
			isplay = 0; //isplay를 0으로 초기화하고, p_path를 비운다. 
			p_path.clear();

			if (!file.exists()) { //파일이 존재하지 않은 경우
				cout << "Target file does not exists." << endl;
				return false;
			}
			else { //파일이 존재하는 경우 
				cout << "We found the target file." << endl;
				isOpen = 1;
			}
			
			ofBuffer buffer(file);

			int cnt = 0; //.maze가 몇줄인지 count하는 변수

			// .maz 파일을 input으로 받아서 적절히 자료구조에 넣는다
			for (ofBuffer::Line get = buffer.getLines().begin(), end = buffer.getLines().end(); get != end; ++get) {
				string line = *get; //한줄씩 읽음
				WIDTH = line.length(); //미로파일의 너비는 line의 길의
				cnt++; //한줄 읽을 때마다 1씩 증가
			}
			HEIGHT = cnt; //미로파일의 높이는 cnt
			maze_col = (WIDTH - 1) / 2; //미로칸의 열의 개수
			maze_row = (HEIGHT - 1) / 2; //미로칸의 행의 개수

			//dots는 '+'의 x, y 위치를 저장하는 이차원 구조체 배열
			dots = new Dot * [maze_row + 1]; //이차원 배열 동적 할당
			for (int i = 0; i < maze_row + 1; i++) { //각 행에 대해 동적 할당
				dots[i] = new Dot[maze_col + 1];
			}
			dots[0][0].x1 = 0; //처음 '+'의 위치는 (0,0)
			dots[0][0].y1 = 0;

			//같은 행에 있는 '+'는 이전 '+'보다 x 좌표가 30 증가하고, y는 그대로
			for (int i = 1; i < maze_col + 1; i++) {
				dots[0][i].x1 = dots[0][i - 1].x1 + 30;
				dots[0][i].y1 = 0;
			}
			//같은 열에 있는 '+'는 이전 '+'보다 y 좌표가 30 증가하고, x는 그대로
			for (int i = 1; i < maze_row + 1; i++) {
				for (int j = 0; j < maze_col + 1; j++) {
					dots[i][j].x1 = dots[i - 1][j].x1;
					dots[i][j].y1 = dots[i - 1][j].y1 + 30;
				}
			}

			//rooms는 방의 위치를 저장하는 일차원 구조체 배열
			rooms = new Dot[maze_col * maze_row ]; //1차원 배열 동적 할당
			rooms[0].x1 = 15; //시작점의 위치는 (15,15) (0과 30 사이)
			rooms[0].y1 = 15;

			//방의 개수만큼 반복하는 반복문. 방마다 위치를 저장하기 위한 반복문.
			for (int k = 1; k < maze_col * maze_row; k++) {
				if (k < maze_col) { //첫번째 행에 있는 방들은 왼쪽에 있는 방의 좌표에서 x 값만 30 증가. 
					rooms[k].x1 = rooms[k - 1].x1 + 30;
					rooms[k].y1 = rooms[k - 1].y1;
				}
				else { //두번째 행부터는 위쪽에 있는 방의 좌표에서 y 값만 30 증가. 
					rooms[k].x1 = rooms[k - maze_col].x1;
					rooms[k].y1 = rooms[k - maze_col].y1 + 30;
				}
			}

			//maze는 .maz 파일정보를 저장하는 2차원 배열
			maze = new int* [HEIGHT];
			for (int i = 0; i < HEIGHT; i++) {
				maze[i] = new int[WIDTH];
			}

			int h = 0; //높이
			int vtx = 0; //방번호

			//.maz를 maze 배열에 저장하기 위한 반복문
			for (ofBuffer::Line get = buffer.getLines().begin(), end = buffer.getLines().end(); get != end; ++get) {
				string line = *get; //한줄씩 읽어서 line에 저장
				for (int i = 0; i < WIDTH; i++) {
					if (i % 2 == 1 && h % 2 == 1) { //홀수열, 홀수행일 때는 방 번호 저장
						maze[h][i] = vtx++;
					}
					else { //그 외에는 '-', '|', '+' 저장
						maze[h][i] = line[i];
					}
				}
				h++; //높이 1 증가
			}

			
			graph = new vector<int>[vtx + 1]; //graph는 인접한 노드를 저장하는 벡터 배열

			//vertex의 4방향 검사 -> 벽이 없으면 인접한 vertex와 연결(vecter 사용)
			//maze가 홀수행과 홀수열일 때 방 번호를 저장하고 있기 때문에 홀수행과 홀수열에 대해서 반복문
			for (int i = 1; i < HEIGHT; i+=2) { //홀수행
				for (int j = 1; j < WIDTH; j+=2) { //홀수열
					for (int k = 0; k < 4; k++) { //상하좌우에 인접 노드 있는지 검사하는 반복문
						if (k == 0) { //위쪽 인접노드 검사
							if (maze[i - 1][j] != '-') { //벽이 없는 경우, 즉 위쪽 방과 인접한 경우
								graph[maze[i][j]].push_back(maze[i - 2][j]); //graph[현재노드]에 위쪽 방번호 추가
							}
						}
						else if (k == 1) { //오른쪽 인접노드 검사
							if (maze[i][j + 1] != '|') { //벽이 없는 경우, 즉 오른쪽 방과 인접한 경우
								graph[maze[i][j]].push_back(maze[i][j+2]); //graph[현재노드]에 오른쪽 방번호 추가
							}
						}
						else if (k == 2) { //아래쪽 인접노드 검사
							if (maze[i + 1][j] != '-') { //벽이 없는 경우, 즉 아래쪽 방과 인접한 경우
								graph[maze[i][j]].push_back(maze[i + 2][j]); //graph[현재노드]에 아래쪽 방번호 추가
							}
						}
						else { //왼쪽 인접노드 검사
							if (maze[i][j - 1] != '|') { //벽이 없는 경우, 즉 왼쪽 방과 인접한 경우
								graph[maze[i][j]].push_back(maze[i][j-2]);  //graph[현재노드]에 왼쪽 방번호 추가
							}
						}
					}
				}
			}
		}
		else {
			printf("  Needs a '.maz' extension\n");
			return false;
		}
	}
}
void ofApp::freeMemory() {

	// malloc한 memory를 free해주는 함수
	//2차원 배열 maze free
	for (int i = 0; i < HEIGHT; i++) {
		delete[] maze[i];
	}
	delete[] maze;

	//2차원 배열 dots free
	for (int i = 0; i < maze_row+1; i++) {
		delete[] dots[i];
	}
	delete[] dots;

	//1차원 배열 paht, best, q_path, visited, play_visit free
	delete[] path;
	delete[] best;
	delete[] q_path;
	delete[] visited;
	delete[] play_visit;

	//vector 배열 graph, parent free
	graph->clear();
	parent->clear();

	//vector p_path free
	p_path.clear();
}

bool ofApp::DFS()//DFS탐색을 하는 함수
{
	stack<int> stack; //stack 생성
	
	visited = new int [maze_row * maze_col]; //visited malloc and 0으로 초기화
	for (int i = 0; i < maze_row * maze_col; i++) {
		visited[i] = 0;
	}
	path = new int[maze_row * maze_col * 5]; //paht malloc and -1으로 초기화
	for (int i = 0; i < maze_row * maze_col*5; i++) {
		path[i] = -1;
	}
	best = new int[maze_row * maze_col]; //best malloc and -1으로 초기화
	for (int i = 0; i < maze_row * maze_col; i++) {
		best[i] = -1;
	}

	num = 0; //path의 길이(방문한 노드의 개수)
	best_num = 0; //최적 경로의 길이 (최적 경로가 거치는 노드의 개수)
	int target = maze_row * maze_col - 1; //목적지 방번호
	int v = 0;
	stack.push(v); //시작점을 stack에 추가
	visited[v] = 1; //시작점의 방문 여부 1
	path[num++] = v; //path에 시작점 추가하고 path의 길이 1 증가

	//DFS 알고리즘을 실행하면서 시작점부터 도착점까지 방문하게 되는 모든 경로를 찾는 반복문
	while (!stack.empty()) { //stack이 비어있지 않으면 계속 반복
		if (stack.top() == target) { //stack의 top이 목적지와 동일하면 반복문 종료
			break;
		}
		if (graph[stack.top()].size()) { //stack의 top에 있는 방 번호와 인접한 노드가 존재할 경우
			int count = 0; //인접한 노드 중 방문하지 않은 노드의 개수를 세는 변수
			//인접한 노드 중에서 방문하지 않은 노드를 찾는 반복문
			for (int i = 0; i < graph[stack.top()].size(); i++) { //인접한 노드의 개수만큼 반복
				int u = graph[stack.top()][i];
				if (visited[u] == 0) { //인접한 노드 u가 아직 방문하지 않은 노드일 경우
					//stack와 path에 u를 추가하고, u의 방문 여부를 1로, count와 num을 1씩 증가
					count++;
					stack.push(u);
					visited[u] = 1;
					path[num++] = u;
				}
			}
			if (count == 0) { //인접한 노드가 모두 방문한 적이 있는 경우, 즉 이전 노드로 되돌아가야 하는 경우
				//현재 노드를 stack에서 제거하고 stack의 top을 path에 추가
				stack.pop();
				int u = stack.top();
				path[num++] = u;
			}
		}
		else { //stack의 top에 있는 방 번호와 인접한 노드가 존재하지 않으면 stack의 top을 pop
			stack.pop();
		}
	}
	
	//최적 경로를 찾는 반복문
	//도착점에 도착했을 때의 stack에 있는 노드가 최적 경로임
	while (!stack.empty()) { //stack이 비어있지 않으면 반복
		//stack의 top부터 stack에 더이상 노드가 없을 때까지 top을 best에 추가
		int u = stack.top(); 
		best[best_num++] = u;
		stack.pop();
	}

	//isdfs를 true로
	isdfs = 1;

	return 0;
}

void ofApp::dfsdraw() //DFS 알고리즘으로 찾은 모든 경로와 최적 경로를 화면에 그리는 함수
{
	//모든 경로를 그리는 반복문
	for (int i = 0; i < num-1; i++) { //(path의 길이 - 1)만큼 반복
		ofDrawLine(rooms[path[i]].x1, rooms[path[i]].y1, rooms[path[i+1]].x1, rooms[path[i+1]].y1); //현재노드에서 다음노드까지 선을 그림
	}

	//최적 경로 표시를 위해 선의 색깔과 두께를 다시 설정
	ofSetColor(0,255,0);
	ofSetLineWidth(2);

	//최적 경로를 그리는 반복문
	for (int j = 0; j < best_num - 1; j++) { //(best의 길이 -1)만큼 반복
		ofDrawLine(rooms[best[j]].x1, rooms[best[j]].y1, rooms[best[j + 1]].x1, rooms[best[j + 1]].y1); //현재노드에서 다음노드까지 선을 그림
	}
}

bool ofApp::BFS() //BFS탐색을 하는 함수
{ 
	queue<int> queue; //queue 생성

	parent = new vector<int>[maze_row * maze_col]; //각 노드의 부모 노드를 저장하는 vector 배열

	visited = new int[maze_row * maze_col]; //visited malloc and 0으로 init
	for (int i = 0; i < maze_row * maze_col; i++) {
		visited[i] = 0;
	}

	q_path = new int[maze_row * maze_col * 5]; //bfs탐색의 모든 경로를 저장하는 q_path를 동적할당하고 -1로 초기화
	for (int i = 0; i < maze_row * maze_col * 5; i++) {
		q_path[i] = -1;
	}

	num2 = 0; //q_paht의 길이

	int target = maze_row * maze_col - 1; //도착점 방 번호
	int v = 0; //시작점 방 번호
	queue.push(v); //queue에 시작점 추가
	visited[v] = 1; //시작점의 방문여부 = 1
	q_path[num2++] = v; //q_path에 시작점 추가하고 num2 1증가

	//BFS 알고리즘을 실행하면서 시작점부터 도착점까지 방문하게 되는 모든 경로를 찾는 반복문
	while (!queue.empty()) { //queue가 비어있지 않으면 반복
		if (queue.back() == target) { //queue의 마지막 원소가 도착지이면 반복문 종료
			break;
		}

		//queue의 front를 w에 저장하고 front pop
		int w = queue.front();
		queue.pop();
		
		//w와 인접한 노드 중에서 방문한 적이 없는 노드를 찾는 반복문
		for (int k = 0; k < graph[w].size(); k++) { //w와 인접한 노드의 개수만큼 반복
			int u = graph[w][k];
			if (visited[u] == 0) { //u가 방문한 적이 없는 경우
				//u의 방문 여부를 1로, queue와 q_path에 u를 추가하고, u의 부모 노드를 w로 저장
				visited[u] = 1;
				parent[u].push_back(w);
				queue.push(u);
				q_path[num2++] = u;
			}
		}
	}

	//isbfs를 1로
	isbfs = 1;

	return 0;
}

void ofApp::bfsdraw() //BFS 알고리즘으로 찾은 모든 경로와 최적 경로를 화면에 그리는 함수
{
	//모든 경로를 그리는 반복문. q_path에 있는 노드들의 부모노드를 따라가면서 draw
	for (int j = num2-1; j > 1; j--) { //q_path의 길이 -1 만큼 반복
		int node = parent[q_path[j]].back(); //node = q_path의 마지막 노드의 부모노드
		ofDrawLine(rooms[q_path[j]].x1, rooms[q_path[j]].y1, rooms[node].x1, rooms[node].y1); //q_path의 마지막 노드와 q_path의 마지막 노드의 부모노드 사이의 선을 그림

		while (node != 0) { //node가 0이 아닌 동안 반복, 즉 시작점까지 반복
			int node2 = parent[node].back(); //node2는 node의 부모노드
			ofDrawLine(rooms[node].x1, rooms[node].y1, rooms[node2].x1, rooms[node2].y1); //node와 node의 부모노드 사이의 선을 그림
			node = node2; //node가 node의 부모노드가 되고 동일한 작업 반복
		}
	}

	//최적 경로를 그리기 위해 선의 색깔과 두께 변경
	ofSetColor(0, 255, 0);
	ofSetLineWidth(2);
	int node = parent[q_path[num2-1]].back(); //node = q_path의 마지막 노드의 부모노드. q_path의 마지막 노드는 도착점이므로 도착점의 부모노드를 따라가는 것이 최적경로임
	ofDrawLine(rooms[q_path[num2-1]].x1, rooms[q_path[num2-1]].y1, rooms[node].x1, rooms[node].y1); //q_path의 마지막 노드와 부모노드 사이의 선을 그림

	//최적 경로를 그리는 반복문
	while (node != 0) { //node가 0이 아닌 동안 반복, 즉 시작점까지 반복
		int node2 = parent[node].back(); //node2는 node의 부모노드
		ofDrawLine(rooms[node].x1, rooms[node].y1, rooms[node2].x1, rooms[node2].y1); //node와 node의 부모노드 사이의 선을 그림
		node = node2; //node가 node의 부모노드가 되고 동일한 작업 반복
	}
}

void ofApp::play_draw()  //사용자의 입력으로부터 얻은 경로를 그리는 함수
{
	//사용자가 입력한 경로를 표시하기 위해 선의 색깔과 두께를 변경
	ofSetColor(0, 0, 255);
	ofSetLineWidth(3);

	//사용자가 입력한 경로를 화면에 그리는 반복문
	for (int k = 0; k < p_path.size() - 1; k++) { //p_path의 크기 -1만큼 반복
		int cur = p_path[k]; //현재 노드
		int ne = p_path[k + 1]; //다음 노드
		ofDrawLine(rooms[cur].x1, rooms[cur].y1, rooms[ne].x1, rooms[ne].y1); //현재 노드와 다음 노드 사이의 선을 그림
	}

	if (p_path.back() == maze_col * maze_row - 1) { //사용자가 입력한 경로가 도착점에 도착했을 때 
		isplay = 2; //isplay를 2로
	}
}

void ofApp::success_draw() //사용자가 도착점에 도착했을 때 성공적으로 미로를 탈출한 것을 나타내기 위해 미로를 새로 그리는 함수
{
	//성공적으로 탈출한 미로의 색깔과 두께 설정
	ofSetColor(51, 153, 51);
	ofSetLineWidth(5);

	//maze 배열을 통해 미로의 모든 벽을 그리는 반복문 
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (maze[i][j] == '-' && i % 2 == 0) { //maze[i][j] 값이 '-'이고, i가 짝수인 경우 (홀수일 때는 방의 번호가 저장되어 있기 때문)
				//두 점을 연결하여 선을 그린다. 이 때 이차원 배열 dots는 .maz 파일에서 '+'의 x, y 좌표를 저장한 구조체 배열임. 
				ofDrawLine(dots[i / 2][(j + 1) / 2].x1, dots[i / 2][(j + 1) / 2].y1, dots[i / 2][(j - 1) / 2].x1, dots[i / 2][(j - 1) / 2].y1);
			}
			else if (maze[i][j] == '|') { //maze[i][j] 값이 '|'인 경우
				//두 점을 연결하여 선을 그린다.
				ofDrawLine(dots[(i + 1) / 2][j / 2].x1, dots[(i + 1) / 2][j / 2].y1, dots[(i - 1) / 2][j / 2].x1, dots[(i - 1) / 2][j / 2].y1);
			}
		}
	}

	//사용자의 최종 경로를 표시하기 위해 선의 색깔 변경
	ofSetColor(0, 0, 255);

	//사용자의 최종 경로를 그리는 반복문
	for (int k = 0; k < p_path.size()-1; k++) {//p_path의 크기 -1만큼 반복
		int cur = p_path[k]; //현재 노드 
		int ne = p_path[k + 1]; //다음 노드 
		ofDrawLine(rooms[cur].x1, rooms[cur].y1, rooms[ne].x1, rooms[ne].y1); //현재 노드와 다음 노드 사이의 선을 그림
	}
}

void ofApp::movement(int next) //키 입력에 따라 해당하는 노드로 이동하는 함수
{
	//다음 방의 번호가 현재 방과 인접한 노드인지 검사하고, 인접한 노드라면 if문 실행
	if (find(graph[p_path.back()].begin(), graph[p_path.back()].end(), next) != graph[p_path.back()].end()) {
		if (play_visit[next] == 1) { //이미 방문한 적이 있는 방. 즉 이미 왔던 곳을 되돌아간 경우를 의미함.
			play_visit[p_path.back()] = 0; //현재 방의 방문 여부를 0으로
			p_path.pop_back(); //p_path에서 해당 방 번호 삭제(되돌아 갔기 때문에)
		}
		else { //방문한 적이 없는 방이라면 next를 p_path에 저장하고, next의 방문 여부를 방문한 것으로 설정
			p_path.push_back(next);
			play_visit[next] = 1;
		}
	}
}
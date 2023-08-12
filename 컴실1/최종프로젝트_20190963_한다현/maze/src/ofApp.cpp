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
	//isdfs, isbfs, isplay, isOpen ��� false�� ����
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
	hPopup = menu->AddPopupMenu(hMenu, "Play"); //play �˾� �޴� ����
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
	if (title == "Open") { //Open�� ������ ���
		readFile(); //readFile �Լ� ȣ��

	}
	if (title == "Exit") { //Exit�� ����Ͽ��� ��� ���α׷� ����
		ofExit(); // Quit the application
	}

	//
	// Window menu
	//
	if (title == "Show DFS") { //Show DFS�� ������ ��� 
		//bShowInfo = bChecked;  // Flag is used elsewhere in Draw()
		if (isOpen) //������ ������ ���, isbfs�� isplay�� 0���� �����ϰ� DFS �Լ� ȣ��
		{
			isbfs = 0;
			isplay = 0;
			DFS(); 
			bShowInfo = bChecked;
		}
		else //������ ������ �ʾ��� ���
			cout << "you must open file first" << endl;

	}

	if (title == "Show BFS") { //Show BFS�� ������ ���
		doTopmost(bChecked); // Use the checked value directly
		if (isOpen) //������ ������ ���, isdfs�� isplay�� 0���� �����ϰ� BFS �Լ� ȣ�� 
		{
			isdfs = 0;
			isplay = 0;
			BFS();
			bShowInfo = bChecked;
		}
		else //������ ������ �ʾ��� ���
			cout << "you must open file first" << endl;
	}

	if (title == "Full screen") { //Full screen�� ������ ��� 
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if (title == "About") { //About�� ������ ��� 
		ofSystemAlertDialog("ofxWinMenu\nbasic example\n\nhttp://spout.zeal.co");
	}

	//
	//Play menu
	//
	if (title == "Play") { //Play�� ������ ��� 
		if (isOpen) { //������ ������ ��� 
			//������ ����� �����Ͱ� �������� �� �ֱ� ������ isplay�� 0���� �ʱ�ȭ�ϰ�, p_path�� ����.
			isplay = 0; 
			p_path.clear();  

			//isdfs�� isbfs�� 0����
			isdfs = 0;
			isbfs = 0;

			//isplay�� 1�� �����ϰ�, play_visit�� �����Ҵ� �ް�, ��� ���� 0���� �ʱ�ȭ �Ѵ�. 
			isplay = 1;
			play_visit = new int[maze_row * maze_col]; 
			for (int i = 0; i < maze_row * maze_col; i++) {
				play_visit[i] = 0;
			}

			//�������� 0�� p_path�� �����ϰ�, play_visit[0]�� 1�� �����Ѵ�. 
			play_visit[0] = 1; 
			p_path.push_back(0);
		}
		else { //������ ������ ���� ��� 
			cout << "you must open file first" << endl;
		}
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {
	//�̷θ� �׸��� ����� ���� �β��� ����
	char str[256];
	ofSetColor(100);
	ofSetLineWidth(5);
	
	// ����� �ڷᱸ���� �̿��� �̷θ� �׸���.
	// �̷��� HEIGHT�� WIDTH��ŭ �ݺ����� �����ϰ�, ������ �迭 maze�� ����� ���� '-'�� '|'�� ��� �� ���� �����Ͽ� ���� �׸���. 
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (maze[i][j] == '-' && i%2 == 0) { //maze[i][j] ���� '-'�̰�, i�� ¦���� ��� (Ȧ���� ���� ���� ��ȣ�� ����Ǿ� �ֱ� ����)
				//�� ���� �����Ͽ� ���� �׸���. �� �� ������ �迭 dots�� .maz ���Ͽ��� '+'�� x, y ��ǥ�� ������ ����ü �迭��. 
				ofDrawLine(dots[i/2][(j + 1) / 2].x1, dots[i/2][(j + 1) / 2].y1, dots[i/2][(j - 1) / 2].x1, dots[i/2][(j - 1) / 2].y1); 
			}
			else if (maze[i][j] == '|') { //maze[i][j] ���� '|'�� ���
				//�� ���� �����Ͽ� ���� �׸���.
				ofDrawLine(dots[(i + 1) / 2][j/2].x1, dots[(i + 1) / 2][j/2].y1, dots[(i - 1) / 2][j/2].x1, dots[(i - 1) / 2][j/2].y1);
			}
		}
	}

	if (isdfs) //isdfs�� true�� ��� (DFS �Լ��� ���������� ����Ǿ��� ���)
	{
		//dfs �˰����� �湮�� ��� ��θ� �׸��� ���� ����� �β��� ����
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen) //������ ������ ���
			dfsdraw(); //dfsdraw �Լ� ����
		else //������ ������ �ʾ��� ��� 
			cout << "You must open file first" << endl;
		bShowInfo = 1; //bShowInfo�� 1�� �����ϰ�, bShowInfo�� ȭ�鿡 ����� �� ����� ����� �β��� ����
		ofSetColor(100); 
		ofSetLineWidth(5); 
	}
	if (isbfs) //isbfs�� true�� ��� (BFS �Լ��� ���������� ����Ǿ��� ���)
	{
		//dfs �˰����� �湮�� ��� ��θ� �׸��� ���� ����� �β��� ����
		ofSetColor(200);
		ofSetLineWidth(5);
		if (isOpen) //������ ������ ���
			bfsdraw(); //bfsdraw �Լ� ����
		else //������ ������ �ʾ��� ��� 
			cout << "You must open file first" << endl;
		bShowInfo = 1; //bShowInfo�� 1�� �����ϰ�, bShowInfo�� ȭ�鿡 ����� �� ����� ����� �β��� ����
		ofSetColor(100);
		ofSetLineWidth(5);
	}
	if (isplay == 1) { //isplay�� 1�� �� (play�� �������� ��)
		//�������� ������ ǥ��
		ofSetColor(0, 255, 0); //�������� ���� green
		ofDrawCircle(rooms[0].x1, rooms[0].y1, 5); //0�� ��忡 ������ 5�� �� �׸�
		ofSetColor(255, 0, 0); //�������� ���� red
		ofDrawCircle(rooms[maze_col * maze_row - 1].x1, rooms[maze_col * maze_row - 1].y1, 5); //������ ��忡 ������ 5�� �� �׸�

		play_draw(); //play_draw �Լ� ȣ��

		bShowInfo = 1; //bShowInfo�� 1�� �����ϰ�, bShowInfo�� ȭ�鿡 ����� �� ����� ����� �β��� ����
		ofSetColor(100);
		ofSetLineWidth(5);
	}
	else if (isplay == 2) { //isplay�� 2�� �� (�������� �������� ��)
		success_draw(); //success_play �Լ� ȣ��
		bShowInfo = 1; //bShowInfo�� 1�� �����ϰ�, bShowInfo�� ȭ�鿡 ����� �� ����� ����� �β��� ����
		ofSetColor(100);
		ofSetLineWidth(5);
	}
	if (bShowInfo) { //bShowInfo�� true�� �� 
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
	if (isplay == 1 && !p_path.empty()) { //isplay�� 1�̰� p_path�� ������� ���� ��, �� play�� ���� ���� �� 
		if (key == OF_KEY_DOWN) { //�Ʒ� ȭ��ǥ�� �ԷµǾ��� ��
			int next = p_path.back() + maze_col; //next�� �Ʒ��� ���� ��ȣ
			movement(next);
		}
		else if (key == OF_KEY_UP) { //�� ȭ��ǥ�� �ԷµǾ��� ��
			int next = p_path.back() - maze_col; //next�� ���� ���� ��ȣ
			movement(next);
		}
		else if (key == OF_KEY_RIGHT) { //������ ȭ��ǥ�� �ԷµǾ��� ��
			int next = p_path.back() + 1; //next�� ������ ���� ��ȣ
			movement(next);
		}
		else if (key == OF_KEY_LEFT) { //���� ȭ��ǥ�� �ԷµǾ��� ��
			int next = p_path.back() - 1; //next�� ���� ���� ��ȣ
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
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select .maz file"); //.maz ������ �����ϴ� dialog
	string filePath;
	size_t pos;
	// Check whether the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User selected a file");

		//We have a file, check it and process it
		string fileName = openFileResult.getName();
		//string fileName = "maze0.maz";
		printf("file name is %s\n", fileName); //���� �̸� ���
		filePath = openFileResult.getPath();
		printf("Open\n");
		pos = filePath.find_last_of(".");
		if (pos != string::npos && pos != 0 && filePath.substr(pos + 1) == "maz") {

			ofFile file(fileName);
			isdfs = 0; //isdfs�� isbfs�� 0���� �ʱ�ȭ
			isbfs = 0;
			isplay = 0; //isplay�� 0���� �ʱ�ȭ�ϰ�, p_path�� ����. 
			p_path.clear();

			if (!file.exists()) { //������ �������� ���� ���
				cout << "Target file does not exists." << endl;
				return false;
			}
			else { //������ �����ϴ� ��� 
				cout << "We found the target file." << endl;
				isOpen = 1;
			}
			
			ofBuffer buffer(file);

			int cnt = 0; //.maze�� �������� count�ϴ� ����

			// .maz ������ input���� �޾Ƽ� ������ �ڷᱸ���� �ִ´�
			for (ofBuffer::Line get = buffer.getLines().begin(), end = buffer.getLines().end(); get != end; ++get) {
				string line = *get; //���پ� ����
				WIDTH = line.length(); //�̷������� �ʺ�� line�� ����
				cnt++; //���� ���� ������ 1�� ����
			}
			HEIGHT = cnt; //�̷������� ���̴� cnt
			maze_col = (WIDTH - 1) / 2; //�̷�ĭ�� ���� ����
			maze_row = (HEIGHT - 1) / 2; //�̷�ĭ�� ���� ����

			//dots�� '+'�� x, y ��ġ�� �����ϴ� ������ ����ü �迭
			dots = new Dot * [maze_row + 1]; //������ �迭 ���� �Ҵ�
			for (int i = 0; i < maze_row + 1; i++) { //�� �࿡ ���� ���� �Ҵ�
				dots[i] = new Dot[maze_col + 1];
			}
			dots[0][0].x1 = 0; //ó�� '+'�� ��ġ�� (0,0)
			dots[0][0].y1 = 0;

			//���� �࿡ �ִ� '+'�� ���� '+'���� x ��ǥ�� 30 �����ϰ�, y�� �״��
			for (int i = 1; i < maze_col + 1; i++) {
				dots[0][i].x1 = dots[0][i - 1].x1 + 30;
				dots[0][i].y1 = 0;
			}
			//���� ���� �ִ� '+'�� ���� '+'���� y ��ǥ�� 30 �����ϰ�, x�� �״��
			for (int i = 1; i < maze_row + 1; i++) {
				for (int j = 0; j < maze_col + 1; j++) {
					dots[i][j].x1 = dots[i - 1][j].x1;
					dots[i][j].y1 = dots[i - 1][j].y1 + 30;
				}
			}

			//rooms�� ���� ��ġ�� �����ϴ� ������ ����ü �迭
			rooms = new Dot[maze_col * maze_row ]; //1���� �迭 ���� �Ҵ�
			rooms[0].x1 = 15; //�������� ��ġ�� (15,15) (0�� 30 ����)
			rooms[0].y1 = 15;

			//���� ������ŭ �ݺ��ϴ� �ݺ���. �渶�� ��ġ�� �����ϱ� ���� �ݺ���.
			for (int k = 1; k < maze_col * maze_row; k++) {
				if (k < maze_col) { //ù��° �࿡ �ִ� ����� ���ʿ� �ִ� ���� ��ǥ���� x ���� 30 ����. 
					rooms[k].x1 = rooms[k - 1].x1 + 30;
					rooms[k].y1 = rooms[k - 1].y1;
				}
				else { //�ι�° ����ʹ� ���ʿ� �ִ� ���� ��ǥ���� y ���� 30 ����. 
					rooms[k].x1 = rooms[k - maze_col].x1;
					rooms[k].y1 = rooms[k - maze_col].y1 + 30;
				}
			}

			//maze�� .maz ���������� �����ϴ� 2���� �迭
			maze = new int* [HEIGHT];
			for (int i = 0; i < HEIGHT; i++) {
				maze[i] = new int[WIDTH];
			}

			int h = 0; //����
			int vtx = 0; //���ȣ

			//.maz�� maze �迭�� �����ϱ� ���� �ݺ���
			for (ofBuffer::Line get = buffer.getLines().begin(), end = buffer.getLines().end(); get != end; ++get) {
				string line = *get; //���پ� �о line�� ����
				for (int i = 0; i < WIDTH; i++) {
					if (i % 2 == 1 && h % 2 == 1) { //Ȧ����, Ȧ������ ���� �� ��ȣ ����
						maze[h][i] = vtx++;
					}
					else { //�� �ܿ��� '-', '|', '+' ����
						maze[h][i] = line[i];
					}
				}
				h++; //���� 1 ����
			}

			
			graph = new vector<int>[vtx + 1]; //graph�� ������ ��带 �����ϴ� ���� �迭

			//vertex�� 4���� �˻� -> ���� ������ ������ vertex�� ����(vecter ���)
			//maze�� Ȧ����� Ȧ������ �� �� ��ȣ�� �����ϰ� �ֱ� ������ Ȧ����� Ȧ������ ���ؼ� �ݺ���
			for (int i = 1; i < HEIGHT; i+=2) { //Ȧ����
				for (int j = 1; j < WIDTH; j+=2) { //Ȧ����
					for (int k = 0; k < 4; k++) { //�����¿쿡 ���� ��� �ִ��� �˻��ϴ� �ݺ���
						if (k == 0) { //���� ������� �˻�
							if (maze[i - 1][j] != '-') { //���� ���� ���, �� ���� ��� ������ ���
								graph[maze[i][j]].push_back(maze[i - 2][j]); //graph[������]�� ���� ���ȣ �߰�
							}
						}
						else if (k == 1) { //������ ������� �˻�
							if (maze[i][j + 1] != '|') { //���� ���� ���, �� ������ ��� ������ ���
								graph[maze[i][j]].push_back(maze[i][j+2]); //graph[������]�� ������ ���ȣ �߰�
							}
						}
						else if (k == 2) { //�Ʒ��� ������� �˻�
							if (maze[i + 1][j] != '-') { //���� ���� ���, �� �Ʒ��� ��� ������ ���
								graph[maze[i][j]].push_back(maze[i + 2][j]); //graph[������]�� �Ʒ��� ���ȣ �߰�
							}
						}
						else { //���� ������� �˻�
							if (maze[i][j - 1] != '|') { //���� ���� ���, �� ���� ��� ������ ���
								graph[maze[i][j]].push_back(maze[i][j-2]);  //graph[������]�� ���� ���ȣ �߰�
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

	// malloc�� memory�� free���ִ� �Լ�
	//2���� �迭 maze free
	for (int i = 0; i < HEIGHT; i++) {
		delete[] maze[i];
	}
	delete[] maze;

	//2���� �迭 dots free
	for (int i = 0; i < maze_row+1; i++) {
		delete[] dots[i];
	}
	delete[] dots;

	//1���� �迭 paht, best, q_path, visited, play_visit free
	delete[] path;
	delete[] best;
	delete[] q_path;
	delete[] visited;
	delete[] play_visit;

	//vector �迭 graph, parent free
	graph->clear();
	parent->clear();

	//vector p_path free
	p_path.clear();
}

bool ofApp::DFS()//DFSŽ���� �ϴ� �Լ�
{
	stack<int> stack; //stack ����
	
	visited = new int [maze_row * maze_col]; //visited malloc and 0���� �ʱ�ȭ
	for (int i = 0; i < maze_row * maze_col; i++) {
		visited[i] = 0;
	}
	path = new int[maze_row * maze_col * 5]; //paht malloc and -1���� �ʱ�ȭ
	for (int i = 0; i < maze_row * maze_col*5; i++) {
		path[i] = -1;
	}
	best = new int[maze_row * maze_col]; //best malloc and -1���� �ʱ�ȭ
	for (int i = 0; i < maze_row * maze_col; i++) {
		best[i] = -1;
	}

	num = 0; //path�� ����(�湮�� ����� ����)
	best_num = 0; //���� ����� ���� (���� ��ΰ� ��ġ�� ����� ����)
	int target = maze_row * maze_col - 1; //������ ���ȣ
	int v = 0;
	stack.push(v); //�������� stack�� �߰�
	visited[v] = 1; //�������� �湮 ���� 1
	path[num++] = v; //path�� ������ �߰��ϰ� path�� ���� 1 ����

	//DFS �˰����� �����ϸ鼭 ���������� ���������� �湮�ϰ� �Ǵ� ��� ��θ� ã�� �ݺ���
	while (!stack.empty()) { //stack�� ������� ������ ��� �ݺ�
		if (stack.top() == target) { //stack�� top�� �������� �����ϸ� �ݺ��� ����
			break;
		}
		if (graph[stack.top()].size()) { //stack�� top�� �ִ� �� ��ȣ�� ������ ��尡 ������ ���
			int count = 0; //������ ��� �� �湮���� ���� ����� ������ ���� ����
			//������ ��� �߿��� �湮���� ���� ��带 ã�� �ݺ���
			for (int i = 0; i < graph[stack.top()].size(); i++) { //������ ����� ������ŭ �ݺ�
				int u = graph[stack.top()][i];
				if (visited[u] == 0) { //������ ��� u�� ���� �湮���� ���� ����� ���
					//stack�� path�� u�� �߰��ϰ�, u�� �湮 ���θ� 1��, count�� num�� 1�� ����
					count++;
					stack.push(u);
					visited[u] = 1;
					path[num++] = u;
				}
			}
			if (count == 0) { //������ ��尡 ��� �湮�� ���� �ִ� ���, �� ���� ���� �ǵ��ư��� �ϴ� ���
				//���� ��带 stack���� �����ϰ� stack�� top�� path�� �߰�
				stack.pop();
				int u = stack.top();
				path[num++] = u;
			}
		}
		else { //stack�� top�� �ִ� �� ��ȣ�� ������ ��尡 �������� ������ stack�� top�� pop
			stack.pop();
		}
	}
	
	//���� ��θ� ã�� �ݺ���
	//�������� �������� ���� stack�� �ִ� ��尡 ���� �����
	while (!stack.empty()) { //stack�� ������� ������ �ݺ�
		//stack�� top���� stack�� ���̻� ��尡 ���� ������ top�� best�� �߰�
		int u = stack.top(); 
		best[best_num++] = u;
		stack.pop();
	}

	//isdfs�� true��
	isdfs = 1;

	return 0;
}

void ofApp::dfsdraw() //DFS �˰������� ã�� ��� ��ο� ���� ��θ� ȭ�鿡 �׸��� �Լ�
{
	//��� ��θ� �׸��� �ݺ���
	for (int i = 0; i < num-1; i++) { //(path�� ���� - 1)��ŭ �ݺ�
		ofDrawLine(rooms[path[i]].x1, rooms[path[i]].y1, rooms[path[i+1]].x1, rooms[path[i+1]].y1); //�����忡�� ���������� ���� �׸�
	}

	//���� ��� ǥ�ø� ���� ���� ����� �β��� �ٽ� ����
	ofSetColor(0,255,0);
	ofSetLineWidth(2);

	//���� ��θ� �׸��� �ݺ���
	for (int j = 0; j < best_num - 1; j++) { //(best�� ���� -1)��ŭ �ݺ�
		ofDrawLine(rooms[best[j]].x1, rooms[best[j]].y1, rooms[best[j + 1]].x1, rooms[best[j + 1]].y1); //�����忡�� ���������� ���� �׸�
	}
}

bool ofApp::BFS() //BFSŽ���� �ϴ� �Լ�
{ 
	queue<int> queue; //queue ����

	parent = new vector<int>[maze_row * maze_col]; //�� ����� �θ� ��带 �����ϴ� vector �迭

	visited = new int[maze_row * maze_col]; //visited malloc and 0���� init
	for (int i = 0; i < maze_row * maze_col; i++) {
		visited[i] = 0;
	}

	q_path = new int[maze_row * maze_col * 5]; //bfsŽ���� ��� ��θ� �����ϴ� q_path�� �����Ҵ��ϰ� -1�� �ʱ�ȭ
	for (int i = 0; i < maze_row * maze_col * 5; i++) {
		q_path[i] = -1;
	}

	num2 = 0; //q_paht�� ����

	int target = maze_row * maze_col - 1; //������ �� ��ȣ
	int v = 0; //������ �� ��ȣ
	queue.push(v); //queue�� ������ �߰�
	visited[v] = 1; //�������� �湮���� = 1
	q_path[num2++] = v; //q_path�� ������ �߰��ϰ� num2 1����

	//BFS �˰����� �����ϸ鼭 ���������� ���������� �湮�ϰ� �Ǵ� ��� ��θ� ã�� �ݺ���
	while (!queue.empty()) { //queue�� ������� ������ �ݺ�
		if (queue.back() == target) { //queue�� ������ ���Ұ� �������̸� �ݺ��� ����
			break;
		}

		//queue�� front�� w�� �����ϰ� front pop
		int w = queue.front();
		queue.pop();
		
		//w�� ������ ��� �߿��� �湮�� ���� ���� ��带 ã�� �ݺ���
		for (int k = 0; k < graph[w].size(); k++) { //w�� ������ ����� ������ŭ �ݺ�
			int u = graph[w][k];
			if (visited[u] == 0) { //u�� �湮�� ���� ���� ���
				//u�� �湮 ���θ� 1��, queue�� q_path�� u�� �߰��ϰ�, u�� �θ� ��带 w�� ����
				visited[u] = 1;
				parent[u].push_back(w);
				queue.push(u);
				q_path[num2++] = u;
			}
		}
	}

	//isbfs�� 1��
	isbfs = 1;

	return 0;
}

void ofApp::bfsdraw() //BFS �˰������� ã�� ��� ��ο� ���� ��θ� ȭ�鿡 �׸��� �Լ�
{
	//��� ��θ� �׸��� �ݺ���. q_path�� �ִ� ������ �θ��带 ���󰡸鼭 draw
	for (int j = num2-1; j > 1; j--) { //q_path�� ���� -1 ��ŭ �ݺ�
		int node = parent[q_path[j]].back(); //node = q_path�� ������ ����� �θ���
		ofDrawLine(rooms[q_path[j]].x1, rooms[q_path[j]].y1, rooms[node].x1, rooms[node].y1); //q_path�� ������ ���� q_path�� ������ ����� �θ��� ������ ���� �׸�

		while (node != 0) { //node�� 0�� �ƴ� ���� �ݺ�, �� ���������� �ݺ�
			int node2 = parent[node].back(); //node2�� node�� �θ���
			ofDrawLine(rooms[node].x1, rooms[node].y1, rooms[node2].x1, rooms[node2].y1); //node�� node�� �θ��� ������ ���� �׸�
			node = node2; //node�� node�� �θ��尡 �ǰ� ������ �۾� �ݺ�
		}
	}

	//���� ��θ� �׸��� ���� ���� ����� �β� ����
	ofSetColor(0, 255, 0);
	ofSetLineWidth(2);
	int node = parent[q_path[num2-1]].back(); //node = q_path�� ������ ����� �θ���. q_path�� ������ ���� �������̹Ƿ� �������� �θ��带 ���󰡴� ���� ���������
	ofDrawLine(rooms[q_path[num2-1]].x1, rooms[q_path[num2-1]].y1, rooms[node].x1, rooms[node].y1); //q_path�� ������ ���� �θ��� ������ ���� �׸�

	//���� ��θ� �׸��� �ݺ���
	while (node != 0) { //node�� 0�� �ƴ� ���� �ݺ�, �� ���������� �ݺ�
		int node2 = parent[node].back(); //node2�� node�� �θ���
		ofDrawLine(rooms[node].x1, rooms[node].y1, rooms[node2].x1, rooms[node2].y1); //node�� node�� �θ��� ������ ���� �׸�
		node = node2; //node�� node�� �θ��尡 �ǰ� ������ �۾� �ݺ�
	}
}

void ofApp::play_draw()  //������� �Է����κ��� ���� ��θ� �׸��� �Լ�
{
	//����ڰ� �Է��� ��θ� ǥ���ϱ� ���� ���� ����� �β��� ����
	ofSetColor(0, 0, 255);
	ofSetLineWidth(3);

	//����ڰ� �Է��� ��θ� ȭ�鿡 �׸��� �ݺ���
	for (int k = 0; k < p_path.size() - 1; k++) { //p_path�� ũ�� -1��ŭ �ݺ�
		int cur = p_path[k]; //���� ���
		int ne = p_path[k + 1]; //���� ���
		ofDrawLine(rooms[cur].x1, rooms[cur].y1, rooms[ne].x1, rooms[ne].y1); //���� ���� ���� ��� ������ ���� �׸�
	}

	if (p_path.back() == maze_col * maze_row - 1) { //����ڰ� �Է��� ��ΰ� �������� �������� �� 
		isplay = 2; //isplay�� 2��
	}
}

void ofApp::success_draw() //����ڰ� �������� �������� �� ���������� �̷θ� Ż���� ���� ��Ÿ���� ���� �̷θ� ���� �׸��� �Լ�
{
	//���������� Ż���� �̷��� ����� �β� ����
	ofSetColor(51, 153, 51);
	ofSetLineWidth(5);

	//maze �迭�� ���� �̷��� ��� ���� �׸��� �ݺ��� 
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (maze[i][j] == '-' && i % 2 == 0) { //maze[i][j] ���� '-'�̰�, i�� ¦���� ��� (Ȧ���� ���� ���� ��ȣ�� ����Ǿ� �ֱ� ����)
				//�� ���� �����Ͽ� ���� �׸���. �� �� ������ �迭 dots�� .maz ���Ͽ��� '+'�� x, y ��ǥ�� ������ ����ü �迭��. 
				ofDrawLine(dots[i / 2][(j + 1) / 2].x1, dots[i / 2][(j + 1) / 2].y1, dots[i / 2][(j - 1) / 2].x1, dots[i / 2][(j - 1) / 2].y1);
			}
			else if (maze[i][j] == '|') { //maze[i][j] ���� '|'�� ���
				//�� ���� �����Ͽ� ���� �׸���.
				ofDrawLine(dots[(i + 1) / 2][j / 2].x1, dots[(i + 1) / 2][j / 2].y1, dots[(i - 1) / 2][j / 2].x1, dots[(i - 1) / 2][j / 2].y1);
			}
		}
	}

	//������� ���� ��θ� ǥ���ϱ� ���� ���� ���� ����
	ofSetColor(0, 0, 255);

	//������� ���� ��θ� �׸��� �ݺ���
	for (int k = 0; k < p_path.size()-1; k++) {//p_path�� ũ�� -1��ŭ �ݺ�
		int cur = p_path[k]; //���� ��� 
		int ne = p_path[k + 1]; //���� ��� 
		ofDrawLine(rooms[cur].x1, rooms[cur].y1, rooms[ne].x1, rooms[ne].y1); //���� ���� ���� ��� ������ ���� �׸�
	}
}

void ofApp::movement(int next) //Ű �Է¿� ���� �ش��ϴ� ���� �̵��ϴ� �Լ�
{
	//���� ���� ��ȣ�� ���� ��� ������ ������� �˻��ϰ�, ������ ����� if�� ����
	if (find(graph[p_path.back()].begin(), graph[p_path.back()].end(), next) != graph[p_path.back()].end()) {
		if (play_visit[next] == 1) { //�̹� �湮�� ���� �ִ� ��. �� �̹� �Դ� ���� �ǵ��ư� ��츦 �ǹ���.
			play_visit[p_path.back()] = 0; //���� ���� �湮 ���θ� 0����
			p_path.pop_back(); //p_path���� �ش� �� ��ȣ ����(�ǵ��� ���� ������)
		}
		else { //�湮�� ���� ���� ���̶�� next�� p_path�� �����ϰ�, next�� �湮 ���θ� �湮�� ������ ����
			p_path.push_back(next);
			play_visit[next] = 1;
		}
	}
}
/*

	ofxWinMenu basic example - ofApp.h

	Copyright (C) 2016-2017 Lynn Jarvis.

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
*/

#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon
#include <stack>


typedef struct Dot { //x, y 좌표를 저장하는 구조체
	int x1;
	int y1;
};

class ofApp : public ofBaseApp {

public:

	void setup(); //초기 설정
	void update();
	void draw(); //화면에 그림을 그리는 함수

	void keyPressed(int key); // Traps escape key if exit disabled
	void keyReleased(int key); //사용자가 키를 release했을 때 적절한 동작을 수행하는 함수
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	bool readFile(); //.maz 파일을 열고 읽어오는 함수
	void freeMemory(); //프로그램에서 사용한 메모리를 해제하는 함수
	bool DFS(); //DFS 탐색
	void dfsdraw(); //DFS 탐색을 통해 얻은 경로를 화면에 그리는 함수
	bool BFS(); //BFS 탐색
	void bfsdraw(); //BFS 탐색을 통해 얻은 경로를 화면에 그리는 함수
	void play_draw(); //사용자가 play하면서 이동한 경로를 그리는 함수
	void success_draw(); //사용자가 미로를 탈출했을 때 미로와 경로를 그리는 함수
	void movement(int next); //키 입력에 따라 해당하는 노드로 이동하는 함수

	int HEIGHT;//미로의 높이
	int WIDTH;//미로의 너비
	int* visited;//방문여부를 저장할 포인터
	int* play_visit; //사용자가 play할 때 방문여부를 저장할 포인터
	int maze_col;//미로칸의 열의 개수
	int maze_row;//미로칸의 행의 개수
	int isOpen; //파일이 열렸는지를 판단하는 변수. 0이면 안열렸고 1이면 열렸다.
	int num; //path의 길이
	int num2; //q_path의 길이
	int best_num; //best의 길이

	Dot** dots; //'+'의 x,y좌표를 저장하는 2차원 구조체 배열
	Dot* rooms; //방의 x,y좌표를 저장하는 구조체 배열
	int** maze; //.maz의 데이터를 저장하는 2차원 배열
	vector<int>* graph; //노드들의 인접노드를 저장하는 vector 배열
	vector<int>* parent; //노드의 부모노드를 저장하는 vector 배열
	vector<int> p_path; //사용자가 play하면서 거치는 경로를 저장하는 vector
	int* path; //DFS 탐색에서 모든 경로를 저장하는 포인터
	int* best; //DFS 탐색에서 최적 경로를 저장하는 포인터
	int* q_path; //BFS 탐색에서 모든 경로를 저장하는 포인터

	// Menu
	ofxWinMenu* menu; // Menu object
	void appMenuFunction(string title, bool bChecked); // Menu return function

	// Used by example app
	ofTrueTypeFont myFont;
	ofImage myImage;
	float windowWidth, windowHeight;
	HWND hWnd; // Application window
	HWND hWndForeground; // current foreground window

	// Example menu variables
	bool bShowInfo;
	bool bFullscreen;
	bool bTopmost;
	bool isdfs; //DFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
	bool isbfs; //BFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
	int isplay; //play_draw와 success_draw 실행 여부를 판단하는 변수. 0이면 실행하지 않고, 1이면 play_draw를 실행, 2이면 success_draw 실행

	// Example functions
	void doFullScreen(bool bFull);
	void doTopmost(bool bTop);

};

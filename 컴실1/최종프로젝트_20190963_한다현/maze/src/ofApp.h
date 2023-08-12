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


typedef struct Dot { //x, y ��ǥ�� �����ϴ� ����ü
	int x1;
	int y1;
};

class ofApp : public ofBaseApp {

public:

	void setup(); //�ʱ� ����
	void update();
	void draw(); //ȭ�鿡 �׸��� �׸��� �Լ�

	void keyPressed(int key); // Traps escape key if exit disabled
	void keyReleased(int key); //����ڰ� Ű�� release���� �� ������ ������ �����ϴ� �Լ�
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	bool readFile(); //.maz ������ ���� �о���� �Լ�
	void freeMemory(); //���α׷����� ����� �޸𸮸� �����ϴ� �Լ�
	bool DFS(); //DFS Ž��
	void dfsdraw(); //DFS Ž���� ���� ���� ��θ� ȭ�鿡 �׸��� �Լ�
	bool BFS(); //BFS Ž��
	void bfsdraw(); //BFS Ž���� ���� ���� ��θ� ȭ�鿡 �׸��� �Լ�
	void play_draw(); //����ڰ� play�ϸ鼭 �̵��� ��θ� �׸��� �Լ�
	void success_draw(); //����ڰ� �̷θ� Ż������ �� �̷ο� ��θ� �׸��� �Լ�
	void movement(int next); //Ű �Է¿� ���� �ش��ϴ� ���� �̵��ϴ� �Լ�

	int HEIGHT;//�̷��� ����
	int WIDTH;//�̷��� �ʺ�
	int* visited;//�湮���θ� ������ ������
	int* play_visit; //����ڰ� play�� �� �湮���θ� ������ ������
	int maze_col;//�̷�ĭ�� ���� ����
	int maze_row;//�̷�ĭ�� ���� ����
	int isOpen; //������ ���ȴ����� �Ǵ��ϴ� ����. 0�̸� �ȿ��Ȱ� 1�̸� ���ȴ�.
	int num; //path�� ����
	int num2; //q_path�� ����
	int best_num; //best�� ����

	Dot** dots; //'+'�� x,y��ǥ�� �����ϴ� 2���� ����ü �迭
	Dot* rooms; //���� x,y��ǥ�� �����ϴ� ����ü �迭
	int** maze; //.maz�� �����͸� �����ϴ� 2���� �迭
	vector<int>* graph; //������ ������带 �����ϴ� vector �迭
	vector<int>* parent; //����� �θ��带 �����ϴ� vector �迭
	vector<int> p_path; //����ڰ� play�ϸ鼭 ��ġ�� ��θ� �����ϴ� vector
	int* path; //DFS Ž������ ��� ��θ� �����ϴ� ������
	int* best; //DFS Ž������ ���� ��θ� �����ϴ� ������
	int* q_path; //BFS Ž������ ��� ��θ� �����ϴ� ������

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
	bool isdfs; //DFS�Լ��� ������״��� �Ǵ��ϴ� ����. 0�̸� ������߰� 1�̸� �����ߴ�.
	bool isbfs; //BFS�Լ��� ������״��� �Ǵ��ϴ� ����. 0�̸� ������߰� 1�̸� �����ߴ�.
	int isplay; //play_draw�� success_draw ���� ���θ� �Ǵ��ϴ� ����. 0�̸� �������� �ʰ�, 1�̸� play_draw�� ����, 2�̸� success_draw ����

	// Example functions
	void doFullScreen(bool bFull);
	void doTopmost(bool bTop);

};

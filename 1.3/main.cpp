#include <windows.h>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <vector>

using namespace std;

int listID;

//����һ����ά����
struct coordinate {	
	GLfloat x, y, z;
	coordinate(GLfloat a, GLfloat b, GLfloat c) : x(a), y(b), z(c) {};
};

//�����������
struct face {
	int facenum;	
	bool four;		//�Ƿ����ĸ���
	int faces[4];
	face(int facen, int f1, int f2, int f3) : facenum(facen) {		//����Ǹ�3��int�����½��Ļ���һ����������
		faces[0] = f1;
		faces[1] = f2;
		faces[2] = f3;
		four = false;
	}
	face(int facen, int f1, int f2, int f3, int f4) : facenum(facen) { //����Ǹ�4��int�����½��Ļ���һ���ı�����
		faces[0] = f1;
		faces[1] = f2;
		faces[2] = f3;
		faces[3] = f4;
		four = true;
	}
};


class ObjModel		//����һ��objģ����
{
private:
	std::string modelName;				//ģ�͵�����
	std::vector<coordinate*> vertex;	//�洢�Ķ���ļ���
	std::vector<face*> faces;			//�洢����������ļ���
	std::vector<coordinate*> normals;      //�洢�ķ������ļ���

public:
	ObjModel();						//�������½�һ��������
	ObjModel(string m);				//ͨ���½�ģ�͵�����������ģ��
	
	void ObjModel::setName(string m);		//����ģ������
	//void copyFrom(ObjModel origin);
	//float* ObjModel::calculateNormal(float* coord1, float* coord2, float* coord3);	//���㷨��������
	void ObjModel::Load();		//��ȡ���ģ��
	int ObjModel::Draw();					// ����ģ��
	void ObjModel::Release();				// �ͷ�ģ�������ڴ�ռ�

};

//����һ��window�ṹ�Է��������
typedef struct {
	int width;		//���ڴ�С
	int height;
	char* title;	//������

	float view_angle;
	float z_near;
	float z_far;
} glutWindow;

glutWindow win;

ObjModel obj("monkey.obj");
//ObjModel transFer;

ObjModel::ObjModel()	//û�ж�ȡʱĬ�Ͻ�defalut
{
	modelName = "defalut";
}

ObjModel::ObjModel(string m)
{
	modelName = m;
}

void copyFrom(ObjModel origin) {

}

void ObjModel::setName(string m) {
	modelName = m;
}

void ObjModel::Load()
{
	std::ifstream in(modelName);	//���Դ�obj
	if (!in.is_open())				//����򲻿���ֱ���˳�����
	{
		cout << "can not open:" <<modelName<< endl;
		exit(0);
	}
	else {
		char buf[256];						//һ�е�����һ�㲻�ᳬ����ô�ࣨ256���ַ���
		std::vector<std::string*> coord;      //�洢ÿһ�е����ݵ���ʱ����
		while (!in.eof())					//��ʼ��ÿһ�����ݷ���������
		{
			in.getline(buf, 256);
			coord.push_back(new std::string(buf));
		}
		for (int i = 0; i<coord.size(); i++)	//������ʱ��������ȡÿһ�е����ݽ��з���
		{
			//�����ͷ��'#'�Ļ���ʾע��,�Ϳ���ֱ��������
			if (coord[i]->c_str()[0] == '#')   
				continue;
			//����ǿ�ͷ��v���Һ����ǿո�Ļ�������һ�����㣨��Ϊ����vn�ȣ��������ﲻ�ܵ������жϵ�һ���ַ���
			else if (coord[i]->c_str()[0] == 'v' && coord[i]->c_str()[1] == ' ') 
			{
				GLfloat tmpx, tmpy, tmpz;		//����������ʱ����
				sscanf(coord[i]->c_str(), "v %f %f %f", &tmpx, &tmpy, &tmpz);	//��ȡ
				vertex.push_back(new coordinate(tmpx, tmpy, tmpz));		//����push��vector����
			}
			//����ǿ�ͷ��vn������һ������
			else if (coord[i]->c_str()[0] == 'v' && coord[i]->c_str()[1] == 'n')  
			{	
				GLfloat tmpx, tmpy, tmpz;		//���ƵĲ���
				sscanf(coord[i]->c_str(), "vn %f %f %f", &tmpx, &tmpy, &tmpz);
				normals.push_back(new coordinate(tmpx, tmpy, tmpz));
			}
			//����ǿ�ͷ��f������һ���������
			/*����Ҫ��
				��Ϊ������������ڲ�ͬ��obj�´�ĸ�ʽ���ܻ��в��죬�����ʽ�����޷���ȷ����!
				�����õ���blender���ɵģ�"f 443//401 319//401 311//401 313//401" �ĸ�ʽ��3����4�������ԣ�;
				������ʽҲ��ʹ��/����ֱ��ʹ�ÿո����ġ�
				�鿴��ʽ�Ļ�������ֱ��ʹ�ü��±���obj�鿴��
			*/
			else if (coord[i]->c_str()[0] == 'f')
			{
				int a, b, c, d, e;
				//�����һ����������,һ����ͻ���3���ո�
				if (count(coord[i]->begin(), coord[i]->end(), ' ') == 3)	
				{
					sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d", &a, &b, &c, &b, &d, &b);	//��ȡ
					faces.push_back(new face(b, a, c, d));		//����
				}
				//��������ı�����
				else {
					sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d %d//%d", &a, &b, &c, &b, &d, &b, &e, &b); //��ȡ
					faces.push_back(new face(b, a, c, d, e));   //����
				}
			}
		}

		//��������ʱ��vector��Ϊ�˷�ֹ�������
		for (int i = 0; i<coord.size(); i++)
			delete coord[i];
	}

}

//���
void ObjModel::Release()
{
	for (int i = 0; i<faces.size(); i++)
		delete faces[i];
	for (int i = 0; i<normals.size(); i++)
		delete normals[i];
	for (int i = 0; i<vertex.size(); i++)
		delete vertex[i];
}

//����
/*ʵ�����Ƕ������Ժ����list���棬���Ҫ���ƵĻ�����display��ͨ�����ص�list��ID����List�Դﵽ��ʾЧ��*/
int ObjModel::Draw()
{
	int num;					//list��ID��Ҳ�Ƿ��ص�ֵ
	num = glGenLists(1);		//�������ID
	glNewList(num, GL_COMPILE);      //�ٴ������List
	for (int i = 0; i<faces.size(); i++)	//��ȡ������Ҫ��ÿһ����
	{
		if (faces[i]->four)      //������ı����棬�����ı���������
		{
			glBegin(GL_QUADS);
			//���÷���������Ҫ��Ϊ�˼�����գ�
			glNormal3f(normals[faces[i]->facenum - 1]->x, normals[faces[i]->facenum - 1]->y, normals[faces[i]->facenum - 1]->z);
			//draw the faces
			/* //for debug ���Բ鿴ÿһ�����ϵ�ÿһ���������
				cout << "x1: " << vertex[faces[i]->faces[0] - 1]->x << " "
				<< "y1: " << vertex[faces[i]->faces[0] - 1]->y << " " 
				<< "z1: " << vertex[faces[i]->faces[0] - 1]->z << " " << "\n"
				<< "x2: " << vertex[faces[i]->faces[1] - 1]->x << " "
				<< "y2: " << vertex[faces[i]->faces[1] - 1]->y << " "
				<< "z2: " << vertex[faces[i]->faces[1] - 1]->z << " " << "\n"
				<< "x3: " << vertex[faces[i]->faces[2] - 1]->x << " "
				<< "y3: " << vertex[faces[i]->faces[2] - 1]->y << " "
				<< "z4: " << vertex[faces[i]->faces[2] - 1]->z << " " << "\n"
				<< "x4: " << vertex[faces[i]->faces[3] - 1]->x << " "
				<< "y4: " << vertex[faces[i]->faces[3] - 1]->y << " "
				<< "z4: " << vertex[faces[i]->faces[3] - 1]->z << " "
				<<"\n"<<"------------------------------------------"<< endl;
			*/
			//����������ϵĶ�������
			glVertex3f(vertex[faces[i]->faces[0] - 1]->x, vertex[faces[i]->faces[0] - 1]->y, vertex[faces[i]->faces[0] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[1] - 1]->x, vertex[faces[i]->faces[1] - 1]->y, vertex[faces[i]->faces[1] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[2] - 1]->x, vertex[faces[i]->faces[2] - 1]->y, vertex[faces[i]->faces[2] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[3] - 1]->x, vertex[faces[i]->faces[3] - 1]->y, vertex[faces[i]->faces[3] - 1]->z);
			glEnd();
		}
		//else�������������������߼���ʵ�ֶ������ı��������Ƶ�
		else {
			glBegin(GL_TRIANGLES);
			glNormal3f(normals[faces[i]->facenum - 1]->x, normals[faces[i]->facenum - 1]->y, normals[faces[i]->facenum - 1]->z);
			glVertex3f(vertex[faces[i]->faces[0] - 1]->x, vertex[faces[i]->faces[0] - 1]->y, vertex[faces[i]->faces[0] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[1] - 1]->x, vertex[faces[i]->faces[1] - 1]->y, vertex[faces[i]->faces[1] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[2] - 1]->x, vertex[faces[i]->faces[2] - 1]->y, vertex[faces[i]->faces[2] - 1]->z);
			glEnd();
		}
	}
	glEndList();
	return num;
}

GLfloat angle = 0.1f;		//������Ϊ��չʾ��ת���ӵı���

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0, 1, 4, 0, 0, 0, 0, 1, 0);
	glPushMatrix();
	glRotatef(angle, 0, 1, 0);
	glRotatef(90, 0, 1, 0);
	angle += 0.01f;
	glCallList(listID);			//����Ҫ��ͨ��ListIDȥʵ�ֻ���
	glPopMatrix();
	glutSwapBuffers();
}


void init()
{
	//һЩ����
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glViewport(0, 0, win.width, win.height);
	GLfloat aspect = (GLfloat)win.width / win.height;
	glLoadIdentity();
	gluPerspective(win.view_angle, aspect, win.z_near, win.z_far);
	glMatrixMode(GL_MODELVIEW);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//���պͲ��ʵ�����
	GLfloat amb_light[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat diffuse[] = { 0.6, 0.6, 0.6, 1 };
	GLfloat specular[] = { 0.7, 0.7, 0.3, 1 };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	obj.Load();				//��ȡobj
	listID = obj.Draw();			//����obj������������Ȼ�����list��
}

//���̿��ƣ���q�˳�����
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
		exit(0);
		break;
	default:
		break;
	}
}
void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);//�ı���ʾ������ʼλ��Ϊ�ͻ��˴������½ǣ�������ԭ�㣩
	glMatrixMode(GL_PROJECTION);		//�޸�ͶӰ����
	glLoadIdentity();					//���뵥λ��
	gluPerspective(win.view_angle, (float)w / h, win.z_near, win.z_far);//��߱ȸ�Ϊ��ǰֵ��������������Ļ��Сһ�£�
	glMatrixMode(GL_MODELVIEW);			//�޸�ģ����ͼ
	glLoadIdentity();
	gluLookAt(0, 1, 4, 0, 0, 0, 0, 1, 0);
}
int main(int argc, char **argv)
{
	//����window�Ĳ���������ʾ
	win.width = 400;
	win.height = 400;
	win.title = "hw1.3";
	win.view_angle = 45;
	win.z_near = 1.0f;
	win.z_far =1000.0f;

	//��ʼ���ƴ���
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(win.width, win.height);
	glutCreateWindow(win.title);

	init();												//��ʼ��
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);			//���ڱ����������ڵ�ʱ��ģ�Ͳ�����
	glutMainLoop();
	return 0;
}
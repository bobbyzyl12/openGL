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

#define Pi 3.141592654
#define spaceKey 32
//����һ����ά����
struct coordinate {	
	GLfloat x, y, z;
	coordinate(GLfloat a, GLfloat b, GLfloat c) : x(a), y(b), z(c) {};
};

//��������ַ�����굥λ���ķ�������������struct��
void unitCoord(coordinate* coord);

//������ֵ����������ֵ�е��м�x%�Ĵ�С
GLfloat calmid(GLfloat a, GLfloat b, int per, int total){
	GLfloat dis = b - a;
	GLfloat res= (dis*((GLfloat)per / (GLfloat)total) + a);
	return res;
}

//���㷨��������
coordinate calculateNormalTRI(coordinate* coord1, coordinate* coord2, coordinate* coord3);	//������

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
	
public:
	ObjModel();						//�������½�һ��������
	ObjModel(string m);				//ͨ���½�ģ�͵�����������ģ��
	
	void ObjModel::setName(string m);		//����ģ������

	void ObjModel::copyFacesFrom(ObjModel old);
	void ObjModel::copyFrom(ObjModel old);
	void ObjModel::unitVertex();
	void ObjModel::trans(ObjModel start, ObjModel end, int current, int total);

	void ObjModel::Load();		//��ȡ���ģ��
	int ObjModel::Draw();					// ����ģ��
	void ObjModel::Release();				// �ͷ�ģ�������ڴ�ռ�

	std::string modelName;				//ģ�͵�����
	std::vector<coordinate*> vertex;	//�洢�Ķ���ļ���
	std::vector<face*> faces;			//�洢����������ļ���
	std::vector<coordinate*> normals;      //�洢�ķ������ļ���

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

ObjModel::ObjModel()	//û�ж�ȡʱĬ�Ͻ�defalut
{
	modelName = "defalut";
}

ObjModel::ObjModel(string m)
{
	modelName = m;
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
			//glNormal3f(normals[faces[i]->facenum - 1]->x, normals[faces[i]->facenum - 1]->y, normals[faces[i]->facenum - 1]->z);
			glNormal3f(normals[i]->x, normals[i]->y, normals[i]->z);
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
			//glNormal3f(normals[faces[i]->facenum - 1]->x, normals[faces[i]->facenum - 1]->y, normals[faces[i]->facenum - 1]->z);
			glNormal3f(normals[i]->x, normals[i]->y, normals[i]->z);
			glVertex3f(vertex[faces[i]->faces[0] - 1]->x, vertex[faces[i]->faces[0] - 1]->y, vertex[faces[i]->faces[0] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[1] - 1]->x, vertex[faces[i]->faces[1] - 1]->y, vertex[faces[i]->faces[1] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[2] - 1]->x, vertex[faces[i]->faces[2] - 1]->y, vertex[faces[i]->faces[2] - 1]->z);
			glEnd();
		}
	}
	glEndList();
	return num;
}


void ObjModel::copyFacesFrom(ObjModel old) {
	for (int i = 0; i < old.faces.size(); ++i) {
		this->faces.push_back(old.faces[i]);
	}
}

void ObjModel::copyFrom(ObjModel old) {
	//�½��������ǵ����İ�ָ�����
	for (int i = 0; i < old.faces.size(); ++i) {
		if (old.faces[i]->four) {
			faces.push_back(new face(old.faces[i]->facenum, old.faces[i]->faces[0], old.faces[i]->faces[1], old.faces[i]->faces[2], old.faces[i]->faces[3]));
		}
		else {
			faces.push_back(new face(old.faces[i]->facenum, old.faces[i]->faces[0], old.faces[i]->faces[1], old.faces[i]->faces[2]));
		}
	}
	for (int i = 0; i < old.vertex.size(); ++i) {
		vertex.push_back(new coordinate(old.vertex[i]->x, old.vertex[i]->y, old.vertex[i]->z));
	}
	for (int i = 0; i < old.normals.size(); ++i) {
		normals.push_back(new coordinate(old.normals[i]->x, old.normals[i]->y, old.normals[i]->z));
	}
}

void ObjModel::unitVertex() {
	//�ȶ��������굥λ��
	for (int i = 0; i < vertex.size(); ++i) {
		unitCoord(vertex[i]);
	}
	//ɾ�����з�����
	int nsize = normals.size();
	for (int i = 0; i <nsize; ++i) {
		normals.pop_back();
	}
	//cout << "n size�� " << normals.size() << endl;
	//�����¶�������������¼�����ķ�����
	for (int i = 0; i < faces.size(); ++i) {
		if (faces[i]->four)//��������������
		{	
			struct coordinate temp = calculateNormalTRI(vertex[faces[i]->faces[0] - 1],
				vertex[faces[i]->faces[1] - 1],
				vertex[faces[i]->faces[2] - 1]
				);
			//cout << 'q' << temp.x << " " << temp.y << " " << temp.z << endl;
			normals.push_back(new coordinate(temp.x,temp.y,temp.z));
		}
		else{			//��������
			struct coordinate temp = calculateNormalTRI(vertex[faces[i]->faces[0] - 1],
				vertex[faces[i]->faces[1] - 1],
				vertex[faces[i]->faces[2] - 1]
				);
			//cout <<'t'<< temp.x << " " << temp.y << " " << temp.z << endl;
			normals.push_back(new coordinate(temp.x, temp.y, temp.z));
		}
	}
}

void  ObjModel::trans(ObjModel start, ObjModel end,int current,int total){
	//�ȴ�������
	for (int i = 0; i < vertex.size(); ++i) {
		//���Ա仯
		vertex[i]->x = calmid((start.vertex[i]->x) ,(end.vertex[i]->x),current,total);
		//cout << start.vertex[i]->x << " " << end.vertex[i]->x <<" "<<vertex[i]->x<< endl;
		vertex[i]->y = calmid((start.vertex[i]->y), (end.vertex[i]->y), current, total);
		vertex[i]->z = calmid((start.vertex[i]->z), (end.vertex[i]->z), current, total);
	}
	//ɾ�����з�����
	int nsize = normals.size();
	for (int i = 0; i <nsize; ++i) {
		normals.pop_back();
	}
	//�����¶�������������¼�����ķ�����
	for (int i = 0; i < faces.size(); ++i) {
		if (faces[i]->four)//��������������
		{
			struct coordinate temp = calculateNormalTRI(vertex[faces[i]->faces[0] - 1],
				vertex[faces[i]->faces[1] - 1],
				vertex[faces[i]->faces[2] - 1]
				);
			//cout << temp.x << " " << temp.y << " " << temp.z << endl;
			normals.push_back(new coordinate(temp.x, temp.y, temp.z));
		}
		else {			//��������
			struct coordinate temp = calculateNormalTRI(vertex[faces[i]->faces[0] - 1],
				vertex[faces[i]->faces[1] - 1],
				vertex[faces[i]->faces[2] - 1]
				);
			//cout << temp.x << " " << temp.y << " " << temp.z << endl;

			normals.push_back(new coordinate(temp.x, temp.y, temp.z));
		}
	}
}

//�����굥λ���ķ�������������struct��
void unitCoord(coordinate* coord){
	GLfloat len = sqrt((coord->x)*(coord->x) + (coord->y)*(coord->y) + (coord->z)*(coord->z));
	coord->x = (coord->x) / len;
	coord->y = (coord->y) / len;
	coord->z = (coord->z) / len;
}

coordinate calculateNormalTRI(coordinate* coord1, coordinate* coord2, coordinate* coord3) {
	/* �������� */
	GLfloat va[3], vb[3], vr[3];
	va[0] = coord1->x - coord2->x;
	va[1] = coord1->y - coord2->y;
	va[2] = coord1->z - coord2->z;

	vb[0] = coord1->x - coord3->x;
	vb[1] = coord1->y - coord3->y;
	vb[2] = coord1->z - coord3->z;

	/* ���㷨ʸ�� */
	vr[0] = va[1] * vb[2] - vb[1] * va[2];
	vr[1] = vb[0] * va[2] - va[0] * vb[2];
	vr[2] = va[0] * vb[1] - vb[0] * va[1];

	/* ��λ�� */
	GLfloat len = sqrt(vr[0] * vr[0] + vr[1] * vr[1] + vr[2] * vr[2]);

	//����
	struct coordinate a = { vr[0] / len,vr[1] / len,vr[2] / len };
	return a;
}


//����
GLfloat angle = 0.1f;		//������Ϊ��չʾ��������תЧ�����ӵı���
glutWindow win;				//���崰��

ObjModel obj("monkey.obj");
ObjModel transferModel;
ObjModel sphereModel;

bool mouseStateR = false;			//��¼����¼�
bool mouseStateL = false;
static GLint mx = 0, my = 0;	//���λ��

coordinate itemPos = { 0.0f,0.0f,0.0f };		//��������
coordinate viewPos = { 0.0f,1.0f,4.0f };		//�۲������

static GLfloat yrotate = Pi / 4; //�۲���y��ļн�
static GLfloat xrotate = Pi / 4; //�۲���x��ļн�
static GLfloat moveSpeed = 0.3f; //�ƶ��ٶ�
static GLfloat rotateSpeed = 0.02f; //��ת�ٶ�
static GLfloat zoomSpeed = 0.05f; //�����ٶ�
static GLfloat viewDis = 5.0f;

int listID;
GLfloat speed = 1.0f;
long times = 100000000000;	//��ʾʱ��
long countFrame = 0;		//��¼��ʾʱ��
int c=0;
int frames=100;
bool start = false;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//�����������۲���λ��
	viewPos.x = itemPos.x + viewDis * sin(yrotate) * cos(xrotate);
	viewPos.y = itemPos.y + viewDis * cos(yrotate);
	viewPos.z = itemPos.z + viewDis * sin(yrotate) * sin(xrotate);
	gluLookAt(viewPos.x, viewPos.y, viewPos.z, itemPos.x,itemPos.y, itemPos.z, 0, 1, 0);

	glPushMatrix();
	//ͣ������ʱ���Զ���ת
	glRotatef(angle, 0, 1, 0);		
	glRotatef(10, 0, 1, 0);
	angle += 0.01f;

	//����ʱ�䣬����������Զ��ر�
	countFrame++;
	if (countFrame > times) exit(0);

	if (c <= frames && start) {		//����Ѿ���ʼ�仯�����ұ仯û�н���
		transferModel.trans(sphereModel, obj, c, frames);	//���ñ���
		listID = transferModel.Draw();			//����obj������������Ȼ���¼id��
		c++; 
		glCallList(listID);			//ͨ��ListIDȥʵ�ֻ���
	}
	else if(!start) {				//û�п�ʼ�仯
		glutSolidSphere(1.0f, 40, 40); //��һ������
	}
	else {							//�仯����
		glCallList(listID);			//ͨ��ListIDȥʵ�ֻ���
	}
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

	/*��������*/
	GLfloat light_position[] = { 1.0f, -1.0f, 1.0f, 0.0f };	//��Դλ��
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	GLfloat light_ambient[] = { 0.1f,0.1f, 0.1f, 1.0f }; //��������
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	GLfloat light_diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f }; //������
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	GLfloat light_specular[] = { 0.7f, 0.7f, 0.4f, 1.0f }; //���淴��
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHTING); //���ù�Դ
	glEnable(GL_LIGHT0);   //ʹ��ָ���ƹ�


	/*��������*/
	GLfloat mat_sp[] = { 1.0f,1.0f,1.0f,1.0f };		//���þ��淴�䷴���
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_sp);
	GLfloat mat_sh[] = { 30.0f };					//���淴��ָ��
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_sh);

	glShadeModel(GL_SMOOTH);		//��ɫ��ʽ����

	glEnable(GL_DEPTH_TEST);		//��Ȼ�������

	obj.Load();				//��ȡ
	sphereModel.setName("sphere");
	transferModel.setName("transfer");
	sphereModel.copyFrom(obj);
	transferModel.copyFrom(obj);
	sphereModel.unitVertex();
	transferModel.unitVertex();
	
}

//���̿��ƣ���q�˳�����
//wsad���������ƶ�
void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
		exit(0);
		break;
	case 's':
	{
		itemPos.x -= moveSpeed * cos(xrotate);
		itemPos.z -= moveSpeed * sin(xrotate);
		break;
	}
	case 'w':
	{
		itemPos.x += moveSpeed * cos(xrotate);
		itemPos.z += moveSpeed * sin(xrotate);
		break;
	}
	case 'a':
	{
		itemPos.z -= moveSpeed * cos(yrotate);
		itemPos.x += moveSpeed * sin(yrotate);
		break;
	}
	case 'd':
	{
		itemPos.z += moveSpeed * cos(yrotate);
		itemPos.x -= moveSpeed * sin(yrotate);
		break;
	}
	case spaceKey:{
		start = true;
	}
	}
	glutPostRedisplay();
}


void mouseDown(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_RIGHT_BUTTON) mouseStateR = true;
		if (button == GLUT_LEFT_BUTTON) mouseStateL= true;
	}
	mx = x, my = y;
}


void mouseMove(int x, int y)
{

	if (mouseStateR)
	{       // �����ٶ�
		xrotate += (x - mx) * rotateSpeed;
		yrotate -= (y - my) * rotateSpeed;
	}
	else if (mouseStateL)
	{
		viewDis += (y - my) * zoomSpeed;
	}
	mx = x, my = y;
	glutPostRedisplay();
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);//�ı���ʾ������ʼλ��Ϊ�ͻ��˴������½ǣ�������ԭ�㣩
	glMatrixMode(GL_PROJECTION);		//�޸�ͶӰ����
	glLoadIdentity();					//���뵥λ��
	gluPerspective(win.view_angle, (float)w / h, win.z_near, win.z_far);//��߱ȸ�Ϊ��ǰֵ��������������Ļ��Сһ�£�
	glMatrixMode(GL_MODELVIEW);			//�޸�ģ����ͼ
	glLoadIdentity();
	gluLookAt(viewPos.x, viewPos.y, viewPos.z, itemPos.x, itemPos.y, itemPos.z, 0, 1, 0);
}

int main(int argc, char **argv)
{
	//����window�Ĳ���������ʾ
	win.width = 400;
	win.height = 400;
	win.title = "hw1.4";
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
	glutMouseFunc(mouseDown);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(mouseMove);
	glutReshapeFunc(reshape);			//���ڱ����������ڵ�ʱ��ģ�Ͳ�����
	glutMainLoop();
	return 0;
}
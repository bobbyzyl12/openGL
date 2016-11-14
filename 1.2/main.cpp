//��ʱ��û����������ô��ɸ������ķ�����ת����ά��ת��ά����������һ�������ת

/*
ƽ��:�ṩ��6��������������ƽ��:w\s\a\d\z\x��x��y��z�����������
��ת:�ṩ��3����������������ת(i/o/q)
���ţ���������ҷ
�˴��ṩ������������������ں���
*/

#include <GL/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

const GLfloat Pi = 3.1415926536f;

/*��¼��ά�ռ��ڵ���Ҫ�ĵ������*/
typedef struct Poisition_3D	//3D����
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
//��Դλ��
GLfloat light_position[] = { 1.0f, -1.0f, 1.0f, 1.0f };

//��¼����¼�
bool mouseStatusR = false;
bool mouseStatusL = false;

/*����2d���ϵľ��뺯��*/
GLfloat calDistance_2D(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2) {
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

/*����3d���ϵľ��뺯��*/
GLfloat calDistance_3D(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2) {
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) + (z1 - z2)*(z1 - z2));
}

GLfloat radiusX = 0;
GLfloat radiusZ = 0; 
GLfloat radiusY = 0;

GLfloat pointsDistance_3D(Poisition_3D a, Poisition_3D b) {
	return sqrt((a.x - b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z));
}

/*��������Ϊ��λ������3ά��*/
void d3Unit(Poisition_3D a)
{
	GLfloat length = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
	a.x = (a.x / length);
	a.y = (a.y / length);
	a.z = (a.z / length);
}


Poisition_3D itemPos;	//����λ��
Poisition_3D viewPos;	//�۲��λ��
Poisition_3D focusPos;	//�۲��λ��
Poisition_3D dis;//����

/*����������۲��֮�������*/
void updateDis() {
	dis.x = itemPos.x - viewPos.x;
	dis.y = itemPos.y - viewPos.y;
	dis.z = itemPos.z - viewPos.z;
	GLfloat length = sqrt(dis.x*dis.x + dis.y*dis.y + dis.z*dis.z);
	dis.x = (dis.x / length);
	dis.y = (dis.y / length);
	dis.z = (dis.z / length);
}


//��¼���λ��
static GLint mx = 0, my = 0;
							  
static GLfloat mSpeed = 0.2f; //�ƶ��ٶ�
static GLfloat rSpeed = 5.0f; //��ת�ٶ�
static GLfloat lSpeed = 0.1f; //�����ٶ�
							  //static bool p = false;

void init(void)
{
	glClearColor(0, 0, 0, 0);	//��ʼ������ɫ
								/*����ȫ�ֱ����ĳ�ʼ��*/
	mouseStatusR = false;		//���״̬
	mouseStatusL = false;
	mx = 0;
	my = 0;
	itemPos.x = 0.0f;			//����λ��
	itemPos.y = 0.0f;
	itemPos.z = 0.0f;
	viewPos.x = 3.0f;			//�۲��
	viewPos.y = 3.0f;
	viewPos.z = 3.0f;
	focusPos.x = 0.0f;			//����λ��
	focusPos.y = 0.0f;
	focusPos.z = 0.0f;
	updateDis();
}

void mouseDown(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		if (button == GLUT_RIGHT_BUTTON) mouseStatusR = true;
		if (button == GLUT_LEFT_BUTTON) mouseStatusL = true;
		mx = x;
		my = y;
	}	
}

void mouseMove(int x, int y)
{

	if (mouseStatusL)		//���Ų���
	{       
		//���Ų������������ı�۲���λ��
		GLfloat moveLen = (y - my) * lSpeed;
		updateDis();	//ʹ��ǰ��updateһ��
		viewPos.x += dis.x * moveLen;  //���¼���۲��λ��
		viewPos.y += dis.y * moveLen;
		viewPos.z += dis.z * moveLen;
	}

	//��ʱ��û����������ô��ɸ������ķ�����ת����ά��ת��ά����������һ�������ת
	/*
	else if (mouseStatusR)	// ������ת
	{
		//xrotate += (x - mx) / 80.0f;	//x��ı仯��
		//yrotate -= (y - my) / 120.0f;	//y��ı仯��
	}*/

	mx = x, my = y;
	glutPostRedisplay();
}

/*
	����λ�øı� =�۲��ı�+�۲콹��ı�+��Դλ�øı�
	���ڱ任��ʽ��ͨ��ת���ӵ�ķ�ʽ,���������ƶ�����Ե�������
	��������������ϵΪ׼
	�ṩ��6��������������ƽ��:w\s\a\d\z\x��x��y��z�����������
	�ṩ��3����������������ת(q\e\r)
*/

void keyDown(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'w':case 'W':
		{
			viewPos.y -= mSpeed;
			focusPos.y -= mSpeed;
			light_position[1] -= mSpeed;
			break;
		}
		case 's': case 'S':
		{
			viewPos.z += mSpeed;
			focusPos.z += mSpeed;
			light_position[2] += mSpeed;
			break;
		}
		case 'a':case 'A':
		{
			viewPos.x += mSpeed;
			focusPos.x += mSpeed;
			light_position[0] += mSpeed;
			break;
		}
		case 'd':case 'D':
		{
			viewPos.x -= mSpeed;
			focusPos.x -= mSpeed;
			light_position[0] -= mSpeed;
			break;
		}
		case 'z':case 'Z':
		{
			viewPos.z -= mSpeed;
			focusPos.z -= mSpeed;
			light_position[2] -= mSpeed;
			break;
		}
		case 'X':case 'x':
		{
			viewPos.y += mSpeed;
			focusPos.y += mSpeed;
			light_position[1] += mSpeed;
			break;
		}
		case 'o': case 'O': {
			radiusX += rSpeed;
			break;
		}
		case 'p': case 'P': {
			radiusY += rSpeed;
			break;
		}
		case 'i':case 'I': {
			radiusZ += rSpeed;
			break;
		}
	}
	glutPostRedisplay();
}


void idle()		//�仯�ӵ�λ��
{
	gluLookAt(viewPos.x, viewPos.y, viewPos.z, focusPos.x, focusPos.y, focusPos.z, 0, 1, 0);
}

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);//����͸�Ӿ��� 
	glLoadIdentity();  //���õ�λ����
	gluPerspective(90.0f, 1.0f, 1.0f, 20.0f);
	glMatrixMode(GL_MODELVIEW); //����ģ����ͼ����
	glLoadIdentity();
	idle();

	/*��������*/
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	GLfloat light_ambient[] = { 0.5f,0.5f, 0.5f, 1.0f }; //��������
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //������
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //���淴��
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

	glRotatef(radiusX, 1.0, 0.0, 0.0);
	glRotatef(radiusY, 0.0, 1.0, 0.0);
	glRotatef(radiusZ, 0.0, 0.0, 1.0);
	glutSolidCube(2.0); //���ڲ�����ת��������
		
	//glutSolidSphere(1.0, 50, 50);	//����
		
	glutSwapBuffers();
}


int main(int argc, char *argv[])
{
	init();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize(500, 500);
	glutCreateWindow("hw1.2");
	glutDisplayFunc(myDisplay);
	glutIdleFunc(idle);  //���ò��ϵ�����ʾ����  
	glutMouseFunc(mouseDown);
	glutKeyboardFunc(keyDown);
	glutMotionFunc(mouseMove);
	glutMainLoop();
	return 0;
}
#include <GL/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

//const int n = 30;
const GLfloat R = 0.5f;
const GLfloat Pi = 3.1415926536f;

void init(void)
{
	glClearColor(0, 0, 0, 0);	//��ʼ������ɫ

	/*��������*/
	GLfloat light_position[] = { 1.0f, -1.0f, 1.0f, 0.0f };	//��Դλ��
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

}

void myDisplay(void)
{
	glClearColor(1, 1, 1, 0);  //���ñ���ɫ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//���buffer
	glutSolidSphere(1.0f, 40, 40); //��һ������
	glFlush();	//����ִ��
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-1.5, 1.5, -1.5*(GLfloat)h / (GLfloat)w, 1.5*(GLfloat)h / (GLfloat)w, -10.0, 10.0);
	else
		glOrtho(-1.5*(GLfloat)w / (GLfloat)h, 1.5*(GLfloat)w / (GLfloat)h, -1.5, 1.5, -10.0, 10.0);

}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
	glutInitWindowSize(500, 500);    //��ʾ���С
	glutInitWindowPosition(100, 100); //ȷ����ʾ�����Ͻǵ�λ��
	glutCreateWindow("homework1.1");
	init();
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
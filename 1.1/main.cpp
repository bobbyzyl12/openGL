#include <GL/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

//const int n = 30;
const GLfloat R = 0.5f;
const GLfloat Pi = 3.1415926536f;

void init(void)
{
	glClearColor(0, 0, 0, 0);	//初始化背景色

	/*光线设置*/
	GLfloat light_position[] = { 1.0f, -1.0f, 1.0f, 0.0f };	//光源位置
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	GLfloat light_ambient[] = { 0.5f,0.5f, 0.5f, 1.0f }; //环境光线
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //漫反射
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //镜面反射
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHTING); //启用光源
	glEnable(GL_LIGHT0);   //使用指定灯光

	/*材质设置*/
	GLfloat mat_sp[] = { 1.0f,1.0f,1.0f,1.0f };		//设置镜面反射反射比
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_sp);
	GLfloat mat_sh[] = { 30.0f };					//镜面反射指数
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_sh);

	glShadeModel(GL_SMOOTH);		//着色方式设置

	glEnable(GL_DEPTH_TEST);		//深度缓冲设置

}

void myDisplay(void)
{
	glClearColor(1, 1, 1, 0);  //设置背景色
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//清空buffer
	glutSolidSphere(1.0f, 40, 40); //画一个球体
	glFlush();	//立即执行
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
	glutInitWindowSize(500, 500);    //显示框大小
	glutInitWindowPosition(100, 100); //确定显示框左上角的位置
	glutCreateWindow("homework1.1");
	init();
	glutDisplayFunc(myDisplay);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}
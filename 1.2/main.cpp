//暂时还没有想明白怎么完成根据鼠标的方向旋转（二维量转三维量），搁置一下鼠标旋转

/*
平移:提供了6个方向进行物体的平移:w\s\a\d\z\x（x，y，z轴的正负方向）
旋转:提供了3个方向对物体进行旋转(i/o/q)
缩放：鼠标左键拖曳
此处提供的是立方体的样例，内含球
*/

#include <GL/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

const GLfloat Pi = 3.1415926536f;

/*记录三维空间内的重要的点的坐标*/
typedef struct Poisition_3D	//3D坐标
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
};
//光源位置
GLfloat light_position[] = { 1.0f, -1.0f, 1.0f, 1.0f };

//记录鼠标事件
bool mouseStatusR = false;
bool mouseStatusL = false;

/*计算2d面上的距离函数*/
GLfloat calDistance_2D(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2) {
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

/*计算3d面上的距离函数*/
GLfloat calDistance_3D(GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2) {
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) + (z1 - z2)*(z1 - z2));
}

GLfloat radiusX = 0;
GLfloat radiusZ = 0; 
GLfloat radiusY = 0;

GLfloat pointsDistance_3D(Poisition_3D a, Poisition_3D b) {
	return sqrt((a.x - b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y) + (a.z-b.z)*(a.z-b.z));
}

/*将向量变为单位向量（3维）*/
void d3Unit(Poisition_3D a)
{
	GLfloat length = sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
	a.x = (a.x / length);
	a.y = (a.y / length);
	a.z = (a.z / length);
}


Poisition_3D itemPos;	//物体位置
Poisition_3D viewPos;	//观察点位置
Poisition_3D focusPos;	//观察点位置
Poisition_3D dis;//向量

/*更新物体与观察点之间的向量*/
void updateDis() {
	dis.x = itemPos.x - viewPos.x;
	dis.y = itemPos.y - viewPos.y;
	dis.z = itemPos.z - viewPos.z;
	GLfloat length = sqrt(dis.x*dis.x + dis.y*dis.y + dis.z*dis.z);
	dis.x = (dis.x / length);
	dis.y = (dis.y / length);
	dis.z = (dis.z / length);
}


//记录鼠标位置
static GLint mx = 0, my = 0;
							  
static GLfloat mSpeed = 0.2f; //移动速度
static GLfloat rSpeed = 5.0f; //旋转速度
static GLfloat lSpeed = 0.1f; //缩放速度
							  //static bool p = false;

void init(void)
{
	glClearColor(0, 0, 0, 0);	//初始化背景色
								/*部分全局变量的初始化*/
	mouseStatusR = false;		//鼠标状态
	mouseStatusL = false;
	mx = 0;
	my = 0;
	itemPos.x = 0.0f;			//物体位置
	itemPos.y = 0.0f;
	itemPos.z = 0.0f;
	viewPos.x = 3.0f;			//观察点
	viewPos.y = 3.0f;
	viewPos.z = 3.0f;
	focusPos.x = 0.0f;			//物体位置
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

	if (mouseStatusL)		//缩放操作
	{       
		//缩放操作可以视作改变观察点的位置
		GLfloat moveLen = (y - my) * lSpeed;
		updateDis();	//使用前先update一下
		viewPos.x += dis.x * moveLen;  //重新计算观察点位置
		viewPos.y += dis.y * moveLen;
		viewPos.z += dis.z * moveLen;
	}

	//暂时还没有想明白怎么完成根据鼠标的方向旋转（二维量转三维量），搁置一下鼠标旋转
	/*
	else if (mouseStatusR)	// 逆向旋转
	{
		//xrotate += (x - mx) / 80.0f;	//x轴的变化量
		//yrotate -= (y - my) / 120.0f;	//y轴的变化量
	}*/

	mx = x, my = y;
	glutPostRedisplay();
}

/*
	物体位置改变 =观察点改变+观察焦点改变+光源位置改变
	由于变换方式是通过转化视点的方式,所以物体移动到边缘后会拉伸
	方向以世界坐标系为准
	提供了6个方向进行物体的平移:w\s\a\d\z\x（x，y，z轴的正负方向）
	提供了3个方向对物体进行旋转(q\e\r)
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


void idle()		//变化视点位置
{
	gluLookAt(viewPos.x, viewPos.y, viewPos.z, focusPos.x, focusPos.y, focusPos.z, 0, 1, 0);
}

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);//设置透视矩阵 
	glLoadIdentity();  //设置单位矩阵
	gluPerspective(90.0f, 1.0f, 1.0f, 20.0f);
	glMatrixMode(GL_MODELVIEW); //设置模型视图矩阵
	glLoadIdentity();
	idle();

	/*光线设置*/
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

	glRotatef(radiusX, 1.0, 0.0, 0.0);
	glRotatef(radiusY, 0.0, 1.0, 0.0);
	glRotatef(radiusZ, 0.0, 0.0, 1.0);
	glutSolidCube(2.0); //用于测试旋转的立方体
		
	//glutSolidSphere(1.0, 50, 50);	//球体
		
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
	glutIdleFunc(idle);  //设置不断调用显示函数  
	glutMouseFunc(mouseDown);
	glutKeyboardFunc(keyDown);
	glutMotionFunc(mouseMove);
	glutMainLoop();
	return 0;
}
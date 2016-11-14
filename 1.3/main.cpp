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

//定义一个三维坐标
struct coordinate {	
	GLfloat x, y, z;
	coordinate(GLfloat a, GLfloat b, GLfloat c) : x(a), y(b), z(c) {};
};

//定义面的数据
struct face {
	int facenum;	
	bool four;		//是否有四个点
	int faces[4];
	face(int facen, int f1, int f2, int f3) : facenum(facen) {		//如果是给3个int索引新建的话是一个三角形面
		faces[0] = f1;
		faces[1] = f2;
		faces[2] = f3;
		four = false;
	}
	face(int facen, int f1, int f2, int f3, int f4) : facenum(facen) { //如果是给4个int索引新建的话是一个四边形面
		faces[0] = f1;
		faces[1] = f2;
		faces[2] = f3;
		faces[3] = f4;
		four = true;
	}
};


class ObjModel		//定义一个obj模型类
{
private:
	std::string modelName;				//模型的名字
	std::vector<coordinate*> vertex;	//存储的顶点的集合
	std::vector<face*> faces;			//存储的面的索引的集合
	std::vector<coordinate*> normals;      //存储的法向量的集合

public:
	ObjModel();						//单纯的新建一个空向量
	ObjModel(string m);				//通过新建模型的名字来定义模型
	
	void ObjModel::setName(string m);		//设置模型名字
	//void copyFrom(ObjModel origin);
	//float* ObjModel::calculateNormal(float* coord1, float* coord2, float* coord3);	//计算法向量方法
	void ObjModel::Load();		//读取这个模型
	int ObjModel::Draw();					// 绘制模型
	void ObjModel::Release();				// 释放模型所存内存空间

};

//定义一个window结构以方便管理窗口
typedef struct {
	int width;		//窗口大小
	int height;
	char* title;	//窗口名

	float view_angle;
	float z_near;
	float z_far;
} glutWindow;

glutWindow win;

ObjModel obj("monkey.obj");
//ObjModel transFer;

ObjModel::ObjModel()	//没有读取时默认叫defalut
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
	std::ifstream in(modelName);	//尝试打开obj
	if (!in.is_open())				//如果打不开则直接退出程序
	{
		cout << "can not open:" <<modelName<< endl;
		exit(0);
	}
	else {
		char buf[256];						//一行的数据一般不会超过这么多（256个字符）
		std::vector<std::string*> coord;      //存储每一行的数据的临时向量
		while (!in.eof())					//开始将每一行数据放入向量中
		{
			in.getline(buf, 256);
			coord.push_back(new std::string(buf));
		}
		for (int i = 0; i<coord.size(); i++)	//遍历临时向量，读取每一行的数据进行分析
		{
			//如果开头是'#'的话表示注释,就可以直接跳过了
			if (coord[i]->c_str()[0] == '#')   
				continue;
			//如果是开头是v并且后面是空格的话，就是一个顶点（因为存在vn等，所以这里不能单纯的判断第一个字符）
			else if (coord[i]->c_str()[0] == 'v' && coord[i]->c_str()[1] == ' ') 
			{
				GLfloat tmpx, tmpy, tmpz;		//定义三个临时变量
				sscanf(coord[i]->c_str(), "v %f %f %f", &tmpx, &tmpy, &tmpz);	//读取
				vertex.push_back(new coordinate(tmpx, tmpy, tmpz));		//将其push回vector里面
			}
			//如果是开头是vn，就是一个顶点
			else if (coord[i]->c_str()[0] == 'v' && coord[i]->c_str()[1] == 'n')  
			{	
				GLfloat tmpx, tmpy, tmpz;		//类似的操作
				sscanf(coord[i]->c_str(), "vn %f %f %f", &tmpx, &tmpy, &tmpz);
				normals.push_back(new coordinate(tmpx, tmpy, tmpz));
			}
			//如果是开头是f，就是一个面的数据
			/*【重要】
				因为面的数据索引在不同的obj下存的格式可能会有差异，如果格式错误将无法正确加载!
				这里用的是blender生成的："f 443//401 319//401 311//401 313//401" 的格式（3个或4个都可以）;
				其他格式也有使用/或者直接使用空格分离的。
				查看格式的话，可以直接使用记事本打开obj查看！
			*/
			else if (coord[i]->c_str()[0] == 'f')
			{
				int a, b, c, d, e;
				//如果是一个三角形面,一行里就会有3个空格
				if (count(coord[i]->begin(), coord[i]->end(), ' ') == 3)	
				{
					sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d", &a, &b, &c, &b, &d, &b);	//读取
					faces.push_back(new face(b, a, c, d));		//存入
				}
				//否则就是四边形面
				else {
					sscanf(coord[i]->c_str(), "f %d//%d %d//%d %d//%d %d//%d", &a, &b, &c, &b, &d, &b, &e, &b); //读取
					faces.push_back(new face(b, a, c, d, e));   //存入
				}
			}
		}

		//用完了临时的vector，为了防止出错，清空
		for (int i = 0; i<coord.size(); i++)
			delete coord[i];
	}

}

//清空
void ObjModel::Release()
{
	for (int i = 0; i<faces.size(); i++)
		delete faces[i];
	for (int i = 0; i<normals.size(); i++)
		delete normals[i];
	for (int i = 0; i<vertex.size(); i++)
		delete vertex[i];
}

//绘制
/*实质上是东西好以后放入list里面，如果要绘制的话则在display中通过返回的list的ID调用List以达到显示效果*/
int ObjModel::Draw()
{
	int num;					//list的ID，也是返回的值
	num = glGenLists(1);		//生成这个ID
	glNewList(num, GL_COMPILE);      //再创建这个List
	for (int i = 0; i<faces.size(); i++)	//读取我们需要的每一个面
	{
		if (faces[i]->four)      //如果是四边形面，则按照四边形面来画
		{
			glBegin(GL_QUADS);
			//设置法向量（主要是为了计算光照）
			glNormal3f(normals[faces[i]->facenum - 1]->x, normals[faces[i]->facenum - 1]->y, normals[faces[i]->facenum - 1]->z);
			//draw the faces
			/* //for debug 可以查看每一个面上的每一个点的坐标
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
			//设置这个面上的顶点数据
			glVertex3f(vertex[faces[i]->faces[0] - 1]->x, vertex[faces[i]->faces[0] - 1]->y, vertex[faces[i]->faces[0] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[1] - 1]->x, vertex[faces[i]->faces[1] - 1]->y, vertex[faces[i]->faces[1] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[2] - 1]->x, vertex[faces[i]->faces[2] - 1]->y, vertex[faces[i]->faces[2] - 1]->z);
			glVertex3f(vertex[faces[i]->faces[3] - 1]->x, vertex[faces[i]->faces[3] - 1]->y, vertex[faces[i]->faces[3] - 1]->z);
			glEnd();
		}
		//else里是三角形面的情况，逻辑和实现都是与四边形面相似的
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

GLfloat angle = 0.1f;		//单纯的为了展示旋转而加的变量

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0, 1, 4, 0, 0, 0, 0, 1, 0);
	glPushMatrix();
	glRotatef(angle, 0, 1, 0);
	glRotatef(90, 0, 1, 0);
	angle += 0.01f;
	glCallList(listID);			//【重要】通过ListID去实现绘制
	glPopMatrix();
	glutSwapBuffers();
}


void init()
{
	//一些设置
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

	//光照和材质的设置
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

	obj.Load();				//读取obj
	listID = obj.Draw();			//绘制obj（将东西画完然后存入list）
}

//键盘控制，按q退出程序
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
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);//改变显示区域，起始位置为客户端窗口左下角（非坐标原点）
	glMatrixMode(GL_PROJECTION);		//修改投影矩阵
	glLoadIdentity();					//导入单位阵
	gluPerspective(win.view_angle, (float)w / h, win.z_near, win.z_far);//宽高比改为当前值，视线区域与屏幕大小一致；
	glMatrixMode(GL_MODELVIEW);			//修改模型视图
	glLoadIdentity();
	gluLookAt(0, 1, 4, 0, 0, 0, 0, 1, 0);
}
int main(int argc, char **argv)
{
	//设置window的参数用于显示
	win.width = 400;
	win.height = 400;
	win.title = "hw1.3";
	win.view_angle = 45;
	win.z_near = 1.0f;
	win.z_far =1000.0f;

	//开始绘制窗口
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(win.width, win.height);
	glutCreateWindow(win.title);

	init();												//初始化
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);			//用于保持拉动窗口的时候模型不变形
	glutMainLoop();
	return 0;
}
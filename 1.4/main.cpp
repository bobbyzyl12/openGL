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
//定义一个三维坐标
struct coordinate {	
	GLfloat x, y, z;
	coordinate(GLfloat a, GLfloat b, GLfloat c) : x(a), y(b), z(c) {};
};

//将给定地址的坐标单位化的方法（传入坐标struct）
void unitCoord(coordinate* coord);

//给两个值，计算两个值中的中间x%的大小
GLfloat calmid(GLfloat a, GLfloat b, int per, int total){
	GLfloat dis = b - a;
	GLfloat res= (dis*((GLfloat)per / (GLfloat)total) + a);
	return res;
}

//计算法向量方法
coordinate calculateNormalTRI(coordinate* coord1, coordinate* coord2, coordinate* coord3);	//三角面

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
	
public:
	ObjModel();						//单纯的新建一个空向量
	ObjModel(string m);				//通过新建模型的名字来定义模型
	
	void ObjModel::setName(string m);		//设置模型名字

	void ObjModel::copyFacesFrom(ObjModel old);
	void ObjModel::copyFrom(ObjModel old);
	void ObjModel::unitVertex();
	void ObjModel::trans(ObjModel start, ObjModel end, int current, int total);

	void ObjModel::Load();		//读取这个模型
	int ObjModel::Draw();					// 绘制模型
	void ObjModel::Release();				// 释放模型所存内存空间

	std::string modelName;				//模型的名字
	std::vector<coordinate*> vertex;	//存储的顶点的集合
	std::vector<face*> faces;			//存储的面的索引的集合
	std::vector<coordinate*> normals;      //存储的法向量的集合

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

ObjModel::ObjModel()	//没有读取时默认叫defalut
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
			//glNormal3f(normals[faces[i]->facenum - 1]->x, normals[faces[i]->facenum - 1]->y, normals[faces[i]->facenum - 1]->z);
			glNormal3f(normals[i]->x, normals[i]->y, normals[i]->z);
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
	//新建，而不是单纯的把指针放入
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
	//先对所有坐标单位化
	for (int i = 0; i < vertex.size(); ++i) {
		unitCoord(vertex[i]);
	}
	//删除所有法向量
	int nsize = normals.size();
	for (int i = 0; i <nsize; ++i) {
		normals.pop_back();
	}
	//cout << "n size： " << normals.size() << endl;
	//根据新顶点和面索引重新计算面的法向量
	for (int i = 0; i < faces.size(); ++i) {
		if (faces[i]->four)//如果这个面是四面
		{	
			struct coordinate temp = calculateNormalTRI(vertex[faces[i]->faces[0] - 1],
				vertex[faces[i]->faces[1] - 1],
				vertex[faces[i]->faces[2] - 1]
				);
			//cout << 'q' << temp.x << " " << temp.y << " " << temp.z << endl;
			normals.push_back(new coordinate(temp.x,temp.y,temp.z));
		}
		else{			//三角形面
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
	//先处理坐标
	for (int i = 0; i < vertex.size(); ++i) {
		//线性变化
		vertex[i]->x = calmid((start.vertex[i]->x) ,(end.vertex[i]->x),current,total);
		//cout << start.vertex[i]->x << " " << end.vertex[i]->x <<" "<<vertex[i]->x<< endl;
		vertex[i]->y = calmid((start.vertex[i]->y), (end.vertex[i]->y), current, total);
		vertex[i]->z = calmid((start.vertex[i]->z), (end.vertex[i]->z), current, total);
	}
	//删除所有法向量
	int nsize = normals.size();
	for (int i = 0; i <nsize; ++i) {
		normals.pop_back();
	}
	//根据新顶点和面索引重新计算面的法向量
	for (int i = 0; i < faces.size(); ++i) {
		if (faces[i]->four)//如果这个面是四面
		{
			struct coordinate temp = calculateNormalTRI(vertex[faces[i]->faces[0] - 1],
				vertex[faces[i]->faces[1] - 1],
				vertex[faces[i]->faces[2] - 1]
				);
			//cout << temp.x << " " << temp.y << " " << temp.z << endl;
			normals.push_back(new coordinate(temp.x, temp.y, temp.z));
		}
		else {			//三角形面
			struct coordinate temp = calculateNormalTRI(vertex[faces[i]->faces[0] - 1],
				vertex[faces[i]->faces[1] - 1],
				vertex[faces[i]->faces[2] - 1]
				);
			//cout << temp.x << " " << temp.y << " " << temp.z << endl;

			normals.push_back(new coordinate(temp.x, temp.y, temp.z));
		}
	}
}

//将坐标单位化的方法（传入坐标struct）
void unitCoord(coordinate* coord){
	GLfloat len = sqrt((coord->x)*(coord->x) + (coord->y)*(coord->y) + (coord->z)*(coord->z));
	coord->x = (coord->x) / len;
	coord->y = (coord->y) / len;
	coord->z = (coord->z) / len;
}

coordinate calculateNormalTRI(coordinate* coord1, coordinate* coord2, coordinate* coord3) {
	/* 计算向量 */
	GLfloat va[3], vb[3], vr[3];
	va[0] = coord1->x - coord2->x;
	va[1] = coord1->y - coord2->y;
	va[2] = coord1->z - coord2->z;

	vb[0] = coord1->x - coord3->x;
	vb[1] = coord1->y - coord3->y;
	vb[2] = coord1->z - coord3->z;

	/* 计算法矢量 */
	vr[0] = va[1] * vb[2] - vb[1] * va[2];
	vr[1] = vb[0] * va[2] - va[0] * vb[2];
	vr[2] = va[0] * vb[1] - vb[0] * va[1];

	/* 单位化 */
	GLfloat len = sqrt(vr[0] * vr[0] + vr[1] * vr[1] + vr[2] * vr[2]);

	//返回
	struct coordinate a = { vr[0] / len,vr[1] / len,vr[2] / len };
	return a;
}


//主体
GLfloat angle = 0.1f;		//单纯的为了展示，增加旋转效果而加的变量
glutWindow win;				//定义窗口

ObjModel obj("monkey.obj");
ObjModel transferModel;
ObjModel sphereModel;

bool mouseStateR = false;			//记录鼠标事件
bool mouseStateL = false;
static GLint mx = 0, my = 0;	//鼠标位置

coordinate itemPos = { 0.0f,0.0f,0.0f };		//物体坐标
coordinate viewPos = { 0.0f,1.0f,4.0f };		//观察点坐标

static GLfloat yrotate = Pi / 4; //观察点和y轴的夹角
static GLfloat xrotate = Pi / 4; //观察点和x轴的夹角
static GLfloat moveSpeed = 0.3f; //移动速度
static GLfloat rotateSpeed = 0.02f; //旋转速度
static GLfloat zoomSpeed = 0.05f; //缩放速度
static GLfloat viewDis = 5.0f;

int listID;
GLfloat speed = 1.0f;
long times = 100000000000;	//显示时间
long countFrame = 0;		//记录显示时间
int c=0;
int frames=100;
bool start = false;

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//用球坐标计算观察点的位置
	viewPos.x = itemPos.x + viewDis * sin(yrotate) * cos(xrotate);
	viewPos.y = itemPos.y + viewDis * cos(yrotate);
	viewPos.z = itemPos.z + viewDis * sin(yrotate) * sin(xrotate);
	gluLookAt(viewPos.x, viewPos.y, viewPos.z, itemPos.x,itemPos.y, itemPos.z, 0, 1, 0);

	glPushMatrix();
	//停下来的时候自动旋转
	glRotatef(angle, 0, 1, 0);		
	glRotatef(10, 0, 1, 0);
	angle += 0.01f;

	//计算时间，如果过长则自动关闭
	countFrame++;
	if (countFrame > times) exit(0);

	if (c <= frames && start) {		//如果已经开始变化，并且变化没有结束
		transferModel.trans(sphereModel, obj, c, frames);	//调用变形
		listID = transferModel.Draw();			//绘制obj（将东西画完然后记录id）
		c++; 
		glCallList(listID);			//通过ListID去实现绘制
	}
	else if(!start) {				//没有开始变化
		glutSolidSphere(1.0f, 40, 40); //画一个球体
	}
	else {							//变化结束
		glCallList(listID);			//通过ListID去实现绘制
	}
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

	/*光线设置*/
	GLfloat light_position[] = { 1.0f, -1.0f, 1.0f, 0.0f };	//光源位置
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	GLfloat light_ambient[] = { 0.1f,0.1f, 0.1f, 1.0f }; //环境光线
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	GLfloat light_diffuse[] = { 0.6f, 0.6f, 0.6f, 1.0f }; //漫反射
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	GLfloat light_specular[] = { 0.7f, 0.7f, 0.4f, 1.0f }; //镜面反射
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

	obj.Load();				//读取
	sphereModel.setName("sphere");
	transferModel.setName("transfer");
	sphereModel.copyFrom(obj);
	transferModel.copyFrom(obj);
	sphereModel.unitVertex();
	transferModel.unitVertex();
	
}

//键盘控制，按q退出程序
//wsad控制物体移动
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
	{       // 除以速度
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
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);//改变显示区域，起始位置为客户端窗口左下角（非坐标原点）
	glMatrixMode(GL_PROJECTION);		//修改投影矩阵
	glLoadIdentity();					//导入单位阵
	gluPerspective(win.view_angle, (float)w / h, win.z_near, win.z_far);//宽高比改为当前值，视线区域与屏幕大小一致；
	glMatrixMode(GL_MODELVIEW);			//修改模型视图
	glLoadIdentity();
	gluLookAt(viewPos.x, viewPos.y, viewPos.z, itemPos.x, itemPos.y, itemPos.z, 0, 1, 0);
}

int main(int argc, char **argv)
{
	//设置window的参数用于显示
	win.width = 400;
	win.height = 400;
	win.title = "hw1.4";
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
	glutMouseFunc(mouseDown);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(mouseMove);
	glutReshapeFunc(reshape);			//用于保持拉动窗口的时候模型不变形
	glutMainLoop();
	return 0;
}
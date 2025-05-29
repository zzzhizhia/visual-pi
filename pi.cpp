////////////////////////////////////////////
// 程序：可视化 π
// 作者：我想做三国志
// 编译环境：Visual Studio 2019，EasyX_20220901
// 编写日期：2023-2-22

#include <graphics.h>

const int WIDTH = 640;				// 屏幕宽度
const int HEIGHT = 480;				// 屏幕高度
const int UP = 0;					// 上方向
const int DOWN = 2;					// 下方向
const int LEFT = 1;					// 左方向
const int RIGHT = 3;				// 右方向
const int NONE = 4;					// 空方向
const int COLNUM = 12;				// 颜色数量，表示经过次数最大值
const int NODEWIDTH = 11;			// 节点宽度
const int NODEHEIGHT = 11;			// 节点高度
const int PATHWIDTH = 7;			// 路径宽度
const int PATHLEN = 4;				// 路径长度
const int adddigit = 10;			// 辅助精度
const int LENGTH = 10000;			// 精度，小数点后位数

char temp[LENGTH + adddigit + 1];	// 下一个要加的数
char last[LENGTH + adddigit + 1];	// 上一个数，累加至今的数
COLORREF ColArr[COLNUM + 2];		// 颜色数组，图例和节点用

// 初始化颜色数组，用于图例
void InitColArr(COLORREF* ColArr, int len)
{
	ColArr[0] = BLACK;
	ColArr[len - 1] = WHITE;
	for (int i = 1; i < len - 1; i++)
	{
		ColArr[i] = HSLtoRGB(240 - (i - 1) * 20, 1, 0.5);
	}
}

// 画节点
void DrawNode(DWORD* buf, int LeftEst, int RightEst, int UpEst, int DownEst, int x, int y, char element)
{
	setfillcolor(ColArr[element]);
	solidrectangle(x * (NODEWIDTH + PATHLEN), y * (NODEHEIGHT + PATHLEN), x * (NODEWIDTH + PATHLEN) + NODEWIDTH - 1, y * (NODEHEIGHT + PATHLEN) + NODEHEIGHT - 1);
	if (element == COLNUM + 1)
	{
		int radius = min(NODEHEIGHT, NODEWIDTH) / 2;
		setfillcolor(RED);
		solidcircle(x * (NODEWIDTH + PATHLEN) + radius, y * (NODEHEIGHT + PATHLEN) + radius, radius);
	}
}

// 画路径
void DrawPath(DWORD* buf, int LeftEst, int RightEst, int UpEst, int DownEst, int x, int y, char Dir)
{
	int beginPX = x * (PATHLEN + NODEWIDTH), beginPY = y * (NODEHEIGHT + PATHLEN);
	int height = PATHWIDTH, width = PATHWIDTH;
	switch (Dir)
	{
	case UP:	beginPX += (NODEWIDTH - PATHWIDTH) / 2; beginPY -= PATHLEN;	height = PATHLEN;	break;
	case DOWN:	beginPX += (NODEWIDTH - PATHWIDTH) / 2; beginPY += NODEHEIGHT;	height = PATHLEN;	break;
	case LEFT:	beginPY += (NODEHEIGHT - PATHWIDTH) / 2; beginPX -= PATHLEN;	width = PATHLEN;	break;
	case RIGHT:	beginPY += (NODEHEIGHT - PATHWIDTH) / 2; beginPX += NODEWIDTH;	width = PATHLEN;	break;
	}
	setfillcolor(WHITE);
	solidrectangle(beginPX, beginPY, beginPX + width - 1, beginPY + height - 1);
}

// 画地图
void DrawMap(DWORD* buf, int LeftEst, int RightEst, int UpEst, int DownEst, char* AuxiliaryMap, char* PathHoriMap, char* PathVertMap)
{
	// 画节点
	for (int i = 0; i < DownEst - UpEst + 1; i++)
		for (int j = 0; j < RightEst - LeftEst + 1; j++)
			DrawNode(buf, LeftEst, RightEst, UpEst, DownEst, j, i, AuxiliaryMap[i * (RightEst - LeftEst + 1) + j]);

	// 画通向右边的道路
	for (int i = 0; i < DownEst - UpEst + 1; i++)
		for (int j = 0; j < RightEst - LeftEst; j++)
			if (PathHoriMap[i * (RightEst - LeftEst) + j] == 1)
				DrawPath(buf, LeftEst, RightEst, UpEst, DownEst, j, i, RIGHT);

	// 画通向下边的道路
	for (int i = 0; i < DownEst - UpEst; i++)
		for (int j = 0; j < RightEst - LeftEst + 1; j++)
			if (PathVertMap[i * (RightEst - LeftEst + 1) + j] == 1)
				DrawPath(buf, LeftEst, RightEst, UpEst, DownEst, j, i, DOWN);
}

// 画一条说明
void DrawReference(TCHAR* arr, int& addHeight, int linesize, int margin)
{
	int height = textheight(arr);
	int width = textwidth(arr);
	outtextxy(margin, addHeight + margin, arr);
	rectangle(margin - linesize / 2, addHeight + margin - linesize / 2, margin + width + linesize / 2, addHeight + margin + height + linesize / 2);
	addHeight += height + margin;
}

// 画图例
void DrawGraphicSymbol(int confirmDig, int presentDis, int farestDis, double aveNum, int* timesArr, COLORREF* colArr, int len)
{
	int addHeight = 0;
	int addWidth = 0;
	int margin = 5;
	int height = 0;
	int width = 0;
	int maxWidth = 0;
	int linesize = 4;

	settextcolor(WHITE);
	settextstyle(12, 0, _T("宋体"));
	setlinecolor(BLACK);
	setlinestyle(PS_SOLID, linesize);
	TCHAR arr[128];
	_stprintf_s(arr, _T("小数位数:%d"), confirmDig);
	DrawReference(arr, addHeight, linesize, margin);

	_stprintf_s(arr, _T("当前距离:%d"), presentDis);
	DrawReference(arr, addHeight, linesize, margin);

	_stprintf_s(arr, _T("最远距离:%d"), farestDis);
	DrawReference(arr, addHeight, linesize, margin);

	_stprintf_s(arr, _T("平均数字:%.2lf"), aveNum);
	DrawReference(arr, addHeight, linesize, margin);

	int AllTimesNum = 0;
	for (int i = 0; i < len; i++)AllTimesNum += timesArr[i];
	addHeight = 0;
	for (int i = 0; i < len; i++)
	{
		if (timesArr[i] == 0) break;
		if (i == len - 1) _stprintf_s(arr, _T("经过>=%d次 %.1f%c"), i + 1, timesArr[i] / (double)AllTimesNum * 100, L'%');
		else _stprintf_s(arr, 128, _T("经过%d次 %.1f%c"), i + 1, timesArr[i] / (double)AllTimesNum * 100, L'%');
		settextcolor(colArr[i]);
		height = textheight(arr);
		width = textwidth(arr);
		maxWidth = max(width, maxWidth);
		if (addHeight + height > HEIGHT)
		{
			addWidth = maxWidth + margin;
			addHeight = 0;
		}
		outtextxy(WIDTH - margin - addWidth - width, addHeight + margin, arr);
		rectangle(WIDTH - margin - addWidth - width - linesize / 2, addHeight + margin - linesize / 2,
			WIDTH - margin - addWidth + linesize / 2, addHeight + margin + height + linesize / 2);
		addHeight += height + margin;
	}
}

// 主函数
int main()
{
	InitColArr(ColArr, COLNUM + 2);
	initgraph(WIDTH, HEIGHT);
	setbkcolor(BLACK);
	BeginBatchDraw();

	HDC mainDC = GetImageHDC(NULL);			// 主窗口的 DC
	SetStretchBltMode(mainDC, HALFTONE);	// 设置拉伸贴图模式

	int beginX = 0, beginY = 0;
	int LeftEst = -5, RightEst = 5, UpEst = -5, DownEst = 5;
	int showWidth = WIDTH - 50, showHeight = HEIGHT - 50;
	char* AuxiliaryMap = new char[(RightEst - LeftEst + 1) * (DownEst - UpEst + 1)];
	char* PathHoriMap = new char[(RightEst - LeftEst) * (DownEst - UpEst + 1)];
	char* PathVertMap = new char[(DownEst - UpEst) * (RightEst - LeftEst + 1)];
	memset(AuxiliaryMap, 0, sizeof(char) * ((RightEst - LeftEst + 1) * (DownEst - UpEst + 1)));
	memset(PathHoriMap, 0, sizeof(char) * (RightEst - LeftEst) * (DownEst - UpEst + 1));
	memset(PathVertMap, 0, sizeof(char) * (DownEst - UpEst) * (RightEst - LeftEst + 1));
	AuxiliaryMap[(beginY + abs(UpEst)) * (RightEst - LeftEst + 1) + beginX + abs(LeftEst)] = COLNUM + 1;

	// 建立 img 对象
	IMAGE img((PATHLEN + NODEWIDTH) * (RightEst - LeftEst + 1) - PATHLEN, (PATHLEN + NODEHEIGHT) * (DownEst - UpEst + 1) - PATHLEN);
	HDC imgDC = GetImageHDC(&img);
	DWORD* pBuf = GetImageBuffer(&img);
	memset(pBuf, 0, sizeof(DWORD) * img.getwidth() * img.getheight());
	SetWorkingImage(&img);
	DrawMap(pBuf, LeftEst, RightEst, UpEst, DownEst, AuxiliaryMap, PathHoriMap, PathVertMap);

	// 贴图到主窗口
	double times = min(showHeight / (double)img.getheight(), showWidth / (double)img.getwidth());
	int BeginShowPX = (WIDTH - showWidth) / 2 + (showWidth - img.getwidth() * times) / 2;
	int BeginShowPY = (HEIGHT - showHeight) / 2 + (showHeight - img.getheight() * times) / 2;
	SetWorkingImage(NULL);
	cleardevice();
	StretchBlt(mainDC, BeginShowPX, BeginShowPY, (int)(img.getwidth() * times + 0.5), (int)(img.getheight() * times + 0.5),
			   imgDC,  0, 0, img.getwidth(), img.getheight(), SRCCOPY);
	FlushBatchDraw();

	memset(temp, 0, sizeof(temp));
	memset(last, 0, sizeof(last));
	int a = 1, b = 3;
	temp[0] = 2;
	last[0] = 2;
	int Rnt = 1;
	int ConfirmDig = 0;
	char Dir = DOWN;
	int farestDis = 0;
	int allNum = 0;
	int timesArr[COLNUM + 1] = { 0 };	// 多的一个为起点准备
	while (Rnt)
	{
		// temp * a
		int C = 0;
		for (int i = LENGTH + adddigit; i >= 0; i--)
		{
			C = temp[i] * a + C;
			temp[i] = C % 10;
			C = C / 10;
		}
		// temp / b
		C = 0;
		for (int i = 0; i <= LENGTH + adddigit; i++)
		{
			C *= 10;
			int middle = (temp[i] + C) / b;
			C = (temp[i] + C) % b;
			temp[i] = middle;
		}
		// last + temp
		C = 0;
		Rnt = 0;
		int lastCountDig = LENGTH + adddigit;
		for (int i = LENGTH + adddigit; i >= 0; i--)
		{
			C = last[i] + temp[i] + C;
			last[i] = C % 10;
			C = C / 10;
			if (temp[i] != 0)lastCountDig = i;
			Rnt |= temp[i];
		}
		a += 1;
		b += 2;
		if (lastCountDig >= ConfirmDig + adddigit)
		{
			allNum += last[ConfirmDig];				// 用于计算平均数
			if (last[ConfirmDig] % 2)// 奇数左转
			{
				Dir = (Dir + 1) % 4;
			}
			else// 偶数右转
			{
				Dir = (Dir + 3) % 4;
			}
			int newX = beginX, newY = beginY;
			switch (Dir)
			{
			case UP:	newY--; break;
			case DOWN:	newY++; break;
			case LEFT:	newX--; break;
			case RIGHT:	newX++; break;
			}
			if (abs(newY) + abs(newX) > farestDis)
				farestDis = abs(newY) + abs(newX);
			int oriX = abs(LeftEst), oriY = abs(UpEst), oriH = DownEst - UpEst + 1, oriW = RightEst - LeftEst + 1;
			bool isRefresh = false;
			if (newX < LeftEst)
			{
				LeftEst--;
				isRefresh = true;
			}
			else if (newX > RightEst)
			{
				RightEst++;
				isRefresh = true;
			}
			if (newY < UpEst)
			{
				UpEst--;
				isRefresh = true;
			}
			else if (newY > DownEst)
			{
				DownEst++;
				isRefresh = true;
			}
			SetWorkingImage(&img);
			if (isRefresh)
			{
				char* tempAuxiliaryMap = new char[(RightEst - LeftEst + 1) * (DownEst - UpEst + 1)];
				char* tempPathHoriMap = new char[(RightEst - LeftEst) * (DownEst - UpEst + 1)];
				char* tempPathVertMap = new char[(DownEst - UpEst) * (RightEst - LeftEst + 1)];
				memset(tempAuxiliaryMap, 0, sizeof(char) * ((RightEst - LeftEst + 1) * (DownEst - UpEst + 1)));
				memset(tempPathHoriMap, 0, sizeof(char) * (RightEst - LeftEst) * (DownEst - UpEst + 1));
				memset(tempPathVertMap, 0, sizeof(char) * (DownEst - UpEst) * (RightEst - LeftEst + 1));
				int beginXX = abs(LeftEst) - oriX, beginYY = abs(UpEst) - oriY;

				for (int i = 0; i < oriH; i++)
					for (int j = 0; j < oriW; j++)
						tempAuxiliaryMap[(beginYY + i) * (RightEst - LeftEst + 1) + beginXX + j] = AuxiliaryMap[i * oriW + j];

				for (int i = 0; i < oriH; i++)
					for (int j = 0; j < oriW - 1; j++)
						tempPathHoriMap[(beginYY + i) * (RightEst - LeftEst) + beginXX + j] = PathHoriMap[i * (oriW - 1) + j];

				for (int i = 0; i < oriH - 1; i++)
					for (int j = 0; j < oriW; j++)
						tempPathVertMap[(beginYY + i) * (RightEst - LeftEst + 1) + beginXX + j] = PathVertMap[i * oriW + j];

				delete[] AuxiliaryMap;
				delete[] PathHoriMap;
				delete[] PathVertMap;
				AuxiliaryMap = tempAuxiliaryMap;
				PathHoriMap = tempPathHoriMap;
				PathVertMap = tempPathVertMap;

				img.Resize((PATHLEN + NODEWIDTH) * (RightEst - LeftEst + 1) - PATHLEN, (PATHLEN + NODEHEIGHT) * (DownEst - UpEst + 1) - PATHLEN);
				imgDC = GetImageHDC(&img);
				pBuf = GetImageBuffer(&img);
				memset(pBuf, 0, sizeof(DWORD) * img.getwidth() * img.getheight());

				DrawMap(pBuf, LeftEst, RightEst, UpEst, DownEst, AuxiliaryMap, PathHoriMap, PathVertMap);

				times = min(showHeight / (double)img.getheight(), showWidth / (double)img.getwidth());
				BeginShowPX = (WIDTH - showWidth) / 2 + (showWidth - img.getwidth() * times) / 2;
				BeginShowPY = (HEIGHT - showHeight) / 2 + (showHeight - img.getheight() * times) / 2;
			}
			if (AuxiliaryMap[(newY + abs(UpEst)) * (RightEst - LeftEst + 1) + newX + abs(LeftEst)] < COLNUM)
			{
				AuxiliaryMap[(newY + abs(UpEst)) * (RightEst - LeftEst + 1) + newX + abs(LeftEst)]++;
				timesArr[AuxiliaryMap[(newY + abs(UpEst)) * (RightEst - LeftEst + 1) + newX + abs(LeftEst)] - 1]++;		// 记录节点经过该次数的数量
			}

			DrawNode(pBuf, LeftEst, RightEst, UpEst, DownEst, newX + abs(LeftEst), newY + abs(UpEst),
				AuxiliaryMap[(newY + abs(UpEst)) * (RightEst - LeftEst + 1) + newX + abs(LeftEst)]);
			DrawPath(pBuf, LeftEst, RightEst, UpEst, DownEst, beginX + abs(LeftEst), beginY + abs(UpEst), Dir);

			switch (Dir)
			{
			case UP:	PathVertMap[(newY + abs(UpEst)) * (RightEst - LeftEst + 1) + newX + abs(LeftEst)] = 1;	break;
			case DOWN:	PathVertMap[(beginY + abs(UpEst)) * (RightEst - LeftEst + 1) + beginX + abs(LeftEst)] = 1;	break;
			case LEFT:	PathHoriMap[(newY + abs(UpEst)) * (RightEst - LeftEst) + newX + abs(LeftEst)] = 1;	break;
			case RIGHT:	PathHoriMap[(beginY + abs(UpEst)) * (RightEst - LeftEst) + beginX + abs(LeftEst)] = 1;	break;
			}
			beginX = newX;
			beginY = newY;
			ConfirmDig++;
			SetWorkingImage(NULL);
			cleardevice();
			StretchBlt(mainDC, BeginShowPX, BeginShowPY, (int)(img.getwidth()* times + 0.5), (int)(img.getheight()* times + 0.5),
					   imgDC,  0, 0, img.getwidth(), img.getheight(), SRCCOPY);
			DrawGraphicSymbol(ConfirmDig - 1, abs(beginX) + abs(beginY), farestDis, allNum / (double)ConfirmDig, timesArr, (COLORREF*)ColArr + 1, COLNUM);
			FlushBatchDraw();
			Sleep(1);
		}
	}
	delete[] AuxiliaryMap;
	delete[] PathHoriMap;
	delete[] PathVertMap;

	double offsetX = 0, offsetY = 0;
	bool isExit = false;
	bool isPress = false;
	int oriX = 0, oriY = 0;
	double oriTimes = times;

	ExMessage msg;
	SetWorkingImage(NULL);
	flushmessage(EX_MOUSE | EX_KEY);
	while (!isExit)
	{
		msg = getmessage(EX_MOUSE | EX_KEY);

		if (msg.message == WM_KEYDOWN && msg.vkcode == VK_RETURN)
			isExit = true;
		else if (msg.message == WM_MOUSEWHEEL)
		{
			double Addtimes = 1;
			if (msg.wheel > 0)Addtimes = 1.1;
			else if (msg.wheel < 0)Addtimes = 0.9;
			times = max(0.25, min(min(HEIGHT / (NODEHEIGHT * 5), WIDTH / (NODEWIDTH * 5)), times * Addtimes));
			BeginShowPX = (WIDTH - img.getwidth() * times) / 2;
			BeginShowPY = (HEIGHT - img.getheight() * times) / 2;
			msg.wheel = 0;
		}
		else
		{
			if (!isPress && msg.lbutton)
			{
				isPress = true;
				oriX = msg.x, oriY = msg.y;
			}
			else if (isPress && msg.lbutton)
			{
				offsetX += (msg.x - oriX), offsetY += (msg.y - oriY);
				oriX = msg.x, oriY = msg.y;
			}
			else if (isPress && !msg.lbutton)
			{
				isPress = false;
			}
		}

		cleardevice();
		StretchBlt(mainDC, BeginShowPX + offsetX, BeginShowPY + offsetY, (int)(img.getwidth()* times + 0.5), (int)(img.getheight()* times + 0.5),
				   imgDC,  0, 0, img.getwidth(), img.getheight(), SRCCOPY);
		DrawGraphicSymbol(ConfirmDig - 1, abs(beginX) + abs(beginY), farestDis, allNum / (double)ConfirmDig, timesArr, (COLORREF*)ColArr + 1, COLNUM);
		FlushBatchDraw();
	}
	closegraph();
	return 0;
}

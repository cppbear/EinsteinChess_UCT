import pygame
import os
import sys
import random
import copy
import socket
import logging
from pygame.locals import *
from sys import exit
from time import ctime, sleep
import logging

WINDOWSIZE = (1200, 680)  # 游戏窗口大小
LINECOLOR = (0, 0, 0)  # 棋盘线的颜色
TEXTCOLOR = (0, 0, 0)  # 标题文本颜色
BLACKGROUND = (255, 255, 255)  # 游戏背景颜色
BLACK = (0, 0, 0)
BLUE = (0, 191, 255)
YELLOW = (255, 255, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
WHITE = (255, 255, 255)
SIZE = (50, 50)  # 棋子大小
AYSTSIZE = (240, 240)  # 爱因斯坦背景头像图片大小
POSAYST = (365, 0)  # 爱因斯坦头像位置
STEP = 60  # 步长：棋子走一步移动的距离
TEXTSIZE = 30  # 标题文字大小
TIPSIZE = 15  # 提示文字大小
COUNT = -1  # 记录当前回合
START = 65  # 棋盘左上角起始点（70，70）
REDWIN = 1  # 代表RED方赢
BLUEWIN = 2  # 代表玩家BLUE方赢
LEFT = 'left'
RIGHT = 'right'
UP = 'up'
DOWN = 'down'
LEFTUP = 'leftup'
RIGHTDOWN = 'rightdown'
RESULT = [0, 0]  # 记录比赛结果
WINSIZE = (400, 90)  # 显示比赛结果窗口大小
INFTY = 10000
SLEEPTIME = 0
clock = pygame.time.Clock()
clients = list()
logging.basicConfig(level=logging.INFO, format='%(asctime)s %(name)s- %(levelname)s - %(message)s', filename='battle.log')
logger = logging.getLogger()

ch = logging.StreamHandler() #日志输出到屏幕控制台
ch.setLevel(logging.INFO) #设置日志等级
formatter = logging.Formatter('%(asctime)s %(name)s- %(levelname)s - %(message)s') #定义日志输出格式
ch.setFormatter(formatter) #选择一个格式
logger.addHandler(ch) #增加指定的handler
playernum = 2
currentnum = 0
start = 0


class Status(object):
    def __init__(self):
        self.map = None
        self.value = None
        self.pawn = None
        self.pro = None
        self.parent = None
        self.pPawn = None
        self.pMove = None
        self.pDice = None
        self.cPawn = None
        self.cPawnSecond = None
        self.cPM = [[],[],[],[],[],[]]
        self.cPMSecond = [[],[],[],[],[],[]]
    def print(self):
        print(self.cPM)

class Player:

    ID = 'ID'
    wins = 0
    def __init__(self, id, sock):
        self.ID = id
        self.clientsock = sock

def init():
    global IMAGE,tip,screen,font,maplib,Lyr,Lyb,Lx,S,matchPro
    pygame.init()
    S = Status()
    screen = pygame.display.set_mode(WINDOWSIZE, 0, 32)  # 设置游戏窗口
    pygame.display.set_caption('Einstein Chess-Player 0 VS Player 1')  # 设置Caption
    font = pygame.font.SysFont("Cambria Math", TEXTSIZE,
                               bold=False, italic=False)  # 设置标题字体格式
    tip = pygame.font.SysFont("arial", TIPSIZE, bold=False, italic=False)  # 设置提示字体
    IMAGE = {
        'R1': pygame.transform.scale(pygame.image.load('picture/white/R1.png').convert(), SIZE),
        'R2': pygame.transform.scale(pygame.image.load('picture/white/R2.png').convert(), SIZE),
        'R3': pygame.transform.scale(pygame.image.load('picture/white/R3.png').convert(), SIZE),
        'R4': pygame.transform.scale(pygame.image.load('picture/white/R4.png').convert(), SIZE),
        'R5': pygame.transform.scale(pygame.image.load('picture/white/R5.png').convert(), SIZE),
        'R6': pygame.transform.scale(pygame.image.load('picture/white/R6.png').convert(), SIZE),
        'B1': pygame.transform.scale(pygame.image.load('picture/white/B1.png').convert(), SIZE),
        'B2': pygame.transform.scale(pygame.image.load('picture/white/B2.png').convert(), SIZE),
        'B3': pygame.transform.scale(pygame.image.load('picture/white/B3.png').convert(), SIZE),
        'B4': pygame.transform.scale(pygame.image.load('picture/white/B4.png').convert(), SIZE),
        'B5': pygame.transform.scale(pygame.image.load('picture/white/B5.png').convert(), SIZE),
        'B6': pygame.transform.scale(pygame.image.load('picture/white/B6.png').convert(), SIZE),
        'Y1': pygame.transform.scale(pygame.image.load('picture/white/Y1.png').convert(), SIZE),
        'Y2': pygame.transform.scale(pygame.image.load('picture/white/Y2.png').convert(), SIZE),
        'Y3': pygame.transform.scale(pygame.image.load('picture/white/Y3.png').convert(), SIZE),
        'Y4': pygame.transform.scale(pygame.image.load('picture/white/Y4.png').convert(), SIZE),
        'Y5': pygame.transform.scale(pygame.image.load('picture/white/Y5.png').convert(), SIZE),
        'Y6': pygame.transform.scale(pygame.image.load('picture/white/Y6.png').convert(), SIZE),
        '1': pygame.transform.scale(pygame.image.load('picture/white/1.png').convert(), SIZE),
        '2': pygame.transform.scale(pygame.image.load('picture/white/2.png').convert(), SIZE),
        '3': pygame.transform.scale(pygame.image.load('picture/white/3.png').convert(), SIZE),
        '4': pygame.transform.scale(pygame.image.load('picture/white/4.png').convert(), SIZE),
        '5': pygame.transform.scale(pygame.image.load('picture/white/5.png').convert(), SIZE),
        '6': pygame.transform.scale(pygame.image.load('picture/white/6.png').convert(), SIZE),
        'BLUEWIN': pygame.transform.scale(pygame.image.load('picture/white/BLUEWIN.png').convert(), WINSIZE),
        'REDWIN': pygame.transform.scale(pygame.image.load('picture/white/REDWIN.png').convert(), WINSIZE),
    }
    # 布局库
    maplib = [[6, 2, 4, 1, 5, 3],
              [6, 5, 2, 1, 4, 3],
              [1, 5, 4, 6, 2, 3],
              [1, 6, 3, 5, 2, 4],
              [1, 6, 4, 3, 2, 5],
              [6, 1, 2, 5, 4, 3],
              [6, 1, 3, 5, 4, 2],
              [1, 6, 4, 2, 3, 5],
              [1, 5, 2, 6, 3, 4],
              [1, 6, 5, 2, 3, 4],
              [1, 2, 5, 6, 3, 4],
              [6, 2, 5, 1, 4, 3],
              [1, 6, 3, 2, 4, 5],
              [6, 2, 3, 1, 5, 4],
              [1, 6, 3, 4, 2, 5],
              [1, 5, 4, 6, 3, 2]
              ]
    resetInfo()
    Lyr = []
    Lyb = []
    Lx = []
    matchPro = 0.85


def loadImage(name, pos, size=SIZE):
    filename = "picture/white/" + name
    screen.blit(pygame.transform.scale(
        pygame.image.load(filename).convert(), size), pos)
def waitForPlayerToPressKey():  # 等待按键
    while True:
        for event in pygame.event.get():
            if event.type == QUIT:
                terminate()
            if event.type == KEYDOWN:
                if event.key == K_ESCAPE:
                    terminate()
                return


def drawStartScreen():  # 开始界面
    screen.fill(WHITE)
    loadImage("AYST.png", (100, 40), AYSTSIZE)
    # drawText('EinStein wurfelt nicht', font, GREEN, screen, 260, 200)
    drawText('PORT 50006 IS WAITING FOR CONNECTION', font, TEXTCOLOR, screen, 5.5, 0.1)
    pygame.display.update()


    # waitForPlayerToPressKey()
def drawWinScreen(result):  # 比赛结束，显示结果界面
    if result == BLUEWIN:
        loadImage("BLUEWIN.png", (50, 500), WINSIZE)
    if result == REDWIN:
        loadImage("REDWIN.png", (50, 5000), WINSIZE)
    # waitForPlayerToPressKey()
    pygame.display.update()
    sleep(SLEEPTIME)

def showWinRate(RedWinRate, BlueWinRate, x):
    global Lyr, Lyb, Lx
    yr = (100 - RedWinRate)/(100/3.0) + 0.6
    yb = (100 - BlueWinRate)/(100/3.0) + 0.6
    x = x/(1000/5) + 4.2
    Lyr.append(copy.deepcopy(yr))
    Lyb.append(copy.deepcopy(yb))
    Lx.append(copy.deepcopy(x))
    for i in range(0, len(Lyr)-1):
        pygame.draw.line(
            screen, RED, (100*Lx[i], 100*Lyr[i]), (100*Lx[i], 100*Lyr[i+1]))
        pygame.draw.line(
            screen, BLUE, (100*Lx[i], 100*Lyb[i]), (100*Lx[i], 100*Lyb[i+1]))
def drawGameScreen(conn0, conn1):  # 游戏比赛界面
    global S

    screen.fill(WHITE)
    # 画棋盘
    for i in range(6):
        x = y = 60*(i+1)
        pygame.draw.line(screen, LINECOLOR, (60, y), (360, y))
        pygame.draw.line(screen, LINECOLOR, (x, 60), (x, 360))


    drawText('RED : '+str(conn0.ID), font, RED, screen, 6, 1)
    drawText('BLUE : '+str(conn1.ID), font, BLUE, screen, 6.5, 1)

    # 加载提示文字
    drawText('Winning Percentage Dynamic Figure', font, BLACK, screen, 0, 7.2)
    drawText(str(conn0.ID) + ' Vs ' + str(conn1.ID), font, BLACK, screen, 0.5, 7.2)
    #drawText('matchPro : '+ str(round(100*matchPro,4))+'%',font,BLACK,screen,1,7.2)


    # 胜率坐标轴
    pygame.draw.line(screen, LINECOLOR, (415, 55), (420, 50))
    pygame.draw.line(screen, LINECOLOR, (425, 55), (420, 50))
    pygame.draw.line(screen, LINECOLOR, (420, 360), (420, 50))
    pygame.draw.line(screen, LINECOLOR, (420, 360), (1000, 360))
    pygame.draw.line(screen, LINECOLOR, (995, 355), (1000, 360))
    pygame.draw.line(screen, LINECOLOR, (995, 365), (1000, 360))


    drawText('RED : '+str(RESULT[0]), font, RED, screen, 6, 7)
    drawText('BLUE : '+str(RESULT[1]), font, BLUE, screen, 6.5, 7)
    
    if(sum(RESULT)):
        RedWinRate = round(100*float(RESULT[0])/sum(RESULT), 2)
        BlueWinRate = round(100*float(RESULT[1])/sum(RESULT), 2)
        drawText('RedWinRate:'+str(round(100 *
                                         float(RESULT[0])/sum(RESULT), 2)), font, RED, screen, 6, 9.5)
        drawText('BlueWinRate:'+str(round(100 *
                                          float(RESULT[1])/sum(RESULT), 2)), font, BLUE, screen, 6.5, 9.5)
        x = sum(RESULT)
        showWinRate(RedWinRate, BlueWinRate, x)

    # 画棋子
    for i in range(5):
        for j in range(5):
            if S.map[i][j] != 0:
                drawPawn(S.map[i][j], i, j)
    pygame.display.update()
    pygame.display.update()


def drawMovePawn(n, ans):  # 可选择移动的棋子
    x = -1
    y = 2
    for v in ans:
        drawPawn(v, x, y)
        y += 1
    if n <= 6:
        loadImage(str(n)+'.png', (310, 5))
    else:
        loadImage(str(n-6)+'.png', (310, 5))
    pygame.display.update()


def drawPawn(value, row, col, size=SIZE):  # 在（row，col）处，画值为value的棋子
    pos_x = col * STEP + START
    pos_y = row * STEP + START
    Pos = (pos_x, pos_y)
    if value <= 6:
        s = 'R' + str(value)
    elif value > 6:
        s = 'B' + str(value-6)
    loadImage(s+'.png', Pos, size)

def drawText(text, font, color, surface, row, col):  # 处理需要描绘的文字：text：文本；font：格式；
    row += 0.2
    x = col * STEP
    y = row * STEP
    textobj = font.render(text, True, color, WHITE)
    textrect = textobj.get_rect()
    textrect.topleft = (x, y)
    surface.blit(textobj, textrect)


def selectPawn(S,n=0):  # 掷骰子，挑选可以移动的棋子
    global COUNT
    if n == 0:
        COUNT += 1
        if COUNT % 2 == 0:
            n = random.randint(1, 6)
        else:
            n = random.randint(7, 12)
        ans = findNearby(n, S.pawn)
    else:
        ans = findNearby(n, S.pawn)
    return n, ans


def terminate():  # 退出游戏
    pygame.quit()
    sys.exit()


def makeMove(p, PawnMoveTo):  # 移动棋子，更新地图信息，和棋子存活情况
    row, col = getLocation(p, S.map)
    x = y = 0
    if PawnMoveTo == LEFT:
        y = -1
    elif PawnMoveTo == RIGHT:
        y = +1
    elif PawnMoveTo == UP:
        x = -1
    elif PawnMoveTo == DOWN:
        x = +1
    elif PawnMoveTo == LEFTUP:
        x = -1
        y = -1
    elif PawnMoveTo == RIGHTDOWN:
        x = +1
        y = +1
    else:
        return False
    # 移动无效
    if notInMap(row+x, col+y):
        return False
    S.map[row][col] = 0
    row = row + x
    col = col + y
    # 是否吃掉自己或对方的棋
    if S.map[row][col] != 0:
        i = S.pawn.index(S.map[row][col])
        S.pawn[i] = 0
    S.map[row][col] = p
    return True
def notInMap(x, y):  # 检测棋子是否在棋盘内移动
    if x in range(0, 5) and y in range(0, 5):
        return False
    return True


def showSelected(p):  # 用红色标记，显示被挑选的棋子
    row, col = getLocation(p, S.map)
    pos_x = col * STEP + START
    pos_y = row * STEP + START
    Pos = (pos_x, pos_y)
    if p > 6:
        s = 'Y' + str(p-6)
    else:
        s = 'Y' + str(p)
    loadImage(s+'.png', Pos)
    # screen.blit(IMAGE[s],Pos)
    pygame.display.update()


def isEnd(S):  # 检测比赛是否结束
    if S.map[0][0] > 6:
        return BLUEWIN
    elif S.map[4][4] > 0 and S.map[4][4] <= 6:
        return REDWIN
    cnt = 0
    for i in range(0, 6):
        if S.pawn[i] == 0:
            cnt += 1
    if cnt == 6:
        return BLUEWIN
    cnt = 0
    for i in range(6, 12):
        if S.pawn[i] == 0:
            cnt += 1
    if cnt == 6:
        return REDWIN
    return False


def resetInfo():  # 重置比赛信息
    S.map = getNewMap()
    S.pawn = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]  # 棋子初始化
    S.pro = [1.0/6, 1.0/6, 1.0/6, 1.0/6, 1.0/6, 1.0 /
           6, 1.0/6, 1.0/6, 1.0/6, 1.0/6, 1.0/6, 1.0/6]
    value = getLocValue(S)
    S.value = getPawnValue(S.pro, value)


def getNewMap():  # 换新图
    r = random.sample(maplib, 1)[0]
    b = random.sample(maplib, 1)[0]
    newMap = [
        [r[0], r[3],  r[5],     0,    0],
        [r[1], r[4],     0,     0,    0],
        [r[2],   0,     0,     0, b[2]+6],
        [0,   0,     0, b[4]+6, b[1]+6],
        [0,   0, b[5]+6, b[3]+6, b[0]+6]
    ]
    return newMap


def getLocValue(S):  # 棋子所在位置的价值
    blueValue = [[99, 10,  6,  3,  1],
                 [10,  8,  4,  2,  1],
                 [6,  4,  4,  2,  1],
                 [3,  2,  2,  2,  1],
                 [1,  1,  1,  1,  1]]
    redValue = [[1,  1,  1,  1,  1],
                [1,  2,  2,  2,  3],
                [1,  2,  4,  4,  6],
                [1,  2,  4,  8, 10],
                [1,  3,  6, 10, 99]]
    V = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    for p in range(1, 13):
        if S.pawn[p-1] != 0:
            row, col = getLocation(p, S.map)
            if p <= 6:
                V[p-1] = redValue[row][col]
            else:
                V[p-1] = blueValue[row][col]
    return V
def getPawnValue(pro, value):  # 棋子价值
    V = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    for i in range(0, 12):
        V[i] = pro[i] * value[i]
    return V

def findNearby(n, nowPawn):  # 寻找可以移动的棋子
    ans = []
    # 如果有对应棋子
    if nowPawn[n-1] != 0:
        ans.append(n)
    #没有对应棋子
    elif n > 6:
        for i in range(n-1, 6, -1):
            if i in nowPawn:
                ans.append(i)
                break
        for i in range(n+1, 13):
            if i in nowPawn:
                ans.append(i)
                break
    elif n <= 6:
        for i in range(n-1, 0, -1):
            if i in nowPawn:
                ans.append(i)
                break
        for i in range(n+1, 7):
            if i in nowPawn:
                ans.append(i)
                break
    return ans


def getLocation(p, Map):  # 返回传入地图下，棋子p的坐标
    for i in range(5):
        for j in range(5):
            if Map[i][j] == p:
                return i, j


def tryMakeMove(p, PawnMoveTo, S):  # 尝试移动，并且返回移动后的棋局地图与棋子存活情况
    newS = copy.deepcopy(S)
    row, col = getLocation(p, newS.map)
    x = y = 0
    if PawnMoveTo == LEFT:
        y = -1
    elif PawnMoveTo == RIGHT:
        y = +1
    elif PawnMoveTo == UP:
        x = -1
    elif PawnMoveTo == DOWN:
        x = +1
    elif PawnMoveTo == LEFTUP:
        x = -1
        y = -1
    elif PawnMoveTo == RIGHTDOWN:
        x = +1
        y = +1
    # 移动无效
    if notInMap(row+x, col+y):
        return False
    newS.map[row][col] = 0
    row = row + x
    col = col + y
    if newS.map[row][col] != 0:
        i = newS.pawn.index(newS.map[row][col])
        newS.pawn[i] = 0
    newS.map[row][col] = p
    # value = getLocValue(newS)  # 获取所有棋子的位置价值
    # newS.pro = getPawnPro(newS)  # 获取所有棋子被摇到的概率
    # newS.value = getPawnValue(value, newS.pro)
    newS.parent = S
    newS.pPawn = p
    newS.pMove = PawnMoveTo
    if p < 7:
        newS.cPawn = [INFTY,INFTY,INFTY,INFTY,INFTY,INFTY]
        newS.cPawnSecond = [INFTY,INFTY,INFTY,INFTY,INFTY,INFTY]
    else:
        newS.cPawn = [-INFTY,-INFTY,-INFTY,-INFTY,-INFTY,-INFTY]
        newS.cPawnSecond = [-INFTY,-INFTY,-INFTY,-INFTY,-INFTY,-INFTY]
    return newS



# 20190302 socket 移动棋子,如果走错直接输
def socketToMove(conn,n, ans, S):
    message = str(S.map) + '|' + str(n)
    conn.sendall(message.encode('UTF-8'))
    try:
        conn.settimeout(15)
        data, address = conn.recvfrom(1024)
    except socket.error as e:
        logger.info(str(e))
        return -1, 'timeout'
    text = (data.decode('UTF-8')[:-1]).split('|')
    try:
        p = int(text[0])
        moveTo = text[1]
    except TypeError as tp:
        return -1, 'not move'

    if (p in ans):
        if p > 0 and p < 7:
            if moveTo == DOWN or moveTo == RIGHT or moveTo == RIGHTDOWN:
                newS = tryMakeMove(p, moveTo, S)
                if newS is not False:
                    return p, moveTo
        elif p > 6 and p < 13:
            if moveTo == UP or moveTo == LEFT or moveTo == LEFTUP:
                newS = tryMakeMove(p, moveTo, S)
                if newS is not False:
                    return p, moveTo
    return -1, 'not move'


def outputResult():
    global clients
    rank = clients.copy()
    if currentnum == playernum:
        for i in range(playernum - 1):
            for j in range(playernum - i - 1):
                if (rank[j].wins < rank[j + 1].wins):
                    rank[j], rank[j + 1] = rank[j + 1], rank[j]
    with open('BATTLERESULT.txt',mode='w', encoding='utf8') as f:
        for c in rank:
            f.writelines(str(c.ID) + '  WINTIMES :  ' + str(c.wins)+str('\n'))
    logger.info('WRITE INTO BATTLERESULT')


def startgame(port, n, detail=True):
    global COUNT
    global playernum
    global clients
    global currentnum
    wins = [0 for i in range(playernum)]
    init()
    if detail:
        drawStartScreen()  # 游戏开始界面
    RESULT[0] = 0
    RESULT[1] = 0
    cnt = n
    rateline = []

    # while(playernum == 0):
    #     sleep(1)

    #客户端
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
        sock.bind(('0.0.0.0', port))

        for i in range(playernum):
            sock.listen()
            conn, addr = sock.accept()
            # 获取用户ID
            playerID = conn.recv(1024).decode('utf8')[:-1]
            clients.append(Player(playerID, conn))
            print('client_{} connected: {}'.format(i, addr))
            logger.info('client_'+str(i)+' connected: '+str(addr))
            currentnum = i+1
            # app.refresh()



    for i in range(0, playernum-1):
        for j in range(i+1, playernum):
            count = cnt
            while count:
                resetInfo()
                #result = playGame(Red, Blue, detail)  # 游戏开始，返回比赛结果
                result = battle(clients[i], clients[j], detail)
                if detail:
                    # pass
                    drawWinScreen(result)
                    if result == REDWIN:
                        clients[i].wins = clients[i].wins+1
                    else:
                        clients[j].wins = clients[j].wins+1
                    # app.refresh()

                RESULT[result - 1] += 1  # 更新比分
                count -= 1
                COUNT = 2000 - count - 1  # 先手方交替
                rateline.append(float(RESULT[0]) / sum(RESULT))
                if count % 5 == 0:
                    # pass
                    print(sum(RESULT), '\t', round(100 * RESULT[0] / sum(RESULT), 4))
    for i in range(playernum):
        try:
            # logger.info('rank : '+str(i)+'  '+ clients[i].ID + ' wins '+str( clients[i].wins))
            clients[i].clientsock.sendall('close'.encode('utf8'))
        except socket.error as e:
            logger.info(str('ERRORS  ') + str(e))
    rank = clients.copy()
    if currentnum == playernum:
        for i in range(playernum - 1):
            for j in range(playernum - i - 1):
                if (rank[j].wins < rank[j + 1].wins):
                    rank[j], rank[j + 1] = rank[j + 1], rank[j]
    for i in range(playernum):
        logger.info('rank : ' + str(i+1) + '  ' + rank[i].ID + ' wins ' + str(rank[i].wins))

    sock.close()


def battle(client0, client1, detail):
    lastInfo = []
    mapNeedRedraw = True  # 是否需要重新绘制地图
    if detail:
        drawGameScreen(client0, client1)
    while True:

        moveTo = None  # 棋子移动方向
        mapNeedRedraw = False
        n, ans = selectPawn(S)
        if detail:
            drawMovePawn(n, ans)
        for event in pygame.event.get():
            if event.type == QUIT:
                terminate()
        try:
            sleep(SLEEPTIME)
            if COUNT % 2 == 0:
                p, moveTo = socketToMove(conn=client0.clientsock, n=n, ans=ans, S=S)
                if p == -1:
                    logger.info(str('RESULT : '+client1.ID+' WIN'))
                    return BLUEWIN
        except socket.error as e:
            logger.info(str('ERRORS  ')+str(e) )
            logger.info(str('RESULT : ' + client1.ID + ' WIN'))
            return BLUEWIN
        except ValueError as e1:
            logger.info(str('ERRORS  ')+str(e1))
            logger.info(str('RESULT : ' + client1.ID + ' WIN'))
            return BLUEWIN


        try:
            if COUNT % 2 == 1:
                p, moveTo = socketToMove(conn=client1.clientsock, n=n, ans=ans, S=S)
                if p == -1:
                    str('RESULT : ' + client0.ID + ' WIN')
                    return REDWIN
        except socket.error as e:
            logger.info(str('ERRORS  ') + str(e))
            logger.info(str('RESULT : ' + client0.ID + ' WIN'))
            return REDWIN
        except ValueError as e1:
            logger.info(str('ERRORS  ') + str(e1))
            logger.info(str('RESULT : ' + client0.ID + ' WIN'))
            return REDWIN


        if moveTo != None:
            moved = makeMove(p, moveTo)
            lastInfo = [n, p, moveTo]
            if moved:
                mapNeedRedraw = True
        if mapNeedRedraw and detail:  # 如果需要重绘棋局界面，则：
            drawGameScreen(client0, client1)  # 重绘棋局界面
            logger.info(str(S.map) + ' |id: ' + str(p) + ' |move : ' + moveTo)
            pass
        result = isEnd(S)  # 检查游戏是否结束，返回游戏结果
        if result:
            lastInfo = []
            logger.info(str('RESULT : '+client0.ID+' WIN') if result == 1 else str('RESULT : '+client1.ID+' WIN'))
            return result



if __name__ == '__main__':
    # 测试局数
    cnt = 100
    result = startgame(port=50006, n=cnt)
    outputResult()
    input('enter any key to stop\n')
    sys.exit()

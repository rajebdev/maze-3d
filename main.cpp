#include <windows.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <iostream>
#include <time.h>
#include <sstream>
#include <math.h>
#include <stdio.h>

using namespace std;

int DEFAULT_SIZE = 9;

int** MAP;
int* ROAD;

float maxOrtho, marginMaze;
int i,j,k;
int rand_x, rand_y, cell;
int right_road, left_road, top_road, bottom_road, road_now;
int x_player, y_player, mazeSize, LENGTH, door_pos, max_index, max_road, road;

int viewMode=0;
float angle3D=0;
float angleX=0, angleY=0, angleZ=0;

int color_x = 0;
int wallModel, playerModel;
bool isTransparant = false;
bool isNight = false;

float pos[] = {2,10.5,1.5};
float viewDir[] = {0,-1,0};
float alpha = 5;
float xMove = 0, zMove = 0;

float COLOR[7][3] = {
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1},
        {1, 1, 0},
        {1, 0, 1},
        {0, 1, 1},
        {0, 0, 0}
};

int mode, prev_mode, save_game;
int noMenu;

string int_to_str(int angka){
    stringstream temp_str;
    temp_str << angka;
    string final_str = temp_str.str();
    return final_str;
}

void setTitle(string title_name){
    char title[title_name.length()+1];
    strcpy(title, title_name.c_str());
    glutSetWindowTitle(title);
}

void make_title(){
    int level = save_game - 7;
    if (mode == 0){
        setTitle("Welcome To Maze 3D Game");
    }else if (mode==1){
        setTitle(("170411100061 - Tugas Besar 4 - Maze Game - Level "+int_to_str(level)));
    }else if (mode == 2){
        setTitle("How to Play Maze 3D");
    }else if (mode == 3){
        setTitle("Credit Maze 3D");
    }else if (mode == 4){
        setTitle("About Maze 3D");
    }else if (mode == 5){
        setTitle("Selamat Anda Sukses di Level "+int_to_str(level));
    }
}

void setMaterialColor(float r, float g, float b, float a)
{
    GLfloat mat_specular[] = { r, g, b, a };
    GLfloat mat_shininess[] = { 10.0 };
    GLfloat mat_diffuse[] = { r, g, b, a };

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
}

void setLigthingColor(float r, float g, float b)
{
        GLfloat light_position_diff[] = { 10.0, 10.0, 5.0, 0.0};
        GLfloat light_position_spec[] = { 10.0, -10.0, 5.0, 0.0 };
        GLfloat diffuse_light[] = { 0.8, 0.8, 1.0, 0.5 };
        GLfloat specular_light[] = { 1.0, 1.0, 1.0, 1.0 };
        GLfloat ambient_light[] = { 0.9, 0.9, 0.9, 1.0 };
        glLightfv(GL_LIGHT0, GL_POSITION, light_position_diff);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
        glLightfv(GL_LIGHT1, GL_POSITION, light_position_spec);
        glLightfv(GL_LIGHT1, GL_SPECULAR, specular_light);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient_light);
}

void create_array()
{
    ROAD = new int[4];
    MAP = new int*[mazeSize];
    for(i = 0; i < mazeSize; ++i){
        MAP[i] = new int[mazeSize];
    }
}

void initRoadIndex()
{
    right_road  = rand_x+2;
    left_road   = rand_x-2;
    top_road    = rand_y-2;
    bottom_road = rand_y+2;
}

void check_road()
{
    road = 0;
    road_now = MAP[rand_x][rand_y];
    initRoadIndex();

    if (right_road <= max_road && MAP[right_road][rand_y] != road_now){
        ROAD[road] = 0;
        road += 1;
    }
    if (left_road >= 0 && MAP[left_road][rand_y] != road_now){
        ROAD[road] = 1;
        road += 1;
    }
    if (top_road >= 0 && MAP[rand_x][top_road] != road_now){
        ROAD[road] = 2;
        road += 1;
    }
    if (bottom_road <= max_road && MAP[rand_x][bottom_road] != road_now){
        ROAD[road] = 3;
        road += 1;
    }
}

void wallRemove(int x_road, int y_road, int xNeighbor, int yNeighbor)
{
    MAP[xNeighbor][yNeighbor] = 0;
    int roadTemp = MAP[x_road][y_road];
    for (i=0; i < mazeSize; i++){
        for (j=0; j < mazeSize; j++){
            if (MAP[i][j] == roadTemp){
//                cout << "temp " << roadTemp  << ", data " << i << "," << j << " ganti dengan " << rand_x << ", " << rand_y << endl;
                MAP[i][j] = MAP[rand_x][rand_y];
            }
        }
    }
}

void make_road()
{
    check_road();
    initRoadIndex();
    if (road > 0){
        int select_road = ROAD[rand() % road];
//        cout << "random x " << rand_x << " y " << rand_y << endl;
//        cout  << "select road " << select_road << endl;
        if (select_road == 0){
            wallRemove(right_road, rand_y, rand_x+1, rand_y);
        }else if (select_road == 1){
            wallRemove(left_road, rand_y, rand_x-1, rand_y);
        }else if (select_road == 2){
            wallRemove(rand_x, top_road, rand_x, rand_y-1);
        }else if (select_road == 3){
            wallRemove(rand_x, bottom_road, rand_x, rand_y+1);
        }
        cell--;
    }
}

void makeDoor()
{
    MAP[door_pos][0] = 0;
    MAP[door_pos][max_index] = 0;
}

void generate_maze()
{
    if (LENGTH % 2 == 0){
        door_pos = LENGTH+1;
    }else{
        door_pos = LENGTH;
    }
    x_player = door_pos;
    y_player = 0;

    create_array();
    cell = LENGTH*LENGTH-1;
    int x = 2;
    for (i=0; i < mazeSize; i++){
        for (j=0; j < mazeSize; j++){
            if (i % 2 == 0 || j % 2 == 0){
                MAP[i][j] = 1;
            }else{
                MAP[i][j] = x;
                x++;
            }
        }
    }
    while (cell != 0){
        rand_x = (rand() % LENGTH + 1)*2 - 1;
        rand_y = (rand() % LENGTH + 1)*2 - 1;
        make_road();
    }
    makeDoor();
}

void reloadOrtho()
{
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-marginMaze, maxOrtho+marginMaze, -marginMaze, maxOrtho+marginMaze, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}


void rotateMaze()
{
    glTranslated(maxOrtho/2, maxOrtho/2, 0);
    glRotated(angleX, 1, 0, 0);
    glRotated(angleY, 0, 1, 0);
    glRotated(angleZ, 0, 0, 1);
    glTranslated(-maxOrtho/2, -maxOrtho/2, 0);
}

void wallModelOne()
{
    glPushMatrix();
    glTranslated(-0.5,-0.5,0);
    glColor4f(1,0,0,1);
    glBegin(GL_POLYGON);
    //back
    glNormal3f(0,0,-1);
        glVertex3d(0,0,0);
        glVertex3d(0,1,0);
        glVertex3d(1,1,0);
        glVertex3d(1,0,0);
    glEnd();
    //front
    glBegin(GL_POLYGON);
    glNormal3f(0,0,1);
        glVertex3d(0,0,1);
        glVertex3d(0,1,1);
        glVertex3d(1,1,1);
        glVertex3d(1,0,1);
    glEnd();
    //left
    glColor4f(1,1,0,1);
    glBegin(GL_POLYGON);
        glNormal3f(-1,0,0);
        glVertex3d(0,0,0);
        glVertex3d(0,1,0);
        glVertex3d(0,1,1);
        glVertex3d(0,0,1);
    glEnd();
    //right
    glBegin(GL_POLYGON);
        glNormal3f(1,0,0);
        glVertex3d(1,0,0);
        glVertex3d(1,1,0);
        glVertex3d(1,1,1);
        glVertex3d(1,0,1);
    glEnd();
    //bottom
    glColor4f(0,1,0,1);
    glBegin(GL_POLYGON);
    glNormal3f(0,-1,0);
        glVertex3d(0,0,0);
        glVertex3d(0,0,1);
        glVertex3d(1,0,1);
        glVertex3d(1,0,0);
    glEnd();
    //top
    glBegin(GL_POLYGON);
    glNormal3f(0,1,0);
        glVertex3d(0,1,0);
        glVertex3d(0,1,1);
        glVertex3d(1,1,1);
        glVertex3d(1,1,0);
    glEnd();
    glPopMatrix();
}

void makeWall(int posX, int posY)
{
    glPushMatrix();
        glTranslated(posX, -posY, 0);
        glTranslated(0.5, maxOrtho, 0.1);
        isTransparant == true ? setMaterialColor(COLOR[color_x][0], COLOR[color_x][1], COLOR[color_x][2],0.5) : setMaterialColor(COLOR[color_x][0], COLOR[color_x][1], COLOR[color_x][2],1);
        if (wallModel == 1){
            glTranslated(0,0,0.6);
            glutSolidCube(1);
        }
        else if (wallModel == 2){
            wallModelOne();
        }
        else if (wallModel == 3){
            glRotated(45,0,0,1);
            glutSolidCone(0.75,1,4,3);
        }
    glPopMatrix();
}

void makePlayer(int posX, int posY)
{
    glPushMatrix();
        glColor3f(0,0,1);
        glTranslated(posX, -posY, 0);
        glTranslated(0.5, maxOrtho, 0.7);
        setMaterialColor(1,1,0, 1);
        if (playerModel == 1){
            glutSolidSphere(0.4,36,36);
        }
        else if (playerModel == 2){
            glTranslated(0,0, -0.5);
            glutSolidCone(0.4,0.8,36,36);
        }
    glPopMatrix();
}


void displayText(char align, float y, char *text, int size_huruf) {
    setMaterialColor(0,0,0, 1);
    char *c;
    void* font_x;
    float width_text, x;

    if (size_huruf == 10){
        font_x = GLUT_BITMAP_HELVETICA_10;
    }else if (size_huruf == 12){
        font_x = GLUT_BITMAP_HELVETICA_12;
    }else if (size_huruf == 18){
        font_x = GLUT_BITMAP_HELVETICA_18;
    }

    if (align == 'c'){
        width_text = glutBitmapLength(font_x, (unsigned char *)text);
        x = (17 - (width_text+2.5)/(size_huruf*1.8))/2;
    }else if (align == 'l'){
        width_text = glutBitmapLength(font_x, (unsigned char *)text);
        x = 2;
    }else if (align == 'r'){
        width_text = glutBitmapLength(font_x, (unsigned char *)text);
        x = 17 - width_text/3;
    }

    glRasterPos3f(x, y, 50);
    for (c=text; *c != '\0'; c++) {
        glutBitmapCharacter(font_x, *c);
    }
}


void makeSelector(int no_menu)
{
    glPushMatrix();
        glTranslated(17/2, 17/2, 70);
        glTranslated(-3.5, 4-2*no_menu, 0);
        setMaterialColor(0,0,0,1);
        glRotated(30,0,0,1);
        glutSolidCone(0.75,1,3,3);
    glPopMatrix();
}


void startingMaze(int ms)
{
    LENGTH = ms;
    max_index = LENGTH*2;
    max_road = max_index-1;
    mazeSize = LENGTH*2+1;
    marginMaze = (double)2.5/(double)DEFAULT_SIZE*(double)LENGTH;
    maxOrtho = mazeSize;
    generate_maze();
    wallModel = 1;
    playerModel = 1;
    noMenu = 1;
    make_title();
    pos[1] = maxOrtho + marginMaze + 2;
    LENGTH % 2 == 0 ? pos[0] = (maxOrtho+marginMaze-0.5)/2: pos[0] = (maxOrtho+marginMaze-2)/2;
}

void setView(){
    isNight == true ? glClearColor(0,0,0,1) : glClearColor(1,1,1,1) ;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(viewMode == 2 && mode == 1) {
        gluPerspective(90.0,1,1,200);
        gluLookAt(pos[0],pos[1],pos[2]+0.5,pos[0]+viewDir[0],pos[1]+viewDir[1],pos[2]+viewDir[2],0,0,1);
    }
    else{
        glOrtho(-marginMaze, maxOrtho+marginMaze, -marginMaze, maxOrtho+marginMaze, -100.0, 100.0);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}

void mainPlay()
{
    srand(time(NULL));
    mode = 1;
    setView();
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glTranslated(xMove, 0, zMove);
    glRotated(angle3D,1,0,0);
    rotateMaze();
    glPushMatrix();
        glTranslated(LENGTH+0.5, LENGTH+1, 0);
        setMaterialColor(0,1,0,0.4);
        glTranslated(0,0,0);
        glScaled(maxOrtho,maxOrtho,0.3);
        glutSolidCube(1);
    glPopMatrix();
    for (i=0; i < mazeSize; i++){
        for (j=0; j < mazeSize; j++){
            if (MAP[i][j] == 1){
                makeWall(i, j);
            }
        }
    }
    makePlayer(x_player, y_player);
    glutSwapBuffers();
}

void mainMenu()
{
    mode = 0;
    setView();
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
        glTranslated(LENGTH+0.5, LENGTH+1, 0.1);
        setMaterialColor(0, 0.2, 0, 0.6);
        glTranslated(0,0,0);
        glScaled(maxOrtho+marginMaze,maxOrtho+marginMaze,0.1);
        glutSolidCube(1);
    glPopMatrix();
    float top_text = 9/2-1.8/2;
    float jarak_text = 9/4;
    displayText('c', 17-top_text, "Menu", 18);
    displayText('c', 17-(top_text+jarak_text*2), "Play Game", 18);
    displayText('c', 17-(top_text+jarak_text*3), "How to Play", 18);
    displayText('c', 17-(top_text+jarak_text*4), "Credits", 18);
    displayText('c', 17-(top_text+jarak_text*5), "About", 18);
    displayText('c', 17-(top_text+jarak_text*6), "Quit", 18);
    makeSelector(noMenu);
    glutSwapBuffers();
}

void mainHelp(void){
    mode = 2;
    setView();
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    float top_text = 9/4-2.5/2;
    float jarak_text = 9/5;
    displayText('c', 17-top_text, "How to Play", 18);
    displayText('l', 17-(top_text+jarak_text*2), "Press :", 18);
    displayText('l', 17-(top_text+jarak_text*3), "W = untuk bergerak ke atas", 18);
    displayText('l', 17-(top_text+jarak_text*4), "S = untuk bergerak ke bawah", 18);
    displayText('l', 17-(top_text+jarak_text*5), "A = untuk bergerak ke kiri", 18);
    displayText('l', 17-(top_text+jarak_text*6), "D = untuk bergerak ke kanan", 18);
    displayText('l', 17-(top_text+jarak_text*7), "Q = untuk turun level", 18);
    displayText('l', 17-(top_text+jarak_text*8), "E = untuk naik level", 18);
    displayText('l', 17-(top_text+jarak_text*9), "R = untuk merubah pola", 18);
    displayText('l', 17-(top_text+jarak_text*10), "C = untuk merubah warna", 18);
    // displayText('l', 17-(top_text+jarak_text*11), "Mouse Klik Kanan = untuk merubah pola", 18);
    displayText('l', 17-(top_text+jarak_text*12), "T = untuk merubah Texture", 18);

    displayText('l', 17-(top_text+jarak_text*14), "Special Key :", 18);
    displayText('l', 17-(top_text+jarak_text*15), "ENTER = untuk select", 18);
    displayText('l', 17-(top_text+jarak_text*16), "ESC   = untuk kembali", 18);
    displayText('c', 17-(top_text+jarak_text*17), "Press ESC to Kembali", 12);
    glutSwapBuffers();
}


void mainCredits(void){
    mode = 3;
    setView();
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    float top_text = 9/4-2.5/2;
    float jarak_text = 9/4;
    displayText('c', 17-top_text, "Credits", 18);
    displayText('c', 17-(top_text+jarak_text*2), "Wijanarko Putra Rajeb", 18);
    displayText('c', 17-(top_text+jarak_text*3), "170411100061", 18);
    displayText('c', 17-(top_text+jarak_text*4), "Grafika Komputer A", 18);
    displayText('c', 17-(top_text+jarak_text*6), "Universitas Trunojoyo Madura", 18);
    displayText('c', 17-(top_text+jarak_text*8), "Press ESC to Kembali", 12);
    glutSwapBuffers();
}


void mainAbout(void){
    mode = 4;
    setView();
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    float top_text = 9/2-2.5/2;
    float jarak_text = 9/4;
    displayText('c', 17-top_text, "About", 18);
    displayText('c', 17-(top_text+jarak_text*2), "Maze 3D ini dibuat dengan menggunakan", 18);
    displayText('c', 17-(top_text+jarak_text*3), "Algoritma Kruskal", 18);
    displayText('c', 17-(top_text+jarak_text*4), "Sehingga pola Mazenya dibuat secara Otomatis", 18);
    displayText('c', 17-(top_text+jarak_text*6), "Press ESC to Kembali", 12);
    glutSwapBuffers();
}


void mainWin(void){
    mode = 5;
    setView();
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    int level = save_game - 8;
    float top_text = 9/2-2.5/2;
    float jarak_text = 9/4;
    string lvl_temp = int_to_str(level);
    char lvl[lvl_temp.length()+1];
    strcpy(lvl, lvl_temp.c_str());
    displayText('c', 17-top_text, "Selamat", 18);
    displayText('c', 17-(top_text+jarak_text*2), "LEVEL", 18);
    displayText('c', 17-(top_text+jarak_text*3), lvl, 18);
    displayText('c', 17-(top_text+jarak_text*4), "CLEAR", 18);
    displayText('c', 17-(top_text+jarak_text*6), "Press ENTER to Continue", 12);
    glutSwapBuffers();
}


void keyboard(unsigned char key, int x, int y)
{
    if (mode == 1)
    {
        if (viewMode){
            if((key=='a' || key=='A') && x_player < max_index && MAP[x_player+1][y_player] != 1){
                x_player += 1;
            }
            if((key=='d' || key=='D') && x_player > 0 && MAP[x_player-1][y_player] != 1){
                x_player -= 1;
            }
            if((key=='s' || key=='S') && y_player > 0 && MAP[x_player][y_player-1] != 1){
                y_player -= 1;
            }
            if((key=='w' || key=='W') && y_player < max_index && MAP[x_player][y_player+1] != 1){
                y_player += 1;
            }
        }else {
            if((key=='d' || key=='D') && x_player < max_index && MAP[x_player+1][y_player] != 1){
                x_player += 1;
            }
            if((key=='a' || key=='A') && x_player > 0 && MAP[x_player-1][y_player] != 1){
                x_player -= 1;
            }
            if((key=='w' || key=='W') && y_player > 0 && MAP[x_player][y_player-1] != 1){
                y_player -= 1;
            }
            if((key=='s' || key=='S') && y_player < max_index && MAP[x_player][y_player+1] != 1){
                y_player += 1;
            }
        }
        if((key=='r' || key=='R')){
            generate_maze();
        }
        if((key=='v' || key=='V')){
            viewMode += 1;
            if (viewMode > 2){
                viewMode = 0;
            }
            if (viewMode == 0){
                angle3D=0; angleX=0; angleY=0; angleZ=0;
            }else if(viewMode == 1) {
                angle3D=-15;
            }else if(viewMode == 2) {
                angle3D=0;
            }
        }
        if (viewMode == 1 || viewMode == 2){
            if(key=='x'){
                angleX+=1;
            }
            if(key=='y'){
                angleY+=1;
            }
            if(key=='z'){
                angleZ+=1;
            }
            if(key=='X'){
                angleX-=1;
            }
            if(key=='Y'){
                angleY-=1;
            }
            if(key=='Z'){
                angleZ-=1;
            }
        }
        if((key=='q' || key=='Q')){
            if (LENGTH>8){
                LENGTH-=1;
                save_game = LENGTH;
                startingMaze(LENGTH);
                reloadOrtho();
            }
        }
        if((key=='e' || key=='E')){
            LENGTH+=1;
            save_game = LENGTH;
            startingMaze(LENGTH);
            reloadOrtho();
        }
        if ((key == 't' || key == 'T')){
            wallModel += 1;
            if (wallModel > 3){
                wallModel = 1;
            }
        }
        if ((key == 'p' || key == 'P')){
            playerModel += 1;
            if (playerModel > 2){
                playerModel = 1;
            }
        }
        if((key=='c' || key=='C')){
            color_x += 1;
            if (color_x > 6){
                color_x = 1;
            }
        }
        if (x_player == door_pos && y_player == max_index){
            LENGTH += 1;
            save_game = LENGTH;
            mode = 5;
        }

        if (key=='b' || key == 'B'){
            if(isTransparant){
                isTransparant = false;
            }else{
                isTransparant = true;
            }
        }

        if (key=='n' || key == 'N'){
            if(isNight){
                isNight = false;
            }else{
                isNight = true;
            }
        }
    }
    else if (mode == 0)
    {
        if((key=='w' || key=='W') && noMenu > 1){
            noMenu -= 1;
        }else if((key=='s' || key=='S') && noMenu < 5){
            noMenu += 1;
        }else if(key==13){
            prev_mode = mode;
            if (noMenu == 1){
                mode = 1;
                startingMaze(save_game);
                reloadOrtho();
            }else if (noMenu == 2){
                mode = 2;
            }else if (noMenu == 3){
                mode = 3;
            }else if (noMenu == 4){
                mode = 4;
            }else if (noMenu == 5){
                exit(0);
            }
        }
    }

    if (key == 13 && mode == 5){
        mode = 1;
        startingMaze(save_game);
        reloadOrtho();
    }

    if (key == 27){
        if (prev_mode == 1){
            prev_mode -= 1;
        }
        if (mode == 1){
            save_game = LENGTH;
        }
        if (mode == 0){
            exit(0);
        }
        mode = prev_mode;
    }

    if (mode != 1){
        glMatrixMode (GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-2.5, 19.5, -2.5, 19.5, -100.0, 100.0);
        glMatrixMode(GL_MODELVIEW);

    }

    if (viewMode == 2 && mode == 1)
    {
        if (key == 'j')
        {
            xMove -= 1;
        }
        if (key == 'l')
        {
            xMove += 1;
        }
        if (key == 'k')
        {
            zMove -= 1;
        }
        if (key == 'i')
        {
            zMove += 1;
        }
        mode = 1;
    }
    


    if (mode == 0)
    {
        mainMenu();
    }
    else if (mode == 1)
    {
        mainPlay();
    }
    else if (mode == 2)
    {
        mainHelp();
    }
    else if (mode == 3)
    {
        mainCredits();
    }
    else if (mode == 4)
    {
        mainAbout();
    }
    else if (mode == 5)
    {
        mainWin();
    }


}

void specKey(int key, int x, int y){
    if (viewMode == 2) {
            if(key == GLUT_KEY_RIGHT ) {
                    alpha-=0.1;
                    viewDir[0] = cos(alpha);
                    viewDir[1] = sin(alpha);
            }else if(key == GLUT_KEY_LEFT) {
                    alpha+=0.1;
                    viewDir[0] = cos(alpha);
                    viewDir[1] = sin(alpha);
            }else if(key == GLUT_KEY_UP) {
                    pos[0]+=1*viewDir[0];
                    pos[1]+=1*viewDir[1];
                    pos[2]+=1*viewDir[2];
            }else if(key == GLUT_KEY_DOWN) {
                    pos[0]-=1*viewDir[0];
                    pos[1]-=1*viewDir[1];
                    pos[2]-=1*viewDir[2];
            }
            mainPlay();
            // cout << pos[0] << " " << pos[1] << " " << pos[2] << endl;
    }
}

void reshape(int w, int h)
{
    glViewport (0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
        glOrtho (-marginMaze, maxOrtho+marginMaze, -marginMaze*(GLfloat)h/(GLfloat)w, (maxOrtho+marginMaze)*(GLfloat)h/(GLfloat)w, -100.0, 100.0);
    else
        glOrtho (-marginMaze*(GLfloat)h/(GLfloat)w, (maxOrtho+marginMaze)*(GLfloat)w/(GLfloat)h, -marginMaze, maxOrtho+marginMaze, -100.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    setView();
 }



void myinit()
{
    glClearColor(1,1,1,1);
    setLigthingColor(1, 1, 1);
    glEnable(GL_LIGHTING);
    glShadeModel (GL_SMOOTH);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    //3Ddependeci
    glShadeModel(GL_FLAT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    save_game = DEFAULT_SIZE;
}

void printHelp()
{
    printf("Press :\n");
    printf("W = untuk bergerak ke atas\n");
    printf("S = untuk bergerak ke bawah\n");
    printf("A = untuk bergerak ke kiri\n");
    printf("D = untuk bergerak ke kanan\n");
    printf("Q = untuk turun level\n");
    printf("E = untuk naik level\n");
    printf("R = untuk merubah pola\n");
    printf("C = untuk merubah warna\n");
    printf("T = untuk merubah Texture\n");
    printf("P = untuk merubah tipe player\n");
    printf("V = untuk merubah view mode\n");
    printf("B = untuk merubah transparancy\n");
    printf("N = untuk merubah kondisi pagi/malam\n");
    printf("J = untuk menggeser maze ke kiri\n");
    printf("L = untuk menggeser maze ke kanan\n");
    printf("K = untuk menggeser maze ke bawah\n");
    printf("I = untuk menggeser maze ke atas\n");
    printf("x       = untuk Memutar sumbu x anticlockwise\n");
    printf("shift+x = untuk Memutar sumbu x clockwise\n");
    printf("y       = untuk Memutar sumbu y anticlockwise\n");
    printf("shift+y = untuk Memutar sumbu y clockwise\n");
    printf("z       = untuk Memutar sumbu z anticlockwise\n");
    printf("shift+z = untuk Memutar sumbu z clockwise\n");
    printf("Arrow UP    = untuk melakukan zoom in camera\n");
    printf("Arrow Down  = untuk melakukan zoom out camera\n");
    printf("Arrow LEFT  = untuk melakukan rotasi camera ke kiri\n");
    printf("Arrow RIGHT = untuk melakukan rotasi camera ke kanan\n");
}


int main(int argc, char* argv[])
{
    srand(time(NULL));
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(100,200);
	glutCreateWindow("170411100061 - Tugas Besar 4 - Maze 3D - Viewing dan Shading");
	startingMaze(DEFAULT_SIZE);
	glutDisplayFunc(mainMenu);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
    glutSpecialFunc(specKey);
    printHelp();
	myinit();
	glutMainLoop();
	return 0;
}


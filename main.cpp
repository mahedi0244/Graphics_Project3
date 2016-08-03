#ifdef __APPLE__
#  include <GLUT/glut.h>
#else
#  include <GL/glut.h>
#endif
#include <math.h>
#include <stdlib.h>
#define PI 3.14159265

int width = 400,height = 600,vert [100][2], test_points[100][2],n = 0,k=0,
type = GL_LINE_STRIP,v,t;

bool rubberbanding = false;
bool antialiasing = false;
bool drawn = false;
float angle = 0;

bool outside_test(int a);

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    if (n == 1 && (type == GL_LINE_STRIP || type == GL_LINE_LOOP)){
        glColor3f(1, 1, 0);
        glBegin(GL_POINTS);
        glVertex2iv(vert[0]);
        glEnd();
    }
    
    glBegin(type);
    glColor3f(1, 1, 0);
    for (int i = 0; i < n; i++)
        glVertex2iv(vert[i]);
    glEnd();
    
    
    glBegin(GL_POINTS);
    for (int i = 0; i < k; i++){
        if (outside_test(i) == false ){
            glColor3f(0, 1, 0);
            
        }
       else
            glColor3f(1, 0, 0);
        glVertex2iv(test_points[i]);
           
        
    }
    glEnd();
    
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case 'a' : antialiasing= !antialiasing;
            if (antialiasing){
                glEnable(GL_BLEND);
                glEnable(GL_LINE_SMOOTH);
            }
            else{
                glDisable(GL_BLEND);
                glDisable(GL_LINE_SMOOTH);
            }
            break;
            
        case 'r' : n = 0; k = 0; drawn = false; type = GL_LINE_STRIP; break;
            
        case 'l' : type = GL_LINE_STRIP; break;
            
        case 'c' : type = GL_LINE_LOOP; drawn = true; break;
            
        case 'v' : type = GL_POINTS; break;
    }
    
    glutPostRedisplay();
}

int findVertex(int x, int y){
    
    int dx, dy;
    for (int i = 0; i < n; i++){
        dx = vert[i][0] - x;
        dy = vert[i][1] - y;
        if (dx*dx + dy*dy <16)
            return i;
    }
    
    return -1;
}

void mouse (int button, int state, int x, int y){
    
    if (drawn == false){
        switch(button){
            case GLUT_LEFT_BUTTON:
                if (state == GLUT_DOWN){
                    v = n++;
                    vert[v][0] = x;
                    vert[v][1] = height - 1 - y;
                    rubberbanding = true;
                    glutPostRedisplay();
                    
                }
                
                else
                    rubberbanding = false;
                break;
                
            case GLUT_RIGHT_BUTTON:
                if (state == GLUT_DOWN && (v = findVertex(x, height-1-y)) != -1){
                    if (glutGetModifiers() == GLUT_ACTIVE_CTRL){
                        for (int i = v; i < n-1; i++){
                            vert[i][0] = vert[i+1][0];
                            vert[i][1] = vert[i+1][1];
                        }
                        n--;
                    }
                    else{
                        vert[v][0] = x;
                        vert[v][1] = height - 1 - y;
                        rubberbanding = true;
                        
                    }
                    glutPostRedisplay();
                }
                else
                    rubberbanding = false;
                break;
        }
    }
    if (drawn == true){
        switch(button){
            case GLUT_LEFT_BUTTON:
                if (state == GLUT_DOWN){
                    t = k++;
                    test_points[t][0] = x;
                    test_points[t][1] = height - 1 - y;
                    glutPostRedisplay();
                }
                break;
        }
    }
    
}

void motion(int x, int y){
    
    if (rubberbanding){
        vert[v][0] = x;
        vert[v][1] = height - 1 - y;
        glutPostRedisplay();
    }
}

//value of a vector
float vector_value(int selected_point[2], int point[2]){
    
    float a = selected_point[0] - point[0];
    float b = selected_point[1] - point[1];
    return sqrt(a*a + b*b);
}

//calculate dot products
float dot_product(int selected_point[2], int point1[2], int point2[2]){

    return ((selected_point[0]-point1[0]) * (selected_point[0]-point2[0])) + ((selected_point[1]-point1[1]) * (selected_point[1]-point2[1]));
}

//calculate the angle between two vectors (defined by three points) from their dot products
float dot_product_angle(int selected_point[2], int point1[2], int point2[2]){
    
    float numerator = dot_product(selected_point, point1, point2);
    
    float a = selected_point[0] - point1[0];
    float b = selected_point[1] - point1[1];
    float c = sqrt(a*a + b*b);
    
    float d = selected_point[0] - point2[0];
    float e = selected_point[1] - point2[1];
    float f = sqrt(d*d + e*e);
    
    float denominator = c * f;
    
    float param = numerator / denominator;
    
    return acos (param) * 180.0 / PI;
}

//calculate cross products
float cross_product(int selected_point[2], int point1[2], int point2[2]){
    
    return ((selected_point[0]-point1[0]) * (selected_point[1]-point2[1])) - ((selected_point[1]-point1[1]) * (selected_point[0]-point2[0]));
}

bool outside_test(int a){
    for (int i = 0; i < n-1; i++){
        if (cross_product(test_points[a], vert[i], vert[i+1]) > 0)
            angle = angle + dot_product_angle(test_points[a], vert[i], vert[i+1]);
        else
            angle = angle - dot_product_angle(test_points[a], vert[i], vert[i+1]);
    }
    if (cross_product(test_points[a], vert[n-1], vert[0]) > 0)
        angle = angle + dot_product_angle(test_points[a], vert[n-1], vert[0]);
    else
        angle = angle - dot_product_angle(test_points[a], vert[n-1], vert[0]);
    
    if (abs(angle) <= .1){
        //debugging comment
        //std:: cout<<"total angle is "<<angle<<". Color should be red"<<std:: endl;
        angle = 0;
        return true;
    }
    else{
        //debugging comment
        //std:: cout<<"total angle is "<<angle<<". Color should be green"<<std:: endl;
        angle =  0;
        return false;
    }
}

int main(int argc, char ** argv){
    int test1[1][2] = {{0,0}};
    int test2[1][2] = {{-6,-2}};
    int test3[1][2] = {{2,88}};
   
    glutInit(& argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(50, 100);
    glutCreateWindow("Md_Mahedi_Rana");
    
    glClearColor(0.0,0.0,0.0,0.0);
    glColor3f(1, 1, 0);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    
    glutMainLoop();
    
}
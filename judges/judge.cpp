#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include "../Forest.h"
#include "../Tree.h"
#include "../oi.h"

#define MAXN 1000

using namespace std;

oi::Scanner *resScanner;
FILE *valfile, *resfile;
Forest * forest;

void ierror(int where)
{
    fprintf(stderr, "Internal error - test data are incorrect at %d\n", where);
    exit(2);
}

int readdata()
{
    int t, n, k;
    int x, y, h, d, c, p;
    if (fscanf(valfile, "%d %d %d", &t, &n, &k) != 3) ierror(3);
    if (n > MAXN || n < 1) ierror(4);
    forest = new Forest(n);
    while(fscanf(valfile, "%d %d %d %d %d %d", &x, &y, &h, &d, &c, &p) == 6)
    {
        Tree * tree = new Tree(h,d,c,p,x,y);
        forest->plantTree(x,y,tree);
    }
    int res = 0;
    int px = 0;
    int py = 0;
    string kind;
    string direction;
    char string_c[10];
    while(t > 0){
        if(resScanner->isEOF())
            break;
        //printf("%d %d\n", px, py);
        resScanner->readString(string_c, 10);
        kind.assign(string_c);
        resScanner->skipWhitespaces();
        resScanner->readString(string_c, 10);
        direction.assign(string_c);
        resScanner->skipWhitespacesUntilEOLN();
        resScanner->readEofOrEoln();
        if(kind == "move"){
            if(direction == "up")
                py++;
            else if(direction == "down")
                py--;
            else if(direction == "left")
                px--;
            else if(direction == "right")
                px++;
            else
                ierror(5);
            if(px < 0 || py < 0 || px >= forest->getSize() || py >= forest->getSize()) ierror(6);
            t--;
        }
        else if(kind == "cut"){
            if(!forest->isTree(px,py)) ierror(7);
            Tree * tree = new Tree(*forest->getTree(px,py));

            if(tree->width <= t){
                t -= tree->width;
                res += tree->getValue();
                int current_height = tree->height;
                forest->removeTree(px,py);
                //printf("CUT %d %d\n", px, py);
                int dx = 0;
                int dy = 0;
                if(direction == "up")
                    dy = 1;
                else if(direction == "down")
                    dy = -1;
                else if(direction == "left")
                    dx = -1;
                else if(direction == "right")
                    dx = 1;
                else
                    ierror(8);
                int tx = px+dx;
                int ty = py+dy;
                current_height--;
                while(current_height > 0)
                {
                    if(tx < 0 || tx >= forest->getSize() || ty < 0 || ty >= forest->getSize())
                        break;
                    if(forest->isTree(tx,ty))
                    {
                        Tree * newTree = new Tree(*forest->getTree(tx,ty));
                        if(tree->isHeavier(*newTree))
                        {
                            res += newTree->getValue();
                            tree = newTree;
                            current_height = tree->height;
                            //printf("FALL %d %d\n", tx, ty);
                        }
                        else
                            break;
                    }
                    tx += dx;
                    ty += dy;
                    current_height--;
                }
            }
            else
                ierror(9);
        }
        else {
            cout << "KIND: "<< kind <<endl;
            ierror(10);
        }
    }
    return res;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("%d\n", argc);
        printf("Usage: %s input useroutput\n", argv[0]);
        return 1;
    }
    valfile = fopen(argv[1], "r");
    resfile = fopen(argv[2], "r");
    if (valfile == NULL) ierror(1);
    if (resfile == NULL) ierror(2);
    resScanner = new oi::Scanner(resfile, oi::EN);
    printf("%d|SUCCESS\n", readdata());
    return 0;
}

#ifndef MAIN_H
#define MAIN_H

struct player  {
    int r;
    int c;
    int w;
    int h;
};

struct enemy{
    int r;
    int c;
    int w;
    int h;
    int dr;
};

struct item {
    int r;
    int c;
    int w;
    int h;
    int status;
};

struct goal {
    int r;
    int c;
    int w;
    int h;
};

#endif

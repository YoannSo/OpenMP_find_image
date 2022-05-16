



unsigned char* getGrey(const  char* path,int* width,int* height);
unsigned char* getNormal(const  char* path,int* width,int* height);

unsigned char* getPetiteImage(int x,int y,unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight);
int* findImage(int debutW,int debutH,int moitieW,int moitieH,unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight,int* resultI,int* resultJ);
int*  test(unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight,int* result);
unsigned char * rgb_to_grey(unsigned char* input,int size);
unsigned char* optiGrey_to_greyImage(unsigned char *input,int width,int height);
int compareImage(unsigned char *img1,unsigned char *img2,int height, int width);
int* findImageInImage(unsigned char *imgSource, int inputImgWidth, int inputImgHeight, unsigned char *searchImg, int searchImgWidth, int searchImgHeight,int* resultI,int* resultJ);

unsigned char * encadrerEnRouge(int x,int y,unsigned char* input,int inputWidth,int inputHeight,int searchWidth,int searchHeight);
void save(unsigned char* saveImg,int width,int height);

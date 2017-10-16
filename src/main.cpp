#include "prefix.hpp"

pthread_mutex_t ptmMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ptmCond = PTHREAD_COND_INITIALIZER;

pthread_t pthreadProducter, pthreadConsumer;

void* ProductFunc(void * pParam);
void* ConsumerFunc(void * pParam);

void ImageProcess(Mat& );

VideoCapture vpCamera;
queue<Mat> qmatCameraQueue;
int nWaitKey = -1;
double dPreProcessTime = 0.0f;

int main(int argc, char **argv) {
    
    
    vpCamera.open(0);
    
    if(!vpCamera.isOpened()){
        cerr << "Camera doesn't open!" << endl;
        exit(-1);
    }
    
    vpCamera.set(CV_CAP_PROP_FRAME_WIDTH, 960);
    vpCamera.set(CV_CAP_PROP_FRAME_HEIGHT, 720);  
    
    int nDevNum = 0;
    namedWindow("CameraCapture");
    int ntrProducter = pthread_create(&pthreadProducter, NULL, ProductFunc, NULL);
    int ntrConsumer = pthread_create(&pthreadConsumer, NULL, ConsumerFunc, NULL);
    
    cout << "The Producter thread creating result is:" << ntrProducter << endl;
    cout << "The Consumer thread creating result is:" << ntrProducter << endl;
    if(0 != ntrProducter){
        cerr << "Producter thread doesn't create!" << endl;
        if(ntrConsumer == 0){
            while(!pthread_cancel(pthreadConsumer)){}
        }
        else{
            exit(-1);
        }
        pthread_join(pthreadConsumer, NULL);
        exit(-1);
    }
    
    if(0 != ntrConsumer){
        cerr << "Consumer thread doesn't create!" << endl;
        if(ntrProducter == 0){
            while(!pthread_cancel(pthreadProducter)){}
        }
        else{
            exit(-1);
        }
        pthread_join(pthreadProducter, NULL);
        exit(-1);
    }
        
    pthread_join(pthreadProducter, NULL);
    pthread_join(pthreadConsumer, NULL);

//     while(27 != nWaitKey){
//         vpCamera >> matCameraFrame;
//         cout << "The frame size is:" << matCameraFrame.cols << "*" << matCameraFrame.rows <<endl;
//         imshow("CameraCapture", matCameraFrame);
//         nWaitKey = waitKey(1);
//     }

    return 0;
}


void* ProductFunc(void * pParam){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL); 
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL); 
    
    while(27 != nWaitKey){
        sleep(0.001);
        pthread_mutex_lock(&ptmMutex);
        Mat matCameraFrame;
        vpCamera >> matCameraFrame;
        qmatCameraQueue.push(matCameraFrame);
        cout << "Producter function run!" << endl;
        cout << qmatCameraQueue.size() << endl;
        pthread_mutex_unlock(&ptmMutex);
        pthread_testcancel();
        pthread_cond_signal(&ptmCond);
    }

    
}

void* ConsumerFunc(void * pParam){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL); 
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL); 

    
    while(27 != nWaitKey){
        cout << "Consumer function start!" << endl;
        pthread_mutex_lock(&ptmMutex);
        cout << "Consumer function in mutex lock!" << endl;
        while(0 == qmatCameraQueue.size()){
            pthread_cond_wait(&ptmCond, &ptmMutex);
            break;
        }
        
        Mat matCameraFrame = qmatCameraQueue.front();
        qmatCameraQueue.pop();
        double dCurrentImageProcessTime = (double)getTickCount();
        ImageProcess(matCameraFrame);
        double dImageProcessTime = ((double)getTickCount() - dCurrentImageProcessTime) / getTickFrequency();
        cout << "Image process time is:" << dImageProcessTime <<endl;
        cout << "The image size is " << matCameraFrame.cols << "*" <<  matCameraFrame.rows << endl;
        
        double dCurrentProcessTime = (double)(getTickCount());
        double dProcessTime = (dCurrentProcessTime - dPreProcessTime) / getTickFrequency();
        dPreProcessTime = dCurrentProcessTime;
        cout << dProcessTime <<endl;
        imshow("CameraCapture", matCameraFrame);
        nWaitKey = waitKey(1);
        cout << "Consumer function run!" << endl;
        pthread_testcancel();
        pthread_mutex_unlock(&ptmMutex);

    }

    cout << "Consumer function end!" << endl;
    
}

void ImageProcess(Mat& matSrcImage){
    if(matSrcImage.empty()){
        cerr << "Input image of ImageProcess function is empty!" << endl;
        exit(-1);
    }
    
    flip(matSrcImage, matSrcImage, 1);
    matSrcImage = matSrcImage(Rect((matSrcImage.cols - 640) /2,
                                   (matSrcImage.rows - 720) /2,
                                   640,
                                   720
    ));
    
    //medianBlur(matSrcImage, matSrcImage, 3);
    //GaussianBlur(matSrcImage, matSrcImage, Size(3, 3), 0.0, 0.0 );
    
    
    
}

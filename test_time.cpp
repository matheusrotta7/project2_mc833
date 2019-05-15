#include<bits/stdc++.h>
#include<sys/time.h>

using namespace std;

int main () {


    struct timeval tv1, tv2;

    gettimeofday(&tv1, NULL);

    system("sleep 1");    
    gettimeofday(&tv2, NULL);
    printf("em microsegundos: %ld\n", tv2.tv_usec - tv1.tv_usec);
    printf("em segundos: %ld\n", tv2.tv_sec - tv1.tv_sec);


    return 0;
}

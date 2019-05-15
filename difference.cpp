#include<bits/stdc++.h>

using namespace std;

int main() {

    FILE* fp1;
    FILE* fp2;

    fp1 = fopen("listener_result.txt", "r");
    fp2 = fopen("talker_result.txt", "r");

    int result, listener, talker;


    while (fscanf(fp1, "%d", &listener) != EOF && fscanf(fp2, "%d", &talker) != EOF) {
        cout << talker - listener << '\n';
    }


    return 0;
}

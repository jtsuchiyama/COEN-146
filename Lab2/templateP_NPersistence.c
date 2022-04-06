//COEN 146L : Lab2, step 3

// Name: Jake Tsuchiyama
// Date: 1/12/22
// Title: Lab2 - HTTP: R.T.T, Persistent, and Non-Persistent
// Description: The program calculates the time for four different scenerios. 
// These scenerios include the time taken for persistent and non-persistent
// cases.

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
        if (argc != 2) {  // check correct usage
		fprintf(stderr, "usage: %s <n connections> \n", argv[0]);
		exit(1);
	}
        //Declare and values to n (n connections), np (np parralel connections), RTT0, RTT1, RTT2, RTTHTTP, RTTDNS, .. 
        int n = atoi(argv[1]);
        int np;

        int RTT0 = 3;
        int RTT1 = 20;
        int RTT2 = 26;
        int RTTHTTP = 47;

        int RTTDNS = RTT0 + RTT1 + RTT2;

        // Question 1
        int RTTOneObject = RTTDNS + 2*RTTHTTP;

        printf("One object: %d msec\n", RTTOneObject);

        // Question 2
        int RTTSixObjectsRef = RTTDNS + 2*RTTHTTP + 6*2*RTTHTTP;

        printf("Non-Persistent 6 objects: %d msec\n", RTTSixObjectsRef);
        
        // Question 3

        //find how many np (parallel connections)
        if(6 % n == 0)
                np = 6/n;
        else            
                np = 6/n + 1;

        int RTTPersistent = RTTDNS + 2*RTTHTTP + np*RTTHTTP;
        int RTTNonPersistent = RTTDNS + 2*RTTHTTP + np*2*RTTHTTP;;
        
        printf("%d parallel connection - Persistent: %d msec\n", n, RTTPersistent);
        printf("%d parallel connection - Non-Persistent: %d msec\n", n, RTTNonPersistent);

return 0;
}
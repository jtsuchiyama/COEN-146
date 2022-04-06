// COEN 146L - Lab7: Link State routing
 
// Name: Jake Tsuchiyama
// Date: 2/23/22
// Title: Lab7 - Implementing Link State Routing 
// Description: The goal of this lab is to develop and implement link state routing using
// Dijkstra's algorithm. Each machine used in routing is defined in the machines file, and 
// the cost of the links is defined in the costs file. 

#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>


//defines
#define	N			4
#define	INFINITE	1000
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))


// types
typedef struct machines
{
	char	name[50];
	char	ip[50];
	int		port;	

} MACHINES;


// global variables
MACHINES	machines[N];
int			costs[N][N];
int			distances[N];
int			myid, nodes;
int			sock;
struct sockaddr_in addr;
struct sockaddr_in otheraddr;
socklen_t addr_size;
pthread_mutex_t	lock;

// print costs
void print_costs(void)
{
	int i, j;
	
	for(i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
		{
			pthread_mutex_lock(&lock);
			printf("%d\t", costs[i][j]);
			pthread_mutex_unlock(&lock);
		}
		printf("\n");
	}
}


// receive info
void *receive_info (void *arg)
{
	int updated[3];
	int nbytes;
	int n0, n1, n2;

	while(1)
	{
		// if the data was not properly received
		if(recvfrom(sock, updated, sizeof(int) * 3, 0, NULL, NULL) != 12)
		{
			printf("error receiving\n");
		}
		
		n0 = ntohl(updated[0]);
		n1 = ntohl(updated[1]);
		n2 = ntohl(updated[2]);

		// update costs
		pthread_mutex_lock(&lock);
		costs[n0][n1] = n2;
		costs[n1][n0] = n2;
		pthread_mutex_unlock(&lock);

		printf("received\n");
		print_costs();
	}
}


// run_link_state
void * run_link_state (void *arg)
{
	int	taken[N];
	int	min, spot;
	int	i, j;
	int	r;

	while (1)
	{
		r = rand () % 10;
		sleep (r);
	
		for (i = 0; i < N; i++)
		{
			taken[i] = 0;
			pthread_mutex_lock (&lock);
			distances[i] = costs[myid][i];
			pthread_mutex_unlock (&lock);
		}
		taken[myid] = 1;
	
		for (i = 1; i < N; i++)
		{
			// find closest node/minimum by iteratively determining the minimum
			min = INFINITE;
			spot = -1;
			for(j=0; j < N; j++)
			{
				if(taken[j] == 0)
				{
					if(distances[j] < min)
					{
						min = distances[j];
						spot = j;
					}
				}
			}
		
			// put the closest node/minimum into the set 
			taken[spot] = 1;			

			// recalculate all the distances according to the added node
			for (j = 0; j < N; j++)
			{
				if (taken[j] == 0)
				{
					// find the minimum between the old and new distances
					pthread_mutex_lock (&lock);
					distances[j] = MIN (distances[j], (distances[spot] + costs[spot][j]));
					pthread_mutex_unlock (&lock);
				}
			}
		}

		printf ("new distances:\n");
		for (i = 0; i < N; i++)
			printf ("%d ", distances[i]);
		printf ("\n");
	}
}

//main()
int main (int argc, char *argv[])
{
	FILE	*fp;
	int		i, j;
	pthread_t	thr1, thr2;
	int		id, cost;
	int		packet[3];

	//Get from the command line, id, machines, cost table
    	if (argc != 5) {
        	printf("Usage: %s <id> <nodes> <machines> <cost table>\n", argv[0]);
        	exit(0);
    	}

	myid = atoi (argv[1]);
	nodes = atoi (argv[2]);

	if (myid >= N)
	{
		printf ("wrong id\n");
		return 1;
	}	

	if (nodes != N)
	{
		printf ("wrong number of nodes\n");
		return 1;
	}
	
	//get info on machines 
	if ((fp = fopen (argv[3], "r")) == NULL)
	{
		printf ("can't open %s\n", argv[3]);
		return 1;
	}

	for (i = 0; i < nodes; i++)
		fscanf (fp, "%s%s%d", machines[i].name, machines[i].ip, &machines[i].port);

	fclose (fp);

	// get costs
	if ((fp = fopen (argv[4], "r")) == NULL)
	{
		printf ("can't open %s\n", argv[4]);
		return 1;
	}

	for (i = 0; i < nodes; i++)
	{
		for (j = 0; j < nodes; j++)
		{
			fscanf (fp, "%d", &costs[i][j]);
		}
	}
	fclose (fp);
		

    	// init address
    	addr.sin_family = AF_INET;
    	addr.sin_port = htons ((short)machines[myid].port);
    	addr.sin_addr.s_addr = htonl (INADDR_ANY);
    	memset ((char *)addr.sin_zero, '\0', sizeof (addr.sin_zero));
    	addr_size = sizeof (addr);

    	// create socket
    	if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
    	{
        	printf ("socket error\n");
        	return 1;
    	}

    	// bind
    	if (bind (sock, (struct sockaddr *)&addr, sizeof (addr)) != 0)
    	{
        	printf ("bind error\n");
        	return 1;
    	}


	// create threads
	pthread_mutex_init (&lock, NULL);
	pthread_create (&thr1, NULL, receive_info, NULL);
	pthread_create (&thr2, NULL, run_link_state, NULL);

	// read changes from the keyboard
	for (i = 0; i < 3; i++)
	{
		printf ("any changes?\n");
		scanf ("%d%d", &id, &cost);

		if (id >= N  ||  id == myid)
		{
			printf ("wrong id\n");
			break;
		}

		pthread_mutex_lock (&lock);
		costs[myid][id] = cost;
		costs[id][myid] = cost;
		pthread_mutex_unlock (&lock);
		print_costs ();

		packet[0] = htonl (myid);
		packet[1] = htonl (id);
		packet[2] = htonl (cost);
		otheraddr.sin_family = AF_INET;
		addr_size = sizeof (otheraddr);

		for (j = 0; j < N; j++)
		{
			if (j != myid)
			{
				otheraddr.sin_port = htons ((short)machines[j].port);
				inet_pton (AF_INET, machines[j].ip, &otheraddr.sin_addr.s_addr);
				sendto (sock, packet, sizeof (packet), 0, (struct sockaddr *)&otheraddr, addr_size);
			}
		}
		printf ("sent\n");
	}

	sleep (20);
 	return 0;
}


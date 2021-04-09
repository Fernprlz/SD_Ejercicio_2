#define MAXSIZE	256
enum opcode {INIT, SET, GET, MOD, DEL ,EXIST, ITEMS};

struct request_struct  {
	int op;

	char *key;
	char *v1;
	int *v2;
	float *v3;

	char q_name[MAXSIZE];
};

typedef struct request_struct *request;

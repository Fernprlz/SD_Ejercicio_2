#include <stdio.h>
#include "claves.c"

int main() {
	int err = 0;


	err = init();
	if (err == -1) {
		printf("Eror en init\n");
		return -1;
	}


	err = set_value("clave1", "dato", 20, 4.5);
	if (err == -1) {
		printf("Eror en set_value\n");
		return -1;
	}


	char v1[256];
	int v2;
	float v3;
	err = get_value("clave1", v1, &v2, &v3);
	if (err == -1) {
		printf("Eror en get_value\n");
		return -1;
	}
	if (v2 != 20) {
		printf("Dato mal recupeado para clave1\n");
		return -1;
	}


	return (0);
}
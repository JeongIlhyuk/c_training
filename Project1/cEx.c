#include <stdio.h>
//#define MAX_MENU 50
#define MAX_FOOD_NAME 50
#define MAX_PRICE_LENGTH 50

typedef struct {
	char name[MAX_FOOD_NAME];
	int price;
} Food;
typedef struct {
	Food* foodArr;
	int count;
	int capacity;
} Menu;
void initMenu(Menu* m) {
	m->capacity = 10;  // 초기 용량, 필요에 따라 조정 가능
	m->foodArr = malloc(m->capacity * sizeof(Food));
	m->count = 0;
}
void addFood(Menu* m, Food nF) {
	if (m->count == m->capacity) {
		m->capacity *= 2;
		m->foodArr = realloc(m->foodArr, m->capacity * sizeof(Food));
	}
	m->foodArr[m->count++] = nF;
}
void freeMenu(Menu* m) {
	free(m->foodArr);
	m->foodArr = NULL;
	m->count = 0;
	m->capacity = 0;
}
int printMain(void) {
	int select;
	printf("1. Add menu item\n");
	printf("2. Remove menu item\n");
	printf("3. Create order\n");
	printf("4. View order\n");
	printf("5. Process payment\n");
	printf("6. Exit\n");
	printf("Select a main menu option:");
	scanf("%d", &select);
	return select;
}
void printMenu(Menu* m) {
	printf("===== Menu =====");
	if (m != NULL) {

	}
}

int main(void) {
	int s;
	s = printMain();
	Menu menu;
	initMenu(&menu);
	switch (s)
	{
	case 1:
		printf("Enter item information");
		printf("Name:");
		Food newFood;
		scanf("%s", newFood.name);
		printf("Price:");
		scanf("%d", newFood.price);
		addFood(&menu, newFood);
		printf("Menu item has been added.");
		break;
	case 2:
		printMenu(&menu);
		break;
	case 3:
		break;
	case 4:
		break;
	case 5:
		break;
	case 6:
		printf("Exiting the program.");
		return 0;
		break;
	default:
		printf("error");
		return -1;
		break;
	}
	return 0;
}
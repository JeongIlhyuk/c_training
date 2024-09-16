#include <stdio.h>
//#define MAX_MENU 50
#define MAX_FOOD_NAME 50
#define MAX_PRICE_LENGTH 50
#define MAX_TABLE 50


typedef struct {
	char name[MAX_FOOD_NAME];
	int price;
} Food;
typedef struct {
	Food* foodArr;
	int count;
	int capacity;
} Menu;

typedef struct {
	char name[MAX_FOOD_NAME];
	char quantity;
}Ordered;
typedef struct {
	int num;//테이블 번호

	Ordered* orderArr;
	int count;//주문된 양
	int capacity;
} Table;

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
		for (int i = 0; i < m->count; i++) {
			printf("%d. %s - %d won\n",i+1,m->foodArr[i].name, m->foodArr[i].price)
		}
	}
}
void newOrder(Table* t, Ordered o) {
	if (t->count == t->capacity) {
		t->capacity *= 2;
		t->orderArr = realloc(t->orderArr, t->capacity * sizeof(Ordered));
	}
	t->orderArr[t->count++] = o;
}

int main(void) {
	int s;
	s = printMain();

	Menu menu;
	initMenu(&menu);
	
	Table t[MAX_TABLE];
	switch (s)
	{
	case 1://신메뉴
		printf("Enter item information");
		printf("Name:");
		Food newFood;
		scanf("%s", newFood.name);
		printf("Price:");
		scanf("%d", newFood.price);
		addFood(&menu, newFood);
		printf("Menu item has been added.");
		break;
	case 2://메뉴 단종
		printMenu(&menu);
		printf("Select the item to remove:");
		int i;
		scanf("%d", &i);
		printf("%s item has been removed from the menu.\n", menu.foodArr[i].name);
		break;
	case 3://주문 생성
		printf("Enter table number:");
		int n;
		scanf("%d", &n);
		printMenu(&menu);


		printf("Select menu item(enter 0 to finish):");
		int i;
		scanf("%d", &i);

		printf("Enter quntity:");
		int q;
		scanf("%d", &q);


		Ordered o;
		o.name = menu.foodArr[i].name;
		o.quantity = q;
		newOrder(t[n - 1], p);

		printf("Order has been created. Total amount %d won", menu.foodArr[i].price * q);
		break;
	case 4:
		printf("Enter table number:");
		int n;
		scanf("%d", &n);
		printf("===== Table %d Order =====\n", n);
		printf("Order items:\n");


		Table cT;
		cT = t[n - 1];
		for (int i = 0; i < cT.count; i++) {
			Ordered orderItem = cT.orderArr[i];
			printf("%s X %d", orderItem.name, orderItem.quantity);
		}
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
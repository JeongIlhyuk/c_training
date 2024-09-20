#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#define MAX_MENU 50
#define MAX_FOOD_NAME 50
#define MAX_PRICE_LENGTH 50
#define MAX_TABLES 50


typedef struct FoodNode {
	char name[MAX_FOOD_NAME];
	int price;
	struct FoodNode* next;
} FoodNode;
typedef struct {
	FoodNode* head;
	int count;
} Menu;

typedef struct {
	int foodIndex;
	int quantity;
}Ordered;
typedef struct {
	Ordered* orderArr;
	int count;//주문된 양
	int total;//총액
	int capacity;
	int totalAmount;
} Table;

int printMain(void) {
	int select;
	printf("1. Add menu item\n");
	printf("2. Remove menu item\n");
	printf("3. Create order\n");
	printf("4. View order\n");
	printf("5. Process payment\n");
	printf("6. Exit\n");
	printf("Enter the main menu number:");
	scanf("%d", &select);
	return select;
}
void printMenu(Menu* m) {
	printf("===== Menu =====\n");
	FoodNode* current = m->head;
	int i = 1;
	while (current != NULL) {
		printf("%d. %s - %d won\n", i++, current->name, current->price);
		current = current->next;
	}
}
int getTableNum() {
	printf("Enter table number:");
	int n;
	scanf("%d", &n);
	return n;
}

void initMenu(Menu* m) {
	m->head = NULL;
	m->count = 0;
}
void addMenu(Menu* m, const char* name, int price) {
	FoodNode* newNode = (FoodNode*)malloc(sizeof(FoodNode));
	if (newNode == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return;
	}
	strcpy_s(newNode->name, sizeof(newNode->name), name);
	newNode->price = price;
	newNode->next = NULL;

	if (m->head == NULL) {
		m->head = newNode;
	}
	else {
		FoodNode* current = m->head;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = newNode;
	}
	m->count++;
	printf("Menu item '%s' has been added.\n", name);
}
void removeMenuItem(Menu* m, int index) {
	if (index < 0 || index >= m->count) {
		printf("Invalid index. Cannot remove item.\n");
		return;
	}

	FoodNode* current = m->head;
	FoodNode* prev = NULL;

	if (index == 0) {
		m->head = current->next;
		printf("%s item has been removed from the menu.\n", current->name);
		free(current);
	}
	else {
		for (int i = 0; i < index; i++) {
			prev = current;
			current = current->next;
		}
		prev->next = current->next;
		printf("%s item has been removed from the menu.\n", current->name);
		free(current);
	}

	m->count--;
}
void freeMenu(Menu* m) {
	FoodNode* current = m->head;
	while (current != NULL) {
		FoodNode* temp = current;
		current = current->next;
		free(temp);
	}
	m->head = NULL;
	m->count = 0;
}

void initTable(Table* t) {
	t->capacity = 10;  // 초기 용량, 필요에 따라 조정 가능
	t->orderArr = malloc(t->capacity * sizeof(Ordered));
	t->count = 0;
	t->total = 0;
}
void addOrder(Table* t, int foodIndex, int quantity, Menu* m) {
	if (t->count == t->capacity) {
		size_t new_capacity = t->capacity * 2;
		Ordered* temp = realloc(t->orderArr, new_capacity * sizeof(Ordered));
		if (temp == NULL) {
			fprintf(stderr, "Memory reallocation failed\n");
			return;
		}
		t->orderArr = temp;
		t->capacity = (int)new_capacity;
	}

	FoodNode* current = m->head;
	int currentIndex = 0;
	while (current != NULL && currentIndex < foodIndex) {
		current = current->next;
		currentIndex++;
	}

	if (current == NULL) {
		fprintf(stderr, "Invalid food index\n");
		return;
	}

	t->orderArr[t->count].foodIndex = foodIndex;
	t->orderArr[t->count].quantity = quantity;
	t->count++;
	t->total += quantity * current->price;
}
void cleanTable(Table* t) {
	if (t->count > 0) {
		for (int i = 0; i < t->count; i++) {
			t->orderArr[i].foodIndex = -1;
			t->orderArr[i].quantity = 0;
		}
	}
	t->count = 0;
	t->total = 0;
}
void freeTable(Table* t) {
	free(t->orderArr);
	t->orderArr = NULL;
	t->count = 0;
	t->capacity = 0;
}
//내용 저장
void saveMenu(Menu* m) {//여기부터 점검하기!
	FILE* file = fopen("menu.dat", "wb");
	if (file == NULL) {
		printf("Error opening file for writing.\n");
		return;
	}

	// 메뉴 항목 수 저장
	fwrite(&m->count, sizeof(int), 1, file);

	// 각 메뉴 항목 저장
	FoodNode* current = m->head;
	while (current != NULL) {
		fwrite(current->name, sizeof(char), MAX_FOOD_NAME, file);
		fwrite(&current->price, sizeof(int), 1, file);
		current = current->next;
	}

	fclose(file);
	printf("Menu saved successfully.\n");
}
void loadMenu(Menu* m) {
	FILE* file = fopen("menu.dat", "rb");
	if (file == NULL) {
		printf("No existing menu file found. Starting with an empty menu.\n");
		return;
	}

	int count;
	if (fread(&count, sizeof(int), 1, file) != 1) {
		printf("Error reading menu count.\n");
		fclose(file);
		return;
	}

	for (int i = 0; i < count; i++) {
		char name[MAX_FOOD_NAME];
		int price;

		if (fread(name, sizeof(char), MAX_FOOD_NAME, file) != MAX_FOOD_NAME ||
			fread(&price, sizeof(int), 1, file) != 1) {
			printf("Error reading menu item %d.\n", i + 1);
			fclose(file);
			return;
		}

		addMenu(m, name, price);
	}

	fclose(file);
	printf("Menu loaded successfully. %d items loaded.\n", count);
}

void saveTables(Table* tables) {
	FILE* file = fopen("tables.dat", "wb");
	if (file == NULL) {
		printf("Error opening file for writing.\n");
		return;
	}
	for (int i = 0; i < MAX_TABLES; i++) {
		fwrite(&tables[i].count, sizeof(int), 1, file);
		fwrite(&tables[i].total, sizeof(int), 1, file);
		fwrite(tables[i].orderArr, sizeof(Ordered), tables[i].count, file);
	}
	fclose(file);
}
void loadTables(Table* tables) {
	FILE* file = fopen("tables.dat", "rb");
	if (file == NULL) {
		printf("No existing tables file found. Starting with empty tables.\n");
		return;
	}

	for (int i = 0; i < MAX_TABLES; i++) {
		if (fread(&tables[i].count, sizeof(int), 1, file) != 1 ||
			fread(&tables[i].total, sizeof(int), 1, file) != 1) {
			printf("Error reading table %d data.\n", i);
			fclose(file);
			return;
		}

		tables[i].capacity = tables[i].count > 0 ? tables[i].count : MAX_TABLES;
		Ordered* temp = realloc(tables[i].orderArr, tables[i].capacity * sizeof(Ordered));
		if (temp == NULL) {
			printf("Memory reallocation failed for table %d.\n", i);
			for (int j = 0; j < i; j++) {
				free(tables[j].orderArr);
				tables[j].orderArr = NULL;
				tables[j].count = 0;
				tables[j].capacity = 0;
			}
			fclose(file);
			return;
		}
		tables[i].orderArr = temp;

		if (tables[i].count > 0) {
			size_t items_read = fread(tables[i].orderArr, sizeof(Ordered), tables[i].count, file);
			if (items_read != tables[i].count) {
				printf("Error reading orders for table %d. Expected %d, read %zu.\n", i, tables[i].count, items_read);
				return;
			}
		}
	}

	fclose(file);
}
int main(void) {
	Menu menu;
	initMenu(&menu);
	loadMenu(&menu);  // 프로그램 시작 시 메뉴 로드
	
	Table table[MAX_TABLES];
	for (int i = 0; i < MAX_TABLES; i++) {
		initTable(&table[i]);
	}
	loadTables(table);

	int s;
	while (1) {
		s = printMain();
		switch (s)
		{
		case 1://신메뉴
		{
			char name[MAX_FOOD_NAME];
			int price;

			printf("Enter item information\n");
			printf("Name: ");
			scanf("%s", name);
			printf("Price: ");
			scanf("%d", &price);

			addMenu(&menu, name, price);
		}
		break;
		case 2://메뉴 단종
			printMenu(&menu);
			printf("Enter the number of the item to remove:");
			int i;
			scanf("%d", &i);
			removeMenuItem(&menu, i - 1);
			break;
		case 3: {//주문 생성
			int n = getTableNum();
			printMenu(&menu);
			while (1) {
				printf("Enter the food menu number:(enter 0 to finish):");
				int i;
				scanf("%d", &i);
				if (i == 0) {
					break;
				}

				printf("Enter quantity:");
				int q;
				scanf("%d", &q);

				addOrder(&table[n - 1], i - 1, q, &menu);
			}
			printf("Order has been created. Total amount %d won\n", table[n - 1].total);
		}
			  break;
		case 4: {
			int n = getTableNum();
			printf("===== Table %d Order =====\n", n);
			printf("Order items:\n");

			Table* cT = &table[n - 1];
			for (int i = 0; i < cT->count; i++) {
				Ordered orderItem = cT->orderArr[i];

				// 메뉴에서 해당 음식 찾기
				FoodNode* current = menu.head;
				for (int j = 0; j < orderItem.foodIndex && current != NULL; j++) {
					current = current->next;
				}

				if (current != NULL) {
					printf("%s X %d\n", current->name, orderItem.quantity);
				}
				else {
					printf("Unknown food item X %d\n", orderItem.quantity);
				}
			}

			printf("Total amount: %d won\n", cT->total);
		}
			  break;
		case 5: {
			int n = getTableNum();
			printf("Payment of %d won has been completed. The table has been reset.\n", table[n - 1].total);
			cleanTable(&table[n - 1]);
		}
			  break;
		case 6:
			printf("Saving data and exiting the program.\n");
			saveMenu(&menu);  // 메뉴 저장
			saveTables(table);  // 테이블 정보 저장
			freeMenu(&menu);
			for (int i = 0; i < MAX_TABLES; i++) {
				freeTable(&table[i]);
			}
			return 0;
		default:
			printf("error\n");
			return -1;
			break;
		}
	}
	freeMenu(&menu);
	return 0;
}
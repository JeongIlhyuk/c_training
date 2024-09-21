#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 100//\0을 제외한 입력 가능한 최대 글자 수


typedef struct FoodNode {
	char name[MAX_INPUT];
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

//입력 규칙
static int getInt() {
	//0을 포함한 자연수만 받음
	//\n을 입력해 주지 않기 때문에 따로 입력해 주어야 함
	char n[MAX_INPUT + 2];// 1 더 큰 버퍼로 최대 글자 수 초과했는지 판단
	char* endptr;
	long number;//int의 범위를 벗어나는지 판단하기 위해 더 큰 범위를 커버하는 long 타입을 사용

	while (1) {
		if (fgets(n, sizeof(n), stdin) == NULL) {
			printf("An input error occurred. Please try again.\n");
		}
		else {
			// 개행 문자 제거
			n[strcspn(n, "\n")] = '\0';//strcspn는 \n 앞의 글자 수를 반환
			if (strlen(n) > MAX_INPUT) {//strlen은 \0을 제외한 글자 수를 반환
				printf("Warning: Please enter a number with %d digits or fewer.\n", MAX_INPUT);
			}
			else {

				// 입력된 문자열의 앞뒤 공백 제거
				char* start = n;
				char* end = n + strlen(n) - 1;
				while (isspace((unsigned char)*start)) start++;
				if (*start == '\0') {
					printf("Error: Empty input.\n");
				}
				else {
					while (end > start && isspace((unsigned char)*end)) end--;
					*(end + 1) = '\0';

					// 숫자 변환 시도
					number = strtol(start, &endptr, 10);

					// 변환 후 남은 문자가 있는지 확인
					if (*endptr != '\0') {//숫자로 변환할 수 없는 문자를 만난 위치
						printf("Error: Please enter a non-negative integer.\n");
					}
					else {

						// 범위 체크
						if (number < 0 || number > INT_MAX) {
							printf("Error: Please enter a non-negative integer with up to %d digits.\n", MAX_INPUT);
						}
						else {
							return (int)number;
						}
					}
				}
			}
		}
	}
}
static char* getString() {
	char* s = (char*)malloc(sizeof(char) * (MAX_INPUT + 2));// 1 더 큰 버퍼로 최대 글자 수 초과했는지 판단
	if (s == NULL) {
		fprintf(stderr, "메모리 할당 실패\n");
		return NULL;
	}
	while (1) {
		if (fgets(s, sizeof(MAX_INPUT + 2), stdin) == NULL) {
			printf("입력 오류 발생\n");
		}
		else {
			s[strcspn(s, "\n")] = '\0';// \n 제거
			if (strlen(s) > MAX_INPUT) {
				printf("경고: 입력이 너무 깁니다. \n");
				continue;
			}
			else {
				return s;
			}
		}
	}
}

static int printMain(void) {
	int select;
	while (1) {
		printf("1. Add menu item\n");
		printf("2. Remove menu item\n");
		printf("3. Create order\n");
		printf("4. View order\n");
		printf("5. Process payment\n");
		printf("6. Exit\n");
		printf("Enter the main menu number:");
		select = getInt();
		printf("\n");
		if (select > 6 || select < 1) {
			printf("Enter a number between 1 and 6\n");
		}
		else {
			return select;
		}
	}
}
static void printMenu(Menu* m) {
	printf("===== Menu =====\n");
	FoodNode* current = m->head;
	int i = 1;
	while (current != NULL) {
		printf("%d. %s - %d won\n", i++, current->name, current->price);
		current = current->next;
	}
}
static int getTableNum() {
	while (1) {
		printf("Enter table number:");
		int n;
		n = getInt();
		printf("\n");;
		if (n == 0) {
			printf("Enter a number greater than or equal to 1:");
		}
		else {
			return n;
		}
	}
}

static void initMenu(Menu* m) {
	m->head = NULL;
	m->count = 0;
}
static void addMenu(Menu* m, const char* name, int price) {
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
static void removeMenuItem(Menu* m, int index) {
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
static void freeMenu(Menu* m) {
	FoodNode* current = m->head;
	while (current != NULL) {
		FoodNode* temp = current;
		current = current->next;
		free(temp);
	}
	m->head = NULL;
	m->count = 0;
}

static void initTable(Table* t) {
	t->capacity = 10;  // 초기 용량, 필요에 따라 조정 가능
	t->orderArr = malloc(t->capacity * sizeof(Ordered));
	t->count = 0;
	t->total = 0;
}
static void addOrder(Table* t, int foodIndex, int quantity, Menu* m) {
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
static void cleanTable(Table* t) {
	if (t->count > 0) {
		for (int i = 0; i < t->count; i++) {
			t->orderArr[i].foodIndex = -1;
			t->orderArr[i].quantity = 0;
		}
	}
	t->count = 0;
	t->total = 0;
}
static void freeTable(Table* t) {
	free(t->orderArr);
	t->orderArr = NULL;
	t->count = 0;
	t->capacity = 0;
}
//내용 저장
static void saveMenu(Menu* m) {
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
		fwrite(current->name, sizeof(char), MAX_INPUT, file);
		fwrite(&current->price, sizeof(int), 1, file);
		current = current->next;
	}

	fclose(file);
	printf("Menu saved successfully.\n");
}
static void loadMenu(Menu* m) {
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
		char name[MAX_INPUT];
		int price;

		if (fread(name, sizeof(char), MAX_INPUT, file) != MAX_INPUT ||
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

static void saveTables(Table* tables) {
	FILE* file = fopen("tables.dat", "wb");
	if (file == NULL) {
		printf("Error opening file for writing.\n");
		return;
	}
	for (int i = 0; i < MAX_INPUT; i++) {
		fwrite(&tables[i].count, sizeof(int), 1, file);
		fwrite(&tables[i].total, sizeof(int), 1, file);
		fwrite(tables[i].orderArr, sizeof(Ordered), tables[i].count, file);
	}
	fclose(file);
}
static void loadTables(Table* tables) {
	FILE* file = fopen("tables.dat", "rb");
	if (file == NULL) {
		printf("No existing tables file found. Starting with empty tables.\n");
		return;
	}

	for (int i = 0; i < MAX_INPUT; i++) {
		if (fread(&tables[i].count, sizeof(int), 1, file) != 1 ||
			fread(&tables[i].total, sizeof(int), 1, file) != 1) {
			printf("Error reading table %d data.\n", i);
			fclose(file);
			return;
		}

		tables[i].capacity = tables[i].count > 0 ? tables[i].count : MAX_INPUT;
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
	
	Table table[MAX_INPUT];
	for (int i = 0; i < MAX_INPUT; i++) {
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
			printf("Enter item information\n");
			printf("Name: ");
			char* n = getString();
			if (n != NULL) {
				printf("Price: ");
				int p = getInt();
				printf("\n");
				addMenu(&menu, n, p);
				free(n); //getString 내에서 할당한 메모리를 해제
			}
			else {
				printf("Memory allocation failed\n");
			}
		}
		break;
		case 2://메뉴 단종
			printMenu(&menu);
			printf("Enter the number of the item to remove:");
			int i = getInt();
			printf("\n");
			removeMenuItem(&menu, i - 1);
			break;
		case 3: {//주문 생성
			int n = getTableNum();
			printMenu(&menu);
			while (1) {
				printf("Enter the food menu number:(enter 0 to finish):");
				int i = getInt();
				printf("\n");
				if (i == 0) {
					break;
				}

				printf("Enter quantity:");
				int q = getInt();
				printf("\n");

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
			for (int i = 0; i < MAX_INPUT; i++) {
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_INPUT 100//'\0'을 제외한 입력 가능한 최대 글자 수
#define MAX_TABLES 100


typedef struct FoodNode {
	char name[MAX_INPUT + 1];
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
	int totalAmount;//총 결제 금액
	int capacity;//주문을 저장할 한도
} Table;

// 입력 규칙. \n을 제거하기 때문에 따로 입력해 줘야 함
static void clearBuffer();//버퍼 제거
static int getInt(void);//음이 아닌 정수만 받음
static char* getString(void);
//테이블 번호 받기
static int getTableNum(void);

// 파일 입출력 함수
static int fileClose(FILE* f);
static void saveFoodList(Menu* m);
static void loadFoodList(Menu* m);
static void saveTables(Table* tables);
static void loadTables(Table* tables);

//메인
static int printMain(void);

// 음식 메뉴 조작
static void initFoodList(Menu* m);
static void addToFoodList(Menu* m, const char* name, int price);
static void removeFromFoodList(Menu* m, int index);
static void printFoodList(Menu* m);
static void freeFoodList(Menu* m);
// 테이블 관련 함수
static void initTable(Table* t);
static void addOrder(Table* t, int foodIndex, int quantity, Menu* m);
static void clearTable(Table* t);
static void freeTable(Table* t);

static void clearBuffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}
static int getInt() {
	char n[MAX_INPUT + 2];// 1 더 큰 버퍼로 최대 글자 수 초과했는지 판단
	char* endptr;
	long num;//int의 범위를 벗어나는지 판단하기 위해 더 큰 범위를 커버하는 long 타입을 사용

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
					num = strtol(start, &endptr, 10);

					// 변환 후 남은 문자가 있는지 확인
					if (*endptr != '\0') {//숫자로 변환할 수 없는 문자를 만난 위치
						printf("Error: Please enter a non-negative integer.\n");
					}
					else {

						// 범위 체크
						if (num < 0 || num > INT_MAX) {
							printf("Error: Please enter a non-negative integer not exceeding %d.\n", INT_MAX);
						}
						else {
							//clearBuffer();
							return (int)num;
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
		if (fgets(s, MAX_INPUT + 2, stdin) == NULL) {
			printf("입력 오류 발생\n");
		}
		else {
			s[strcspn(s, "\n")] = '\0';// \n 제거
			if (strlen(s) <= MAX_INPUT) {
				//clearBuffer();
				return s;
			}
			else {
				printf("경고: 입력이 너무 깁니다. \n");
			}
		}
	}
}

static int printMain(void) {
	int s;
	while (1) {
		printf("\n1. Add menu item\n");
		printf("2. Remove menu item\n");
		printf("3. Create order\n");
		printf("4. View order\n");
		printf("5. Process payment\n");
		printf("6. Exit\n");
		printf("Enter the main menu number:");
		s = getInt();
		if (s > 6 || s < 1) {
			printf("Enter a number between 1 and 6\n");
		}
		else {
			return s;
		}
	}
}
static void printFoodList(Menu* m) {
	printf("\n===== Menu =====\n");
	FoodNode* node = m->head;
	int i = 1;
	while (node != NULL) {
		printf("%d. %s - %d won\n", i++, node->name, node->price);
		node = node->next;
	}
}
static int getTableNum() {
	while (1) {
		printf("\nEnter table number:");
		int n;
		n = getInt();
		if (n == 0) {
			printf("Enter a number greater than or equal to 1:");
		}
		else {
			return n;
		}
	}
}

static void initFoodList(Menu* m) {
	m->head = NULL;
	m->count = 0;
}
static void addToFoodList(Menu* m, const char* name, int price) {
	FoodNode* fn = (FoodNode*)malloc(sizeof(FoodNode));
	if (fn == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
	}
	else {
		strncpy_s(fn->name, sizeof(fn->name), name, MAX_INPUT);//\0 제외한 글자 수
		fn->name[MAX_INPUT] = '\0';
		fn->price = price;
		fn->next = NULL;

		if (m->head == NULL) {
			m->head = fn;
		}
		else {
			FoodNode* node = m->head;
			while (node->next != NULL) {
				node = node->next;
			}
			node->next = fn;
		}
		m->count++;
		printf("\nMenu item '%s' has been added.\n", name);
	}
}
static void removeFromFoodList(Menu* m, int index) {
	if (index < 0 || index >= m->count) {
		printf("Invalid index. Cannot remove item.\n");
	}
	else {

		FoodNode* node = m->head;
		FoodNode* prev = NULL;

		if (index == 0) {
			m->head = node->next;
			printf("\n%s item has been removed from the menu.\n", node->name);
			free(node);
		}
		else {
			for (int i = 0; i < index; i++) {
				prev = node;
				node = node->next;
			}
			prev->next = node->next;
			printf("%s item has been removed from the menu.\n", node->name);
			free(node);
		}

		m->count--;
	}
}
static void freeFoodList(Menu* m) {
	FoodNode* node = m->head;
	while (node != NULL) {
		FoodNode* temp = node;
		node = node->next;
		free(temp);
	}
	m->head = NULL;
	m->count = 0;
}

static void initTable(Table* t) {
	t->capacity = 10;  // 초기 용량, 필요에 따라 조정 가능
	t->orderArr = (Ordered*)malloc(t->capacity * sizeof(Ordered));
	t->count = 0;
	t->totalAmount = 0;
}
static void addOrder(Table* t, int index, int quantity, Menu* m) {
	if (t->count == t->capacity) {
		//size_t newCapacity = t->capacity * 2;수정할 부분
		size_t newCapacity;

		if (t->capacity > SIZE_MAX / 3 * 2) {
			newCapacity = SIZE_MAX;
		}
		else {
			// 1.5배 증가
			newCapacity = (size_t)t->capacity + ((size_t)t->capacity >> 1);
		}
		Ordered* temp = realloc(t->orderArr, newCapacity * sizeof(Ordered));
		if (temp == NULL) {
			fprintf(stderr, "Memory reallocation failed\n");
			return;
		}
		t->orderArr = temp;
		t->capacity = (int)newCapacity;
	}

	FoodNode* node = m->head;
	int i = 0;
	while (node != NULL && i < index) {
		node = node->next;
		i++;
	}

	if (node == NULL) {
		fprintf(stderr, "Invalid food index\n");
	}
	else {
		t->orderArr[t->count].foodIndex = index;
		t->orderArr[t->count].quantity = quantity;
		t->count++;
		t->totalAmount += quantity * node->price;
	}
}
static void clearTable(Table* t) {
	if (t->count > 0) {
		for (int i = 0; i < t->count; i++) {
			t->orderArr[i].foodIndex = -1;
			t->orderArr[i].quantity = 0;
		}
	}
	t->count = 0;
	t->totalAmount = 0;
}
static void freeTable(Table* t) {
	if (t != NULL) {
		if (t->orderArr != NULL) {
			free(t->orderArr);
			t->orderArr = NULL;
		}
		t->count = 0;
		t->capacity = 0;
		t->totalAmount = 0;
	}
}
//내용 저장
static int fileClose(FILE* f) {
	if (fclose(f) != 0) {
		perror("Error closing file");
		return -1;
	}
	return 0;
}
static void saveFoodList(Menu* m) {
	FILE* file = fopen("menu.dat", "wb");
	if (file == NULL) {
		printf("Error opening file for writing.\n");
	}
	else {

		// 메뉴 항목 수 저장
		size_t written = fwrite(&m->count, sizeof(int), 1, file);//파일에 작성한 항목 수
		if (written != 1) {
			printf("Error writing menu count.\n");
			fileClose(file);
		}
		else {
			// 각 메뉴 항목 저장
			FoodNode* node = m->head;
			while (node != NULL) {
				written = fwrite(node->name, sizeof(char), MAX_INPUT, file);
				if (written != MAX_INPUT) {
					printf("Error writing menu item name.\n");
					fileClose(file);
					return;
				}
				written = fwrite(&node->price, sizeof(int), 1, file);
				if (written != 1) {
					printf("Error writing menu item price.\n");
					fileClose(file);
					return;
				}
				node = node->next;
			}
			if (fileClose(file) == 0) {
				printf("Menu saved successfully.\n");
			}
		}
	}
}
static void loadFoodList(Menu* m) {
	FILE* file = fopen("menu.dat", "rb");
	if (file == NULL) {
		printf("No existing menu file found. Starting with an empty menu.\n");
	}
	else {

		int count;
		if (fread(&count, sizeof(int), 1, file) != 1) {
			printf("Error reading menu count.\n");
			fileClose(file);
		}
		else {

			for (int i = 0; i < count; i++) {
				char name[MAX_INPUT + 1];
				int price;

				if (fread(name, sizeof(char), MAX_INPUT, file) != MAX_INPUT ||
					fread(&price, sizeof(int), 1, file) != 1) {
					printf("Error reading menu item %d.\n", i + 1);
					fileClose(file);
					return;
				}

				addToFoodList(m, name, price);
			}

			fileClose(file);
			printf("Menu loaded successfully. %d items loaded.\n", count);
		}
	}
}
static void saveTables(Table* tables) {
	FILE* file = fopen("tables.dat", "wb");
	if (file == NULL) {
		printf("Error opening file for writing.\n");
	}
	else {
		for (int i = 0; i < MAX_TABLES; i++) {
			// 테이블 카운트 저장
			size_t written = fwrite(&tables[i].count, sizeof(int), 1, file);
			if (written != 1) {
				printf("Error writing count for table %d.\n", i);
				fileClose(file);
				return;
			}
			// 테이블 총액 저장
			written = fwrite(&tables[i].totalAmount, sizeof(int), 1, file);
			if (written != 1) {
				printf("Error writing totalAmount for table %d.\n", i);
				fileClose(file);
				return;
			}
			// 주문 내역 저장
			if (tables[i].count > 0) {
				written = fwrite(tables[i].orderArr, sizeof(Ordered), tables[i].count, file);
				if (written != tables[i].count) {
					printf("Error writing orders for table %d. Expected %d, wrote %zu.\n", i, tables[i].count, written);
					fileClose(file);
					return;
				}
			}
		}
		if (fileClose(file) == 0) {
			printf("Tables saved successfully.\n");
		}
	}
}
static void loadTables(Table* tables) {
	FILE* file = fopen("tables.dat", "rb");
	if (file == NULL) {
		printf("No existing tables file found. Starting with empty tables.\n");
	}
	else {

		for (int i = 0; i < MAX_TABLES; i++) {
			if (fread(&tables[i].count, sizeof(int), 1, file) != 1 ||
				fread(&tables[i].totalAmount, sizeof(int), 1, file) != 1) {
				printf("Error reading table %d data.\n", i);
				fileClose(file);
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
				fileClose(file);
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
		fileClose(file);
	}
}
int main(void) {
	Menu menu;
	initFoodList(&menu);
	loadFoodList(&menu);  // 프로그램 시작 시 메뉴 로드
	
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
			printf("\nEnter item information\n");
			printf("Name: ");
			char* n = getString();
			if (n != NULL) {
				printf("Price: ");
				int p = getInt();
				addToFoodList(&menu, n, p);
				free(n); //getString 내에서 할당한 메모리를 해제
			}
			else {
				printf("Memory allocation failed\n");
			}
		}
		break;
		case 2://메뉴 단종
			printFoodList(&menu);
			printf("Enter the number of the item to remove:");
			int i = getInt();
			removeFromFoodList(&menu, i - 1);
			break;
		case 3: {//주문 생성
			int n = getTableNum();
			printFoodList(&menu);
			while (1) {
				printf("Enter the food menu number:(enter 0 to finish):");
				int i = getInt();
				if (i == 0) {
					break;
				}

				printf("Enter quantity:");
				int q = getInt();
				printf("\n");

				addOrder(&table[n - 1], i - 1, q, &menu);
			}
			printf("Order has been created. Total amount %d won\n", table[n - 1].totalAmount);
		}
			  break;
		case 4: {
			int n = getTableNum();
			printf("\n===== Table %d Order =====\n", n);
			printf("Order items:\n");

			Table* t = &table[n - 1];
			for (int i = 0; i < t->count; i++) {
				Ordered orderItem = t->orderArr[i];

				// 메뉴에서 해당 음식 찾기
				FoodNode* node = menu.head;
				for (int j = 0; j < orderItem.foodIndex && node != NULL; j++) {
					node = node->next;
				}

				if (node != NULL) {
					printf("%s X %d\n", node->name, orderItem.quantity);
				}
				else {
					printf("Unknown food item X %d\n", orderItem.quantity);
				}
			}

			printf("Total amount: %d won\n", t->totalAmount);
		}
			  break;
		case 5: {
			int n = getTableNum();
			printf("\nPayment of %d won has been completed. The table has been reset.\n", table[n - 1].totalAmount);
			clearTable(&table[n - 1]);
		}
			  break;
		case 6:
			printf("\nSaving data and exiting the program.\n");
			saveFoodList(&menu);  // 메뉴 저장
			saveTables(table);  // 테이블 정보 저장

			freeFoodList(&menu);
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
	freeFoodList(&menu);
	return 0;
}
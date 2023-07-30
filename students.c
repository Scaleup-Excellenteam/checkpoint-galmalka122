#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<stdint.h>
#include <ctype.h>

#define MAIN_MENU_OPTIONS 8
#define NAME_LEN 15
#define CELLPHONE_LEN 11
#define NUM_OF_COURSES 10
#define NUM_OF_CLASSES 10
#define NUM_OF_LEVELS 12
// first and last name = NAME_LEN * 2, level and class = 4, grades (3 bytes max for each course)
#define BUFFER_SIZE 160
#define DB_FILE_PATH "resources/students.txt"


void clrscr()
{
    system("@cls||clear");
}


typedef struct student{

	char firstName[NAME_LEN];
	char lastName[NAME_LEN];
	char cellPhone[CELLPHONE_LEN];
	
	/* exactly 12 bytes (1 byte for class and level + 10 byte for grades),
	 instead of 48 byte with int */
	uint8_t level;
	uint8_t class;
	uint8_t grades[NUM_OF_COURSES];
	
}student;


typedef struct studentList{

	student* std;
	struct studentList* next;
	
}studentList;

typedef struct course{

	char name[10];
	
}course;



typedef struct class{

	studentList* head;
	
}class;


typedef struct level{
	
	class classes[NUM_OF_CLASSES];
	
}level_t;


typedef struct school{

	level_t levels[NUM_OF_LEVELS];
	
}school;

typedef struct menu_option{

	char* name;
	void (*action)(studentList*);
	
}menu_option;

typedef struct student_action{

	char* name;
	int (*action)(student*, char*);
	
}student_action;

typedef struct edit_action{

	char* name;
	int (*action)(void*, char*);
	
}edit_action;



school s;

int waitForResponse(){

	char confirm;

	while(confirm != 'Y' && confirm != 'N'){
		
		printf("Are you sure you want to proceed? (Y/N): ");
		scanf(" %c", &confirm);
		confirm = toupper(confirm);
	}
	
	return confirm == 'Y';

}

// Initialize all the classes in school to be empty
void initClass(class* cls){

	cls->head = NULL;
	
}



size_t validate_by(char* input, int (*is_valid)(int)){

	size_t i = -1;
	// Check if all input characters are valid
	while(input[++i]){
		if(is_valid(input[i]) == 0){
			return 0;
		}
	
	}
	// Check the size of name
	return i;
	
}

int setName(void* name, char* detail){

	if(validate_by(detail, isalpha) > 0){
		strcpy((char*)name, detail);
		return 1;
	}
	
	return 0;

}

int setPhone(void* phone, char* detail){

	if(validate_by(detail, isdigit) == 10){
		strcpy((char*)phone, detail);
		return 1;
		}
	
		return 0;

}


int setLevel(void* level, char* detail){

	int digit_value;
		
	if(validate_by(detail, isdigit) > 0){
	
		digit_value = atoi(detail);
		
		if(0 < digit_value && digit_value <= 12){
			*((uint8_t*)level) = (uint8_t)digit_value;
			return 1;	
		}
			
		else return 0;
	}
		
	return 0;
}

int setClass(void* class, char* detail){

	int digit_value;
		
	if(validate_by(detail, isdigit) > 0){
	
		digit_value = atoi(detail);
		
		if(0 < digit_value && digit_value <= 10){
			*((uint8_t*)class) = (uint8_t)digit_value;
			return 1;	
		}
	}
		
	return 0;
}

int setGrade(void* grade, char* detail){
	printf("%p", &grade);
	int digit_value;
		
	if(validate_by(detail, isdigit) > 0){
	
		digit_value = atoi(detail);
		
		if(0 <= digit_value && digit_value <= 100){
			*((uint8_t*)grade) = (uint8_t)digit_value;
			return 1;	
		}
	}
		
	return 0;
}



// Initialize student from DB line
student* initStudent(char* details){

	int grade;
	student* std = NULL;
    
	std = (student*) malloc(sizeof(student));
	
	char *detail = strtok(details, " ");
	
	
	// Get first name
	if(setName(std->firstName, detail) == 0){
	
		free(std);
		fprintf(stderr, "Invalid input: %s", detail);
		return NULL;
	
	}
	
	detail = strtok(NULL, " ");
	
	// Get last name
	if(setName(std->lastName, detail) == 0){
	
		free(std);
		fprintf(stderr, "Invalid input: %s", detail);
		return NULL;
	
	}

	detail = strtok(NULL, " ");
	
	// Get cellphone
	if(setPhone(std->cellPhone, detail) == 0){

		free(std);
		fprintf(stderr, "Invalid input: %s", detail);
		return NULL;
	
	}
	
	detail = strtok(NULL, " ");
	
	// Get level
	if(setLevel(&std->level, detail) == 0){
	
		free(std);
		fprintf(stderr, "Invalid input: %s", detail);
		return NULL;
	
	}
	
	detail = strtok(NULL, " ");
	
	// Get class
	if(setClass(&std->class, detail) == 0){
	
		free(std);
		fprintf(stderr, "Invalid input: %s", detail);
		return NULL;
	
	}
	
	// Get all grades
	for(grade = 0; grade < NUM_OF_COURSES; grade++){
	
		detail = strtok(NULL, " ");
		detail[strcspn(detail, "\r\n")] = 0;
		
		if(setGrade(&std->grades[grade], detail) == 0){
		
			free(std);
			fprintf(stderr, "Invalid grade: %sIn line: %s", detail, details);
			return NULL;
			
			}
	}
	
	return std;
};		

// Prints the student's name, phone, level and class
void printDetails(student* std){

	int grade;
	
	clrscr();
	printf("Student details:\n");
	printf("Name: %s %s\nCellphone: %s\nLevel: %hhu\nClass: %hhu\n", std->firstName, std->lastName, std->cellPhone, std->level, std->class);
	
	for(grade = 0; grade < NUM_OF_COURSES; grade++){
	
		printf("%hhu\n", std->grades[grade]);
	}
	
	printf("Press any key to continue...\n");
    getchar();
    
}

double getAverage(student* std){

	size_t i = -1;
	int sum = 0;
	
	while(++i < 10){
	
		sum += std->grades[i];
	
	}
	
	return sum / 10;

}

void assignStudentToClass(student* newStd){
	
	double average;
	studentList *new_node, *prev;
	studentList* temp = s.levels[newStd->level - 1].classes[newStd->class - 1].head;
	average = getAverage(newStd);
	
	// Initialize new node
	new_node = (studentList*) malloc(sizeof(studentList));

	// Insert to the start of the list
	if(temp == NULL){
	
		temp = new_node;
		temp->std = newStd;
		return;
		
	}
	
	while(temp != NULL && getAverage(temp->std) >= average){
	
		prev = temp;
		temp = temp->next;
	
	}

	new_node->std = newStd;
	new_node->next = temp;
	prev->next = new_node;
	
}

void printLevelAverage(int level){

	int class;
	studentList *node;
	
	for(class = 0; class < 10; class++){
		node = s.levels[level].classes[class].head;
		
		printDetails(node->std);
		while(node != NULL){
			printf("%f\n", getAverage(node->std));
			node = node->next;		
		}
	
	}

}



// Initialize the school from the DB file
void init(){

	FILE* file;
	
	char buf[BUFFER_SIZE];
	
	student* newStd = NULL;
	
	// Open the file and check for failure
	file = fopen(DB_FILE_PATH, "r");
	if (file == NULL) {
    	perror("Cannot open DB file\n");
    	exit(1);
    }
	
	// Iterate each line, extract students details and insert into class
	while(fgets(buf, BUFFER_SIZE, file) != NULL){
		
		newStd = initStudent(buf);
		
		if(newStd != NULL)
			assignStudentToClass(newStd);
			
		else return;
		
	}
	
	fclose(file);
}


void actionForAllClasses(void (*action)(class*)){

	int lvl;
	int cls;
	
	// Iterate through all classes and delete all students
	for(lvl = 0; lvl < NUM_OF_LEVELS; lvl++){
	
		for(cls = 0; cls < NUM_OF_CLASSES; cls++){
		
			action(&s.levels[lvl].classes[cls]);
		
		}
		
	}


}

void destroyStudents(class* cls){

	studentList* next = NULL;
	studentList* cur = cls->head;
	
	while(cur != NULL){
				
		// Delete student
		free(cur->std);
		next = cur->next;
		
		// Delete node
		free(cur);						
		cur = next;
	}

}


int compareByName(student* std, char* str){
	
	// Concatenate first and last name
	char full_name[2 * NAME_LEN + 1];
	sprintf(full_name, "%s %s", std->firstName, std->lastName);
	
	return strcmp(full_name, str);
	
}


int compareByPhone(student* std, char* str){
	
	
	return strcmp(std->cellPhone, str);
	
}


studentList* findStudentBy(int (*cmp)(student*, char*), char* str){
	
	int lvl;
	int cls;
	
	studentList* prev = NULL;
	studentList* temp = NULL;
	
	
	// Iterate through all classes and compare phone
	for(lvl = 0; lvl <= NUM_OF_LEVELS; lvl++){
	
		for(cls = 0; cls < NUM_OF_CLASSES; cls++){
			
			// The start of the class list
			prev = s.levels[lvl].classes[cls].head;
			temp = prev->next;
			// Compare the name with each student
			
			if (temp != NULL && cmp(temp->std, str) == 0)
				return temp;
			
			while(temp != NULL){
			 	if(cmp(temp->std, str) == 0)
			 		return prev;
			 	
				prev = temp;
	    		temp = temp->next;
				
			}
		}
	}
	
	return NULL;
}

// Creates a new student and assign him to his class
void createStudent(){

	char buf[BUFFER_SIZE];
	
	student* newStd = NULL;
	
	printf("Enter the student's details - <first name> <last name> <cellphone> <level> <class> <grades>\n");
	
	fgets(buf, BUFFER_SIZE, stdin);
	
	newStd = initStudent(buf);
	assignStudentToClass(newStd);
	
	printDetails(newStd);	
}


uint8_t getChoice(char* title, menu_option* options, size_t num_of_options){

	
	size_t choice;
	int input = -1;
	
	while(1){
		
		clrscr();
		
		if(input == -2) printf("Invalid selection\n");
		
		// Print the options
		printf("%s\n\nPlease select an action:\n", title);
		printf("--------------------------------\n");
		for(choice = 0; choice < num_of_options; choice++){
		
			printf("%ld: %s\n", choice + 1, options[choice].name);
			
		}
		
		printf("%ld: Quit\n\nEnter your choice: ", choice + 1);

		
		scanf("%d%*c", &input);
		
		if(input > 0 && input <= num_of_options + 1){
			return input - 1;
			
		}
		
		
		input = -2;
	}
}


student* getStudent(studentList* node){

	class cls = s.levels[node->std->level].classes[node->std->class];

	if(cls.head == node)
			return node->std;
			
		else
			return node->next->std;

}


void findDetails(studentList* node){

	printDetails(getStudent(node));

}

int editDetail(void* value, int(*action)(void* oldDetail, char* newDetail)){

	char input[2 * NAME_LEN];
	printf("\nNew value: ");
	
	fgets(input, 2 * NAME_LEN, stdin);
	input[strcspn(input, "\r\n")] = 0;
	
	return action(value, input);

}

int editGrade(void* grades, int(*action)(void* oldDetail, char* newDetail)){

	char input[2 * NAME_LEN];
	int course_num;
	printf("Grades\n");
	for(course_num = 0; course_num < 10; course_num++)
		printf("%d: %hhu\n", course_num + 1, *((uint8_t*)grades + (course_num * sizeof(uint8_t))));
	printf("Select course to set the new value: ");
	scanf("%d%*c", &course_num);
	
	printf("\nNew Grade: ");
	
	fgets(input, 2 * NAME_LEN, stdin);
	input[strcspn(input, "\r\n")] = 0;
	
	return setGrade((uint8_t*)grades + (course_num - 1) * sizeof(uint8_t), input);

}

edit_action edit_options[6] = {{"Edit First Name", setName},
						   	   {"Edit Last Name", setName},
						       {"Edit Cellphone", setPhone},
						       {"Edit Level", setLevel},
						       {"Edit Class", setClass},
						       {"Edit Grade", editGrade}};
							   
void editDetails(studentList* node){

	uint8_t choice;
	int res;
	student* std = getStudent(node);
	
	void* attrs[6] = {std->firstName, std->lastName, std->cellPhone, &std->level, &std->class, &std->grades };
	
	choice = getChoice("Edit Student Details", edit_options, 6);
	
	if(choice == 6) return;
	
	if(choice == 5) res = editGrade(attrs[choice], edit_options[choice].action);
	
	else
		res = editDetail(attrs[choice], edit_options[choice].action);
	
	if(res == 0)
	
		fprintf(stderr, "Invalid input");
	
	
	else 
		printf("Operation succeeded\n");
		
	
}



void removeStudent(studentList* node){
	
	class cls = s.levels[node->std->level].classes[node->std->class];
	if(waitForResponse() == 1){
	
		if(cls.head == node)
			cls.head = node->next;
			
		else
			node->next = node->next->next;
		
		free(node->next);
	}
}



menu_option main_menu_options[MAIN_MENU_OPTIONS] = {{"Add Student", createStudent},
													{"Delete Student",removeStudent},
													{"Edit Student",editDetails},
													{"Print Student",findDetails},
													{"Top Ten Students",printLevelAverage},
													{"Retiring Candidates",removeStudent},
													{"Average By Level",removeStudent},
													{"Export DB",removeStudent}};
													
student_action compare_options[2] = {{"Find student by full name", compareByName},
								     {"Find student by cellphone", compareByPhone}};

void actionOnStudent(menu_option option){

	uint8_t choice;
	char input[2 * NAME_LEN];
	studentList* student_node;

	choice = getChoice(option.name, compare_options, 2);
	
	if(choice == 2) return;
			
	printf("Input: ");
	
	fgets(input, 2 * NAME_LEN, stdin);
	input[strcspn(input, "\r\n")] = 0;
	
	printf("Searching...\n");
	
	student_node = findStudentBy(compare_options[choice].action, input);
	if(student_node != NULL)
		option.action(student_node);
	
	else 
		printf("The requested student does not exist\n");

	
}

void menu(){
	
	uint8_t choice;
	int c;
	
	while(choice != MAIN_MENU_OPTIONS){
		choice = getChoice("Menu", main_menu_options, MAIN_MENU_OPTIONS);
		if(0 <= choice && choice < 4){
			
			actionOnStudent(main_menu_options[choice]);
		
		}
		
		else if (choice == 4){
		
			scanf("%d%*c", &c);
			printLevelAverage(c);
			waitForResponse();
		}
		
	}
}

int main(){
	clrscr();
	actionForAllClasses(initClass);
	init();
	menu();
	actionForAllClasses(destroyStudents);
	return 0;
	}

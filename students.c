#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 30
#define CELLPHONE_MAX_LEN 11
#define NUM_OF_COURSES 10
#define NUM_OF_CLASSES 10
#define NUM_OF_LEVELS 12
#define DB_FILE_PATH "resources/students.txt"


typedef struct student{

	char firstName[MAX_NAME_LEN];
	char lastName[MAX_NAME_LEN];
	char cellPhone[CELLPHONE_MAX_LEN];
	unsigned char level;
	unsigned char class;
	unsigned char grades[NUM_OF_COURSES];
	
}student_t;


typedef struct studentList{

	student_t* std;
	struct studentList* next;
	
}studentList_t;


typedef struct class{

	studentList_t* head;
	
}class_t;


typedef struct school{

	class_t classes[NUM_OF_LEVELS][NUM_OF_CLASSES];
	
}school_t;


school_t s;

// Initialize all the classes in school to be empty
void initSchool(){

	int lvl;
	int cls;
	
	for(lvl = 0; lvl < NUM_OF_LEVELS; lvl++){
	
		for(cls = 0; cls < NUM_OF_CLASSES; cls++){
		
			s.classes[lvl][cls].head = NULL;
		}
	}
}

// Insert a student into his class
void push(studentList_t** head, student_t* std) {

	// Initialize new node
    studentList_t * new_node;
    new_node = malloc(sizeof(studentList_t));

	// Insert to the start of the list
    new_node->std = std;
    new_node->next = *head;
    *head = new_node;
}	

// Initialize student from DB line
void initStudent(char* details, student_t* std){

	int grade;
	
	// Get the first name from the string
	char *detail = strtok(details, " ");
	strcpy(std->firstName, detail);
	
	// Get the last name
	detail = strtok(NULL, " ");
	strcpy(std->lastName, detail);
	
	// Get the cell phone
	detail = strtok(NULL, " ");
	strcpy(std->cellPhone, detail);
	
	// Get the level
	detail = strtok(NULL, " ");
	std->level = atoi(detail);
	
	// Get the class
	detail = strtok(NULL, " ");
	std->class = atoi(detail);
	
	// Get all grades
	for(grade = 0; grade < NUM_OF_COURSES; grade++){
		detail = strtok(NULL, " ");
		std->grades[grade] = atoi(detail);
	}
};		

// Prints the student's name, phone, level and class
void printStd(student_t* std){

	printf("%s %s %s %hhu %hhu", std->firstName, std->lastName, std->cellPhone, std->level, std->class);
}


// Initialize the school from the DB file
void init(){

	FILE* file;
	int grade;
	char buf[100];
	student_t* newStd = NULL;
	
	file = fopen(DB_FILE_PATH, "r");
	
	// Iterate each line, extract students details and insert into class
	while(fgets(buf, 100, file) != NULL){
	
		newStd = malloc(sizeof(student_t));	
		initStudent(buf, newStd);
		push(&s.classes[newStd->level][newStd->class].head, newStd);
		
	}
	
	fclose(file);
}

void deallocateSchool(){

	int lvl;
	int cls;

	studentList_t* cur = NULL;
	studentList_t* next = NULL;
	
	// Iterate through all classes and delete all students
	for(lvl = 0; lvl < NUM_OF_LEVELS; lvl++){
	
		for(cls = 0; cls < NUM_OF_CLASSES; cls++){
			
			// The start of the class list
			cur = s.classes[lvl][cls].head;
			
			while(cur != NULL){
				
				// Delete student
				free(cur->std);
				next = cur->next;
				
				// Delete node
				free(cur);						
				cur = next;
			}
			
		}
	}
	
	
}

int main(){
	initSchool();	
	init();
	deallocateSchool();
	return 0;
	}

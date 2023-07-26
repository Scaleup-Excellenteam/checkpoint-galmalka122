#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Student{

	// Student's first name
	char firstName[30];
	
	// Student's first name
	char lastName[30];
	
	// Student's phone
	char cellPhone[11];
	
	// Student's level
	unsigned char level;
	
	// Student's class
	unsigned char class;
	
	// Store the grade in char to save space (using %hhu)
	unsigned char grades[10];

};

// Student node in a Students Linked list of all students in a class
struct Students{
	
	struct Student* student;
	struct Students* next;
	
};

// Linked list of all Students who registered to the class
struct Class{
	
	struct Students* head;
};


struct School{

	struct Class classes[12][10];
	
};


// Initialize student from DB line
struct Student* initStudent(char* details){
	int i = 0;
	int grade;
	struct Student* std = (struct Student *) malloc(sizeof(struct Student));
	
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
	for(i;i<10;i++){
		detail = strtok(NULL, " ");
		std->grades[i] = atoi(detail);
	}
	
	return std;
};

static struct School s;

// Initialize all classes to be empty
void initSchool(){
	int lvl = 0;
	int cls = 0;
	for(lvl; lvl < 12; lvl++){
		for(cls; cls < 10; cls++){
			s.classes[lvl][cls].head = NULL;
		}
	}
}

// Insert student into the classes list
void assignToClass(struct Student* std){

	// Retrive the correct class by the student's level and class
	struct Class cls = s.classes[std->level][std->class];
	
	
	struct Students* cur = cls.head;
	if(cur == NULL){
		cur = (struct Students*)malloc(sizeof(struct Students));
		cur->student = std;
		cur->next = NULL;
	}
	else{
		while(cur->next != NULL){
			cur = cur->next;
		}
		cur->next = (struct Students*)malloc(sizeof(struct Students));
		cur->next->student = std;
		cur->next->next = NULL;
	}
	
}

// Initialize the School struct from the DB file
void init(){
	FILE *fptr;
	int i = 0;
	char myString[100];
	struct Student* std;
	fptr = fopen("./resources/students.txt", "r");
	
	// Read all students data from the file
	while(fgets(myString, 100, fptr) != NULL){
 		std = initStudent(myString);
 		assignToClass(std);
 		
 	}
 	
	// Close the file
	fclose(fptr); 
	
	
}

int main() {

	init();
    	return 0;
}


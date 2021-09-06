#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define SIZE 50

typedef struct{
  char fname[SIZE];
  char lname[SIZE];
  char mi;
}nametype;

typedef struct{
  nametype name;
  unsigned int id;
  float rate;
}employeetype;

typedef struct node{
  employeetype employee;
  struct node *next;
}node, *nPtr;

void addEmployee(nPtr *list);
void insertLast(nPtr *list, nPtr new);
void insertSorted(nPtr *list, nPtr new);
employeetype getInfo();
int deleteEmployee(nPtr *list, unsigned int id);
int editEmployee(nPtr list, unsigned int id);
void displayList(nPtr list);
void saveToFile(char *filename, nPtr list);
void readFromFile(char *filename, nPtr *list);

int main()
{
  nPtr list = NULL;
  char user, filename[SIZE] = "employeelist.dat";
  unsigned int id;

  do{
    readFromFile(filename, &list);
    printf("\n----- MENU -----\na. Add Entry\nb. Delete Entry\nc. Edit Entry\nd. Display List\n0. Exit\n\nInput: ");
    scanf(" %c", &user);fflush(stdin);

    switch(tolower(user)){
        case 'a':
            addEmployee(&list);
            break;
        case 'b':
            printf("Input ID of employee to DELETE: ");
            scanf("%d", &id);
            deleteEmployee(&list, id);
            break;
        case 'c':
            printf("Input ID of employee to EDIT: ");
            scanf("%d", &id);
            editEmployee(list, id);
            break;
        case 'd':
            displayList(list);
            break;
        case '0':
            break;
        default:
            printf("\nInvalid Input!\n");
    }
    saveToFile(filename, list);
    list = NULL;
  }while(user != '0');

  return 0;
}

void addEmployee(nPtr *list)
{
    nPtr new = (nPtr)malloc(sizeof(node));
    new->employee = getInfo();
    insertSorted(list, new);
    printf("\nSuccessfully inserted new employee.");
}

void insertLast(nPtr *list, nPtr new)
{
    new->next = NULL;
    if(*list == NULL){
        *list = new;
    }
    else{
        nPtr temp;
        for(temp = *list; temp->next != NULL; temp = temp->next){}
        temp->next = new;
    }
}

void insertSorted(nPtr *list, nPtr new)
{
  nPtr temp;
    employeetype hold;
    if(*list == NULL){
      *list = new;
    }
    else{
      for(temp = *list; temp->next != NULL && temp->employee.id < new->employee.id; temp = temp->next){}

      if(temp->next == NULL){
        temp->next = new;
        if(new->employee.id < temp->employee.id){
          hold = temp->employee;
          temp->employee = new->employee;
          new->employee = hold;
        }
      }
      else{
        new->next = temp->next;
        temp->next = new;
        hold = temp->employee;
        temp->employee = new->employee;
        new->employee = hold;
      }
    }
}

employeetype getInfo()
{
    employeetype new;

    printf("\nEnter ID: ");
    scanf("%u", &new.id);fflush(stdin);
    printf("Enter first name: ");
    scanf("%s", &new.name.fname);fflush(stdin);
    printf("Enter last name: ");
    scanf("%s", &new.name.lname);fflush(stdin);
    printf("Enter middle initial: ");
    scanf(" %c", &new.name.mi);fflush(stdin);
    printf("Enter pay rate: ");
    scanf("%f", &new.rate);

    return new;
}

int deleteEmployee(nPtr *list, unsigned int id)
{

      int retVal = 0;
      unsigned int x;
      for(x = (*list)->employee.id; x < id && (*list) != NULL; list = &(*list)->next, x = (*list)->employee.id){}
      if(x == id){
          *list = (*list)->next;
          retVal = 1;
          printf("\nSuccessfully delete employee #%u", id);
      }
      else{
          printf("\nFailed to find employee #%u", id);
      }

      return retVal;
}

int editEmployee(nPtr list, unsigned int id)
{
    int retVal = 0, x;
    for(x = list->employee.id; x < id && list != NULL; list = list->next, x = list->employee.id){}
    if(x == id){
      list->employee = getInfo();
      retVal = 1;
      printf("\nSuccessfully edited employee #%u", id);
    }
    else{
      printf("\nFailed to find employee #%u", id);
    }

    return retVal;
}

void displayList(nPtr list)
{
    printf("\n\n%-10s - %-20s - %-5s - %-15s - %8s", "ID", "First Name", "MI", "Last Name", "Pay Rate");
  	printf("\n----------------------------------------------------------------------------\n\n");
  	for( ; list != NULL; list = list->next){
  		printf("%-10u - %-20s - %-5c - %-15s - %.2f\n", list->employee.id, list->employee.name.fname, list->employee.name.mi, list->employee.name.lname, list->employee.rate);
  	}
  	printf("----------------------------------------------------------------------------\n");
}

void saveToFile(char *filename, nPtr list)
{
    FILE *fp;
    fp = fopen(filename, "w");

    if(fp != NULL){
        for( ; list != NULL; list = list->next){
            fwrite(&list->employee, sizeof(employeetype), 1, fp);
        }

        fclose(fp);
    }
    else{
        printf("\nError in saving data!");
    }
}

void readFromFile(char *filename, nPtr *list)
{
    FILE *fp;
    nPtr temp;
    fp = fopen(filename, "r");
    if(fp != NULL){
        temp = (nPtr)malloc(sizeof(node));
        while(fread(&temp->employee, sizeof(employeetype), 1, fp)){
            insertLast(list, temp);
            temp = (nPtr)malloc(sizeof(node));
        }

        fclose(fp);
    }
    else{
        printf("\nFile may not exist / Error in opening file!");
    }
}

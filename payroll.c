#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

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

typedef struct{
	employeetype employee;
	char time[SIZE];
}timeStamp;

void addEmployee(nPtr *list);
void insertLast(nPtr *list, nPtr new);
void insertSorted(nPtr *list, nPtr new);
employeetype getInfo();

int deleteEmployee(nPtr *list, unsigned int id);
int editEmployee(nPtr list, unsigned int id);

void displayList(nPtr list);
void displayEmployee(employeetype e);
int findEmployee(nPtr list, unsigned int id, employeetype *e); //just scrolls through the list and returns 1 if the employee is found, otherwise returns 0
void calculatePay(nPtr list);

void saveToFile(char *filename, nPtr list);
void readFromFile(char *filename, nPtr *list);

timeStamp displayPaySlip(unsigned int id, float hoursWork, float overTime, nPtr list); 

int main()
{
  nPtr list = NULL;
  char user, filename[SIZE] = "employeelist.dat";
  unsigned int id;
  float hrsWork, overTime;
  do{
    readFromFile(filename, &list);
    printf("\n----- MENU -----\na. Add Entry\nb. Delete Entry\nc. Edit Entry\nd. Display List\ne. Calculate Pay\nf. Generate PaySlip \n0. Exit\n\nInput: ");
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
        case 'e':
        	calculatePay(list);
        	break;
        case 'f':
        	printf("Input ID of employee to generate PaySlip:\n");
            scanf("%d", &id);
            printf("Input hours of work:\n");
            scanf("%f", &hrsWork);
            printf("Input hours of OverTime(Optional-- Enter 0(zero) for empty value)):\n");
            scanf("%f", &overTime);
        	displayPaySlip(id,hrsWork,overTime,list);
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
    printf("\n\n%-10s - %-20s   %-5s   %-15s - %8s", "ID", "First Name", "MI", "Last Name", "Pay Rate");
  	printf("\n----------------------------------------------------------------------------\n\n");
  	for( ; list != NULL; list = list->next){
  		printf("%-10u - %-20s   %-5c   %-15s - %.2f\n", list->employee.id, list->employee.name.fname, list->employee.name.mi, list->employee.name.lname, list->employee.rate);
  	}
  	printf("----------------------------------------------------------------------------\n");
}

void displayEmployee(employeetype e)
{
	printf("\n\nID: %u", e.id);
	printf("\nFull Name: %s ", e.name.fname);
	if(e.name.mi){
		printf("%c. ", e.name.mi);
	}
	printf("%s", e.name.lname);
	printf("\nRate: %.2f", e.rate);
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

int findEmployee(nPtr list, unsigned int id, employeetype *e)
{
	int retval = 0;
	while(list != NULL && list->employee.id != id){
		list = list->next;
	}
	if(list != NULL){
		*e = list->employee;
		retval = 1;
	}
	
	return retval;
}
void calculatePay(nPtr list)
{
	unsigned int id;
	float otpay, normalpay, hrs, ot;
	employeetype e;
	
	printf("\nPlease enter the ID number of the employee: ");
	scanf("%u", &id);
	
	if(findEmployee(list, id, &e)){//findEmployee will return 1 if the employee is found, otherwise returns 0
		displayEmployee(e);
		printf("\n\nPlease enter # of hours worked(not including OT): ");
		scanf("%f", &hrs);
		printf("Please enter # of hours worked in OT: ");
		scanf("%f", &ot);
		
		normalpay = e.rate * hrs;
		otpay = e.rate * 1.5 * ot;
		
		printf("\nNormal pay: Php %.2f", normalpay);
		printf("\nOvertime pay(150%): Php %.2f", otpay);
		printf("\nTotal pay: Php%.2f\n", normalpay+otpay);
	}
	else{
		printf("\nError! Employee with ID %u not found!", id);
	}
	
}

timeStamp displayPaySlip(unsigned int id,float hoursWork,float overTime,nPtr list)
{
	time_t rawtime;
    struct tm *info;
   	char buffer[80];
   	char date[80];
   	employeetype e;
	float normalpay;
   	time( &rawtime );
	char stringValue[80];
	timeStamp retVal;
	float otpay = 0;
	
   	info = localtime( &rawtime );
	//transform the structure time details to string
   	strftime(buffer,80,"%x - %I:%M%p", info);
   	strftime(date,80,"%B %d, %Y", info);
	
	if(findEmployee(list, id, &e)){
		
		normalpay = e.rate * hoursWork;
		if(overTime != 0){
			otpay = e.rate * 1.5 * overTime;
		} 		
 		
		printf("\n");
   		printf("----------------------------------------\n");
   		printf("%23s\n","COMPANY NAME");
   		printf("%24s\n\n","sample address");
   		printf("%5s %d %15s %5s \n","Employee ID: ",e.id, "Date: ",date);
   		printf("%5s %s %s, %c \n\n","Name: ",e.name.lname,e.name.fname,e.name.mi);
   		printf("%5s %.2f \n","Number of Hours Work: ", hoursWork);
   		printf("%5s %.2f \n","Total OT: ", overTime);
   		printf("%5s %.2f \n","Employee Rate: ",e.rate);
   		printf("%5s %.2f\n","Normal Pay: ",normalpay);
   		if(otpay != 0 ){
   			printf("%5s %.2f\n","Overtime Pay: ",otpay);
		}
		printf("%5s %.2f\n\n","Net Pay: ",normalpay+otpay);
   		printf("%5s %15s \n\n","Prepared By: ", "Approved By: ");
   		
   		retVal.employee = e;
   		strcpy(retVal.time,buffer);
	} else {
		printf("Employee Not Found! \n");
	}
	
	return retVal;
}

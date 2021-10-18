#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "alt.h"

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
	float grosspay;
	char time[SIZE];
	char date[SIZE];
}timeStamp;

typedef struct timeNode{
	timeStamp timeStamp;
	struct timeNode *next; 
}timeNode, *tnPtr;

void addEmployee(nPtr *list);
void insertLast(nPtr *list, nPtr new);
void insertSorted(nPtr *list, nPtr new);
employeetype getInfo();

int deleteEmployee(nPtr *list, unsigned int id);
int editEmployee(nPtr list, unsigned int id);

void displayList(nPtr list);
void displayEmployee(employeetype e);
int findEmployee(nPtr list, unsigned int id, employeetype *e); //just scrolls through the list and returns 1 if the employee is found, otherwise returns 0

void saveToFile(char *filename, nPtr list);
void readFromFile(char *filename, nPtr *list);

timeStamp displayPaySlip(unsigned int id, float hoursWork, float overTime, nPtr list, tnPtr tnList);
timeStamp displayPaySlipWithTax(unsigned int id, float hoursWork, float overtimeHours, nPtr list);
float calculateSSSContribution(float grosspay);
float calculatePagIbigContribution(float grosspay);
float calculatePhilHealthContribution(float grosspay);

void addPayslipToList(timeStamp timeStamp, tnPtr *tnList);
void appendPayslipToFile(char *payslipFileName, timeStamp timeStamp);
void readFromPayslipFile(char *payslipFileName, tnPtr *tnList);
void showPayslipHistoryData(tnPtr tnList);
int main()
{
  nPtr list = NULL;
  tnPtr tnList = NULL;
  char user, filename[SIZE] = "employeelist.dat", payslipFileName[SIZE] = "paysliplist.dat";
  unsigned int id;
  float hrsWork, overTime;
  do{
    readFromFile(filename, &list);
    readFromPayslipFile(payslipFileName, &tnList);
    setcolor(GRAY,BLACK);
    printf("\n----- MENU -----\n");
    setcolor(WHITE,BLACK);
    printf("a. Add Entry\nb. Delete Entry\nc. Edit Entry\nd. Display List\ne. [beta]Generate Payslip with Tax \n0. Exit\n\nInput: ");
    scanf(" %c", &user);fflush(stdin);
    
    clrscr();
	
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
        case 'f': //not in menu anymore. just keeping it here until tax is final
        	printf("Input ID of employee to generate PaySlip:\n");
          	scanf("%d", &id);
          	printf("Input hours of work:\n");
          	scanf("%f", &hrsWork);
          	printf("Input hours of OverTime(Optional-- Enter 0(zero) for empty value)):\n");
          	scanf("%f", &overTime);
        	displayPaySlip(id,hrsWork,overTime,list,tnList);
        	break;
        case 'e':
        	printf("Input ID of employee to generate PaySlip:\n");
          	scanf("%d", &id);
          	printf("Input hours of work:\n");
          	scanf("%f", &hrsWork);
          	printf("Input hours of overtime work:\n");
          	scanf("%f", &overTime);
        	displayPaySlipWithTax(id,hrsWork,overTime,list);
        	break;
		case 'h':
			showPayslipHistoryData(tnList);
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
    
    employeetype e;
    
    if(findEmployee(*list, new->employee.id, &e)){
    	printf("\nEmployee #%d already exists!\n", e.id);
	} else {
		insertSorted(list, new);
    	printf("\nSuccessfully inserted new employee.\n");
	}
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
          printf("\nSuccessfully deleted employee #%u!\n", id);
      }
      else{
          printf("\nFailed to find employee #%u!\n", id);
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
	setcolor(GRAY,BLACK);
    printf("\n\n%-10s - %-20s   %-5s   %-15s - %8s", "ID", "First Name", "MI", "Last Name", "Pay Rate");
  	printf("\n----------------------------------------------------------------------------\n\n");
  	setcolor(WHITE,BLACK);
  	for( ; list != NULL; list = list->next){
  		printf("%-10u - %-20s   %-5c   %-15s - %.2f\n", list->employee.id, list->employee.name.fname, list->employee.name.mi, list->employee.name.lname, list->employee.rate);
  	}
  	setcolor(GRAY,BLACK);
  	printf("----------------------------------------------------------------------------\n");
  	setcolor(WHITE,BLACK);
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

void readFromPayslipFile(char *payslipFileName, tnPtr *tnList)
{
    FILE *fp;
    tnPtr temp, *trav;
	int count=0;
    fp = fopen(payslipFileName, "r");
    if(fp != NULL){
        temp = (tnPtr)malloc(sizeof(timeNode));
        while(fread(&temp->timeStamp, sizeof(timeStamp), 1, fp)){
            /* insert to the tn list */
			temp->next = NULL;
          	
            if(*tnList == NULL){
            	*tnList = temp;

			 }
			else{
			 	for(trav = tnList; (*trav) != NULL; trav = &(*trav)->next){}
				 (*trav) = temp;
			 }
            
            temp = (tnPtr)malloc(sizeof(timeNode));
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

timeStamp displayPaySlip(unsigned int id,float hoursWork,float overTime,nPtr list, tnPtr tnList)
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
		retVal.grosspay = normalpay+otpay;
   		strcpy(retVal.time,buffer);
		strcpy(retVal.date, date);
   		addPayslipToList(retVal, &tnList);

		appendPayslipToFile("paysliplist.dat", retVal);
   		
	} else {
		printf("Employee Not Found! \n");
	}
	return retVal;
}

void addPayslipToList(timeStamp newtimeStamp, tnPtr *tnList){
	tnPtr *trav;
	tnPtr new = (tnPtr)malloc(sizeof(timeNode));
	new->timeStamp = newtimeStamp;
	
	if(*tnList == NULL){
		*tnList = new;
	}else{
		for(trav = tnList; (*trav)->next != NULL; trav = &(*trav)->next){}
		(*trav)->next = new;
	}
}

void showPayslipHistoryData(tnPtr tnList){
	
	tnPtr trav;
   	printf("\n----------------------------------------\n");
   	printf("%30s\n","Payslip History");
	printf("----------------------------------------\n");
	for(trav = tnList; trav != NULL; trav = trav->next){
		
		printf("Date:\t %s \t\n", trav->timeStamp.date);
		printf("     \t %s \t\n", trav->timeStamp.time);

		printf("\n\n %s %c %s \t ID: %d \n", trav->timeStamp.employee.name.fname, trav->timeStamp.employee.name.mi, trav->timeStamp.employee.name.lname,trav->timeStamp.employee.id);
		printf("Total Pay: %.2f\n\n", trav->timeStamp.grosspay);
		printf("----------------------------------------\n");
	}
}

void savePayslipListToFile(char *payslipFileName, tnPtr tnList){
	FILE *fp;
    fp = fopen(payslipFileName, "w");

    if(fp != NULL){
        for( ; tnList != NULL; tnList = tnList->next){
            fwrite(&tnList->timeStamp, sizeof(timeStamp), 1, fp);
        }

        fclose(fp);
    }
    else{
        printf("\nError in saving data!");
    }
}

void appendPayslipToFile(char *payslipFileName, timeStamp timeStamp){
	FILE *fp;
    fp = fopen(payslipFileName, "a");

    if(fp != NULL){
		fwrite(&timeStamp, 1, sizeof(timeStamp), fp);
		printf("\n\nPayslip saved on data\n\n");
        fclose(fp);
    }
    else{
        printf("\nError in saving data!");
    }
    
}

float calculateSSSContribution(float grosspay)
{
	//=====base values=====
	int credit = 3000;	//minimum credit value
	float minVal = 3250;	//minimum monthly salary
	float maxVal;	//to be set if the grosspay is greater than the minimum value
	float rangeDifference = 499.99;	//the difference between the ranges, this is added to the maximum value when the ranges increase
	float SSSContribution = .045;	//total SSS Contribution percentage for employees effective January 2021
	//=====base values=====
	
	//=====max values=====
	int maxRangeValue = 24750; //max range value for monthly salary
	int maxCredit = 20000; //max credit value 
	//=====max values=====
	
	int creditIncrement = 500;	//credit increments on every loop until the correct compensation range is found
	float retVal;	//to be assigned the total SSS Contribution which is deducted from the monthly salary
	
	if( grosspay < maxRangeValue ){
		
		if(grosspay <= minVal){
			retVal = credit * SSSContribution;	
		} else {
			maxVal = minVal;
			maxVal += rangeDifference;
			
			while( !(grosspay >= minVal && grosspay <= maxVal) && minVal != maxRangeValue){
				if(credit == maxCredit){
					credit = maxCredit;
				} else {
					credit += creditIncrement;
				}	
//				maxVal = minVal = ceil(maxVal);
				maxVal += rangeDifference;
			}
			
			if(minVal == maxRangeValue){
				retVal = credit * SSSContribution;
			}

      //the increment always stops a level lower than the proper compensation range, will fix 
			retVal = (credit + creditIncrement) * SSSContribution;			
		}
		
	} else {
		retVal = maxCredit * SSSContribution;
	}
	return retVal;
}

float calculatePagIbigContribution(float grosspay)
{
	//if monthly basic salary is between 1,000.00 and 1,500.00
	float lowerContribution = 0.01;
	
	//if monthly basic salary is higher than 1,500.00
	float higherContribution = 0.02;
	
	float retVal = 0.00;
	
	if(grosspay >= 1000 && grosspay <= 1500){
		retVal = grosspay * lowerContribution;
	} else {
		retVal = grosspay * higherContribution;
	}
	
	return retVal;
}

float calculatePhilHealthContribution(float grosspay)
{
	//based on the PhilHealth Premium Rate for 2020 (PhilHealth is still following the 2020 rate)
	float low_monthly_salary = 10000;
	float mid_monthly_salary_minimum = 10000.01;
	float mid_monthly_salary_maximum = 69999.99;
	float high_monthly_salary = 70000.00;
	float monthly_premium_rate = 0.03;
	
	//employee share
	int employee_share = 2;
	
	//fixed rates
	int low_fixed_rate = 175;
	int high_fixed_rate = 1225;
	
	float retVal = 0;
	
	if(grosspay <= low_monthly_salary){
		retVal = low_fixed_rate;
	} else if(grosspay >= mid_monthly_salary_minimum && grosspay <= mid_monthly_salary_maximum){
		retVal = (grosspay * monthly_premium_rate) / employee_share;
	} else {
		retVal = high_fixed_rate;
	}
	return retVal;
}

timeStamp displayPaySlipWithTax(unsigned int id, float hoursWork, float overtimeHours, nPtr list)
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
	
	int avgWorkingDays = 21;
	float grosspay;
	float otpay;
	float netpay;
	float totalEmployeeContributions = 0;
	
	info = localtime( &rawtime );
	//transform the structure time details to string
	strftime(buffer,80,"%x - %I:%M%p", info);
	strftime(date,80,"%B %d, %Y", info);
	
	if(findEmployee(list, id, &e)){
		
		normalpay = e.rate * hoursWork;
		
		otpay = (e.rate * 1.5) * overtimeHours;
		grosspay = (normalpay * avgWorkingDays) + otpay;
		totalEmployeeContributions += calculateSSSContribution(grosspay);
		totalEmployeeContributions += calculatePagIbigContribution(grosspay);
		totalEmployeeContributions += calculatePhilHealthContribution(grosspay);
		netpay = grosspay - totalEmployeeContributions;
		
 		sprintf(stringValue, "%g", normalpay);	
 		
	  	printf("\n\n");
   		printf("------------------------------------------------------------\n");
   		printf("%23s\n","COMPANY NAME");
   		printf("%24s\n\n","sample address");
   		printf("%5s %d %15s %5s \n","Employee ID: ",e.id, "Date: ",date);
   		printf("%5s %s %s, %c \n\n","Name: ",e.name.lname,e.name.fname,e.name.mi);
   		printf("%5s %.2f \n","Number of Hours Work(w/ or w/o OT): ", hoursWork);
   		printf("%5s %.2f \n\n","Employee Rate: ",e.rate);
   		printf("%5s %.2f\n","Gross Pay(including Overtime Compensation): ",grosspay);
   		printf("%5s %.2f \n\n","Total Employee Contributions: ",totalEmployeeContributions);
   		printf("%5s %.2f\n","Net Pay(amount after deductions): ",netpay);
   		printf("%5s %15s \n\n","Prepared By: ", "Approved By: ");
   		printf("------------------------------------------------------------\n");
   		
   		retVal.employee = e;
   		strcpy(retVal.time,buffer);
	} else {
		  printf("Employee Not Found! \n");
	}
	
	return retVal;
}
